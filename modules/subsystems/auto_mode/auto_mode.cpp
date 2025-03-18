#include "auto_mode.h"
#include "matrix_button.h"

DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);
DigitalOut redLed(LED3);

void auto_mode_init()
{
    greenLed = OFF;
    redLed = OFF;
    yellowLed = OFF;
    
    // Initialize matrix keypad for configuration
    matrixKeypadInit();
}

void auto_mode_update()
{
    // Process keypad input if not yet completed
    if (!inputComplete) {
        processKeypadInput();
        return; // Don't proceed with traffic light cycle until configuration is complete
    }
    
    // Convert seconds to milliseconds for timing
    int greenTimeMs = greenTime * 1000;
    int yellowTimeMs = yellowTime * 1000;
    int redTimeMs = redTime * 1000;
    
    // Green light phase
    greenLed = ON;
    yellowLed = OFF;
    redLed = OFF;
    thread_sleep_for(greenTimeMs);

    // Yellow blinking phase
    int elapsedTime = 0;
    while (elapsedTime < yellowTimeMs)
    {
        greenLed = OFF;
        yellowLed = !yellowLed;
        thread_sleep_for(YELLOW_BLINK_TIME);
        elapsedTime += YELLOW_BLINK_TIME;
    }

    // Red light phase
    greenLed = OFF;
    yellowLed = OFF;
    redLed = ON;
    thread_sleep_for(redTimeMs);
}