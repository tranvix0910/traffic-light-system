#include "traffic_light_system.h"
#include "arm_book_lib.h"
#include "display.h"
#include "matrix_keypad.h"

// I/O definitions
DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);
DigitalOut redLed(LED3);

DigitalIn changeStateButton(D10);
DigitalIn redButton(D11);
DigitalIn yellowButton(D12);
DigitalIn greenButton(D13);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

// Traffic light state variables
uint32_t previousMillis = 0;
uint32_t stateStartMillis = 0;
TrafficLightState state = GREEN;
bool yellowBlinkState = false;

// Manual mode variables
bool manualMode = false;
ManualLightState manualLightState = MANUAL_OFF;

// Manual mode timing variables
uint32_t manualYellowPreviousMillis = 0;
bool manualYellowBlinkState = false;
uint32_t preRedStartTime = 0;

// Time input variables
TimeInputState inputState = INPUT_NONE;
char timeInputBuffer[3] = {0};
int timeInputIndex = 0;

// Light duration times (in seconds)
int redLightDuration = 10;
int yellowLightDuration = 3;
int greenLightDuration = 7;

// Button state tracking
bool lastButtonState = false;

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

void trafficLightSystemInit()
{
    inputsInit();
    outputsInit();
    turnOffAllLights();
    matrixKeypadInit(10);
    userInterfaceDisplayInit();
    
    // Set default values
    state = GREEN;
    stateStartMillis = us_ticker_read() / 1000;
    manualMode = false;
    manualLightState = MANUAL_OFF;
    inputState = INPUT_COMPLETE;
}

void updateDisplayInfo()
{
    // Display current mode
    displayCharPositionWrite(6, 0);
    if (manualMode) {
        displayStringWrite("Manual  ");
    } else {
        displayStringWrite("Auto    ");
    }
    
    // Display current state
    displayCharPositionWrite(7, 1);
    if (manualMode) {
        switch (manualLightState) {
            case MANUAL_OFF:
                displayStringWrite("OFF     ");
                break;
            case MANUAL_RED:
                displayStringWrite("RED     ");
                break;
            case MANUAL_YELLOW:
                displayStringWrite("YELLOW  ");
                break;
            case MANUAL_YELLOW_BLINK:
                displayStringWrite("YEL-BLINK");
                break;
            case MANUAL_PRE_RED:
                displayStringWrite("PRE-RED ");
                break;
            case MANUAL_GREEN:
                displayStringWrite("GREEN   ");
                break;
        }
    } else {
        switch (state) {
            case GREEN:
                displayStringWrite("GREEN   ");
                break;
            case YELLOW_BLINK:
                displayStringWrite("YEL-BLINK");
                break;
            case RED:
                displayStringWrite("RED     ");
                break;
        }
    }
}

void trafficLightSystemUpdate()
{
    uint32_t currentMillis = us_ticker_read() / 1000;
    
    // Process keypad input
    char key = matrixKeypadUpdate();
    if (key != '\0' && manualMode) {
        handleTimeInput(key, inputState, timeInputIndex, timeInputBuffer, 
                      redLightDuration, yellowLightDuration, greenLightDuration);
    }
    
    // Check mode change button
    bool buttonState = changeStateButton;
    if (buttonState && !lastButtonState) {
        manualMode = !manualMode;
        if (manualMode) {
            turnOffAllLights();
            manualLightState = MANUAL_OFF;
            inputState = INPUT_COMPLETE;
            uartUsb.write("Entering manual mode\r\n", 21);
            uartUsb.write("Press A/B/C to set light times, or use buttons to control lights\r\n", 63);
        } else {
            inputState = INPUT_COMPLETE;
            state = GREEN;
            stateStartMillis = currentMillis;
            uartUsb.write("Entering automatic mode\r\n", 24);
        }
    }
    lastButtonState = buttonState;
    
    if (manualMode) {
        // Handle manual button presses
        handleManualButtons(currentMillis, manualLightState, manualYellowBlinkState, 
                          manualYellowPreviousMillis, preRedStartTime);
        
        // Update manual mode lights
        updateManualLights(currentMillis, manualLightState, manualYellowBlinkState, 
                         manualYellowPreviousMillis, preRedStartTime);
    } else {
        // Automatic mode
        updateAutomaticMode(currentMillis, state, stateStartMillis, previousMillis, yellowBlinkState);
    }
    
    // Update display information
    updateDisplayInfo();
}
