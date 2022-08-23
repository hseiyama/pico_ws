#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "iod_main.h"

#define PWM_5A_GPIO_GP10    GPIO_GP10_PWM
#define PWM_5B_GPIO_GP11    GPIO_GP11_PWM
#define PWM_5_CLKDIV        (100.0) // 分周比：周期 5[Khz]/100.0=50[hz]->0.02[s]
#define PWM_5A_DUTY_INIT    IOD_PWM1_DUTY_MAX
#define PWM_5B_DUTY_INIT    IOD_PWM2_DUTY_MAX

// ■■外部公開関数■■
void iod_pwm_init() {
    // PWM(5A,5B)の初期設定
    gpio_set_function(PWM_5A_GPIO_GP10, GPIO_FUNC_PWM);
    gpio_set_function(PWM_5B_GPIO_GP11, GPIO_FUNC_PWM);
    uint u32a_slice_num = pwm_gpio_to_slice_num(PWM_5A_GPIO_GP10);      // スライス番号を取得
    pwm_set_wrap(u32a_slice_num, PWM_5_WRAP - 1);                       // PWMの分解能を設定
    pwm_set_clkdiv(u32a_slice_num, PWM_5_CLKDIV);                       // 分周比を設定（0～256未満の実数値で設定）
    pwm_set_chan_level(u32a_slice_num, PWM_CHAN_A, PWM_5A_DUTY_INIT);   // duty(High期間)の設定
    pwm_set_chan_level(u32a_slice_num, PWM_CHAN_B, PWM_5B_DUTY_INIT);   // duty(High期間)の設定
    pwm_set_enabled(u32a_slice_num, true);                              // PWM出力の有効/無効を設定
}

void iod_pwm_main_1ms() {
}

void iod_pwm_main_in() {
}

void iod_pwm_main_out() {
}

void iod_call_pwm1_set_duty(uint16_t u16a_level) {
    // PWM(5A)のduty設定
    uint u32a_slice_num = pwm_gpio_to_slice_num(PWM_5A_GPIO_GP10);
    pwm_set_chan_level(u32a_slice_num, PWM_CHAN_A, u16a_level);
}

void iod_call_pwm2_set_duty(uint16_t u16a_level) {
    // PWM(5B)のduty設定
    uint u32a_slice_num = pwm_gpio_to_slice_num(PWM_5B_GPIO_GP11);
    pwm_set_chan_level(u32a_slice_num, PWM_CHAN_B, u16a_level);
}
