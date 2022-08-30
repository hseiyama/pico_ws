#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "iod_main.h"
#include "apl_main.h"

enum request_state {
    REQUEST_NONE = 0,
    REQUEST_BLINK,
    REQUEST_PWM0,
    REQUEST_PWM1,
    REQUEST_BTN1_INTR,
    REQUEST_BTN2_INTR,
    REQUEST_MCORE,
    REQUEST_WDOG,
    REQUEST_STATE_NUM
};

enum blink_state {
    BLINK_500MS = 0,
    BLINK_1000MS,
    BLINK_2000MS,
    BLINK_STATE_NUM
};

enum pwm_group {
    PWM0 = 0,
    PWM1,
    PWM_GROUP_NUM
};

enum btn_intr_group {
    BTN1_INTR = 0,
    BTN2_INTR,
    BTN_INTR_GROUP_NUM
};

typedef void (* fp_pwm_duty_set)(uint16_t);
typedef void (* fp_btn_intr_enable)(bool);

struct pwm_request {
    uint16_t u16_duty;
    bool bl_state;
};

struct pwm_duty {
    fp_pwm_duty_set fp_set;
    uint16_t u16_max;
};

static const uint16_t acu16s_blink_time[BLINK_STATE_NUM] = {
    500, 1000, 2000
};

static const struct pwm_duty acsts_pwm_duty[PWM_GROUP_NUM] = {
    {iod_call_pwm0_set_duty, IOD_PWM0_DUTY_MAX},
    {iod_call_pwm1_set_duty, IOD_PWM1_DUTY_MAX}
};

static const fp_btn_intr_enable afps_btn_intr[BTN_INTR_GROUP_NUM] = {
    iod_call_btn1_intr_enabled,
    iod_call_btn2_intr_enabled
};

static enum request_state u8s_request_sate;
static enum blink_state u8s_blink_state;
static struct sys_timer asts_blink_timer[BLINK_STATE_NUM];
static bool abls_blink_value[BLINK_STATE_NUM];
struct pwm_request asts_pwm_request[PWM_GROUP_NUM];
static uint8_t au8s_rx_message[IOD_UART_BUFF_SIZE];
static uint8_t au8s_tx_message[IOD_UART_BUFF_SIZE];
static struct sys_timer sts_monitor_timer;
// core1用
static uint8_t u8s_core1_count;

static void request_init();
static void blink_init();
static void pwm_init();
static void request_input();
static bool request_sate(uint8_t);
static bool request_sate_none(uint8_t);
static bool request_sate_blink(uint8_t);
static bool request_sate_pwm(uint8_t, enum pwm_group);
static bool request_sate_btn_intr(uint8_t, enum btn_intr_group);
static bool request_sate_mcore(uint8_t);
static bool request_sate_wdog(uint8_t);
static bool blink_update(bool);
static void pwm_update(uint16_t);

// 外部公開関数
void apl_init() {
    request_init();
    blink_init();
    pwm_init();
    memset(au8s_rx_message, 0, sizeof(au8s_rx_message));
    memset(au8s_tx_message, 0, sizeof(au8s_tx_message));
    // 監視タイマーの開始
    sys_call_timer_start(&sts_monitor_timer);
    // 起動要因を確認
    if (iod_call_wdog_isboot()) {
        iod_call_uart_transmit("reboot by watchdog\r\n");
    } else {
        iod_call_uart_transmit("framework2\r\n");
    }
    iod_call_mcore_start();
}

void apl_main() {
    bool bla_in_btn0_value;
    uint16_t u16a_in_adc_value;
    bool bla_out_led1_value;
    uint8_t u8a_index;

    // 入力処理
    iod_read_btn0_value(&bla_in_btn0_value);
    iod_read_adc_value(&u16a_in_adc_value);

    // UART要求の入力
    request_input();
    // LED点滅関連の更新
    bla_out_led1_value = blink_update(bla_in_btn0_value);
    // PWM関連の更新
    pwm_update(u16a_in_adc_value);

    // 監視タイマーが満了した場合
    if (sys_call_timer_check(&sts_monitor_timer, 2000)) {
        //snprintf(au8s_tx_message, sizeof(au8s_tx_message), "2000ms Pass(%lld)\r\n", time_us_64());
        snprintf(au8s_tx_message, sizeof(au8s_tx_message), "adc value = 0x%04x\r\n", u16a_in_adc_value);
        iod_call_uart_transmit(au8s_tx_message);
        // 監視タイマーの再開
        sys_call_timer_start(&sts_monitor_timer);
    }

    // 出力処理
    iod_write_led0_value(abls_blink_value[BLINK_1000MS]);
    iod_write_led1_value(bla_out_led1_value);
}

void apl_intr_btn1_down() {
    iod_call_uart_transmit("interrupt btn1\r\n");
}
void apl_intr_btn2_down() {
    iod_call_uart_transmit("interrupt btn2\r\n");
}

// core1用
void apl_core1_task_init() {
    u8s_core1_count = 0;
}

void apl_core1_task_main() {
    sleep_ms(1000);
    snprintf(au8s_tx_message, sizeof(au8s_tx_message), "core1 task(%d)\r\n", u8s_core1_count);
    u8s_core1_count++;
    iod_call_uart_transmit(au8s_tx_message);
}

// 内部関数
static void request_init() {
    u8s_request_sate = REQUEST_NONE;
}

static void blink_init() {
    uint8_t u8a_index;

    u8s_blink_state = BLINK_1000MS;
    for (u8a_index = 0; u8a_index < BLINK_STATE_NUM; u8a_index++) {
        abls_blink_value[u8a_index] = IOD_LED0_VALUE_INIT;
        // 点滅タイマーの開始
        sys_call_timer_start(&asts_blink_timer[u8a_index]);
    }
}

static void pwm_init() {
    uint8_t u8a_index;

    for (u8a_index = 0; u8a_index < PWM_GROUP_NUM; u8a_index++) {
        asts_pwm_request[u8a_index].bl_state = true;
        asts_pwm_request[u8a_index].u16_duty = acsts_pwm_duty[u8a_index].u16_max;
    }
}

static void request_input() {
    // UART受信した場合
    if (iod_call_uart_receive(au8s_rx_message)) {
        // 要求状態を更新
        bool bla_request_update = request_sate(au8s_rx_message[0]);
        // 受信メッセージをUART送信
        if (bla_request_update) {
            snprintf(au8s_tx_message, sizeof(au8s_tx_message), "request='%c'\r\n", au8s_rx_message[0]);
        } else {
            snprintf(au8s_tx_message, sizeof(au8s_tx_message), "request error '%c'\r\n", au8s_rx_message[0]);
        }
        iod_call_uart_transmit(au8s_tx_message);
    }
}

static bool request_sate(uint8_t u8a_request) {
    bool bla_rcode = false;
    enum pwm_group u8a_pwm_id; 
    enum btn_intr_group u8a_btn_intr_id; 

    switch (u8s_request_sate) {
        case REQUEST_NONE:
            bla_rcode = request_sate_none(u8a_request);
            break;
        case REQUEST_BLINK:
            bla_rcode = request_sate_blink(u8a_request);
            break;
        case REQUEST_PWM0:
        case REQUEST_PWM1:
            u8a_pwm_id = (enum pwm_group)(u8s_request_sate - REQUEST_PWM0);
            bla_rcode = request_sate_pwm(u8a_request, u8a_pwm_id);
            break;
        case REQUEST_BTN1_INTR:
        case REQUEST_BTN2_INTR:
            u8a_btn_intr_id = (enum btn_intr_group)(u8s_request_sate - REQUEST_BTN1_INTR);
            bla_rcode = request_sate_btn_intr(u8a_request, u8a_btn_intr_id);
            break;
        case REQUEST_MCORE:
            bla_rcode = request_sate_mcore(u8a_request);
            break;
        case REQUEST_WDOG:
            bla_rcode = request_sate_wdog(u8a_request);
            break;
    }

    return bla_rcode;
}

static bool request_sate_none(uint8_t u8a_request) {
    bool bla_rcode = false;

    switch (u8a_request) {
        case 'a':
            u8s_request_sate = REQUEST_BLINK;
            bla_rcode = true;
            break;
        case 'b':
            u8s_request_sate = REQUEST_PWM0;
            bla_rcode = true;
            break;
        case 'c':
            u8s_request_sate = REQUEST_PWM1;
            bla_rcode = true;
            break;
        case 'd':
            u8s_request_sate = REQUEST_BTN1_INTR;
            bla_rcode = true;
            break;
        case 'e':
            u8s_request_sate = REQUEST_BTN2_INTR;
            bla_rcode = true;
            break;
        case 'x':
            u8s_request_sate = REQUEST_MCORE;
            bla_rcode = true;
            break;
        case 'z':
            u8s_request_sate = REQUEST_WDOG;
            bla_rcode = true;
            break;
    }

    return bla_rcode;
}

static bool request_sate_blink(uint8_t u8a_request) {
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
    u8s_request_sate = REQUEST_NONE;

    return bla_rcode;
}

static bool request_sate_pwm(uint8_t u8a_request, enum pwm_group u8a_pwm_id) {
    bool bla_rcode = false;
    uint16_t u16a_level;

    switch (u8a_request) {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
            u16a_level = u8a_request - '0';
            asts_pwm_request[u8a_pwm_id].u16_duty = acsts_pwm_duty[u8a_pwm_id].u16_max * u16a_level / 4;
            asts_pwm_request[u8a_pwm_id].bl_state = true;
            bla_rcode = true;
            break;
        case '5':
            asts_pwm_request[u8a_pwm_id].bl_state = false;
            bla_rcode = true;
            break;
    }
    u8s_request_sate = REQUEST_NONE;

    return bla_rcode;
}

static bool request_sate_btn_intr(uint8_t u8a_request, enum btn_intr_group u8a_btn_intr_id) {
    bool bla_rcode = false;
    bool bla_enabled;

    switch (u8a_request) {
        case '0':
        case '1':
            bla_enabled = (bool)(u8a_request - '0');
            afps_btn_intr[u8a_btn_intr_id](bla_enabled);
            bla_rcode = true;
            break;
    }
    u8s_request_sate = REQUEST_NONE;

    return bla_rcode;
}

static bool request_sate_mcore(uint8_t u8a_request) {
    bool bla_rcode = false;

    switch (u8a_request) {
        case '0':
            iod_call_mcore_stpo();
            bla_rcode = true;
            break;
        case '1':
            iod_call_mcore_start();
            bla_rcode = true;
            break;
    }
    u8s_request_sate = REQUEST_NONE;

    return bla_rcode;
}

static bool request_sate_wdog(uint8_t u8a_request) {
    bool bla_rcode = false;

    switch (u8a_request) {
        case '0':
            // 要求をキャンセル
            bla_rcode = true;
            break;
        case '1':
            // 無限ループでリセットを誘発
            while (true) {
                tight_loop_contents();
            }
            bla_rcode = true;
            break;
    }
    u8s_request_sate = REQUEST_NONE;

    return bla_rcode;
}

static bool blink_update(bool bla_btn_value) {
    bool bla_led_value;
    uint8_t u8a_index;

    // LED出力用の保持値を更新
    for (u8a_index = 0; u8a_index < BLINK_STATE_NUM; u8a_index++) {
        // 点滅タイマーが満了した場合
        if (sys_call_timer_check(&asts_blink_timer[u8a_index], acu16s_blink_time[u8a_index])) {
            // LED出力用の保持値を反転する
            abls_blink_value[u8a_index] = !abls_blink_value[u8a_index];
            // 点滅タイマーの再開
            sys_call_timer_start(&asts_blink_timer[u8a_index]);
        }
    }

    // ボタンの入力値により、LED1の出力値を決定する
    if (bla_btn_value) {
        bla_led_value = abls_blink_value[u8s_blink_state];
    } else {
        bla_led_value = !abls_blink_value[u8s_blink_state];
    }

    return bla_led_value;
}

static void pwm_update(uint16_t u16a_adc_value) {
    uint16_t u16a_pwm_duty;
    uint8_t u8a_index;

    // 要求により、PWMのduty値を決定する
    for (u8a_index = 0; u8a_index < PWM_GROUP_NUM; u8a_index++) {
        if (asts_pwm_request[u8a_index].bl_state){
            u16a_pwm_duty = asts_pwm_request[u8a_index].u16_duty;
        } else {
            u16a_pwm_duty = acsts_pwm_duty[u8a_index].u16_max * u16a_adc_value / IOD_ADC_VALUE_MAX;
        }
        acsts_pwm_duty[u8a_index].fp_set(u16a_pwm_duty);
    }
}
