#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"

#define BUTTON_PIN 21

volatile bool running = false;
volatile uint32_t elapsed_time = 0;
absolute_time_t last_press_time;

bool repeating_timer_callback(struct repeating_timer *t) {
    if (running) {
        elapsed_time++;
        printf("Elapsed Time: %d seconds\n", elapsed_time);
    }
    return true;
}

void button_callback(uint gpio, uint32_t events) {
    absolute_time_t now = get_absolute_time();
    if (absolute_time_diff_us(last_press_time, now) < 200000) {
        // Debounce: Ignore if less than 200ms since last press
        return;
    }
    last_press_time = now;

    if (events & GPIO_IRQ_EDGE_FALL) {
        // Button pressed
        running = true;
        elapsed_time = 0;
    } else if (events & GPIO_IRQ_EDGE_RISE) {
        // Button released
        running = false;
    }
}

int main() {
    stdio_init_all();
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);
    gpio_set_irq_enabled_with_callback(BUTTON_PIN, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &button_callback);

    struct repeating_timer timer;
    add_repeating_timer_ms(1000, repeating_timer_callback, NULL, &timer);

    while (true) {
        tight_loop_contents();
    }
    return 0;
}