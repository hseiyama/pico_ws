/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"

static volatile uint64_t u64s_timer_sys;
static volatile uint8_t u8s_timer_5ms;

static bool sys_timer_callback(struct repeating_timer *);
static void sys_init();
static void sys_main_1ms();
static void sys_main_5ms();

static bool sys_timer_callback(struct repeating_timer *st_timer) {
    u64s_timer_sys++;
    u8s_timer_5ms++;
    sys_main_1ms();
    printf("Repeat at %lld\n", time_us_64());
    return true;
}

static void sys_init() {
    u64s_timer_sys = 0;
    u8s_timer_5ms = 0;
    stdio_init_all();
}

static void sys_main_1ms() {
    printf("sys_main_1ms\n");
}

static void sys_main_5ms() {
    printf("sys_main_5ms\n");
}

void main() {
    struct repeating_timer st_timer;

    sys_init();
    add_repeating_timer_ms(-1000, sys_timer_callback, NULL, &st_timer);
    printf("Hello Timer!\n");

    while (true) {
        if(u8s_timer_5ms >= 5) {
            u8s_timer_5ms = 0;
            sys_main_5ms();
        }
    }
}
