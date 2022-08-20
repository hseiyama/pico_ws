#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "iod_main.h"

#define UART0_ID        uart0
#define UART0_TX_GPIO   (0)
#define UART0_RX_GPIO   (1)
#define LED0_GPIO_GP25  PICO_DEFAULT_LED_PIN
#define LED1_GPIO_GP7   (7)
#define BTN_GPIO_GP2    (2)

static void iod_init_port();
static void iod_init_uart();
static void iod_clear_rx_fifo(uart_inst_t *);

void iod_init() {
    // ポートの初期設定
    iod_init_port();
    // UARTの初期設定
    iod_init_uart();
}

static void iod_init_port() {
    // GPIO(GP2)の初期設定（ポート入力：プルアップ）
    gpio_init(BTN_GPIO_GP2);
    gpio_set_dir(BTN_GPIO_GP2, GPIO_IN);
    gpio_pull_up(BTN_GPIO_GP2);
    // GPIO(GP25)の初期設定（ポート出力）
    gpio_init(LED0_GPIO_GP25);
    gpio_put(LED0_GPIO_GP25, IOD_LED0_VALUE_INIT);
    gpio_set_dir(LED0_GPIO_GP25, GPIO_OUT);
    // GPIO(GP7)の初期設定（ポート出力）
    gpio_init(LED1_GPIO_GP7);
    gpio_put(LED1_GPIO_GP7, IOD_LED1_VALUE_INIT);
    gpio_set_dir(LED1_GPIO_GP7, GPIO_OUT);
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

static void iod_clear_rx_fifo(uart_inst_t *uart) {
    while (uart_is_readable(UART0_ID)) {
        uart_getc(uart);
    }
}

void iod_main_1ms() {
}

void iod_main_in() {
}

void iod_main_out() {
}

void iod_read_btn_value(bool *pbla_btn_value) {
    static bool bls_btn_value_filter = IOD_BTN_VALUE_INIT;
    static uint8_t u8s_count_filter = 0;
    bool bla_btn_value_temp;

    // GPIO(GP2)の入力
    bla_btn_value_temp = gpio_get(BTN_GPIO_GP2);
    // ポート値が変化している場合
    if (bla_btn_value_temp != bls_btn_value_filter) {
        u8s_count_filter++;
        // 4回連続でポート値の変化が継続している場合
        if (u8s_count_filter >= 4) {
            u8s_count_filter = 0;
            bls_btn_value_filter = bla_btn_value_temp;
        }
    // ポート値に変化がない場合
    } else {
        u8s_count_filter = 0;
    }

    // 戻り値の設定
    *pbla_btn_value = bls_btn_value_filter;
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
