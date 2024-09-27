#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"
#include "hardware/timer.h"

#define PWM_PIN 0
#define ADC_PIN 26
#define PWM_FREQ 20
#define SAMPLE_INTERVAL_MS 25

volatile bool sample_flag = false;

void timer_callback(void) {
    sample_flag = true;
}

int main() {
    stdio_init_all();

    // Configure PWM
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    uint slice_num = pwm_gpio_to_slice_num(PWM_PIN);
    uint chan = pwm_gpio_to_channel(PWM_PIN);

    uint32_t clock = 125000000;
    uint32_t divider = 125;
    uint32_t wrap = clock / (divider * PWM_FREQ) - 1;

    pwm_set_clkdiv(slice_num, divider);
    pwm_set_wrap(slice_num, wrap);
    pwm_set_chan_level(slice_num, chan, wrap / 2);  // 50% duty cycle
    pwm_set_enabled(slice_num, true);

    // Configure ADC
    adc_init();
    adc_gpio_init(ADC_PIN);
    adc_select_input(0);  // ADC0 is GPIO26

    // Configure timer interrupt
    struct repeating_timer timer;
    add_repeating_timer_ms(SAMPLE_INTERVAL_MS, timer_callback, NULL, &timer);

    while (1) {
        if (sample_flag) {
            uint32_t adc_value = adc_read();
            printf("%02d:%02d:%02d:%03d -> ADC Value: %d\n", 
                   to_ms_since_boot(get_absolute_time()) / 3600000 % 24,
                   to_ms_since_boot(get_absolute_time()) / 60000 % 60,
                   to_ms_since_boot(get_absolute_time()) / 1000 % 60,
                   to_ms_since_boot(get_absolute_time()) % 1000,
                   adc_value);
            sample_flag = false;
        }
    }

    return 0;
}