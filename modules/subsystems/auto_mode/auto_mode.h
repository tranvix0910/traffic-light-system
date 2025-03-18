#ifndef AUTO_MODE_H
#define AUTO_MODE_H

#include "mbed.h"
#include "arm_book_lib.h"
#include "matrix_button.h"

// Default timing values (will be overridden by keypad input)
#define YELLOW_BLINK_TIME 500       

void auto_mode_init();
void auto_mode_update();

#endif
