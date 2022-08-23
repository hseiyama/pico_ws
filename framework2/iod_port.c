#include "pico/stdlib.h"
#include "iod_main.h"

#define IOD_PORT_FLT_NUM    (4)

#define BTN_GPIO_GP2        GPIO_GP2_PORT
#define LED0_GPIO_GP25      GPIO_GP25_PORT
#define LED1_GPIO_GP7       GPIO_GP7_PORT

enum iod_port_pull {
    PORT_PULL_NONE,
    PORT_PULL_UP,
    PORT_PULL_DOWN
};

struct iod_port_filter {
    bool bl_value;
    uint8_t u8_count;
};

static struct iod_port_filter sts_btn_filter;

static void iod_port_set_in(uint , enum iod_port_pull);
static void iod_port_set_out(uint , bool);
static void iod_port_filter(bool, struct iod_port_filter *);

// ■■外部公開関数■■
void iod_port_init() {
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

void iod_port_main_1ms() {
}

void iod_port_main_in() {
    // GPIO(GP2)のフィルタ処理
    bool bla_btn_value_now = gpio_get(BTN_GPIO_GP2);
    iod_port_filter(bla_btn_value_now, &sts_btn_filter);
}

void iod_port_main_out() {
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

// ■■内部関数■■
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
