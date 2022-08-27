#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "iod_main.h"

// 外部公開関数
void iod_wdog_init() {
    // ウォッチドッグを有効に設定（10ms）
    watchdog_enable(10, true);
}

void iod_wdog_main_1ms() {
}

void iod_wdog_main_in() {
}

void iod_wdog_main_out() {
}

void iod_call_wdog_update() {
    watchdog_update();
}

bool iod_call_wdog_isboot() {
    return watchdog_caused_reboot();
}
