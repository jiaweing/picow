#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define TRIGGER_PIN 1
#define ECHO_PIN 0
#define MAX_DISTANCE 400 // Maximum distance in cm
#define TIMEOUT_US 23200 // Timeout in microseconds (based on MAX_DISTANCE)

volatile uint64_t start_time = 0;
volatile uint64_t end_time = 0;
volatile bool measurement_done = false;

void echo_isr(uint gpio, uint32_t events) {
    if (gpio_get(ECHO_PIN)) {
        start_time = time_us_64();
    } else {
        end_time = time_us_64();
        measurement_done = true;
    }
}

void init_ultrasonic_sensor() {
    gpio_init(TRIGGER_PIN);
    gpio_init(ECHO_PIN);
    gpio_set_dir(TRIGGER_PIN, GPIO_OUT);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    gpio_set_irq_enabled_with_callback(ECHO_PIN, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_isr);
}

float measure_distance() {
    measurement_done = false;
    
    // Trigger pulse
    gpio_put(TRIGGER_PIN, 1);
    sleep_us(10);
    gpio_put(TRIGGER_PIN, 0);

    // Wait for measurement to complete or timeout
    uint64_t timeout_start = time_us_64();
    while (!measurement_done) {
        if (time_us_64() - timeout_start > TIMEOUT_US) {
            return -1.0f; // Timeout occurred
        }
        tight_loop_contents();
    }

    // Calculate distance
    uint64_t duration = end_time - start_time;
    float distance = (duration * 0.0343) / 2.0;

    return (distance > MAX_DISTANCE) ? -1.0f : distance;
}

bool is_obstacle_detected(float safety_threshold) {
    float distance = measure_distance();
    return (distance > 0 && distance <= safety_threshold);
}