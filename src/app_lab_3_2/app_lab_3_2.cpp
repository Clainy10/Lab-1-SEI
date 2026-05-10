#include "app_lab_3_2/app_lab_3_2.h"
#include "serial_stdio/serial_stdio.h"
#include "Arduino_FreeRTOS.h"
#include "dd_NTC/dd_NTC.h"
#include "../XFunctions_ADC---DAC/Filters.h"

#define SENSOR_TASK_PERIOD_MS 250
#define SENSOR_TASK_OFFSET_MS 50
#define STATUS_TASK_PERIOD_MS 500

#define V_REF 5.0f
#define ADC_MAX 1023.0f
#define NTC_SERIES_RESISTOR 10000.0f
#define NTC_R25 10000.0f
#define NTC_BETA 3950.0f
#define TEMP_MIN_C 0.0f
#define TEMP_MAX_C 50.0f

static volatile uint16_t g_ntc_raw_value = 0;
static volatile float g_ntc_voltage = 0.0f;
static volatile float g_ntc_voltage_filtered = 0.0f;
static volatile float g_ntc_temperature_filtered = 0.0f;
static volatile uint32_t g_sample_count = 0;

static void ntc_task(void *pvParameters);
static void status_task(void *pvParameters);

void app_lab_3_2_setup()
{
    serial_stdio_setup();
    dd_ntc_setup();

    printf("Lab 3.2 - NTC signal conditioning\r\n");
    printf("Starting acquisition, filtering and conversion tasks...\r\n\r\n");

    xTaskCreate(ntc_task, "NTC Task", 256, NULL, 2, NULL);
    xTaskCreate(status_task, "Status Task", 256, NULL, 1, NULL);

    vTaskStartScheduler();
}

void app_lab_3_2_run()
{
}

void ntc_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    vTaskDelay(SENSOR_TASK_OFFSET_MS / portTICK_PERIOD_MS);

    while (1)
    {
        uint16_t rawValue = dd_ntc_read_raw();
        float voltage = filter_adcToVoltage((float)rawValue, V_REF, ADC_MAX);
        float filteredVoltage = filter_weightedAverageFilter(filter_saltAndPepperFilter(voltage), 0.2f);
        float temperature = filter_adc_VoltageToTemperature(filteredVoltage, V_REF, NTC_SERIES_RESISTOR, NTC_R25, NTC_BETA);
        temperature = filter_applySaturation(temperature, TEMP_MIN_C, TEMP_MAX_C);

        g_ntc_raw_value = rawValue;
        g_ntc_voltage = voltage;
        g_ntc_voltage_filtered = filteredVoltage;
        g_ntc_temperature_filtered = temperature;
        g_sample_count++;

        vTaskDelayUntil(&xLastWakeTime, SENSOR_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}

void status_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    while (1)
    {
        printf("=== Lab 3.2 status ===\r\n");
        printf("Samples acquired: %u\r\n", (unsigned int)g_sample_count);
        printf("ADC raw: %u\r\n", (unsigned int)g_ntc_raw_value);
        printf("Voltage: %.2f V (filtered %.2f V)\r\n", g_ntc_voltage, g_ntc_voltage_filtered);
        printf("Temperature: %.2f C\r\n", g_ntc_temperature_filtered);
        printf("Next update in %d ms\r\n\r\n", STATUS_TASK_PERIOD_MS);

        vTaskDelayUntil(&xLastWakeTime, STATUS_TASK_PERIOD_MS / portTICK_PERIOD_MS);
    }
}
