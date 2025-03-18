#include "auto_mode.h"

DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);
DigitalOut redLed(LED3);

void auto_mode_init()
{
    greenLed = OFF;
    redLed = OFF;
    yellowLed = OFF;
}

void auto_mode_update()
{
    greenLed = ON;
    yellowLed = OFF;
    redLed = OFF;
    thread_sleep_for(GREEN_LIGHT_TIME);

    int elapsedTime = OFF;
    while (elapsedTime < YELLOW_LIGHT_TIME)
    {
        greenLed = OFF;
        yellowLed = OFF;
        yellowLed = !yellowLed;
        thread_sleep_for(YELLOW_BLINK_TIME);
        elapsedTime += YELLOW_BLINK_TIME;
    }

    greenLed = OFF;
    yellowLed = OFF;
    redLed = ON;
    thread_sleep_for(RED_LIGHT_TIME);
}