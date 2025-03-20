#ifndef TRAFFIC_LIGHT_SYSTEM_H
#define TRAFFIC_LIGHT_SYSTEM_H

#include "mbed.h"
#include "automation_mode.h"
#include "manual_mode.h"

// Time definitions
#define TIME_INCREMENT_MS   10
#define DEBOUNCE_BUTTON_TIME_MS 40
#define PRE_RED_YELLOW_DURATION 2000 // 2s yellow blink before red

// Light duration variables (in seconds)
extern int redLightDuration;
extern int yellowLightDuration;
extern int greenLightDuration;

// Initialize inputs and outputs
void inputsInit();
void outputsInit();

// Initialize the user interface display
void userInterfaceDisplayInit();

// Turn off all traffic lights
void turnOffAllLights();

// Initialize the complete traffic light system
void trafficLightSystemInit();

// Update and process the traffic light system state
void trafficLightSystemUpdate();

#endif // TRAFFIC_LIGHT_SYSTEM_H
