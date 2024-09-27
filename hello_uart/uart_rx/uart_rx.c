#include "pico/stdlib.h"
#include "hardware/uart.h"
#include <stdio.h>

#define BUTTON_PIN 22
#define TX_PIN 8
#define RX_PIN 9
#define UART_ID uart1
#define BAUD_RATE 115200

void uart_send_char(char c) {
    uart_putc(UART_ID, c);
}

char uart_receive_char() {
    if (uart_is_readable(UART_ID)) {
        return uart_getc(UART_ID);
    }
    return 0;
}

void uart_print_char(char c) {
    printf("%c", c);
}

int main() {
    stdio_init_all();

    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    uart_init(UART_ID, BAUD_RATE);

    gpio_set_function(TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(RX_PIN, GPIO_FUNC_UART);

    char current_char = 'A';
    bool button_pressed = false;

    while (true) {
        // Check if button is pressed
        button_pressed = !gpio_get(BUTTON_PIN);

        if (button_pressed) {
            // Transmit alphabets A-Z sequentially when button is pressed
            uart_send_char(current_char);
            current_char++;
            if (current_char > 'Z') {
                current_char = 'A';  // Loop back to 'A'
            }
        } else {
            // Transmit the numeric value '1' every second when button is not pressed
            uart_send_char('1');
        }

        // Receive and process data from UART
        char received_char = uart_receive_char();
        if (received_char != 0) {
            if (received_char >= 'A' && received_char <= 'Z') {
                // Convert uppercase to lowercase and print
                uart_print_char(received_char + 32);
            } else if (received_char == '1') {
                // If '1' is received, print '2'
                uart_print_char('2');
            }
        }

        sleep_ms(1000); // 1-second delay between transmissions
    }

    return 0;
}
