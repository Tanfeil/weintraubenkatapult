// automatic_utils.h
#ifndef AUTOMATIC_UTILS_H
#define AUTOMATIC_UTILS_H

#include <Servo.h>
#include <Wire.h>
#include <Arduino.h>
#include "manual_utils.h"
#include "automatic_utils.h"

// Initializes the automatic mode setup
void automaticSetup(Servo& servo1, Servo& servo2, int outOfRangLED, int automaticModeLED);

// Executes the automatic launch sequence
void executeAutomaticMode();

// Helper functions
int readSRF02();
int getClosestPotValue(float measuredDistance);
int getPotValue(float measuredDistance);

#endif // AUTOMATIC_UTILS_H