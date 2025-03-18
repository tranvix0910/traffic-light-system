#ifndef MATRIX_BUTTON_H
#define MATRIX_BUTTON_H

#include "mbed.h"

#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4
#define MAX_INPUT_LENGTH                         2
#define DEBOUNCE_KEY_TIME_MS                     40
#define TIME_INCREMENT_MS                        5


// Define input states
typedef enum {
    GREEN,
    YELLOW,
    RED
} InputState_t;

// Function declarations
void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();
void processKeypadInput();

// External variables for global use
extern char inputBuffer[MAX_INPUT_LENGTH + 1];
extern int inputIndex;
extern bool inputComplete;
extern InputState_t currentInputState;
extern int greenTime;
extern int yellowTime;
extern int redTime;

#endif
