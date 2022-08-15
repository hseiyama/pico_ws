/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#include "pico/stdlib.h"

#define SYS_TIME_MAX (0xFFFFFFFFFFFFFFFF)

struct sys_time {
	uint64_t u64_time;
	bool bl_state;
};

static volatile uint64_t u64s_timer_sys;
static volatile uint8_t u8s_timer_5ms;
static struct sys_time sts_timer_1000ms;

static bool sys_timer_callback(struct repeating_timer *);
static void sys_init();
static void sys_main_1ms();
static void sys_main_5ms();
static void sys_timer_start(struct sys_time *);
static void sys_timer_stop(struct sys_time *);
static bool sys_timer_check(struct sys_time *, uint64_t);
static uint64_t sys_timer_diff(struct sys_time *, uint64_t);
static bool sys_timer_isrun(struct sys_time *);

static bool sys_timer_callback(struct repeating_timer *sta_repeat_timer) {
    u64s_timer_sys++;
    u8s_timer_5ms++;
    sys_main_1ms();
    return true;
}

static void sys_init() {
    u64s_timer_sys = 0;
    u8s_timer_5ms = 0;
    stdio_init_all();
    sys_timer_start(&sts_timer_1000ms);
}

static void sys_main_1ms() {
}

static void sys_main_5ms() {
    if(sys_timer_check(&sts_timer_1000ms, 1000)) {
        printf("sys_timer_check: 1000ms Pass(%lld)\n", time_us_64());
        sys_timer_start(&sts_timer_1000ms);
    }
}

void main() {
    struct repeating_timer sta_repeat_timer;

    sys_init();
    add_repeating_timer_ms(-1, sys_timer_callback, NULL, &sta_repeat_timer);
    printf("Hello Framework!\n");

    while (true) {
        if(u8s_timer_5ms >= 5) {
            u8s_timer_5ms = 0;
            sys_main_5ms();
        }
    }
}

static void sys_timer_start(struct sys_time *sta_sys_timer) {
    sta_sys_timer->u64_time = u64s_timer_sys;
    sta_sys_timer->bl_state = true;
}

static void sys_timer_stop(struct sys_time *sta_sys_timer) {
    sta_sys_timer->u64_time = SYS_TIME_MAX;
    sta_sys_timer->bl_state = false;
}

static bool sys_timer_check(struct sys_time *sta_sys_timer, uint64_t u64a_wait_time) {
    uint64_t u64a_time_diff;
    bool bla_rcode;

    u64a_time_diff = sys_timer_diff(sta_sys_timer, u64a_wait_time);
    bla_rcode = (u64a_time_diff > 0) ? false : true;
    return bla_rcode;
}

static uint64_t sys_timer_diff(struct sys_time *sta_sys_timer, uint64_t u64a_wait_time) {
    uint64_t u64a_pass_time;
    uint64_t u64a_rtime;

    // タイマーが開始している場合
    if(sta_sys_timer->bl_state) {
        // 経過時間を算出する
        if(u64s_timer_sys >= sta_sys_timer->u64_time) {
            u64a_pass_time = u64s_timer_sys - sta_sys_timer->u64_time;
        } else {
            u64a_pass_time = SYS_TIME_MAX - (sta_sys_timer->u64_time - u64s_timer_sys);
        }
        // 待ち時間との差を算出する（経過後は0を設定）
        if(u64a_wait_time > u64a_pass_time) {
            u64a_rtime = u64a_wait_time - u64a_pass_time;
        } else {
            u64a_rtime = 0;
        }
    // タイマーが停止している場合は0を設定
    } else {
        u64a_rtime = 0;
    }
    return u64a_rtime;
}

static bool sys_timer_isrun(struct sys_time *sta_sys_timer) {
    return sta_sys_timer->bl_state;
}
