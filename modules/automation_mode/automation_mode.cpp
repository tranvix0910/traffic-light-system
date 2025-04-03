#include "automation_mode.h"
#include "arm_book_lib.h"

// LED declarations
extern DigitalOut greenLed;
extern DigitalOut yellowLed;
extern DigitalOut redLed;

void updateAutomaticMode(uint32_t currentMillis, TrafficLightState &state, 
                       uint32_t &stateStartMillis, uint32_t &previousMillis, 
                       bool &yellowBlinkState)
{
    switch (state) {
        case GREEN:
            greenLed = ON;
            if (currentMillis - stateStartMillis >= greenLightDuration * 1000) {
                greenLed = OFF;
                state = YELLOW_BLINK;
                stateStartMillis = currentMillis;
            }
            break;
        
        case YELLOW_BLINK:
            if (currentMillis - previousMillis >= YELLOW_BLINK_TIME) {
                previousMillis = currentMillis;
                yellowBlinkState = !yellowBlinkState;
                yellowLed = yellowBlinkState;
            }
            if (currentMillis - stateStartMillis >= yellowLightDuration * 1000) {
                yellowLed = OFF;
                state = RED;
                stateStartMillis = currentMillis;
            }
            break;
        
        case RED:
            redLed = ON;
            if (currentMillis - stateStartMillis >= redLightDuration * 1000) {
                redLed = OFF;
                state = GREEN;
                stateStartMillis = currentMillis;
            }
            break;
    }
} 