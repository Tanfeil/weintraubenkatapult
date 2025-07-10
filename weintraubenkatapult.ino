// katapult_main.ino
#include <Servo.h>
#include "automatic_utils.h"
#include "manual_utils.h"

// Pin definitions
const int automaticLaunchButtonPin = 11;
const int manualLaunchButtonPin = 10;
const int strengthMeterPins[] = {3, 4, 5, 6, 7}; // Strength meter LED pins
const int outOfRangePin = 2;
const int readyPin = 8;
const int automaticModePin = 12;
const int manualModePin = 13;
const int strenghtServoPin = A2;
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

    // Setup automatic mode (setup manual mode first for some variables and leds to get set)
    manualSetup(potentiometerPin, readyPin, strengthServo, holdServo, strengthMeterPins);
    automaticSetup(strengthServo, holdServo, outOfRangePin, automaticModePin);

    // Initialize button state
    automaticLaunchButtonPrevState = digitalRead(automaticLaunchButtonPin);
}

void loop() {
    //int currentAngle = strengthServo.read();
    //updateStrengthMeter(currentAngle);

    // Check button state and debounce      
    automaticLaunchButtonCurrState = digitalRead(automaticLaunchButtonPin);   
    manualLaunchButtonCurrState = digitalRead(manualLaunchButtonPin);

    if ((millis() - lastDebounceTime) > debounceDelay) {
        setStrengthPotentiometer();

        if (automaticLaunchButtonCurrState == LOW && automaticLaunchButtonPrevState == HIGH) {
            digitalWrite(manualModePin, LOW);
            executeAutomaticMode(); // Trigger the automatic sequence
            Serial.println("Automatic");
            digitalWrite(manualModePin, HIGH);
            lastDebounceTime = millis();
        }

        if (manualLaunchButtonCurrState == LOW && manualLaunchButtonPrevState == HIGH) {
            executeManualMode(); // Trigger the manual sequence
            Serial.println("Manual");
            lastDebounceTime = millis();
        }

    }

    automaticLaunchButtonPrevState = automaticLaunchButtonCurrState; 
    manualLaunchButtonPrevState = manualLaunchButtonCurrState;
}
