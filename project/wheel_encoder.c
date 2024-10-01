#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define ENCODER_PIN 2  // GPIO pin for the encoder
#define WHEEL_CIRCUMFERENCE 20.0f  // Wheel circumference in cm (adjust as needed)
#define PULSES_PER_REVOLUTION 20  // Number of pulses per wheel revolution (adjust based on your encoder)

volatile uint32_t pulse_count = 0;
volatile uint64_t last_pulse_time = 0;

void encoder_callback(uint gpio, uint32_t events) {
    pulse_count++;
    last_pulse_time = time_us_64();
}

void init_wheel_encoder() {
    gpio_init(ENCODER_PIN);
    gpio_set_dir(ENCODER_PIN, GPIO_IN);
    gpio_pull_up(ENCODER_PIN);
    
    gpio_set_irq_enabled_with_callback(ENCODER_PIN, GPIO_IRQ_EDGE_RISE, true, &encoder_callback);
}

float get_distance() {
    float revolutions = (float)pulse_count / PULSES_PER_REVOLUTION;
    return revolutions * WHEEL_CIRCUMFERENCE;
}

float get_speed() {
    static uint32_t last_count = 0;
    static uint64_t last_time = 0;
    
    uint64_t current_time = time_us_64();
    uint32_t current_count = pulse_count;
    
    float time_diff = (current_time - last_time) / 1000000.0f;  // Convert to seconds
    float count_diff = current_count - last_count;
    
    last_time = current_time;
    last_count = current_count;
    
    if (time_diff > 0) {
        float revolutions = count_diff / PULSES_PER_REVOLUTION;
        return (revolutions * WHEEL_CIRCUMFERENCE) / time_diff;  // Speed in cm/s
    }
    
    return 0.0f;
}

void reset_encoder() {
    pulse_count = 0;
    last_pulse_time = 0;
}