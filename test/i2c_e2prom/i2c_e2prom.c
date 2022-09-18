#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// I2C defines
// This example will use I2C0 on GPIO8 (SDA) and GPIO9 (SCL) running at 400KHz.
// Pins can be changed, see the GPIO function select table in the datasheet for information on GPIO assignments
#define I2C_PORT i2c0
#define I2C_SDA (20)
#define I2C_SCL (21)

int main()
{
    stdio_init_all();

    // I2C Initialisation. Using it at 1Mhz.
    i2c_init(I2C_PORT, 1000*1000);
    
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C_SDA);
    //gpio_pull_up(I2C_SCL);

    puts("Hello, world!");

    // 書き込み操作コマンド（ページ書き込み）
//    uint8_t au8a_tx_buff[5] = {0x00, 0x00, 0x99, 0x88, 0x77};
    // 読み出し操作コマンド（順次読み出し）
    uint8_t au8a_tx_buff[2] = {0x00, 0x00};
    static uint8_t au8a_rx_buff[8];
    static uint32_t u32a_tx;
    static uint32_t u32a_rx;
    // 書き込み操作
//    u32a_tx = i2c_write_blocking(I2C_PORT, 0x54, au8a_tx_buff, sizeof(au8a_tx_buff), false);
    // 読み出し操作
    u32a_tx = i2c_write_blocking(I2C_PORT, 0x54, au8a_tx_buff, sizeof(au8a_tx_buff), true);
    u32a_rx = i2c_read_blocking(I2C_PORT, 0x54, au8a_rx_buff, sizeof(au8a_rx_buff), false);

    return 0;
}
