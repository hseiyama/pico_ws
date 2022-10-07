#include <stdio.h>
#include "pico/stdlib.h"

#define GPIO_GP2 (2)

static void print_gpio(uint gpio) {
    enum gpio_function u8a_func;
    bool bla_pull_up;
    bool bla_pull_down;

    u8a_func = gpio_get_function(gpio);
    bla_pull_up = gpio_is_pulled_up(gpio);
    bla_pull_down = gpio_is_pulled_down(gpio);

    printf("(func, up, down) = (0x%x, %d, %d)\n", u8a_func, bla_pull_up, bla_pull_down);
}

void main() {

    stdio_init_all();
    print_gpio(GPIO_GP2);

    gpio_pull_up(GPIO_GP2);
    print_gpio(GPIO_GP2);

    gpio_init(GPIO_GP2);
    print_gpio(GPIO_GP2);

    gpio_pull_down(GPIO_GP2);
    print_gpio(GPIO_GP2);

    gpio_deinit(GPIO_GP2);
    print_gpio(GPIO_GP2);

    gpio_disable_pulls(GPIO_GP2);
    print_gpio(GPIO_GP2);
}
