#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define TRIGGER_PIN 1
#define ECHO_PIN 0
#define MAX_DISTANCE 400 // Maximum distance in cm
#define TIMEOUT_US 23200 // Timeout in microseconds (based on MAX_DISTANCE)

// FreeRTOS handles
static QueueHandle_t distance_queue;
static SemaphoreHandle_t measurement_mutex;

// Measurement data structure
typedef struct {
    uint64_t start_time;
    uint64_t end_time;
    bool measurement_done;
} MeasurementData;

static volatile MeasurementData current_measurement = {0, 0, false};

void echo_isr(uint gpio, uint32_t events) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if (xSemaphoreTakeFromISR(measurement_mutex, &xHigherPriorityTaskWoken) == pdTRUE) {
        if (gpio_get(ECHO_PIN)) {
            current_measurement.start_time = time_us_64();
        } else {
            current_measurement.end_time = time_us_64();
            current_measurement.measurement_done = true;
        }
        xSemaphoreGiveFromISR(measurement_mutex, &xHigherPriorityTaskWoken);
    }
    
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void ultrasonic_task(void *params) {
    TickType_t last_wake_time = xTaskGetTickCount();
    float distance;
    
    while (1) {
        // Reset measurement flag
        if (xSemaphoreTake(measurement_mutex, portMAX_DELAY) == pdTRUE) {
            current_measurement.measurement_done = false;
            xSemaphoreGive(measurement_mutex);
        }
        
        // Trigger pulse
        gpio_put(TRIGGER_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(1));  // 10 microseconds delay
        gpio_put(TRIGGER_PIN, 0);
        
        // Wait for measurement or timeout
        TickType_t start_tick = xTaskGetTickCount();
        bool timeout = false;
        bool measurement_complete = false;
        
        while (!measurement_complete && !timeout) {
            if (xSemaphoreTake(measurement_mutex, pdMS_TO_TICKS(10)) == pdTRUE) {
                measurement_complete = current_measurement.measurement_done;
                xSemaphoreGive(measurement_mutex);
            }
            
            if ((xTaskGetTickCount() - start_tick) > pdMS_TO_TICKS(50)) {  // 50ms timeout
                timeout = true;
            }
            
            vTaskDelay(1);  // Small delay to prevent hogging CPU
        }
        
        // Calculate and send distance
        if (measurement_complete) {
            if (xSemaphoreTake(measurement_mutex, portMAX_DELAY) == pdTRUE) {
                uint64_t duration = current_measurement.end_time - current_measurement.start_time;
                distance = (duration * 0.0343) / 2.0;
                
                if (distance > MAX_DISTANCE) {
                    distance = -1.0f;
                }
                
                xSemaphoreGive(measurement_mutex);
            }
        } else {
            distance = -1.0f;  // Timeout or error
        }
        
        xQueueSend(distance_queue, &distance, 0);
        
        // Run every 100ms
        vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(100));
    }
}

void init_ultrasonic_sensor() {
    // Create FreeRTOS objects
    distance_queue = xQueueCreate(10, sizeof(float));
    measurement_mutex = xSemaphoreCreateMutex();
    
    // Initialize GPIO
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);
    
    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_isr);
    
    // Create ultrasonic task
    xTaskCreate(ultrasonic_task, "Ultrasonic Task", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
}

float measure_distance() {
    float distance = -1.0f;
    if (xQueuePeek(distance_queue, &distance, 0) != pdTRUE) {
        return -1.0f;
    }
    return distance;
}

bool is_obstacle_detected(float safety_threshold) {
    float distance = measure_distance();
    return (distance > 0 && distance <= safety_threshold);
}
