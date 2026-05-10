#include "app_lab_3_1/app_lab_3_1.h"
#include "dd_NTC/dd_NTC.h"
#include "serial_stdio/serial_stdio.h"
#include "Arduino_FreeRTOS.h"

#define SENSOR_TASK_PERIOD_MS 250
#define SENSOR_TASK_OFFSET_MS 50
#define STATUS_TASK_PERIOD_MS 500

static volatile uint16_t g_ntc_raw_value = 0;
static volatile float g_ntc_voltage = 0.0f;
static volatile float g_ntc_temperature = 0.0f;
static volatile uint32_t g_sample_count = 0;

static void ntc_task(void *pvParameters);
static void IDLE_status_task(void *pvParameters);

void app_lab_3_1_setup(void)
{
    serial_stdio_setup();
    dd_ntc_setup();

    printf("Lab 3.1 - NTC signal acquisition\r\n");
    printf("Starting sensor acquisition and status tasks...\r\n\r\n");

    xTaskCreate(ntc_task, "NTC Task", 256, NULL, 2, NULL);
    xTaskCreate(IDLE_status_task, "Status Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
}

void app_lab_3_1_run(void)
{

}

void ntc_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    vTaskDelay(SENSOR_TASK_OFFSET_MS / portTICK_PERIOD_MS);

    while (1)
    {
        g_ntc_raw_value = dd_ntc_read_raw();
        g_ntc_voltage = dd_ntc_read_voltage();
        g_ntc_temperature = dd_ntc_read_temperature_celsius();
        g_sample_count++;

        vTaskDelayUntil(&xLastWakeTime, SENSOR_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

void IDLE_status_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        printf("=== Lab 3.1 status ===\r\n");
        printf("Samples acquired: %u\r\n", (unsigned int)g_sample_count);
        printf("ADC raw: %u\r\n", (unsigned int)g_ntc_raw_value);
        printf("Voltage: %.2f V\r\n", g_ntc_voltage);
        printf("Temperature: %.2f C\r\n", g_ntc_temperature);
        printf("Next update in %d ms\r\n\r\n", STATUS_TASK_PERIOD_MS);

        vTaskDelayUntil(&xLastWakeTime, STATUS_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}
