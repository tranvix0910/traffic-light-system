#include "manual_mode.h"
#include "traffic_light_system.h"
#include "arm_book_lib.h"
#include <cstring>

// LED declarations
extern DigitalOut greenLed;
extern DigitalOut yellowLed;
extern DigitalOut redLed;

// Button declarations
extern DigitalIn redButton;
extern DigitalIn yellowButton;
extern DigitalIn greenButton;

// UART declaration
extern UnbufferedSerial uartUsb;

// Button state tracking variables
static bool lastRedButtonState = false;
static bool lastYellowButtonState = false;
static bool lastGreenButtonState = false;

void handleManualButtons(uint32_t currentMillis, ManualLightState &manualLightState, 
                         bool &manualYellowBlinkState, uint32_t &manualYellowPreviousMillis, 
                         uint32_t &preRedStartTime)
{
    // Check red button
    bool currentRedButtonState = redButton;
    if (currentRedButtonState && !lastRedButtonState) {
        // If current state is red, turn off
        if (manualLightState == MANUAL_RED) {
            manualLightState = MANUAL_OFF;
        } else {
            // Start yellow blinking before switching to red
            manualLightState = MANUAL_PRE_RED;
            manualYellowPreviousMillis = currentMillis;
            manualYellowBlinkState = true;
            preRedStartTime = currentMillis; // Save start time
        }
    }
    lastRedButtonState = currentRedButtonState;
    
    // Check yellow button
    bool currentYellowButtonState = yellowButton;
    if (currentYellowButtonState && !lastYellowButtonState) {
        // If current state is yellow, turn off
        if (manualLightState == MANUAL_YELLOW || manualLightState == MANUAL_YELLOW_BLINK) {
            manualLightState = MANUAL_OFF;
        } else {
            // Turn on yellow blinking
            manualLightState = MANUAL_YELLOW_BLINK;
            manualYellowPreviousMillis = currentMillis;
            manualYellowBlinkState = true;
        }
    }
    lastYellowButtonState = currentYellowButtonState;
    
    // Check green button
    bool currentGreenButtonState = greenButton;
    if (currentGreenButtonState && !lastGreenButtonState) {
        // If current state is green, turn off
        if (manualLightState == MANUAL_GREEN) {
            manualLightState = MANUAL_OFF;
        } else {
            // Turn on green light
            manualLightState = MANUAL_GREEN;
        }
    }
    lastGreenButtonState = currentGreenButtonState;
}

void handleTimeInput(char key, TimeInputState &inputState, int &timeInputIndex, 
                    char *timeInputBuffer, int &redLightDuration, 
                    int &yellowLightDuration, int &greenLightDuration)
{
    switch(key) {
        case 'A':
            // Start inputting red light time
            inputState = INPUT_NONE;
            timeInputIndex = 0;
            timeInputBuffer[0] = timeInputBuffer[1] = '\0';
            uartUsb.write("Enter red light time (2 digits):\r\n", 33);
            break;
        case 'B':
            // Start inputting yellow light time
            inputState = INPUT_RED_TIME;
            timeInputIndex = 0;
            timeInputBuffer[0] = timeInputBuffer[1] = '\0';
            uartUsb.write("Enter yellow light time (2 digits):\r\n", 36);
            break;
        case 'C':
            // Start inputting green light time
            inputState = INPUT_YELLOW_TIME;
            timeInputIndex = 0;
            timeInputBuffer[0] = timeInputBuffer[1] = '\0';
            uartUsb.write("Enter green light time (2 digits):\r\n", 35);
            break;
        default:
            // Handle number input and # key
            if (inputState != INPUT_COMPLETE) {
                if (key >= '0' && key <= '9' && timeInputIndex < 2) {
                    timeInputBuffer[timeInputIndex++] = key;
                    char buffer[30];
                    sprintf(buffer, "Entered: %c\r\n", key);
                    uartUsb.write(buffer, strlen(buffer));
                }
                else if (key == '#' && timeInputIndex > 0) {
                    // Convert buffer to number and save to variable
                    timeInputBuffer[timeInputIndex] = '\0';
                    int time = atoi(timeInputBuffer);
                    
                    switch(inputState) {
                        case INPUT_NONE:
                            redLightDuration = time;
                            inputState = INPUT_COMPLETE;
                            char buffer[50];
                            sprintf(buffer, "Red light time set to %d seconds\r\n", time);
                            uartUsb.write(buffer, strlen(buffer));
                            break;
                            
                        case INPUT_RED_TIME:
                            yellowLightDuration = time;
                            inputState = INPUT_COMPLETE;
                            char buffer2[50];
                            sprintf(buffer2, "Yellow light time set to %d seconds\r\n", time);
                            uartUsb.write(buffer2, strlen(buffer2));
                            break;
                            
                        case INPUT_YELLOW_TIME:
                            greenLightDuration = time;
                            inputState = INPUT_COMPLETE;
                            char buffer3[50];
                            sprintf(buffer3, "Green light time set to %d seconds\r\n", time);
                            uartUsb.write(buffer3, strlen(buffer3));
                            break;
                            
                        default:
                            break;
                    }
                    
                    // Reset buffer for next input
                    timeInputIndex = 0;
                    timeInputBuffer[0] = timeInputBuffer[1] = '\0';
                }
            }
            break;
    }
}

void updateManualLights(uint32_t currentMillis, ManualLightState manualLightState, 
                       bool &manualYellowBlinkState, uint32_t &manualYellowPreviousMillis, 
                       uint32_t preRedStartTime)
{
    // Process light states
    switch (manualLightState) {
        case MANUAL_OFF:
            turnOffAllLights();
            break;
            
        case MANUAL_RED:
            redLed = ON;
            yellowLed = OFF;
            greenLed = OFF;
            break;
            
        case MANUAL_YELLOW:
            redLed = OFF;
            yellowLed = ON;
            greenLed = OFF;
            break;
            
        case MANUAL_YELLOW_BLINK:
            // Yellow light blinking
            redLed = OFF;
            greenLed = OFF;
            
            if (currentMillis - manualYellowPreviousMillis >= YELLOW_BLINK_TIME) {
                manualYellowPreviousMillis = currentMillis;
                manualYellowBlinkState = !manualYellowBlinkState;
            }
            yellowLed = manualYellowBlinkState;
            break;
            
        case MANUAL_PRE_RED:
            // Yellow blinking before red
            redLed = OFF;
            greenLed = OFF;
            
            if (currentMillis - manualYellowPreviousMillis >= YELLOW_BLINK_TIME) {
                manualYellowPreviousMillis = currentMillis;
                manualYellowBlinkState = !manualYellowBlinkState;
            }
            yellowLed = manualYellowBlinkState;
            break;
            
        case MANUAL_GREEN:
            redLed = OFF;
            yellowLed = OFF;
            greenLed = ON;
            break;
    }
} 