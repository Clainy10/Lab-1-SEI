#include "app_lab_2_1_task_2.h"
#include "dd_led/dd_led.h"
#include "dd_button/dd_button.h"

#define LED_GREEN 0
#define LED_RED 1

void app_lab_2_1_task_2_setup() {
    dd_led_setup();
}


void app_lab_2_1_task_2_run() {
    if(dd_led_read_state(LED_GREEN) == false)
    {
        dd_led_on(LED_RED);
    }
    else
    {
        dd_led_off(LED_RED);
    }
}