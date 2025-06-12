#include <Wire.h>
#include <Servo.h>

// Servo setup
Servo servo;               // Launch angle servo
Servo positional_servo;    // Holding servo

const int launchButtonPin = 8;

// Debounce
int launchButtonState = HIGH;
int lastLaunchButtonState = HIGH;
bool launchReady = false;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

// SRF02 I2C address
const int SRF02_ADDR = 112;

// ✅ Updated calibration table
const int calibPotVals[] = { 300, 350, 400, 450, 500, 600, 700, 800, 850, 900, 950, 1023 };
const float calibDistances[] = {47.5, 68, 75.5, 88, 102.5, 112, 116, 111, 107, 105, 96, 93.5 };

const int tableSize = sizeof(calibPotVals) / sizeof(calibPotVals[0]);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  servo.attach(9);
  positional_servo.attach(A1);
  pinMode(launchButtonPin, INPUT_PULLUP);

  // Set both servos to 0 at startup
  servo.write(0);
  positional_servo.write(0);

  delay(200);  // Let power stabilize
  lastLaunchButtonState = digitalRead(launchButtonPin);
  launchReady = true;
}

void loop() {
  int reading = digitalRead(launchButtonPin);

  if (reading != launchButtonState) {
    lastDebounceTime = millis();
    launchButtonState = reading;
  }

  if ((millis() - lastDebounceTime) > debounceDelay && launchReady) {
    if (launchButtonState == LOW && lastLaunchButtonState == HIGH) {
      Serial.println(">> Launch triggered!");

      // 1. Measure distance and add 5 cm offset
      int distance = readSRF02() + 5;

      Serial.print("Adjusted distance (with bowl offset): ");
      Serial.print(distance);
      Serial.println(" cm");

      // 2. Find potentiometer value
      int potVal = getPotValue(distance);
      Serial.print("Chosen potentiometer value: ");
      Serial.println(potVal);

      // 3. Launch Sequence
      servo.write(0);
      positional_servo.write(0);
      delay(2000);

      positional_servo.write(90);
      delay(300);

      int launchAngle = map(potVal, 0, 1023, 0, 180);
      servo.write(launchAngle);
      Serial.print("Launching at angle: ");
      Serial.println(launchAngle);
      delay(2000);

      positional_servo.write(0);
      delay(300);

      servo.write(0);
      Serial.println(">> Launch complete.");
    }

    lastLaunchButtonState = launchButtonState;
  }
}

// Reads distance from SRF02 in cm
int readSRF02() {
  Wire.beginTransmission(SRF02_ADDR);
  Wire.write(byte(0x00));  // Command register
  Wire.write(byte(0x51));  // Measure in cm
  Wire.endTransmission();
  delay(70);  // Wait for measurement

  Wire.beginTransmission(SRF02_ADDR);
  Wire.write(byte(0x02));  // Read high byte
  Wire.endTransmission();

  Wire.requestFrom(SRF02_ADDR, 2);
  int distance = 0;
  if (Wire.available() >= 2) {
    distance = Wire.read() << 8;
    distance |= Wire.read();
  }
  return distance;
}

// Finds the closest potentiometer value from calibration table
int getClosestPotValue(float measuredDistance) {
  float minDiff = 10000;
  int closestPot = calibPotVals[0];

  for (int i = 0; i < tableSize; i++) {
    float diff = abs(calibDistances[i] - measuredDistance);
    if (diff < minDiff) {
      minDiff = diff;
      closestPot = calibPotVals[i];
    }
  }
  return closestPot;
}

// Finds the potentiometer value by linear interpolation
int getPotValue(float measuredDistance) {
  for (int i = 0; i < tableSize - 1; i++) {
    float distanceMin = calibDistances[i];
    float distanceMax = calibDistances[i + 1];

    float potiMin = calibPotVals[i];
    float potiMax = calibPotVals[i + 1];

    if (distanceMin <= measuredDistance < distanceMax) {
      return map(measuredDistance, distanceMin, distanceMax, potiMin, potiMax);
    }
  }

  return 0;
}
