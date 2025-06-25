#ifndef MANUAL_UTILS_H
#define MANUAL_UTILS_H

#include <Arduino.h>
#include <Servo.h>

// Initializes the manual mode setup
void manualSetup(int potentiometerPin, int readPin, Servo& servo1, Servo& servo2, int strengthMeterLeds[]);
void initializeServos();

// Executes the manual launch sequence
void executeManualMode();

void setHoldServo(int val);
void setStrengthServo(int val);
void setStrengthPotentiometer();

void releaseStrength();
void reset();
int readPotentiometer();

void hold();
void release();

void launch();

void updateStrengthMeterManual(int angle); 
#endif