#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "iod_main.h"
#include "apl_main.h"

enum btn_intr_group {
    BTN1_INTR = 0,
    BTN2_INTR,
    BTN_INTR_GROUP_NUM
};

typedef void (* fp_btn_intr_enable)(bool);

static const fp_btn_intr_enable afps_btn_intr[BTN_INTR_GROUP_NUM] = {
    iod_call_btn1_intr_enabled,
    iod_call_btn2_intr_enabled
};

uint8_t au8g_tx_message[IOD_UART_BUFF_SIZE];
struct flash_data sts_flash_data;

static struct sys_timer sts_monitor_timer;

static void process_request();
static void process_request_set(enum btn_intr_group, enum request_event);
static void process_init();
static void process_deinit();
static void process_reinit();
static void process_main();
static void flash_init();
static void flash_deinit();
static void monitor_init();
static void monitor_main();

// 外部公開関数
void apl_init() {
    memset(au8g_tx_message, 0, sizeof(au8g_tx_message));
    memset(&sts_flash_data, 0, sizeof(sts_flash_data));
    flash_init();

    // APLモジュールの外部公開関数をコール
    apl_request_init();
    apl_blink_init();
    apl_pwm_init();

    process_init();
    monitor_init();
}

void apl_deinit() {
    process_deinit();

    // APLモジュールの外部公開関数をコール
    apl_pwm_deinit();
    apl_blink_deinit();
    apl_request_deinit();

    flash_deinit();
}

void apl_reinit() {
    memset(au8g_tx_message, 0, sizeof(au8g_tx_message));
    memset(&sts_flash_data, 0, sizeof(sts_flash_data));
    flash_init();

    // APLモジュールの外部公開関数をコール
    apl_request_reinit();
    apl_blink_reinit();
    apl_pwm_reinit();

    process_reinit();
    monitor_init();
}

void apl_main() {
    // APLモジュールの外部公開関数をコール
    apl_request_main();
    apl_blink_main();
    apl_pwm_main();

    process_main();
    monitor_main();
}

void apl_intr_btn1_down() {
    iod_call_uart_transmit("interrupt btn1\r\n");
}
void apl_intr_btn2_down() {
    iod_call_uart_transmit("interrupt btn2\r\n");
}

// 内部関数
static void process_request() {
    enum request_event u8a_event;
    enum request_event u8a_event_base;

    apl_read_request_event(&u8a_event);
    switch (u8a_event) {
        case EVENT_BTN1_INTR_DISENABLE:
        case EVENT_BTN1_INTR_ENABLE:
            process_request_set(BTN1_INTR, u8a_event);
            break;
        case EVENT_BTN2_INTR_DISENABLE:
        case EVENT_BTN2_INTR_ENABLE:
            u8a_event_base = EVENT_BTN1_INTR_DISENABLE + (u8a_event - EVENT_BTN2_INTR_DISENABLE);
            process_request_set(BTN2_INTR, u8a_event_base);
            break;
        case EVENT_SLEEP:
            sys_call_sleep_request();
            break;
        case EVENT_WDOG_RESET:
            // ウォッチドッグのリセットを誘発
            while (true) {
                tight_loop_contents();
            }
            break;
        case EVENT_MCORE_STOP:
            iod_call_mcore_stop();
            break;
        case EVENT_MCORE_START:
            iod_call_mcore_start();
            break;
    }
}

static void process_request_set(enum btn_intr_group u8a_btn_intr_id, enum request_event u8a_event) {
    bool bla_enabled;

    switch (u8a_event) {
        case EVENT_BTN1_INTR_DISENABLE:
        case EVENT_BTN1_INTR_ENABLE:
            bla_enabled = (bool)(u8a_event - EVENT_BTN1_INTR_DISENABLE);
            afps_btn_intr[u8a_btn_intr_id](bla_enabled);
            break;
    }
}

static void process_init() {
    // 起動要因を確認
    if (iod_call_wdog_isboot()) {
        iod_call_uart_transmit("reboot by watchdog\r\n");
    } else {
        iod_call_uart_transmit("framework2\r\n");
    }
    iod_call_mcore_start();
}

static void process_deinit() {
    iod_call_mcore_stop();
}

static void process_reinit() {
    iod_call_uart_transmit("wakeup\r\n");
    iod_call_mcore_start();
}

static void process_main() {
    // 要求イベントの処理
    process_request();
}

static void flash_init() {
    if (iod_call_flash_read((uint8_t *)&sts_flash_data, sizeof(sts_flash_data))) {
        snprintf(au8g_tx_message, sizeof(au8g_tx_message), "flash data = %d\r\n", sts_flash_data.u32_count);
        iod_call_uart_transmit(au8g_tx_message);
        sts_flash_data.u32_count++;
    }
}

static void flash_deinit() {
    iod_call_flash_write((uint8_t *)&sts_flash_data, sizeof(sts_flash_data));
}

static void monitor_init() {
    // 監視タイマーの開始
    sys_call_timer_start(&sts_monitor_timer);
}

static void monitor_main() {
    uint16_t u16a_in_adc_value;

    // 入力処理
    iod_read_adc_value(&u16a_in_adc_value);

    // 監視タイマーが満了した場合
    if (sys_call_timer_check(&sts_monitor_timer, 2000)) {
        //snprintf(au8g_tx_message, sizeof(au8g_tx_message), "2000ms Pass(%lld)\r\n", time_us_64());
        snprintf(au8g_tx_message, sizeof(au8g_tx_message), "adc value = 0x%04x\r\n", u16a_in_adc_value);
        iod_call_uart_transmit(au8g_tx_message);
        // 監視タイマーの再開
        sys_call_timer_start(&sts_monitor_timer);
    }
}
