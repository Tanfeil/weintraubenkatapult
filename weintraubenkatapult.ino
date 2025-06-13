// katapult_main.ino
#include <Servo.h>
#include "automatic_utils.h"
#include "manual_utils.h"

// Pin definitions
const int automaticLaunchButtonPin = 8;
const int manualLaunchButtonPin = 7;
const int strengthMeterPins[] = {2, 3, 4, 5, 6}; // Strength meter LED pins
const int outOfRangePin = 13;
const int readyPin = 10;
const int automaticModePin = 11;
const int manualModePin = 12;
const int strenghtServoPin = 9;
const int holdServoPin = A1;
const int potentiometerPin = A0;

// Servo setup
Servo strengthServo;
Servo holdServo;

// Button state variables
int automaticLaunchButtonCurrState = HIGH;
int automaticLaunchButtonPrevState = HIGH;

int manualLaunchButtonCurrState = HIGH;
int manualLaunchButtonPrevState = HIGH;

unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

void setup() {
    Serial.begin(9600);

    // Attach servos and initialize setup
    strengthServo.attach(strenghtServoPin);
    holdServo.attach(holdServoPin);

    // Attach buttons
    pinMode(automaticLaunchButtonPin, INPUT_PULLUP);
    pinMode(manualLaunchButtonPin, INPUT_PULLUP);

    // Initialize LEDs
    for (int i = 0; i < 5; i++) {
        pinMode(strengthMeterPins[i], OUTPUT);
        digitalWrite(strengthMeterPins[i], LOW);
    }

    pinMode(manualModePin, OUTPUT);
    digitalWrite(manualModePin, HIGH);

    // Setup automatic mode
    automaticSetup(strengthServo, holdServo, outOfRangePin, automaticModePin);
    manualSetup(potentiometerPin, readyPin, strengthServo, holdServo);

    // Initialize button state
    automaticLaunchButtonPrevState = digitalRead(automaticLaunchButtonPin);
}

void updateStrengthMeter(int angle) {
    int level = map(angle, 0, 180, 0, 5); // Map servo angle to LED levels (0-5)
    for (int i = 0; i < 5; i++) {
        if (i < level) {
            digitalWrite(strengthMeterPins[i], HIGH);
        } else {
            digitalWrite(strengthMeterPins[i], LOW);
        }
    }
}

void loop() {
    int currentAngle = strengthServo.read();
    updateStrengthMeter(currentAngle);

    // Check button state and debounce
    int reading = digitalRead(automaticLaunchButtonPin);
    if (reading != automaticLaunchButtonPrevState) {
        lastDebounceTime = millis();
        automaticLaunchButtonCurrState = reading;
    }

    reading = digitalRead(manualLaunchButtonPin);
    if (reading != automaticLaunchButtonPrevState) {
        lastDebounceTime = millis();
        manualLaunchButtonCurrState = reading;
    }

    if ((millis() - lastDebounceTime) > debounceDelay) {
        setStrengthPotentiometer()

        if (automaticLaunchButtonCurrState == LOW && automaticLaunchButtonPrevState == HIGH) {
            digitalWrite(manualModePin, LOW);
            executeAutomaticMode(); // Trigger the automatic sequence
            automaticLaunchButtonPrevState = automaticLaunchButtonCurrState;
            digitalWrite(manualModePin, HIGH);
        }

        if (manualLaunchButtonCurrState == LOW && manualLaunchButtonPrevState == HIGH) {
            executeManualMode(); // Trigger the manual sequence
            manualLaunchButtonPrevState = manualLaunchButtonCurrState;
        }

    }
}
