#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// SPI Defines
// We are going to use SPI 0, and allocate it to the following GPIO pins
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define SPI_PORT spi0
#define SPI_RX  (16)
#define SPI_CSN (17)
#define SPI_SCK (18)
#define SPI_TX  (19)

#define EEP_TARGET_SIZE     (128) // 93AA46C-I/P のROMサイズ
#define EEP_BLOCK_SIZE      (16) // 読み書きのサイズ（任意）
#define EEP_HEADER_SIZE     sizeof(uint32_t) // EEP_BLOCK_SIZE のうちヘッダー部のサイズ
#define EEP_HEADER_VALUE_A  (0xAA55AA55)
#define EEP_HEADER_VALUE_B  (0x55AA55AA)
#define EEP_BLOCK_NUM       (EEP_TARGET_SIZE / EEP_BLOCK_SIZE)
#define IOD_SPI_EEP_DATA_SIZE   (EEP_BLOCK_SIZE - EEP_HEADER_SIZE)

// 書き込み許可コマンド
// EWEN SB+Opcode=10011, dummy=5bit
#define EEP_OPERATE_EWEN1       (0b00000010)
#define EEP_OPERATE_EWEN2       (0b01100000)
#define EEP_OPERATE_EWEN_SIZE   (2)
// 書き込み禁止コマンド
// EWDS SB+Opcode=10011, dummy=5bit
#define EEP_OPERATE_EWDS1       (0b00000010)
#define EEP_OPERATE_EWDS2       (0b00000000)
#define EEP_OPERATE_EWDS_SIZE   (2)
// 書き込み操作コマンド（8ビット書き込み）
// WRITE SB+Opcode=101, Address=7bit, DataIn=8bit
#define EEP_OPERATE_WRITE1      (0b00000010)
#define EEP_OPERATE_WRITE2      (0b10000000)
#define EEP_OPERATE_WRITE_SIZE  (3)
// 読み出し操作コマンド（順次読み出し）
// Microwire通信では、READ命令でダミー「0」ビットが出力される
// その対策として、命令を 1ビット前にずらしておく
// READ SB+Opcode=110, Address=7bit, dummy=1bit
#define EEP_OPERATE_READ1       (0b00000110)
#define EEP_OPERATE_READ2       (0b00000000)
#define EEP_OPERATE_READ_SIZE   (2)

static uint8_t au8s_tx_buffer[3];

static void iod_spi_eep_init();
static void iod_spi_eep_block_read(uint8_t, uint8_t *, uint16_t);
static void iod_spi_eep_write_enable();
static void iod_spi_eep_write_disable();
static bool iod_spi_eep_block_write(uint8_t, uint8_t *, uint16_t);
static void iod_spi_eep_write(uint8_t, uint8_t);
static bool iod_spi_eep_busy();

// 外部公開関数
int main()
{
    uint8_t au8a_data[] = {0x64, 0x36, 0x29, 0xDF, 0xFC, 0xAE, 0x58};
    uint8_t au8a_rx_buffer[8];
    uint8_t u8a_index;

    stdio_init_all();

    iod_spi_eep_init();

    iod_spi_eep_block_write(0x40, au8a_data, sizeof(au8a_data));

    iod_spi_eep_block_read(0x40, au8a_rx_buffer, sizeof(au8a_rx_buffer));

    printf("au8a_rx_buffer = ");
    for (u8a_index = 0; u8a_index < sizeof(au8a_rx_buffer); u8a_index++) {
        printf("0x%01x ", au8a_rx_buffer[u8a_index]);
    }
    printf("\n");
}

// 内部関数
static void iod_spi_eep_init() {
    // SPI機能の初期化（クロックは 2MHz）
    spi_init(SPI_PORT, 2000*1000);
    gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(SPI_CSN, GPIO_FUNC_SIO);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);

    // Microwire通信では、CS端子は HIがアクティブ（LOWで初期化）
    gpio_put(SPI_CSN, false);
    gpio_set_dir(SPI_CSN, GPIO_OUT);
}

static void iod_spi_eep_block_read(uint8_t u8a_address, uint8_t *pu8a_buffer, uint16_t u16a_size) {
    au8s_tx_buffer[0] = EEP_OPERATE_READ1;
    au8s_tx_buffer[1] = EEP_OPERATE_READ2 | ((u8a_address & 0x7F) << 1);
    gpio_put(SPI_CSN, true);
    spi_write_blocking(SPI_PORT, au8s_tx_buffer, EEP_OPERATE_READ_SIZE);
    spi_read_blocking(SPI_PORT, 0, pu8a_buffer, u16a_size);
    gpio_put(SPI_CSN, false);
}

static void iod_spi_eep_write_enable() {
    au8s_tx_buffer[0] = EEP_OPERATE_EWEN1;
    au8s_tx_buffer[1] = EEP_OPERATE_EWEN2;
    gpio_put(SPI_CSN, true);
    spi_write_blocking(SPI_PORT, au8s_tx_buffer, EEP_OPERATE_EWEN_SIZE);
    gpio_put(SPI_CSN, false);
}

static void iod_spi_eep_write_disable() {
    au8s_tx_buffer[0] = EEP_OPERATE_EWDS1;
    au8s_tx_buffer[1] = EEP_OPERATE_EWDS2;
    gpio_put(SPI_CSN, true);
    spi_write_blocking(SPI_PORT, au8s_tx_buffer, EEP_OPERATE_EWDS_SIZE);
    gpio_put(SPI_CSN, false);
}

static bool iod_spi_eep_block_write(uint8_t u8a_address, uint8_t *pu8a_buffer, uint16_t u16a_size) {
    uint16_t u16a_index;

    iod_spi_eep_write_enable();
    for (u16a_index = 0; u16a_index < u16a_size; u16a_index++) {
        iod_spi_eep_write(u8a_address + u16a_index, pu8a_buffer[u16a_index]);
        // BUSY状態の間は待機（約3.4ms）
        while (iod_spi_eep_busy()) {
            tight_loop_contents();
        }
    }
    iod_spi_eep_write_disable();
}

static void iod_spi_eep_write(uint8_t u8a_address, uint8_t u8a_data) {
    au8s_tx_buffer[0] = EEP_OPERATE_WRITE1;
    au8s_tx_buffer[1] = EEP_OPERATE_WRITE2 | (u8a_address & 0x7F);
    au8s_tx_buffer[2] = u8a_data;
    gpio_put(SPI_CSN, true);
    spi_write_blocking(SPI_PORT, au8s_tx_buffer, EEP_OPERATE_WRITE_SIZE);
    gpio_put(SPI_CSN, false);
}

static bool iod_spi_eep_busy() {
    uint8_t u8a_data;
    bool bla_rcode;

    gpio_put(SPI_CSN, true);
    spi_read_blocking(SPI_PORT, 0, &u8a_data, 1);
    gpio_put(SPI_CSN, false);
    bla_rcode = (u8a_data != 0xFF)? true: false;

    return bla_rcode;
}
