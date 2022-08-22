#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "iod_main.h"

#define IOD_PORT_FLT_NUM    (4)
#define IOD_ADC_FLT_NUM     (4)

#define BTN_GPIO_GP2        (2)
#define LED0_GPIO_GP25      PICO_DEFAULT_LED_PIN
#define LED1_GPIO_GP7       (7)
#define UART0_ID            uart0
#define UART0_TX_GPIO       (0)
#define UART0_RX_GPIO       (1)
#define ADC0_GPIO_GP26      (26)
#define ADC0_CHANNEL        (0)
#define PWM_5A_GPIO_GP10    (10)
#define PWM_5B_GPIO_GP11    (11)
#define PWM_5_CLKDIV        (100.0) // 分周比：周期 5[Khz]/100.0=50[hz]->0.02[s]
#define PWM_5A_DUTY_INIT    IOD_PWM1_DUTY_MAX
#define PWM_5B_DUTY_INIT    IOD_PWM2_DUTY_MAX

enum iod_port_pull {
    PORT_PULL_NONE,
    PORT_PULL_UP,
    PORT_PULL_DOWN
};

struct iod_port_filter {
    bool bl_value;
    uint8_t u8_count;
};

struct iod_adc_filter {
    uint16_t au16_value_mem[IOD_ADC_FLT_NUM];
    uint16_t u16_value;
    uint8_t u8_count;
    uint8_t u8_index;
};

static struct iod_port_filter sts_btn_filter;
static struct iod_adc_filter sts_adc0_filter;

static void iod_init_port();
static void iod_port_set_in(uint , enum iod_port_pull);
static void iod_port_set_out(uint , bool);
static void iod_init_uart();
static void iod_clear_rx_fifo(uart_inst_t *);
static void iod_init_adc();
static void iod_init_pwm();
static void iod_port_filter(bool, struct iod_port_filter *);
static void iod_adc_filter(uint16_t, struct iod_adc_filter *);

void iod_init() {
    // ポートの初期設定
    iod_init_port();
    // UARTの初期設定
    iod_init_uart();
    // ADCの初期設定
    iod_init_adc();
    // PWMの初期設定
    iod_init_pwm();
}

static void iod_init_port() {
    // GPIO(GP2)の初期設定（ポート入力：プルアップ）
    iod_port_set_in(BTN_GPIO_GP2, PORT_PULL_UP);
    // GPIO(GP25)の初期設定（ポート出力）
    iod_port_set_out(LED0_GPIO_GP25 , IOD_LED0_VALUE_INIT);
    // GPIO(GP7)の初期設定（ポート出力）
    iod_port_set_out(LED1_GPIO_GP7 , IOD_LED1_VALUE_INIT);

    // GPIO(GP2)のフィルタ初期設定
    sts_btn_filter.bl_value = IOD_BTN_VALUE_INIT;
    sts_btn_filter.u8_count = 0;
}

static void iod_port_set_in(uint u32a_gpio, enum iod_port_pull u8a_pull) {
    // GPIOの初期設定（ポート入力）
    gpio_init(u32a_gpio);
    gpio_set_dir(u32a_gpio, GPIO_IN);
    // プルアップまたはプルダウンの設定
    if (u8a_pull == PORT_PULL_UP) {
        gpio_pull_up(u32a_gpio);
    } else if (u8a_pull == PORT_PULL_DOWN) {
        gpio_pull_down(u32a_gpio);
    }
}

static void iod_port_set_out(uint u32a_gpio, bool bla_value) {
    // GPIOの初期設定（ポート出力）
    gpio_init(u32a_gpio);
    gpio_put(u32a_gpio, bla_value);
    gpio_set_dir(u32a_gpio, GPIO_OUT);
}

static void iod_init_uart() {
    // UART0の初期設定
    uart_init(UART0_ID, 115200);
    gpio_set_function(UART0_TX_GPIO, GPIO_FUNC_UART);
    gpio_set_function(UART0_RX_GPIO, GPIO_FUNC_UART);
    // UART0のRX_FIFOをクリア（機能の動作安定まで時間待ち：1us）
    sleep_us(1);
    iod_clear_rx_fifo(UART0_ID);
}

static void iod_clear_rx_fifo(uart_inst_t *psta_uart_id) {
    while (uart_is_readable(psta_uart_id)) {
        uart_getc(psta_uart_id);
    }
}

static void iod_init_adc() {
    // ADC0の初期設定
    adc_init();
    adc_gpio_init(ADC0_GPIO_GP26);
    adc_select_input(ADC0_CHANNEL);

    // ADC0のフィルタ初期設定
    memset(&sts_adc0_filter, 0, sizeof(sts_adc0_filter));
}

static void iod_init_pwm() {
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

void iod_main_1ms() {
}

void iod_main_in() {
    // GPIO(GP2)のフィルタ処理
    bool bla_btn_value_now = gpio_get(BTN_GPIO_GP2);
    iod_port_filter(bla_btn_value_now, &sts_btn_filter);
    // ADC0のフィルタ処理
    uint16_t u16a_adc0_value_now = adc_read();
    iod_adc_filter(u16a_adc0_value_now, &sts_adc0_filter);
}

static void iod_port_filter(bool bla_value_now, struct iod_port_filter *psta_filter) {
    // ポート値が変化している場合
    if (bla_value_now != psta_filter->bl_value) {
        (psta_filter->u8_count)++;
        // フィルタの回数分、連続でポート値の変化が継続している場合
        if (psta_filter->u8_count >= IOD_PORT_FLT_NUM) {
            psta_filter->u8_count = 0;
            psta_filter->bl_value = bla_value_now;
        }
    // ポート値に変化がない場合
    } else {
        psta_filter->u8_count = 0;
    }
}

static void iod_adc_filter(uint16_t u16a_value_now, struct iod_adc_filter *psta_filter) {
    uint32_t u32a_value_sum = 0;
    uint8_t u8a_index;

    // フィルタの個数分、ADCを記憶する
    psta_filter->au16_value_mem[psta_filter->u8_index] = u16a_value_now;
    psta_filter->u8_index++;
    psta_filter->u8_count++;
    // インデックスとカウンタを修正
    if (psta_filter->u8_index >= IOD_ADC_FLT_NUM) {
        psta_filter->u8_index = 0;
    }
    if (psta_filter->u8_count >= IOD_ADC_FLT_NUM) {
        psta_filter->u8_count = IOD_ADC_FLT_NUM;
    }
    // ADCの平均値を計算する
    for (u8a_index = 0; u8a_index < psta_filter->u8_count; u8a_index++) {
        u32a_value_sum += psta_filter->au16_value_mem[u8a_index];
    }
    psta_filter->u16_value = (uint16_t)(u32a_value_sum / psta_filter->u8_count);
}

void iod_main_out() {
}

void iod_read_btn_value(bool *pbla_btn_value) {
    *pbla_btn_value = sts_btn_filter.bl_value;
}

void iod_write_led0_value(bool bla_led_value) {
    // GPIO(GP7)の出力
    gpio_put(LED0_GPIO_GP25, bla_led_value);
}

void iod_write_led1_value(bool bla_led_value) {
    // GPIO(GP7)の出力
    gpio_put(LED1_GPIO_GP7, bla_led_value);
}

bool iod_call_uart_receive(uint8_t *pu8a_message) {
    uint8_t u8a_index = 0;
    bool bla_rcode = false;

    while (uart_is_readable(UART0_ID)
    && (u8a_index < IOD_UART_BUFF_SIZE - 1)) {
        bla_rcode = true;
        // UART0の受信
        pu8a_message[u8a_index] = uart_getc(UART0_ID);
        u8a_index++;
    }
    pu8a_message[u8a_index] = 0x00;

    return bla_rcode;
}

void iod_call_uart_transmit(uint8_t *pu8a_message) {
    // UART0の送信
    uart_puts(UART0_ID, pu8a_message);
}

void iod_read_adc_value(uint16_t *pu16a_adc_value) {
    *pu16a_adc_value = sts_adc0_filter.u16_value;
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
