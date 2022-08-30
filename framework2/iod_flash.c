/**
 * 補足１：flash関係の命令を実行すると、以下の実行時間がかかるようだ
 *           flash_range_erase() :約30ms
 *           flash_range_program() :約570us
 * 補足２：flashのデータ操作は以下のように実行される
 *           ① flash_range_erase()で、全データを「0xFF」に消去
 *           ② flash_range_program()で、対象ビットを「0」に書き込む
 *             ※②では、一度ビットを「0」にすると、「1」には戻せない
 * 注意１：flash_range_erase()の実行中に、割り込みが起きると例外が発生する
 * 注意２：CORE1でflash_range_erase()を実行すると、意図しない動作が起きる
 * 注意３：flash_range_erase()を中断すると、次回デバッガーの接続が困難になる
 */

#include <string.h>
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "pico/multicore.h"
#include "iod_main.h"

#define FLASH_TARGET_OFFSET (256 * 1024)
//#define FLASH_HEADER_SIZE   sizeof(uint32_t) // FLASH_PAGE_SIZE のうちヘッダー部のサイズ
#define FLASH_HEADER_VALUE  (0xAA55AA55)
#define FLASH_PAGE_NUM      (FLASH_SECTOR_SIZE / FLASH_PAGE_SIZE)

#define FLASH_ADDRESS(index) (pcu8s_target_address + (FLASH_PAGE_SIZE * (index)))

struct iod_flash_buffer {
    uint32_t u32_header;
    uint8_t au8_data[IOD_FLASH_DATA_SIZE];
};

static const uint8_t *pcu8s_target_address = (const uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);

struct iod_flash_buffer sts_flash_buffer;

static bool iod_flash_search_read_index(uint8_t *);
static bool iod_flash_read_data(uint8_t *, uint16_t);
static bool iod_flash_search_write_index(uint8_t *);
static void iod_flash_write_data();

// 外部公開関数
void iod_flash_init() {
}

void iod_flash_main_1ms() {
}

void iod_flash_main_in() {
}

void iod_flash_main_out() {
}

bool iod_call_flash_read(uint8_t *pu8a_buffer, uint16_t u16a_size) {
    bool bla_rcode = false;

    if (u16a_size <= FLASH_PAGE_SIZE) {
        bla_rcode = iod_flash_read_data(pu8a_buffer, u16a_size);
    }
    return bla_rcode;
}

bool iod_call_flash_write(uint8_t *pu8a_buffer, uint16_t u16a_size) {
    bool bla_rcode = false;

    if (u16a_size <= IOD_FLASH_DATA_SIZE) {
        sts_flash_buffer.u32_header = FLASH_HEADER_VALUE;
        memcpy(sts_flash_buffer.au8_data, pu8a_buffer, u16a_size);
        iod_flash_write_data();
        bla_rcode = true;
    }
    return bla_rcode;
}

static void iod_flash_core1_write_data();
static void iod_flash_core1_write_data() {
    flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);

    while (true) {
        tight_loop_contents();
    }
}

void iod_call_flash_erase() {
    static bool bla_reset = false;

    if (bla_reset) {
        multicore_reset_core1();
    }
    bla_reset = true;
    multicore_launch_core1(iod_flash_core1_write_data);
}

// 内部関数
static bool iod_flash_search_read_index(uint8_t *pu8a_index_latest) {
    struct iod_flash_buffer *psta_flash_buffer;
    bool bla_rcode = false;
    uint8_t u8a_index;

    for (u8a_index = 0; u8a_index < FLASH_PAGE_NUM; u8a_index++) {
        psta_flash_buffer = (struct iod_flash_buffer *)FLASH_ADDRESS(u8a_index);
        if (psta_flash_buffer->u32_header == FLASH_HEADER_VALUE) {
            *pu8a_index_latest = u8a_index;
            bla_rcode = true;
        } else {
            break;
        }
    }

    return bla_rcode;
}

static bool iod_flash_read_data(uint8_t *pu8a_buffer, uint16_t u16a_size) {
    struct iod_flash_buffer *psta_flash_buffer;
    uint8_t u8a_index_latest;
    bool bla_rcode;

    bla_rcode = iod_flash_search_read_index(&u8a_index_latest);
    if (bla_rcode) {
        psta_flash_buffer = (struct iod_flash_buffer *)FLASH_ADDRESS(u8a_index_latest);
        memcpy(pu8a_buffer, psta_flash_buffer->au8_data, IOD_FLASH_DATA_SIZE);
    }

    return bla_rcode;
}

static bool iod_flash_search_write_index(uint8_t *pu8a_index_next) {
    struct iod_flash_buffer *psta_flash_buffer;
    bool bla_rcode = false;
    uint8_t u8a_index;

    for (u8a_index = 0; u8a_index < FLASH_PAGE_NUM; u8a_index++) {
        psta_flash_buffer = (struct iod_flash_buffer *)FLASH_ADDRESS(u8a_index);
        if (psta_flash_buffer->u32_header != FLASH_HEADER_VALUE) {
            *pu8a_index_next = u8a_index;
            bla_rcode = true;
            break;
        }
    }

    return bla_rcode;
}

static void iod_flash_write_data() {
    uint8_t pu8a_index_next;
    bool bla_rcode;

    pu8a_index_next = 0;
    bla_rcode = iod_flash_search_write_index(&pu8a_index_next);
    if (!bla_rcode) {
        // フラッシュ領域を消去（セクター単位）
        flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
    }
    // フラッシュ領域に書き込み（ページ単位）
    uint32_t u32a_flash_offs = FLASH_TARGET_OFFSET + (FLASH_PAGE_SIZE * pu8a_index_next);
    flash_range_program(u32a_flash_offs, (uint8_t *)&sts_flash_buffer, FLASH_PAGE_SIZE);
}
