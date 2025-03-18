#ifndef AUTO_MODE_H
#define AUTO_MODE_H

#include "mbed.h"
#include "arm_book_lib.h"

#define GREEN_LIGHT_TIME  8000
#define YELLOW_LIGHT_TIME 5000
#define RED_LIGHT_TIME    7000
#define YELLOW_BLINK_TIME 500       


void auto_mode_init();
void auto_mode_update();

#endif
