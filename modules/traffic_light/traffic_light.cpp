#include "traffic_light.h"
#include "arm_book_lib.h"
#include "display.h"

// Khai báo các LED
extern DigitalOut greenLed;
extern DigitalOut yellowLed;
extern DigitalOut redLed;

// Khai báo các nút nhấn
extern DigitalIn changeStateButton;
extern DigitalIn redButton;
extern DigitalIn yellowButton;
extern DigitalIn greenButton;

void inputsInit()
{
    changeStateButton.mode(PullDown);
    redButton.mode(PullDown);
    yellowButton.mode(PullDown);
    greenButton.mode(PullDown);
}

void outputsInit()
{
    greenLed = OFF;
    yellowLed = OFF;
    redLed = OFF;
}

void userInterfaceDisplayInit()
{
    displayInit();

    displayCharPositionWrite(0, 0);
    displayStringWrite("Mode: ");

    displayCharPositionWrite(0, 1);
    displayStringWrite("State: ");
}

void turnOffAllLights()
{
    greenLed = OFF;
    yellowLed = OFF;
    redLed = OFF;
}

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