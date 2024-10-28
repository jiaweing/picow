#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define ENCODER_PIN 2  // GPIO pin for the encoder
#define WHEEL_CIRCUMFERENCE 20.0f  // Wheel circumference in cm
#define PULSES_PER_REVOLUTION 20  // Number of pulses per wheel revolution

// FreeRTOS handles
static QueueHandle_t encoder_queue;
static SemaphoreHandle_t data_mutex;

// Encoder data structure
typedef struct {
    uint32_t pulse_count;
    uint64_t timestamp;
} EncoderData;

// Global variables protected by mutex
static volatile EncoderData current_data = {0, 0};

void encoder_callback(uint gpio, uint32_t events) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (xSemaphoreTakeFromISR(data_mutex, &xHigherPriorityTaskWoken) == pdTRUE) {
        current_data.pulse_count++;
        current_data.timestamp = time_us_64();
        xSemaphoreGiveFromISR(data_mutex, &xHigherPriorityTaskWoken);
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void encoder_task(void *params) {
    EncoderData data;
    TickType_t last_wake_time = xTaskGetTickCount();
    
    while (1) {
        if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
            data = current_data;
            xSemaphoreGive(data_mutex);
            
            // Send data to queue
            xQueueSend(encoder_queue, &data, 0);
        }
        
        // Run every 100ms
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
    }
}

void init_wheel_encoder() {
    // Create FreeRTOS objects
    encoder_queue = xQueueCreate(10, sizeof(EncoderData));
    data_mutex = xSemaphoreCreateMutex();
    
    // Initialize GPIO
    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_PIN);
    
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);
    
    // Create encoder task
    xTaskCreate(encoder_task, "Encoder Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

float get_distance() {
    EncoderData data;
    float distance = 0.0f;
    
    if (xQueuePeek(encoder_queue, &data, 0) == pdTRUE) {
        float revolutions = (float)data.pulse_count / PULSES_PER_REVOLUTION;
        distance = revolutions * WHEEL_CIRCUMFERENCE;
    }
    
    return distance;
}

float get_speed() {
    static EncoderData last_data = {0, 0};
    EncoderData current;
    float speed = 0.0f;
    
    if (xQueuePeek(encoder_queue, &current, 0) == pdTRUE) {
        float time_diff = (current.timestamp - last_data.timestamp) / 1000000.0f;  // Convert to seconds
        float count_diff = current.pulse_count - last_data.pulse_count;
        
        if (time_diff > 0) {
            float revolutions = count_diff / PULSES_PER_REVOLUTION;
            speed = (revolutions * WHEEL_CIRCUMFERENCE) / time_diff;  // Speed in cm/s
        }
        
        last_data = current;
    }
    
    return speed;
}

void reset_encoder() {
    if (xSemaphoreTake(data_mutex, portMAX_DELAY) == pdTRUE) {
        current_data.pulse_count = 0;
        current_data.timestamp = 0;
        xSemaphoreGive(data_mutex);
    }
}
