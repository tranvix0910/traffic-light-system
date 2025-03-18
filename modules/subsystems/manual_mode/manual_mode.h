#ifndef MANUAL_MODE_H
#define MANUAL_MODE_H

#include "mbed.h"

#define DEBOUNCE_BUTTON_TIME_MS 40

void manual_mode_init();
void manual_mode_update();

#endif
