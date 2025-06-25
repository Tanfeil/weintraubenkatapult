#include "manual_utils.h"

Servo* strengthServoMan = nullptr;
Servo* holdServoMan = nullptr;

int potentiometerPin = -1;
int readyPin  = -1;

int strengthMeterPinsManual[5];

void manualSetup(int potentiometer, int readyLED, Servo& servo1, Servo& servo2, int strengthMeterLeds[]) {
    strengthServoMan = &servo1;
    holdServoMan = &servo2;

    potentiometerPin = potentiometer;
    readyPin = readyLED;

    for (int i = 0; i < 5 ; ++i) {
        strengthMeterPinsManual[i] = strengthMeterLeds[i];
    }

    pinMode(readyPin, OUTPUT);
    digitalWrite(readyPin, LOW);

    initializeServos();
}

void initializeServos() {
    // Initialize servos
    int mode = digitalRead(readyPin);

    if (mode == LOW) {
        reset();
    }
}

void setStrengthServo(int val) {
    // Set the potentiometer-controlled servo position
    updateStrengthMeterManual(val);
    strengthServoMan->write(val);
}

void setHoldServo(int val) {
    // Rotate the positional servo by 90 degrees each time the button is clicked
    holdServoMan->write(val);  // Set the positional servo to the new position
    delay(200);  // Debounce delay
}

void releaseStrength() {
    setStrengthServo(0);
}

void hold() {
    setHoldServo(90);
}
    
void release() {
    setHoldServo(0);
}

int readPotentiometer() {
    // Read the potentiometer value (0-1023)
    int potentiometer_val = analogRead(potentiometerPin);
  
    // Map potentiometer value (0-1023) to servo position (0-180)
    return map(potentiometer_val, 0, 1023, 0, 180);
}

void reset() {
    release();
    delay(800);

    releaseStrength();
    delay(1200);

    hold();
    delay(300);

    setStrengthPotentiometer();

    digitalWrite(readyPin, HIGH);
}

void setStrengthPotentiometer() {
    setStrengthServo(readPotentiometer());
}

void launch() {
    //initializeServos();

    digitalWrite(readyPin, LOW);
    
    release();
    delay(300);

    reset();
}

void executeManualMode() {
    setStrengthPotentiometer();

    launch();
}

void updateStrengthMeterManual(int angle) {
    int level = map(angle, 0, 170, 0, 5); // Map servo angle to LED levels (0-5)
    for (int i = 0; i < 5; i++) {
        if (i < level) {
            digitalWrite(strengthMeterPinsManual[i], HIGH);
        } else {
            digitalWrite(strengthMeterPinsManual[i], LOW);
        }
    }
} 
