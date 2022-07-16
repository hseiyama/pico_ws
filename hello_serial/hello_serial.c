/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"

#define BUTTON_GPIO (2)
#define LED_GPIO  PICO_DEFAULT_LED_PIN

int main() {
    unsigned char count = 0;
    bool event;
    char *message = NULL;
    stdio_init_all();
    /* ボタン入力用のポート設定 */
    gpio_init(BUTTON_GPIO);
    gpio_set_dir(BUTTON_GPIO, GPIO_IN);
    gpio_pull_up(BUTTON_GPIO);
    //gpio_put(BUTTON_GPIO, true); /* 明示的な出力設定は不要だった */
    /* LED出力用のポート設定 */
    gpio_init(LED_GPIO);
    gpio_set_dir(LED_GPIO, GPIO_OUT);

    while (true) {
        count++;
        /* ボタン入力値を判定 */
        event = gpio_get(BUTTON_GPIO);
        if (!event) {
            message = " + push";
            count = 0;
            gpio_put(LED_GPIO, true);
        } else {
            message = "";
            gpio_put(LED_GPIO, false);
        }

        printf("Hello, world! (serial-%d)%s\n", count, message);
        sleep_ms(1000);
    }
    return 0;
}
