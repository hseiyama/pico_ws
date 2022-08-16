#include <stdio.h>
#include "pico/stdlib.h"
#include "sys_main.h"
#include "tgt_main.h"

static struct sys_time sts_timer_1000ms;

void tgt_init() {
    stdio_init_all();
    sys_timer_start(&sts_timer_1000ms);
}

void tgt_timer_1ms() {
}

void tgt_main() {
    if(sys_timer_check(&sts_timer_1000ms, 1000)) {
        printf("sys_timer_check: 1000ms Pass(%lld)\n", time_us_64());
        sys_timer_start(&sts_timer_1000ms);
    }
}
