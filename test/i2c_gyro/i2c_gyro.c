#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c1
#define I2C_SDA (10)
#define I2C_SCL (11)
#define I2C_ADR (12)
#define I2C_CSN (13)

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(I2C_PORT, 400*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);
    gpio_init(I2C_ADR);
    gpio_init(I2C_CSN);
    gpio_set_dir(I2C_ADR, GPIO_OUT);
    gpio_set_dir(I2C_CSN, GPIO_OUT);
    gpio_put(I2C_ADR, false);
    gpio_put(I2C_CSN, true);

    puts("Hello, world!");

    // L3GD20H設定（CTRL1レジスタ PD=1：通常モード）
    uint8_t au8a_tx_buff_cmd1[2] = {0x20, 0x0F}; // register address, data
    i2c_write_blocking(I2C_PORT, 0x6A, au8a_tx_buff_cmd1, sizeof(au8a_tx_buff_cmd1), false);
    // L3GD20H設定（CTRL4レジスタ FS1-FS0=10：2000dps）
    uint8_t au8a_tx_buff_cmd2[2] = {0x23, 0x20}; // register address, data
    i2c_write_blocking(I2C_PORT, 0x6A, au8a_tx_buff_cmd2, sizeof(au8a_tx_buff_cmd2), false);
    // 読み出し操作コマンド（順次読み出し）
    uint8_t au8a_tx_buff[1] = {0xA8}; // 0x80(multiple data read/write) | 0x28-0x2D(register address)
    static uint8_t au8a_rx_buff[6];
    static uint32_t u32a_tx;
    static uint32_t u32a_rx;
    // 読み出し操作
    u32a_tx = i2c_write_blocking(I2C_PORT, 0x6A, au8a_tx_buff, sizeof(au8a_tx_buff), true);
    u32a_rx = i2c_read_blocking(I2C_PORT, 0x6A, au8a_rx_buff, sizeof(au8a_rx_buff), false);

    uint8_t u8a_index;
    printf("au8a_rx_buffer = ");
    for (u8a_index = 0; u8a_index < sizeof(au8a_rx_buff); u8a_index++) {
        printf("0x%01x ", au8a_rx_buff[u8a_index]);
    }
    printf("\n");

    return 0;
}
