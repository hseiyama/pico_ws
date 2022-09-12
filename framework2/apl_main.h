enum request_event {
    EVENT_NONE = 0,
    EVENT_BLINK_500MS,
    EVENT_BLINK_1000MS,
    EVENT_BLINK_2000MS,
    EVENT_PWM0_LEVEL0,
    EVENT_PWM0_LEVEL1,
    EVENT_PWM0_LEVEL2,
    EVENT_PWM0_LEVEL3,
    EVENT_PWM0_LEVEL4,
    EVENT_PWM0_ADC_VALUE,
    EVENT_PWM1_LEVEL0,
    EVENT_PWM1_LEVEL1,
    EVENT_PWM1_LEVEL2,
    EVENT_PWM1_LEVEL3,
    EVENT_PWM1_LEVEL4,
    EVENT_PWM1_ADC_VALUE,
    EVENT_BTN1_INTR_DISENABLE,
    EVENT_BTN1_INTR_ENABLE,
    EVENT_BTN2_INTR_DISENABLE,
    EVENT_BTN2_INTR_ENABLE,
    EVENT_SLEEP,
    EVENT_WDOG_RESET,
    EVENT_MCORE_STOP,
    EVENT_MCORE_START,
    EVENT_NUM
};

// 構造体のサイズは最大値（IOD_FLASH_DATA_SIZE）を超えないこと
struct flash_data {
    uint32_t u32_count;
};

extern uint8_t au8g_tx_message[IOD_UART_BUFF_SIZE];
extern struct flash_data sts_flash_data;

// apl_main
extern void apl_init();
extern void apl_deinit();
extern void apl_reinit();
extern void apl_main();
extern void apl_intr_btn1_down();
extern void apl_intr_btn2_down();
// apl_request
extern void apl_request_init();
extern void apl_request_deinit();
extern void apl_request_reinit();
extern void apl_request_main();
extern void apl_read_request_event(enum request_event *);
// apl_blink
extern void apl_blink_init();
extern void apl_blink_deinit();
extern void apl_blink_reinit();
extern void apl_blink_main();
// apl_pwm
extern void apl_pwm_init();
extern void apl_pwm_deinit();
extern void apl_pwm_reinit();
extern void apl_pwm_main();
// apl_core1
extern void apl_core1_task_init();
extern void apl_core1_task_main();
