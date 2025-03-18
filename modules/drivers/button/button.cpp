#include "button.h"
#include "auto_mode.h"
#include "manual_mode.h"

DigitalIn buttonChangeMode(BUTTON1);   
volatile bool isAutoMode = true;

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