#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/irq.h"

#define TIMEOUT 1000000 // Define a suitable timeout value

volatile uint64_t start_time = 0;
volatile uint64_t end_time = 0;
volatile bool measurement_done = false;

void echo_irq_handler(uint gpio, uint32_t events) {
    if (events & GPIO_IRQ_EDGE_RISE) {
        start_time = time_us_64();
        printf("Rising edge detected\n");
    } else if (events & GPIO_IRQ_EDGE_FALL) {
        end_time = time_us_64();
        measurement_done = true;
        printf("Falling edge detected\n");
    }
}

void setupUltrasonicPins(uint trigPin, uint echoPin) {
    gpio_init(trigPin);
    gpio_init(echoPin);
    gpio_set_dir(trigPin, GPIO_OUT);
    gpio_set_dir(echoPin, GPIO_IN);
    gpio_set_irq_enabled_with_callback(echoPin, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true, &echo_irq_handler);
    printf("Pins initialized\n");
}

void triggerUltrasonic(uint trigPin) {
    gpio_put(trigPin, 1);
    sleep_us(10);
    gpio_put(trigPin, 0);
    printf("Triggered ultrasonic sensor\n");
}

uint64_t getPulseWidth() {
    if (measurement_done) {
        measurement_done = false;
        return end_time - start_time;
    }
    return 0;
}

int main() {
    stdio_init_all();
    uint trigPin = 1;
    uint echoPin = 0;

    setupUltrasonicPins(trigPin, echoPin);

    while (1) {
        triggerUltrasonic(trigPin);
        sleep_ms(60); // Wait for the echo to be received

        uint64_t pulseWidth = getPulseWidth();
        if (pulseWidth > 0) {
            printf("Pulse width: %llu us\n", pulseWidth);
        } else {
            printf("Timeout or no echo received\n");
        }

        sleep_ms(1000);
    }

    return 0;
}