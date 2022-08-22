#include <stdio.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "apl_main.h"
#include "iod_main.h"

enum apl_blink_state {
    BLINK_500MS = 0,
    BLINK_1000MS,
    BLINK_2000MS,
    BLINK_STATE_NUM
};

static const uint16_t acu16s_wait_time[BLINK_STATE_NUM] ={
    500, 1000, 2000
};
static struct sys_timer asts_blink_timer[BLINK_STATE_NUM];
static bool abls_led_value[BLINK_STATE_NUM];
static enum apl_blink_state u8s_blink_state;

static bool apl_blink_sate_update(uint8_t);
static bool apl_pwm_sate_update(uint8_t);

void apl_init() {
    uint8_t u8a_index;

    for (u8a_index = 0; u8a_index < BLINK_STATE_NUM; u8a_index++) {
        abls_led_value[u8a_index] = IOD_LED1_VALUE_INIT;
        // 点滅タイマーの開始
        sys_call_timer_start(&asts_blink_timer[u8a_index]);
    }
    u8s_blink_state = BLINK_1000MS;
}

void apl_main() {
    static uint8_t au8s_rx_message[IOD_UART_BUFF_SIZE];
    static uint8_t au8s_tx_message[IOD_UART_BUFF_SIZE];
    bool bla_in_btn_value;
    bool bla_out_led1_value;
    uint16_t u16a_adc_value;
    uint8_t u8a_index;

    // 入力処理
    iod_read_btn_value(&bla_in_btn_value);
    iod_read_adc_value(&u16a_adc_value);

    // UART受信した場合
    if (iod_call_uart_receive(au8s_rx_message)) {
        // 点滅状態を変更
        bool bla_blink_update = apl_blink_sate_update(au8s_rx_message[0]);
        bool bla_pwm_update = apl_pwm_sate_update(au8s_rx_message[0]);
        // 受信メッセージをUART送信
        if (bla_blink_update || bla_pwm_update) {
            snprintf(au8s_tx_message, sizeof(au8s_tx_message), "request='%c'\r\n", au8s_rx_message[0]);
        } else {
            snprintf(au8s_tx_message, sizeof(au8s_tx_message), "request error '%c'\r\n", au8s_rx_message[0]);
        }
        iod_call_uart_transmit(au8s_tx_message);
    }

    // LED出力用の保持値を更新
    for (u8a_index = 0; u8a_index < BLINK_STATE_NUM; u8a_index++) {
        // 点滅タイマーが満了した場合
        if (sys_call_timer_check(&asts_blink_timer[u8a_index], acu16s_wait_time[u8a_index])) {
            // LED出力用の保持値を反転する
            abls_led_value[u8a_index] = !abls_led_value[u8a_index];
            // 点滅タイマーの再開
            sys_call_timer_start(&asts_blink_timer[u8a_index]);
            // 1000ms毎に実行進捗メッセージをUART送信
            if (u8a_index == BLINK_1000MS) {
                //snprintf(au8s_tx_message, sizeof(au8s_tx_message), "1000ms Pass(%lld)\r\n", time_us_64());
                snprintf(au8s_tx_message, sizeof(au8s_tx_message), "adc value = 0x%04x\r\n", u16a_adc_value);
                iod_call_uart_transmit(au8s_tx_message);
            }
        }
    }

    // ボタンの入力値により、LED1の出力値を決定する
    if (bla_in_btn_value) {
        bla_out_led1_value = abls_led_value[u8s_blink_state];
    } else {
        bla_out_led1_value = !abls_led_value[u8s_blink_state];
    }

    // 出力処理
    iod_write_led0_value(abls_led_value[BLINK_1000MS]);
    iod_write_led1_value(bla_out_led1_value);
}

static bool apl_blink_sate_update(uint8_t u8a_request) {
    bool bla_rcode = false;

    switch (u8a_request) {
        case '0':
            u8s_blink_state = BLINK_500MS;
            bla_rcode = true;
            break;
        case '1':
            u8s_blink_state = BLINK_1000MS;
            bla_rcode = true;
            break;
        case '2':
            u8s_blink_state = BLINK_2000MS;
            bla_rcode = true;
            break;
    }

    return bla_rcode;
}

static bool apl_pwm_sate_update(uint8_t u8a_request) {
    bool bla_rcode = false;

    switch (u8a_request) {
        case 'q':
            iod_call_pwm1_set_duty(0);
            bla_rcode = true;
            break;
        case 'w':
            iod_call_pwm1_set_duty(IOD_PWM1_DUTY_MAX * 1 / 4);
            bla_rcode = true;
            break;
        case 'e':
            iod_call_pwm1_set_duty(IOD_PWM1_DUTY_MAX * 2 / 4);
            bla_rcode = true;
            break;
        case 'r':
            iod_call_pwm1_set_duty(IOD_PWM1_DUTY_MAX * 3 / 4);
            bla_rcode = true;
            break;
        case 't':
            iod_call_pwm1_set_duty(IOD_PWM1_DUTY_MAX);
            bla_rcode = true;
            break;
        case 'a':
            iod_call_pwm2_set_duty(0);
            bla_rcode = true;
            break;
        case 's':
            iod_call_pwm2_set_duty(IOD_PWM2_DUTY_MAX * 1 / 4);
            bla_rcode = true;
            break;
        case 'd':
            iod_call_pwm2_set_duty(IOD_PWM2_DUTY_MAX * 2 / 4);
            bla_rcode = true;
            break;
        case 'f':
            iod_call_pwm2_set_duty(IOD_PWM2_DUTY_MAX * 3 / 4);
            bla_rcode = true;
            break;
        case 'g':
            iod_call_pwm2_set_duty(IOD_PWM2_DUTY_MAX);
            bla_rcode = true;
            break;
    }

    return bla_rcode;
}
