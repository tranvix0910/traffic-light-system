#include "mbed.h"
#include "arm_book_lib.h"

// DEFINES
#define YELLOW_BLINK_TIME                       500       
#define KEYPAD_NUMBER_OF_ROWS                    4
#define KEYPAD_NUMBER_OF_COLS                    4
#define MAX_INPUT_LENGTH                         2
#define DEBOUNCE_KEY_TIME_MS                     40
#define TIME_INCREMENT_MS                        5
#define DEBOUNCE_BUTTON_TIME_MS                  40

//=====[Declaration of public data types]======================================

typedef enum {
    GREEN,
    YELLOW,
    RED
} InputState_t;

typedef enum {
    MATRIX_KEYPAD_SCANNING,
    MATRIX_KEYPAD_DEBOUNCE,
    MATRIX_KEYPAD_KEY_HOLD_PRESSED
} matrixKeypadState_t;

DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);
DigitalOut redLed(LED3);

DigitalOut keypadRowPins[KEYPAD_NUMBER_OF_ROWS] = {PB_3, PB_5, PC_7, PA_15};
DigitalIn keypadColPins[KEYPAD_NUMBER_OF_COLS]  = {PB_12, PB_13, PB_15, PC_6};

DigitalIn buttonGreen(D2);
DigitalIn buttonYellow(D3);
DigitalIn buttonRed(D4);
DigitalIn buttonChangeMode(D5);   


// Traffic light timing variables
int greenTime = 8;
int yellowTime = 5;
int redTime = 7;

char inputBuffer[MAX_INPUT_LENGTH + 1]; // Buffer to store input seconds
int inputIndex = 0;
bool inputComplete = false;
int accumulatedDebounceMatrixKeypadTime = 0;
int matrixKeypadCodeIndex = 0;
char matrixKeypadLastKeyPressed = '\0';
char matrixKeypadIndexToCharArray[] = {
    '1', '2', '3', 'A',
    '4', '5', '6', 'B',
    '7', '8', '9', 'C',
    '*', '0', '#', 'D',
};

bool isAutoMode = true;

matrixKeypadState_t matrixKeypadState;
InputState_t currentInputState = GREEN;



// MATRIX BUTTON
void matrixKeypadInit();
char matrixKeypadScan();
char matrixKeypadUpdate();
void processKeypadInput();

// AUTO MODE
void auto_mode_init();
void auto_mode_update();

// MANUAL MODE
void manual_mode_init();
void manual_mode_update();

// BUTTON
void button_init();
void toggle_mode()
void button_change_mode();

int main()
{

    
}

// AUTO MODE FUNCTIONS
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

// MANUAL MODE FUNCTIONS
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

// BUTTON FUNCTIONS
void toggle_mode() {
    isAutoMode = !isAutoMode;
    if (isAutoMode) {
        led_auto = 1;
        led_manual = 0;
    } else {
        led_auto = 0;
        led_manual = 1;
    }
}

void button_init() {
    
}

void button_change_mode() {
    if(buttonChangeMode){
        toggle_mode();
        if (isAutoMode) {
            auto_mode_init();
        } else {
            manual_mode_init();
        }
    }
}

// MATRIX BUTTON FUNCTIONS
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
