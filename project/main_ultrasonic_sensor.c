#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Function prototypes from ultrasonic_sensor.c
void init_ultrasonic_sensor();
float measure_distance();
bool is_obstacle_detected(float safety_threshold);

#define SAFETY_THRESHOLD 20.0f // 20 cm safety threshold

int main() {
    stdio_init_all();

    printf("Ultrasonic Sensor Demo\n");

    // Initialize the ultrasonic sensor
    init_ultrasonic_sensor();

    // Main loop
    while (1) {
        // Measure distance
        float distance = measure_distance();

        if (distance < 0) {
            printf("Error: Measurement timeout or out of range\n");
        } else {
            printf("Distance: %.2f cm\n", distance);

            if (is_obstacle_detected(SAFETY_THRESHOLD)) {
                printf("WARNING: Obstacle detected within safety threshold!\n");
            }
        }

        // Wait for a short period before the next measurement
        sleep_ms(500);
    }

    return 0;
}