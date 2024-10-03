#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TEMP_TASK_PRIORITY      ( tskIDLE_PRIORITY + 1UL )
#define MOVING_AVG_TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL )
#define SIMPLE_AVG_TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL )
#define PRINT_TASK_PRIORITY     ( tskIDLE_PRIORITY + 1UL )

#define BUFFER_SIZE 10

// Queues for inter-task communication
static QueueHandle_t xTempQueue;
static QueueHandle_t xMovingAvgQueue;
static QueueHandle_t xSimpleAvgQueue;
static QueueHandle_t xPrintQueue;

// Function to read temperature from RP2040's built-in temperature sensor
float read_temperature() {
    adc_select_input(4);
    uint16_t raw = adc_read();
    float voltage = raw * 3.3f / (1 << 12);
    float temperature = 27.0f - (voltage - 0.706f) / 0.001721f;
    return temperature;
}

// Task to read temperature and send to other tasks
void vTempTask(void *pvParameters) {
    TickType_t xLastWakeTime;
    const TickType_t xFrequency = pdMS_TO_TICKS(1000); // 1 second

    xLastWakeTime = xTaskGetTickCount();

    for (;;) {
        float temp = read_temperature();
        xQueueSend(xTempQueue, &temp, 0);
        xQueueSend(xTempQueue, &temp, 0);  // Send to both tasks
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

// Task to calculate moving average
void vMovingAvgTask(void *pvParameters) {
    float buffer[BUFFER_SIZE] = {0};
    int index = 0;
    float sum = 0;

    for (;;) {
        float temp;
        if (xQueueReceive(xTempQueue, &temp, portMAX_DELAY) == pdPASS) {
            sum -= buffer[index];
            sum += temp;
            buffer[index] = temp;
            index = (index + 1) % BUFFER_SIZE;

            float movingAvg = sum / BUFFER_SIZE;
            xQueueSend(xMovingAvgQueue, &movingAvg, 0);
        }
    }
}

// Task to calculate simple average
void vSimpleAvgTask(void *pvParameters) {
    float sum = 0;
    int count = 0;

    for (;;) {
        float temp;
        if (xQueueReceive(xTempQueue, &temp, portMAX_DELAY) == pdPASS) {
            sum += temp;
            count++;

            float simpleAvg = sum / count;
            xQueueSend(xSimpleAvgQueue, &simpleAvg, 0);
        }
    }
}

// Task to handle all printf statements
void vPrintTask(void *pvParameters) {
    for (;;) {
        float movingAvg, simpleAvg;
        if (xQueueReceive(xMovingAvgQueue, &movingAvg, portMAX_DELAY) == pdPASS &&
            xQueueReceive(xSimpleAvgQueue, &simpleAvg, portMAX_DELAY) == pdPASS) {
            printf("Moving Average: %.2f°C, Simple Average: %.2f°C\n", movingAvg, simpleAvg);
        }
    }
}

int main() {
    // Initialize stdio
    stdio_init_all();

    // Initialize ADC for temperature sensor
    adc_init();
    adc_set_temp_sensor_enabled(true);

    // Create queues
    xTempQueue = xQueueCreate(2, sizeof(float));  // Increased to 2 for two receiving tasks
    xMovingAvgQueue = xQueueCreate(1, sizeof(float));
    xSimpleAvgQueue = xQueueCreate(1, sizeof(float));
    xPrintQueue = xQueueCreate(1, sizeof(char) * 100);

    // Create tasks
    xTaskCreate(vTempTask, "Temperature", configMINIMAL_STACK_SIZE, NULL, TEMP_TASK_PRIORITY, NULL);
    xTaskCreate(vMovingAvgTask, "Moving Average", configMINIMAL_STACK_SIZE, NULL, MOVING_AVG_TASK_PRIORITY, NULL);
    xTaskCreate(vSimpleAvgTask, "Simple Average", configMINIMAL_STACK_SIZE, NULL, SIMPLE_AVG_TASK_PRIORITY, NULL);
    xTaskCreate(vPrintTask, "Print", configMINIMAL_STACK_SIZE, NULL, PRINT_TASK_PRIORITY, NULL);

    // Start the scheduler
    vTaskStartScheduler();

    // We should never get here
    for (;;) {
    }
}