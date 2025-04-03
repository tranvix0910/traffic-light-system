//
//                       _oo0oo_
//                      o8888888o
//                      88" . "88
//                      (| -_- |)
//                      0\  =  /0
//                    ___/`---'\___
//                  .' \\|     |// '.
//                 / \\|||  :  |||// \
//                / _||||| -:- |||||- \
//               |   | \\\  -  /// |   |
//               | \_|  ''\---/''  |_/ |
//               \  .-\__  '-'  ___/-. /
//             ___'. .'  /--.--\  `. .'___
//          ."" '<  `.___\_<|>_/___.' >' "".
//         | | :  `- \`.;`\ _ /`;.`/ - ` : | |
//         \  \ `_.   \_ __\ /__ _/   .-` /  /
//     =====`-.____`.___ \_____/___.-`___.-'=====
//                       `=---='
//
//
//     ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"
#include "matrix_keypad.h"

#define GREEN_LIGHT_TIME     7000    // 7s
#define YELLOW_BLINK_TIME    500     // 500ms blink interval
#define YELLOW_BLINK_DURATION 8000   // 8s
#define RED_LIGHT_TIME       10000   // 10s
#define PRE_RED_YELLOW_DURATION 2000 // 2s yellow blink before red

#define TIME_INCREMENT_MS   10
#define DEBOUNCE_BUTTON_TIME_MS 40

typedef enum {
    GREEN,
    YELLOW_BLINK,
    RED
} TrafficLightState;

// Trạng thái của đèn trong chế độ manual
typedef enum {
    MANUAL_OFF,           // Đèn tắt
    MANUAL_RED,           // Đèn đỏ bật
    MANUAL_YELLOW,        // Đèn vàng bật thường
    MANUAL_YELLOW_BLINK,  // Đèn vàng nhấp nháy
    MANUAL_PRE_RED,       // Đèn vàng nhấp nháy trước khi chuyển đỏ
    MANUAL_GREEN          // Đèn xanh bật
} ManualLightState;

// Định nghĩa trạng thái nhập thời gian
typedef enum {
    INPUT_NONE,          // Chưa nhập
    INPUT_RED_TIME,      // Đang nhập thời gian đèn đỏ
    INPUT_YELLOW_TIME,   // Đang nhập thời gian đèn vàng
    INPUT_GREEN_TIME,    // Đang nhập thời gian đèn xanh
    INPUT_COMPLETE       // Đã nhập xong
} TimeInputState;

DigitalOut greenLed(LED1);
DigitalOut yellowLed(LED2);
DigitalOut redLed(LED3);

DigitalIn changeStateButton(D10);
DigitalIn redButton(D11);
DigitalIn yellowButton(D12);
DigitalIn greenButton(D13);

UnbufferedSerial uartUsb(USBTX, USBRX, 115200);

uint32_t previousMillis = 0;
uint32_t stateStartMillis = 0;
TrafficLightState state = GREEN;
bool yellowBlinkState = false;

bool manualMode = false;

bool lastRedButtonState = false;
bool lastYellowButtonState = false;
bool lastGreenButtonState = false;

// Thay thế biến trạng thái đèn bằng một biến enum duy nhất
ManualLightState manualLightState = MANUAL_OFF;

// Biến quản lý thời gian
uint32_t manualYellowPreviousMillis = 0;
bool manualYellowBlinkState = false;
uint32_t preRedStartTime = 0; // Thời điểm bắt đầu nhấp nháy vàng trước khi đỏ

// Biến lưu thời gian cho từng đèn (đơn vị: giây)
int redLightDuration = 10;    // Mặc định 10 giây
int yellowLightDuration = 3;  // Mặc định 3 giây
int greenLightDuration = 7;   // Mặc định 7 giây

// Biến cho việc nhập thời gian
TimeInputState inputState = INPUT_NONE;
char timeInputBuffer[3] = {0}; // Buffer để lưu 2 chữ số và null terminator
int timeInputIndex = 0;
uint32_t manualTimerStart = 0;  // Thời điểm bắt đầu đếm ngược

void turnOffAllLights() {
    greenLed = OFF;
    yellowLed = OFF;
    redLed = OFF;
}

void inputsInit()
{
    changeStateButton.mode(PullDown);
    redButton.mode(PullDown);
    yellowButton.mode(PullDown);
    greenButton.mode(PullDown);
}

void outputsInit()
{
    greenLed = OFF;
    yellowLed = OFF;
    redLed = OFF;
}

void userInterfaceDisplayInit()
{
    displayInit();

    displayCharPositionWrite ( 0, 0 );
    displayStringWrite( "Mode: " );

    displayCharPositionWrite ( 0, 1 );
    displayStringWrite( "State: " );
}

int main()
{
    inputsInit();
    outputsInit();
    turnOffAllLights();
    matrixKeypadInit(10);
    userInterfaceDisplayInit();
    
    uint32_t currentMillis = 0;
    bool lastButtonState = false;
    
    while (true) {
        currentMillis = us_ticker_read() / 1000;
        
        // Xử lý input từ keypad
        char key = matrixKeypadUpdate();
        if (key != '\0') {
            // Xử lý phím số và các phím đặc biệt
            if (manualMode) {
                switch(key) {
                    case 'A':
                        // Bắt đầu nhập thời gian cho đèn đỏ
                        inputState = INPUT_NONE;
                        timeInputIndex = 0;
                        timeInputBuffer[0] = timeInputBuffer[1] = '\0';
                        uartUsb.write("Enter red light time (2 digits):\r\n", 33);
                        break;
                    case 'B':
                        // Bắt đầu nhập thời gian cho đèn vàng
                        inputState = INPUT_RED_TIME;
                        timeInputIndex = 0;
                        timeInputBuffer[0] = timeInputBuffer[1] = '\0';
                        uartUsb.write("Enter yellow light time (2 digits):\r\n", 35);
                        break;
                    case 'C':
                        // Bắt đầu nhập thời gian cho đèn xanh
                        inputState = INPUT_YELLOW_TIME;
                        timeInputIndex = 0;
                        timeInputBuffer[0] = timeInputBuffer[1] = '\0';
                        uartUsb.write("Enter green light time (2 digits):\r\n", 34);
                        break;
                    default:
                        // Xử lý nhập số và phím #
                        if (inputState != INPUT_COMPLETE) {
                            if (key >= '0' && key <= '9' && timeInputIndex < 2) {
                                timeInputBuffer[timeInputIndex++] = key;
                                char buffer[30];
                                sprintf(buffer, "Entered: %c\r\n", key);
                                uartUsb.write(buffer, strlen(buffer));
                            }
                            else if (key == '#' && timeInputIndex > 0) {
                                // Chuyển buffer thành số và lưu vào biến tương ứng
                                timeInputBuffer[timeInputIndex] = '\0';
                                int time = atoi(timeInputBuffer);
                                
                                switch(inputState) {
                                    case INPUT_NONE:
                                        redLightDuration = time;
                                        inputState = INPUT_COMPLETE;
                                        char buffer[50];
                                        sprintf(buffer, "Red light time set to %d seconds\r\n", time);
                                        uartUsb.write(buffer, strlen(buffer));
                                        break;
                                        
                                    case INPUT_RED_TIME:
                                        yellowLightDuration = time;
                                        inputState = INPUT_COMPLETE;
                                        char buffer2[50];
                                        sprintf(buffer2, "Yellow light time set to %d seconds\r\n", time);
                                        uartUsb.write(buffer2, strlen(buffer2));
                                        break;
                                        
                                    case INPUT_YELLOW_TIME:
                                        greenLightDuration = time;
                                        inputState = INPUT_COMPLETE;
                                        char buffer3[50];
                                        sprintf(buffer3, "Green light time set to %d seconds\r\n", time);
                                        uartUsb.write(buffer3, strlen(buffer3));
                                        break;
                                        
                                    default:
                                        break;
                                }
                                
                                // Reset buffer cho lần nhập tiếp theo
                                timeInputIndex = 0;
                                timeInputBuffer[0] = timeInputBuffer[1] = '\0';
                            }
                        }
                        break;
                }
            }
        }
        
        // Kiểm tra nút nhấn để chuyển chế độ
        bool buttonState = changeStateButton;
        if (buttonState && !lastButtonState) {
            manualMode = !manualMode;
            if (manualMode) {
                turnOffAllLights();
                manualLightState = MANUAL_OFF;
                inputState = INPUT_COMPLETE;  // Đặt là COMPLETE để có thể bật tắt đèn ngay
                uartUsb.write("Entering manual mode\r\n", 21);
                uartUsb.write("Press A/B/C to set light times, or use buttons to control lights\r\n", 63);
            } else {
                inputState = INPUT_COMPLETE;
                state = GREEN;
                stateStartMillis = currentMillis;
            }
        }
        lastButtonState = buttonState;
        if (buttonState && !lastButtonState) { // Phát hiện nút được nhấn
            manualMode = !manualMode;
            
            // Khi chuyển sang chế độ manual, tắt tất cả đèn và reset trạng thái
            if (manualMode) {
                turnOffAllLights();
                manualLightState = MANUAL_OFF;
                manualYellowBlinkState = false;
            }
        }
        lastButtonState = buttonState;
        
        if (manualMode) {
            // Kiểm tra nút đỏ
            bool currentRedButtonState = redButton;
            if (currentRedButtonState && !lastRedButtonState) {
                // Nếu trạng thái hiện tại là đèn đỏ, tắt đèn
                if (manualLightState == MANUAL_RED) {
                    manualLightState = MANUAL_OFF;
                } else {
                    // Bắt đầu nhấp nháy đèn vàng trước khi chuyển sang đèn đỏ
                    manualLightState = MANUAL_PRE_RED;
                    manualYellowPreviousMillis = currentMillis;
                    manualYellowBlinkState = true;
                    preRedStartTime = currentMillis; // Lưu thời điểm bắt đầu
                }
            }
            lastRedButtonState = currentRedButtonState;
            
            // Kiểm tra nút vàng
            bool currentYellowButtonState = yellowButton;
            if (currentYellowButtonState && !lastYellowButtonState) {
                // Nếu trạng thái hiện tại là đèn vàng, tắt đèn
                if (manualLightState == MANUAL_YELLOW || manualLightState == MANUAL_YELLOW_BLINK) {
                    manualLightState = MANUAL_OFF;
                } else {
                    // Bật đèn vàng nhấp nháy
                    manualLightState = MANUAL_YELLOW_BLINK;
                    manualYellowPreviousMillis = currentMillis;
                    manualYellowBlinkState = true;
                }
            }
            lastYellowButtonState = currentYellowButtonState;
            
            // Kiểm tra nút xanh
            bool currentGreenButtonState = greenButton;
            if (currentGreenButtonState && !lastGreenButtonState) {
                // Nếu trạng thái hiện tại là đèn xanh, tắt đèn
                if (manualLightState == MANUAL_GREEN) {
                    manualLightState = MANUAL_OFF;
                } else {
                    // Bật đèn xanh
                    manualLightState = MANUAL_GREEN;
                }
            }
            lastGreenButtonState = currentGreenButtonState;
            
            // Xử lý các trạng thái đèn
            switch (manualLightState) {
                case MANUAL_OFF:
                    turnOffAllLights();
                    break;
                    
                case MANUAL_RED:
                    redLed = ON;
                    yellowLed = OFF;
                    greenLed = OFF;
                    break;
                    
                case MANUAL_YELLOW:
                    redLed = OFF;
                    yellowLed = ON;
                    greenLed = OFF;
                    break;
                    
                case MANUAL_YELLOW_BLINK:
                    // Nhấp nháy đèn vàng
                    redLed = OFF;
                    greenLed = OFF;
                    
                    if (currentMillis - manualYellowPreviousMillis >= YELLOW_BLINK_TIME) {
                        manualYellowPreviousMillis = currentMillis;
                        manualYellowBlinkState = !manualYellowBlinkState;
                    }
                    yellowLed = manualYellowBlinkState;
                    break;
                    
                case MANUAL_PRE_RED:
                    // Nhấp nháy đèn vàng trước khi chuyển sang đỏ
                    redLed = OFF;
                    greenLed = OFF;
                    
                    if (currentMillis - manualYellowPreviousMillis >= YELLOW_BLINK_TIME) {
                        manualYellowPreviousMillis = currentMillis;
                        manualYellowBlinkState = !manualYellowBlinkState;
                    }
                    yellowLed = manualYellowBlinkState;
                    
                    // Kiểm tra nếu đã đủ 2 giây, chuyển sang đèn đỏ
                    if (currentMillis - preRedStartTime >= PRE_RED_YELLOW_DURATION) {
                        manualLightState = MANUAL_RED;
                    }
                    break;
                    
                case MANUAL_GREEN:
                    redLed = OFF;
                    yellowLed = OFF;
                    greenLed = ON;
                    break;
            }
            
        } else {
            // Chế độ tự động
            switch (state) {
                case GREEN:
                    greenLed = ON;
                    if (currentMillis - stateStartMillis >= GREEN_LIGHT_TIME) {
                        greenLed = OFF;
                        state = YELLOW_BLINK;
                        stateStartMillis = currentMillis;
                    }
                    break;
                
                case YELLOW_BLINK:
                    if (currentMillis - previousMillis >= YELLOW_BLINK_TIME) {
                        previousMillis = currentMillis;
                        yellowBlinkState = !yellowBlinkState;
                        yellowLed = yellowBlinkState;
                    }
                    if (currentMillis - stateStartMillis >= YELLOW_BLINK_DURATION) {
                        yellowLed = OFF;
                        state = RED;
                        stateStartMillis = currentMillis;
                    }
                    break;
                
                case RED:
                    redLed = ON;
                    if (currentMillis - stateStartMillis >= RED_LIGHT_TIME) {
                        redLed = OFF;
                        state = GREEN;
                        stateStartMillis = currentMillis;
                    }
                    break;
            }
        }
        
        ThisThread::sleep_for(10ms);
    }
}