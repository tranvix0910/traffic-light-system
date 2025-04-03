#ifndef MANUAL_MODE_H
#define MANUAL_MODE_H

#include "mbed.h"

// Manual mode light states
typedef enum {
    MANUAL_OFF,           // All lights off
    MANUAL_RED,           // Red light on
    MANUAL_YELLOW,        // Yellow light on steady
    MANUAL_YELLOW_BLINK,  // Yellow light blinking
    MANUAL_PRE_RED,       // Yellow blinking before switching to red
    MANUAL_GREEN          // Green light on
} ManualLightState;

// Time input states
typedef enum {
    INPUT_NONE,          // No input yet
    INPUT_RED_TIME,      // Inputting red light time
    INPUT_YELLOW_TIME,   // Inputting yellow light time
    INPUT_GREEN_TIME,    // Inputting green light time
    INPUT_COMPLETE       // Input complete
} TimeInputState;

// Handle button presses in manual mode
void handleManualButtons(uint32_t currentMillis, ManualLightState &manualLightState, 
                         bool &manualYellowBlinkState, uint32_t &manualYellowPreviousMillis, 
                         uint32_t &preRedStartTime);

// Process time input from matrix keypad
void handleTimeInput(char key, TimeInputState &inputState, int &timeInputIndex, 
                    char *timeInputBuffer, int &redLightDuration, 
                    int &yellowLightDuration, int &greenLightDuration);

// Update light status in manual mode
void updateManualLights(uint32_t currentMillis, ManualLightState manualLightState, 
                       bool &manualYellowBlinkState, uint32_t &manualYellowPreviousMillis, 
                       uint32_t preRedStartTime);

#endif // MANUAL_MODE_H 