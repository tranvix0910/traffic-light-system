#ifndef TRAFFIC_LIGHT_H
#define TRAFFIC_LIGHT_H

#include "mbed.h"

// Thời gian các trạng thái đèn giao thông
#define YELLOW_BLINK_TIME    500     // 500ms blink interval
#define YELLOW_BLINK_DURATION 8000   // 8s
#define PRE_RED_YELLOW_DURATION 2000 // 2s yellow blink before red

#define TIME_INCREMENT_MS   10
#define DEBOUNCE_BUTTON_TIME_MS 40

// Các định nghĩa trạng thái của đèn giao thông
typedef enum {
    GREEN,
    YELLOW_BLINK,
    RED
} TrafficLightState;

// Khai báo các biến thời gian cho đèn (ms)
extern int redLightDuration;
extern int yellowLightDuration; 
extern int greenLightDuration;

// Khởi tạo các input và output
void inputsInit();
void outputsInit();

// Khởi tạo giao diện hiển thị
void userInterfaceDisplayInit();

// Tắt tất cả các đèn
void turnOffAllLights();

// Cập nhật trạng thái đèn trong chế độ tự động
void updateAutomaticMode(uint32_t currentMillis, TrafficLightState &state, 
                      uint32_t &stateStartMillis, uint32_t &previousMillis, 
                      bool &yellowBlinkState);

#endif // TRAFFIC_LIGHT_H 