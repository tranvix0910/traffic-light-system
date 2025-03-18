#include "manual_mode.h"

DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);d
DigitalOut redLed(LED3);

DigitalIn buttonGreen(D2);
DigitalIn buttonYellow(D3);
DigitalIn buttonRed(D4);

void manual_mode_init()
{
    buttonGreen.mode("PullDown");
    buttonYellow.mode("PullDown");
    buttonRed.mode("PullDown");
}

void manual_mode_update()
{
    if (buttonGreen)
    {
        greenLed = ON;
        yellowLed = OFF;
        redLed = OFF;
    }
    else if (buttonYellow)
    {
        greenLed = OFF;
        yellowLed = ON;
        redLed = OFF;
    }
    else if (buttonRed)
    {
        greenLed = OFF;
        yellowLed = OFF;
        redLed = ON;
    }
}