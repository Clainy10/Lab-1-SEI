#include "app_lab_2_1_task_1.h"
#include "dd_led/dd_led.h"
#include "dd_button/dd_button.h"

#define LED_GREEN 0

void app_lab_2_1_task_1_setup() {
    dd_led_setup();
    dd_button_init();
}


void app_lab_2_1_task_1_run() {
    if (dd_button_read()) {
        dd_led_on(LED_GREEN);
    } else {
        dd_led_off(LED_GREEN);
    }
}