#include "app_lab_2_2.h"
#include "dd_led/dd_led.h"
#include "dd_button/dd_button.h"
#include "serial_stdio/serial_stdio.h"
#include "app_lab_2_2/MyTasks.h"

#include "Arduino_FreeRTOS.h"


#define TASK_1_RECURRENCE 10 / portTICK_PERIOD_MS
#define TASK_1_OFFSET 100 / portTICK_PERIOD_MS

#define TASK_2_RECURRENCE 10 / portTICK_PERIOD_MS
#define TASK_2_OFFSET 105 / portTICK_PERIOD_MS

#define TASK_3_RECURRENCE 10 / portTICK_PERIOD_MS
#define TASK_3_OFFSET 110 / portTICK_PERIOD_MS










void app_lab_2_2_setup() 
{ 
    serial_stdio_setup();
    dd_led_setup();
    dd_button_init();

    task_1_setup_1();
    task_2_setup_1();
    // task_3_setup_1();

    xTaskCreate(task_1, "Task 1", 1000, NULL, 1, NULL);
    xTaskCreate(task_2, "Task 2", 1000, NULL, 1, NULL);
    xTaskCreate(task_3, "Task 3", 1000, NULL, 1, NULL);

    vTaskStartScheduler();
    printf("System initialized. Starting scheduler...\r\n");
}

void app_lab_2_2_run()
{
    
}                  