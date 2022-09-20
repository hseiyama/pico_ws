#include <stdio.h>
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



int main()
{
    stdio_init_all();

    // SPI initialisation. This example will use SPI at 2MHz.
    spi_init(SPI_PORT, 2000*1000);
    gpio_set_function(SPI_RX, GPIO_FUNC_SPI);
    //gpio_set_function(SPI_CSN, GPIO_FUNC_SIO);
    gpio_set_function(SPI_SCK, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX, GPIO_FUNC_SPI);
    
    // Chip select is active-high, so we'll initialise it to a driven-low state
    gpio_init(SPI_CSN);
    gpio_set_dir(SPI_CSN, GPIO_OUT);
    gpio_put(SPI_CSN, false);

    puts("Hello, world!");

    // 書き込み操作コマンド（8ビット書き込み）
    // EWEN SB+Opcode=10011, dummy=5bit
//    uint8_t au8a_tx_buff_we[2] = {0b00000010, 0b01100000};
    // WRITE SB+Opcode=101, Address=7bit, DataIn=8bit
//    uint8_t au8a_tx_buff[3] = {0b00000010, 0b10000000 | 0x7E, 0x7E};
    // 読み出し操作コマンド（順次読み出し）
    // READ命令でダミーのゼロ •ビットが出力されるので、
    // その対策として 1ビット分を前にずらしておく
    // READ SB+Opcode=110, Address=7bit, dummy=1bit
    uint8_t au8a_tx_buff[2] = {0b00000110, 0x7E << 1};
    static uint8_t au8a_rx_buff[8];
    static uint32_t u32a_tx;
    static uint32_t u32a_rx;
//    gpio_put(SPI_CSN, true);
//    u32a_tx = spi_write_blocking(SPI_PORT, au8a_tx_buff_we, sizeof(au8a_tx_buff_we));
//    gpio_put(SPI_CSN, false);
//    sleep_ms(1);
    gpio_put(SPI_CSN, true);
    u32a_tx = spi_write_blocking(SPI_PORT, au8a_tx_buff, sizeof(au8a_tx_buff));
    u32a_rx = spi_read_blocking(SPI_PORT, 0, au8a_rx_buff, sizeof(au8a_rx_buff));
//    u32a_rx = spi_write_read_blocking(SPI_PORT, au8a_tx_buff, au8a_rx_buff, sizeof(au8a_rx_buff));
    gpio_put(SPI_CSN, false);

    return 0;
}
