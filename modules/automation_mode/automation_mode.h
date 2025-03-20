#ifndef AUTOMATION_MODE_H
#define AUTOMATION_MODE_H

#include "mbed.h"

// Time definition for yellow blink mode
#define YELLOW_BLINK_TIME 500 // 500ms blink interval

// Traffic light state definitions
typedef enum {
    GREEN,
    YELLOW_BLINK,
    RED
} TrafficLightState;

// External time variables (in seconds)
extern int redLightDuration;
extern int yellowLightDuration; 
extern int greenLightDuration;

// Update the traffic light in automatic mode
void updateAutomaticMode(uint32_t currentMillis, TrafficLightState &state, 
                      uint32_t &stateStartMillis, uint32_t &previousMillis, 
                      bool &yellowBlinkState);

#endif // AUTOMATION_MODE_H 