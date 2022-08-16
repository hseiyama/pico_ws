#include <stdio.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "tgt_main.h"

#define LED_GPIO_7 (7)

static struct sys_time sts_timer_1000ms;
static bool bls_gpio7_value;

void tgt_init() {
    // 標準出力の初期設定
    stdio_init_all();
    // GPIO(GP7)の初期設定
    gpio_init(LED_GPIO_7);
    gpio_set_dir(LED_GPIO_7, GPIO_OUT);
    bls_gpio7_value = false;
    gpio_put(LED_GPIO_7, bls_gpio7_value);
    // 1000msタイマーの開始
    sys_timer_start(&sts_timer_1000ms);
}

void tgt_timer_1ms() {
}

void tgt_main() {
    // 1000msタイマーの確認
    if(sys_timer_check(&sts_timer_1000ms, 1000)) {
        // GPIO(GP7)の出力設定
        bls_gpio7_value = !bls_gpio7_value;
        gpio_put(LED_GPIO_7, bls_gpio7_value);
        // 標準出力に実行進捗を出力
        printf("sys_timer_check: 1000ms Pass(%lld)\n", time_us_64());
        // 1000msタイマーの再開
        sys_timer_start(&sts_timer_1000ms);
    }
}
