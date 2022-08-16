#include <stdio.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "tgt_main.h"

#define LED_GPIO_7 (7)
#define BTN_GPIO_2 (2)

static struct sys_time sts_timer_1000ms;
static bool bls_led_value;

static void tgt_init_port();

static void tgt_init_port() {
    // GPIO(GP2)の初期設定（ポート入力：プルアップ）
    gpio_init(BTN_GPIO_2);
    gpio_set_dir(BTN_GPIO_2, GPIO_IN);
    gpio_pull_up(BTN_GPIO_2);
    // GPIO(GP7)の初期設定（ポート出力）
    gpio_init(LED_GPIO_7);
    gpio_set_dir(LED_GPIO_7, GPIO_OUT);
    gpio_put(LED_GPIO_7, bls_led_value);
}

void tgt_init() {
    bls_led_value = false;
    // 標準出力の初期設定
    stdio_init_all();
    // ポートの初期設定
    tgt_init_port();
    // 1000msタイマーの開始
    sys_timer_start(&sts_timer_1000ms);
}

void tgt_timer_1ms() {
}

void tgt_main() {
    bool bla_in_btn_value;
    bool bla_out_led_value;

    // GPIO(GP2)の入力
    bla_in_btn_value = gpio_get(BTN_GPIO_2);

    // 1000msタイマーが満了した場合
    if(sys_timer_check(&sts_timer_1000ms, 1000)) {
        // GPIO(GP7)の出力用の保持値を反転する
        bls_led_value = !bls_led_value;
        // 標準出力に実行進捗を出力
        printf("sys_timer_check: 1000ms Pass(%lld)\n", time_us_64());
        // 1000msタイマーの再開
        sys_timer_start(&sts_timer_1000ms);
    }

    // GPIO(GP2)の入力値により、GPIO(GP7)の出力値を決定する
    if(bla_in_btn_value) {
        bla_out_led_value = bls_led_value;
    } else {
        bla_out_led_value = !bls_led_value;
    }

    // GPIO(GP7)の出力
    gpio_put(LED_GPIO_7, bla_out_led_value);
}
