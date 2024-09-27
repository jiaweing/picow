/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include <stdio.h> // Include for printf

int main() {
#ifndef PICO_DEFAULT_LED_PIN
#warning blink example requires a board with a regular LED
#else
    uint a = 1;
    const uint LED_PIN = PICO_DEFAULT_LED_PIN;
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    
    // Initialize stdio for printf
    stdio_init_all();

    while (true) {
        gpio_put(LED_PIN, 1);
        sleep_ms(a << 1); // Blink on
        gpio_put(LED_PIN, 0);
        sleep_ms(a << 1); // Blink off

        // Print the current value of 'a'
        printf("Current delay: %u ms\n", a << 1);

        // Check if 'a' has reached 2048
        if (a == 2048) {
            a = 1; // Reset to 1
        } else {
            a <<= 1; // Double the delay
        }
    }
#endif
}