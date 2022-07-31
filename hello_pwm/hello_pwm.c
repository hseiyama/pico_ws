/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// Output PWM signals on pins 6 and 7

#include "pico/stdlib.h"
#include "hardware/pwm.h"

int main() {
    /// \tag::setup_pwm[]

    // Tell GPIO 6 and 7 they are allocated to the PWM
    gpio_set_function(6, GPIO_FUNC_PWM);
    gpio_set_function(7, GPIO_FUNC_PWM);

    // Find out which PWM slice is connected to GPIO 6 (it's slice 3)
    uint slice_num = pwm_gpio_to_slice_num(6);

    // Set period of 25000 cycles (0 to 24999 inclusive)
    pwm_set_wrap(slice_num, 24999);
    // Set channel A output high for 6250 cycle (1/4) before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_A, 6250);
    // Set initial B output high for 18750 cycles (3/4) before dropping
    pwm_set_chan_level(slice_num, PWM_CHAN_B, 18750);
    // Set the PWM running
    pwm_set_enabled(slice_num, true);
    /// \end::setup_pwm[]

    // Note we could also use pwm_set_gpio_level(gpio, x) which looks up the
    // correct slice and channel for a given GPIO.
}
