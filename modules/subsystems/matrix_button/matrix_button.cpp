#include "matrix-button.h"


typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

// Global variables
DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {PB_3, PB_5, PC_7, PA_15};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {PB_12, PB_13, PB_15, PC_6};

char inputBuffer[MAX_INPUT_LENGTH + 1]; // Buffer to store input seconds
int inputIndex = 0;
bool inputComplete = false;
InputState_t currentInputState = GREEN;

// Traffic light timing variables
int greenTime = 8;
int yellowTime = 5;
int redTime = 7;

int accumulatedDebounceMatrixKeypadTime = 0;
int matrixKeypadCodeIndex = 0;
char matrixKeypadLastKeyPressed = '\0';
char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

matrixKeypadState_t matrixKeypadState;

void matrixKeypadInit()
{
    matrixKeypadState = MATRIX_KEYPAD_SCANNING;
    int pinIndex = 0;
    for( pinIndex=0; pinIndex<KEYPAD_NUMBER_OF_COLS; pinIndex++ ) {
        (keypadColPins[pinIndex]).mode(PullUp);
    }
    
    // Initialize input state
    currentInputState = GREEN;
    inputIndex = 0;
    inputComplete = false;
    memset(inputBuffer, 0, sizeof(inputBuffer));
    printf("Bắt đầu cấu hình. Nhập thời gian cho đèn xanh:\n");
}

char matrixKeypadScan()
{
    int row = 0;
    int col = 0;
    int i = 0;

    for( row=0; row<KEYPAD_NUMBER_OF_ROWS; row++ ) {

        for( i=0; i<KEYPAD_NUMBER_OF_ROWS; i++ ) {
            keypadRowPins[i] = ON;
        }

        keypadRowPins[row] = OFF;

        for( col=0; col<KEYPAD_NUMBER_OF_COLS; col++ ) {
            if( keypadColPins[col] == OFF ) {
                return matrixKeypadIndexToCharArray[row*KEYPAD_NUMBER_OF_ROWS + col];
            }
        }
    }
    return '\0';
}

char matrixKeypadUpdate()
{
    char keyDetected = '\0';
    char keyReleased = '\0';

    switch( matrixKeypadState ) {

    case MATRIX_KEYPAD_SCANNING:
        keyDetected = matrixKeypadScan();
        if( keyDetected != '\0' ) {
            matrixKeypadLastKeyPressed = keyDetected;
            accumulatedDebounceMatrixKeypadTime = 0;
            matrixKeypadState = MATRIX_KEYPAD_DEBOUNCE;
        }
        break;

    case MATRIX_KEYPAD_DEBOUNCE:
        if( accumulatedDebounceMatrixKeypadTime >=
            DEBOUNCE_KEY_TIME_MS ) {
            keyDetected = matrixKeypadScan();
            if( keyDetected == matrixKeypadLastKeyPressed ) {
                matrixKeypadState = MATRIX_KEYPAD_KEY_HOLD_PRESSED;
            } else {
                matrixKeypadState = MATRIX_KEYPAD_SCANNING;
            }
        }
        accumulatedDebounceMatrixKeypadTime =
            accumulatedDebounceMatrixKeypadTime + TIME_INCREMENT_MS;
        break;

    case MATRIX_KEYPAD_KEY_HOLD_PRESSED:
        keyDetected = matrixKeypadScan();
        if( keyDetected != matrixKeypadLastKeyPressed ) {
            if( keyDetected == '\0' ) {
                keyReleased = matrixKeypadLastKeyPressed;
            }
            matrixKeypadState = MATRIX_KEYPAD_SCANNING;
        }
        break;

    default:
        matrixKeypadInit();
        break;
    }
    return keyReleased;
}

void processKeypadInput() {
    char key = matrixKeypadUpdate();

    if (key != '\0') {
        // Display the pressed key
        printf("%c", key);
        
        if (key >= '0' && key <= '9' && inputIndex < MAX_INPUT_LENGTH) {
            // Add digit to buffer
            inputBuffer[inputIndex++] = key;
            inputBuffer[inputIndex] = '\0'; // Ensure null termination
        } else if (key == '#') { // # key confirms the current value
            if (inputIndex > 0) {
                int timeValue = atoi(inputBuffer); // Convert string to integer
                
                // Store value for the current light
                switch (currentInputState) {
                    case GREEN:
                        greenTime = timeValue;
                        currentInputState = YELLOW;
                        printf("\nĐèn xanh: %d giây\nNhập thời gian cho đèn vàng: ", greenTime);
                        break;
                    case YELLOW:
                        yellowTime = timeValue;
                        currentInputState = RED;
                        printf("\nĐèn vàng: %d giây\nNhập thời gian cho đèn đỏ: ", yellowTime);
                        break;
                    case RED:
                        redTime = timeValue;
                        inputComplete = true;
                        printf("\nĐèn đỏ: %d giây\nCấu hình hoàn tất!", redTime);
                        break;
                }
            }

            // Reset buffer for next input
            inputIndex = 0;
            memset(inputBuffer, 0, sizeof(inputBuffer));
        } else if (key == '*') { // * key clears the current input
            inputIndex = 0;
            memset(inputBuffer, 0, sizeof(inputBuffer));
            printf("\nĐã xóa. Nhập lại: ");
        }
    }
}