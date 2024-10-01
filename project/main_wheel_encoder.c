#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

// Function prototypes from wheel_encoder.c
void init_wheel_encoder();
float get_distance();
float get_speed();
void reset_encoder();

int main() {
    stdio_init_all();

    printf("Wheel Encoder Demo\n");

    // Initialize the wheel encoder
    init_wheel_encoder();

    // Main loop
    while (1) {
        // Get current distance and speed
        float distance = get_distance();
        float speed = get_speed();

        // Print the results
        printf("Distance: %.2f cm, Speed: %.2f cm/s\n", distance, speed);

        // Wait for a short period before the next measurement
        sleep_ms(500);
    }

    return 0;
}