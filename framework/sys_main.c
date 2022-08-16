/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "pico/stdlib.h"
#include "sys_main.h"
#include "tgt_main.h"

#define SYS_TIME_MAX (0xFFFFFFFFFFFFFFFF)

static volatile uint64_t u64s_timer_sys;
static volatile uint8_t u8s_timer_5ms;

static void sys_init();
static void sys_main_1ms();
static void sys_main_5ms();
static bool sys_timer_callback(struct repeating_timer *);

static void sys_init() {
    u64s_timer_sys = 0;
    u8s_timer_5ms = 0;
    tgt_init();
}

static void sys_main_1ms() {
    tgt_timer_1ms();
}

static void sys_main_5ms() {
    tgt_main();
}

static bool sys_timer_callback(struct repeating_timer *sta_repeat_timer) {
    u64s_timer_sys++;
    u8s_timer_5ms++;
    sys_main_1ms();
    return true;
}

void main() {
    struct repeating_timer sta_repeat_timer;

    sys_init();
    // 1msのタイマー割り込みを設定
    // （引数 delay_ms=-1：前回の割り込み開始時から計測）
    add_repeating_timer_ms(-1, sys_timer_callback, NULL, &sta_repeat_timer);

    while (true) {
        // 5ms経過毎に条件が成立
        if(u8s_timer_5ms >= 5) {
            u8s_timer_5ms = 0;
            sys_main_5ms();
        }
    }
}

void sys_timer_start(struct sys_time *sta_sys_timer) {
    sta_sys_timer->u64_time = u64s_timer_sys;
    sta_sys_timer->bl_state = true;
}

void sys_timer_stop(struct sys_time *sta_sys_timer) {
    sta_sys_timer->u64_time = SYS_TIME_MAX;
    sta_sys_timer->bl_state = false;
}

bool sys_timer_check(struct sys_time *sta_sys_timer, uint64_t u64a_wait_time) {
    uint64_t u64a_time_diff;
    bool bla_rcode;

    u64a_time_diff = sys_timer_diff(sta_sys_timer, u64a_wait_time);
    // 待ち時間が経過している場合は、trueを設定
    bla_rcode = (u64a_time_diff > 0) ? false : true;
    return bla_rcode;
}

uint64_t sys_timer_diff(struct sys_time *sta_sys_timer, uint64_t u64a_wait_time) {
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
        // 待ち時間との差を算出する（経過後は、0を設定）
        if(u64a_wait_time > u64a_pass_time) {
            u64a_rtime = u64a_wait_time - u64a_pass_time;
        } else {
            u64a_rtime = 0;
        }
    // タイマーが停止している場合は、0を設定
    } else {
        u64a_rtime = 0;
    }
    return u64a_rtime;
}

bool sys_timer_isrun(struct sys_time *sta_sys_timer) {
    return sta_sys_timer->bl_state;
}
