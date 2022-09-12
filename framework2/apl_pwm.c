#include "pico/stdlib.h"
#include "iod_main.h"
#include "apl_main.h"

enum pwm_group {
    PWM0 = 0,
    PWM1,
    PWM_GROUP_NUM
};

typedef void (* fp_pwm_duty_set)(uint16_t);

struct pwm_request {
    uint16_t u16_duty;
    bool bl_state;
};

struct pwm_duty {
    fp_pwm_duty_set fp_set;
    uint16_t u16_max;
};

static const struct pwm_duty acsts_pwm_duty[PWM_GROUP_NUM] = {
    {iod_call_pwm0_set_duty, IOD_PWM0_DUTY_MAX},
    {iod_call_pwm1_set_duty, IOD_PWM1_DUTY_MAX}
};

struct pwm_request asts_pwm_request[PWM_GROUP_NUM];

static void pwm_request();
static void pwm_update(uint16_t);

// 外部公開関数
void apl_pwm_init() {
    uint8_t u8a_index;

    for (u8a_index = 0; u8a_index < PWM_GROUP_NUM; u8a_index++) {
        asts_pwm_request[u8a_index].bl_state = true;
        asts_pwm_request[u8a_index].u16_duty = acsts_pwm_duty[u8a_index].u16_max;
    }
}

void apl_pwm_deinit() {
}

void apl_pwm_reinit() {
}

void apl_pwm_main() {
    uint16_t u16a_in_adc_value;

    // 入力処理
    iod_read_adc_value(&u16a_in_adc_value);

    // 要求イベントの処理
    pwm_request();
    // PWM関連の更新
    pwm_update(u16a_in_adc_value);
}

// 内部関数
static void pwm_request() {
    enum request_event u8a_event;
    enum pwm_group u8a_pwm_id;
    uint16_t u16a_level;

    apl_read_request_event(&u8a_event);
    switch (u8a_event) {
        case EVENT_PWM0_LEVEL0:
        case EVENT_PWM0_LEVEL1:
        case EVENT_PWM0_LEVEL2:
        case EVENT_PWM0_LEVEL3:
        case EVENT_PWM0_LEVEL4:
            u8a_pwm_id = PWM0;
            u16a_level = (uint16_t)(u8a_event - EVENT_PWM0_LEVEL0);
            asts_pwm_request[u8a_pwm_id].u16_duty = acsts_pwm_duty[u8a_pwm_id].u16_max * u16a_level / 4;
            asts_pwm_request[u8a_pwm_id].bl_state = true;
            break;
        case EVENT_PWM0_ADC_VALUE:
            u8a_pwm_id = PWM0;
            asts_pwm_request[u8a_pwm_id].bl_state = false;
            break;
        case EVENT_PWM1_LEVEL0:
        case EVENT_PWM1_LEVEL1:
        case EVENT_PWM1_LEVEL2:
        case EVENT_PWM1_LEVEL3:
        case EVENT_PWM1_LEVEL4:
            u8a_pwm_id = PWM1;
            u16a_level = u8a_event - EVENT_PWM1_LEVEL0;
            asts_pwm_request[u8a_pwm_id].u16_duty = acsts_pwm_duty[u8a_pwm_id].u16_max * u16a_level / 4;
            asts_pwm_request[u8a_pwm_id].bl_state = true;
            break;
        case EVENT_PWM1_ADC_VALUE:
            u8a_pwm_id = PWM1;
            asts_pwm_request[u8a_pwm_id].bl_state = false;
            break;
    }
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
