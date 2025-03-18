#ifndef MATRIX_BUTTON_H
#define MATRIX_BUTTON_H

#include "mbed.h"

#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4

void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();
void processKeypadInput();


#endif
