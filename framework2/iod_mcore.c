#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/util/queue.h"
#include "iod_main.h"
#include "apl_main.h"

queue_t sts_queue_core0;
queue_t sts_queue_core1;

static void iod_mcore_core1_task();

// 外部公開関数
void iod_mcore_init() {
    queue_init(&sts_queue_core0, sizeof(uint32_t), 2);
    queue_init(&sts_queue_core1, sizeof(uint32_t), 2);
}

void iod_mcore_main_1ms() {
}

void iod_mcore_main_in() {
}

void iod_mcore_main_out() {
}

void iod_call_mcore_start() {
    multicore_reset_core1();
    multicore_launch_core1(iod_mcore_core1_task);
}

void iod_call_mcore_stpo() {
    multicore_reset_core1();
}

bool iod_call_mcore_fifo_push(uint32_t u32a_data) {
    bool bla_rcode = multicore_fifo_wready();
    if (bla_rcode) {
        multicore_fifo_push_blocking(u32a_data);
    }

    return bla_rcode;
}

bool iod_call_mcore_fifo_pop(uint32_t *pu32a_data) {
    bool bla_rcode = multicore_fifo_rvalid();
    if (bla_rcode) {
        *pu32a_data = multicore_fifo_pop_blocking();
    }

    return bla_rcode;
}

bool iod_call_mcore_queue_add_core0(uint32_t u32a_data) {
    bool bla_rcode = queue_try_add(&sts_queue_core0, &u32a_data);
    return bla_rcode;
}

bool iod_call_mcore_queue_remove_core0(uint32_t *pu32a_data) {
    bool bla_rcode = queue_try_remove(&sts_queue_core0, pu32a_data);
    return bla_rcode;
}

bool iod_call_mcore_queue_add_core1(uint32_t u32a_data) {
    bool bla_rcode = queue_try_add(&sts_queue_core1, &u32a_data);
    return bla_rcode;
}

bool iod_call_mcore_queue_remove_core1(uint32_t *pu32a_data) {
    bool bla_rcode = queue_try_remove(&sts_queue_core1, pu32a_data);
    return bla_rcode;
}

// 内部関数
static void iod_mcore_core1_task() {
    apl_core1_task_init();
    while (true) {
        apl_core1_task_main();
    }
}
