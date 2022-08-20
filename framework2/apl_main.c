#include <stdio.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "apl_main.h"
#include "iod_main.h"

static struct sys_timer sts_timer_1000ms;
static bool bls_led_value;

void apl_init() {
    bls_led_value = IOD_LED1_VALUE_INIT;
    // 1000msタイマーの開始
    sys_call_timer_start(&sts_timer_1000ms);
}

void apl_main() {
    static uint8_t au8s_rx_message[IOD_UART_BUFF_SIZE];
    static uint8_t au8s_tx_message[IOD_UART_BUFF_SIZE];
    bool bla_in_btn_value;
    bool bla_out_led1_value;

    // 入力処理
    iod_read_btn_value(&bla_in_btn_value);

    // UART受信した場合、受信メッセージをUART送信
    if (iod_call_uart_receive(au8s_rx_message)) {
        snprintf(au8s_tx_message, sizeof(au8s_tx_message), "%s\r\n", au8s_rx_message);
        iod_call_uart_transmit(au8s_tx_message);
    }

    // 1000msタイマーが満了した場合
    if (sys_call_timer_check(&sts_timer_1000ms, 1000)) {
        // GPIO(GP7)の出力用の保持値を反転する
        bls_led_value = !bls_led_value;
        // 実行進捗メッセージをUART送信
        snprintf(au8s_tx_message, sizeof(au8s_tx_message), "1000ms Pass(%lld)\r\n", time_us_64());
        iod_call_uart_transmit(au8s_tx_message);
        // 1000msタイマーの再開
        sys_call_timer_start(&sts_timer_1000ms);
    }

    // GPIO(GP2)の入力値により、GPIO(GP7)の出力値を決定する
    if (bla_in_btn_value) {
        bla_out_led1_value = bls_led_value;
    } else {
        bla_out_led1_value = !bls_led_value;
    }

    // 出力処理
    iod_write_led0_value(!bla_out_led1_value);
    iod_write_led1_value(bla_out_led1_value);
}
