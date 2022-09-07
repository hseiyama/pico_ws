#include <string.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/resets.h"
#include "iod_main.h"

#define IOD_ADC_FLT_NUM     (4)

#define ADC0_GPIO_GP26      GPIO_GP26_ADC
#define ADC0_CHANNEL        (0)

struct iod_adc_filter {
    uint16_t au16_value_mem[IOD_ADC_FLT_NUM];
    uint16_t u16_value;
    uint8_t u8_count;
    uint8_t u8_index;
};

static struct iod_adc_filter sts_adc0_filter;

static void iod_adc_filter(uint16_t, struct iod_adc_filter *);

// 外部公開関数
void iod_adc_init() {
    // ADC0の初期設定
    adc_init();
    adc_gpio_init(ADC0_GPIO_GP26);
    adc_select_input(ADC0_CHANNEL);

    // ADC0のフィルタ初期設定
    memset(&sts_adc0_filter, 0, sizeof(sts_adc0_filter));
}

void iod_adc_deinit() {
    // HWブロックを初期化
    reset_block(RESETS_RESET_ADC_BITS);
    unreset_block_wait(RESETS_RESET_ADC_BITS);
}

void iod_adc_main_1ms() {
}

void iod_adc_main_in() {
    // ADC0のフィルタ処理
    uint16_t u16a_adc0_value_now = adc_read();
    iod_adc_filter(u16a_adc0_value_now, &sts_adc0_filter);
}

void iod_adc_main_out() {
}

void iod_read_adc_value(uint16_t *pu16a_adc_value) {
    *pu16a_adc_value = sts_adc0_filter.u16_value;
}

// 内部関数
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
