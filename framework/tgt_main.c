#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "sys_main.h"
#include "tgt_main.h"

#define UART0_ID        uart0
#define UART0_TX_GP0    (0)
#define UART0_RX_GP1    (1)
#define LED_GPIO_7      (7)
#define BTN_GPIO_2      (2)

static struct sys_time sts_timer_1000ms;
static bool bls_led_value;

static void tgt_init_port();
static void tgt_init_uart();
static void tgt_read_btn_value(bool *);
static void tgt_write_led_value(bool);

void tgt_init() {
    bls_led_value = false;
    // 標準出力の初期設定
    stdio_init_all();
    // ポートの初期設定
    tgt_init_port();
    // UARTの初期設定
    tgt_init_uart();
    // 1000msタイマーの開始
    sys_timer_start(&sts_timer_1000ms);
}

void tgt_timer_1ms() {
}

void tgt_main() {
    static uint8_t au8s_message[64];
    bool bla_in_btn_value;
    bool bla_out_led_value;

    // 入力処理
    tgt_read_btn_value(&bla_in_btn_value);

    // 1000msタイマーが満了した場合
    if(sys_timer_check(&sts_timer_1000ms, 1000)) {
        // GPIO(GP7)の出力用の保持値を反転する
        bls_led_value = !bls_led_value;
        // 標準出力に実行進捗を出力
        snprintf(au8s_message, sizeof(au8s_message), "sys_timer_check: 1000ms Pass(%lld)\r\n", time_us_64());
        uart_puts(UART0_ID, au8s_message);
        // 1000msタイマーの再開
        sys_timer_start(&sts_timer_1000ms);
    }

    // GPIO(GP2)の入力値により、GPIO(GP7)の出力値を決定する
    if(bla_in_btn_value) {
        bla_out_led_value = bls_led_value;
    } else {
        bla_out_led_value = !bls_led_value;
    }

    // 出力処理
    tgt_write_led_value(bla_out_led_value);
}

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

static void tgt_init_uart() {
    uart_init(UART0_ID, 115200);
    gpio_set_function(UART0_TX_GP0, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_GP1, GPIO_FUNC_UART);
}

static void tgt_read_btn_value(bool *pbla_btn_value) {
    static bool bls_btn_value_filter = true;
    static uint8_t u8s_count_filter = 0;
    bool bla_btn_value_temp;

    // GPIO(GP2)の入力
    bla_btn_value_temp = gpio_get(BTN_GPIO_2);
    // ポート値が変化している場合
    if(bla_btn_value_temp != bls_btn_value_filter) {
        u8s_count_filter++;
        // 4回連続でポート値の変化が継続している場合
        if(u8s_count_filter >= 4) {
            u8s_count_filter = 0;
            bls_btn_value_filter = bla_btn_value_temp;
        }
    // ポート値に変化がない場合
    } else {
        u8s_count_filter = 0;
    }
    // 戻り値の設定
    *pbla_btn_value = bls_btn_value_filter;
}

static void tgt_write_led_value(bool bla_led_value) {
    // GPIO(GP7)の出力
    gpio_put(LED_GPIO_7, bla_led_value);
}
