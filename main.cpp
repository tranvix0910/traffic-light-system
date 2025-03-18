#include "smart_light.h"

int main()
{
    smart_light_init();

    while (true){
        smart_light_update();
    }
    // greenLed = OFF;
    // redLed = OFF;
    // yellowLed = OFF;

    // while (true) {
    //     greenLed = 1;
    //     yellowLed = 0;
    //     redLed = 0;
    //     thread_sleep_for(GREEN_LIGHT_TIME);

    //     int elapsedTime = 0;
    //     while (elapsedTime < YELLOW_LIGHT_TIME) {
    //         greenLed = 0;
    //         yellowLed = !yellowLed;
    //         thread_sleep_for(YELLOW_BLINK_TIME);
    //         elapsedTime += YELLOW_BLINK_TIME;
    //     }
    //     yellowLed = 0;

    //     greenLed = 0;
    //     yellowLed = 0;
    //     redLed = 1;
    //     thread_sleep_for(RED_LIGHT_TIME);
    // }
}
