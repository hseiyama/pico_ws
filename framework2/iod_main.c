#include "pico/stdlib.h"
#include "iod_main.h"

// 外部公開関数
void iod_init() {
    iod_port_init();
    iod_uart_init();
    iod_adc_init();
    iod_pwm_init();
    iod_flash_init();
    iod_wdog_init();
}

void iod_main_1ms() {
    iod_port_main_1ms();
    iod_uart_main_1ms();
    iod_adc_main_1ms();
    iod_pwm_main_1ms();
    iod_flash_main_1ms();
    iod_wdog_main_1ms();
}

void iod_main_in() {
    iod_port_main_in();
    iod_uart_main_in();
    iod_adc_main_in();
    iod_pwm_main_in();
    iod_flash_main_in();
    iod_wdog_main_in();
}

void iod_main_out() {
    iod_port_main_out();
    iod_uart_main_out();
    iod_adc_main_out();
    iod_pwm_main_out();
    iod_flash_main_out();
    iod_wdog_main_out();
}
