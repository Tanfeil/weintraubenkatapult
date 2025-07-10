// automatic_utils.cpp
#include "automatic_utils.h"

// SRF02 I2C address
const int SRF02_ADDR = 112;

// Calibration data
const int calibPotVals[] = { 300, 350, 400, 450, 500, 600, 700, 800, 850, 900, 950, 1023 };
const float calibDistances_old[] = {47.5, 68, 75.5, 88, 102.5, 112, 116, 111, 107, 105, 96, 93.5 };
const float calibDistances[] = {14, 41, 61, 85, 97, 110, 110, 110, 110, 110, 110, 110 };
const int tableSize = sizeof(calibPotVals) / sizeof(calibPotVals[0]);


Servo* strengthServoAuto = nullptr;
Servo* holdServoAuto = nullptr;
int outOfRangePin = -1;
int automaticModePin  = -1;

void automaticSetup(Servo& servo1, Servo& servo2, int outOfRangLED, int automaticModeLED) {
    Wire.begin();

    strengthServoAuto = &servo1;
    holdServoAuto = &servo2;

    outOfRangePin = outOfRangLED;
    automaticModePin = automaticModeLED;

    pinMode(outOfRangePin, OUTPUT);
    digitalWrite(outOfRangePin, LOW);

    pinMode(automaticModePin, OUTPUT);
    digitalWrite(automaticModePin, LOW);

    // Initialize servos
    initializeServos();
}

void outOfRange() {
    for (int i = 0; i <= 3; i++) {
        digitalWrite(outOfRangePin, HIGH);
        delay(200);
        digitalWrite(outOfRangePin, LOW);
        delay(200);
    }

    digitalWrite(automaticModePin, LOW);
}

void executeAutomaticMode() {
    digitalWrite(automaticModePin, HIGH);

    Serial.println(">> Launch triggered!");

    // Measure distance and add 5 cm offset
    int distance = readSRF02() + 12;

    Serial.print("Adjusted distance (with bowl offset): ");
    Serial.print(distance);
    Serial.println(" cm");

    // Find potentiometer value
    int potVal = getPotValue(distance);
    Serial.print("Chosen potentiometer value: ");
    Serial.println(potVal);

    if (potVal == 0) {
        outOfRange();
        Serial.println("Target out of Range");
        return;
    }

    int launchAngle = map(potVal, 0, 1023, 0, 180);

    setStrengthServo(launchAngle);
    
    Serial.print("Launching at angle: ");
    Serial.println(launchAngle);

    delay(2000);

    launch();

    Serial.println(">> Launch complete.");

    digitalWrite(automaticModePin, LOW);
}

int readSRF02() {
    Wire.beginTransmission(SRF02_ADDR);
    Wire.write(byte(0x00));
    Wire.write(byte(0x51));
    Wire.endTransmission();
    delay(70);

    Wire.beginTransmission(SRF02_ADDR);
    Wire.write(byte(0x02));
    Wire.endTransmission();

    Wire.requestFrom(SRF02_ADDR, 2);
    int distance = 0;
    if (Wire.available() >= 2) {
    distance = Wire.read() << 8;
    distance |= Wire.read();
    }
    return distance;
}

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

int getPotValue(float measuredDistance) {
    for (int i = 0; i < tableSize - 1; i++) {
        float distanceMin = calibDistances[i];
        float distanceMax = calibDistances[i + 1];

        float potiMin = calibPotVals[i];
        float potiMax = calibPotVals[i + 1];
        
        if (distanceMin <= measuredDistance && measuredDistance < distanceMax) {
            return map(measuredDistance, distanceMin, distanceMax, potiMin, potiMax);
        } else {
            Serial.println(String(distanceMin) + "<" + String(measuredDistance) + "<" + String(distanceMax));

        }
    }

    return 0;
}
