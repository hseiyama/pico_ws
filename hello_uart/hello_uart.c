/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */


#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"

/// \tag::hello_uart[]

#define UART_ID uart0
#define BAUD_RATE 115200

// We are using pins 0 and 1, but see the GPIO function select table in the
// datasheet for information on which other pins can be used.
#define UART_TX_PIN 0
#define UART_RX_PIN 1

#define LED_GPIO  (7)

static u_int64_t u16_time;
static u_int64_t u16_diff;

int main() {
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);
    gpio_put(LED_GPIO, false);

    // Set up our UART with the required speed.
    uart_init(UART_ID, BAUD_RATE);

    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART

    // Send out a character without any conversions
    u16_time = time_us_64();
    gpio_put(LED_GPIO, true);
    uart_putc_raw(UART_ID, 'A');
    gpio_put(LED_GPIO, false);
    u16_diff = time_us_64() - u16_time;

    // Send out a character but do CR/LF conversions
    //uart_set_translate_crlf(UART_ID, true);
    u16_time = time_us_64();
    gpio_put(LED_GPIO, true);
    uart_putc(UART_ID, 'B');
    gpio_put(LED_GPIO, false);
    u16_diff = time_us_64() - u16_time;
    //uart_putc(UART_ID, '\r');
    //uart_putc(UART_ID, '\n');

    // Send out a string, with CR/LF conversions
    u16_time = time_us_64();
    gpio_put(LED_GPIO, true);
    uart_puts(UART_ID, " Hello, UART!\r\n");
    gpio_put(LED_GPIO, false);
    u16_diff = time_us_64() - u16_time;

    char ch = uart_getc(UART_ID);
    uart_puts(UART_ID, "ch1=");
    ch = uart_getc(UART_ID);
    uart_putc_raw(UART_ID, ch);
    uart_puts(UART_ID, "\r\nch2=");
    ch = uart_getc(UART_ID);
    uart_putc_raw(UART_ID, ch);
}

/// \end::hello_uart[]
