#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "iod_main.h"
#include "iod_i2c.h"

#define I2C1_ID             i2c1
#define I2C1_SDA_GPIO_GP10  GPIO_GP10_I2C
#define I2C1_SCL_GPIO_GP11  GPIO_GP11_I2C
#define I2C1_ADR_GPIO_GP12  GPIO_GP12_I2C
#define I2C1_CSN_GPIO_GP13  GPIO_GP13_I2C

// L3GD20H のアドレス
// Address 7bit: SlaveAddress=0b1101010
#define GYRO_ADDRESS        (0b01101010)
// L3GD20H のレジスタアドレス
#define GYRO_REG_CTRL1      (0x20)
#define GYRO_REG_CTRL2      (0x21)
#define GYRO_REG_CTRL3      (0x22)
#define GYRO_REG_CTRL4      (0x23)
#define GYRO_REG_CTRL5      (0x24)
#define GYRO_REG_OUT_X_L    (0x28)
#define GYRO_REG_OUT_X_H    (0x29)
#define GYRO_REG_OUT_Y_L    (0x2A)
#define GYRO_REG_OUT_Y_H    (0x2B)
#define GYRO_REG_OUT_Z_L    (0x2C)
#define GYRO_REG_OUT_Z_H    (0x2D)

enum iod_i2c_gyro_group {
    GYRO_X = 0,
    GYRO_Y,
    GYRO_Z,
    GYRO_GROUP_NUM
};

const uint8_t acu8s_gyro_address[GYRO_GROUP_NUM] = {
    GYRO_REG_OUT_X_L,
    GYRO_REG_OUT_Y_L,
    GYRO_REG_OUT_Z_L
};

static int16_t as16s_gyro_value[GYRO_GROUP_NUM];
static uint8_t au8s_tx_buffer[32];

static void iod_i2c_gyro_read(uint8_t, uint8_t *, uint8_t);
static void iod_i2c_gyro_write(uint8_t, uint8_t);

// 外部公開関数
void iod_i2c_gyro_init() {
    memset(as16s_gyro_value, 0, sizeof(as16s_gyro_value));
    memset(au8s_tx_buffer, 0, sizeof(au8s_tx_buffer));

    // I2C1の初期設定（クロックは 400KHz）
    i2c_init(I2C1_ID, 400*1000);
    gpio_set_function(I2C1_SDA_GPIO_GP10, GPIO_FUNC_I2C);
    gpio_set_function(I2C1_SCL_GPIO_GP11, GPIO_FUNC_I2C);
    //gpio_pull_up(I2C1_SDA_GPIO_GP10);
    //gpio_pull_up(I2C1_SCL_GPIO_GP11);

    // I2C用の設定（SPI共用のポートを設定）
    gpio_init(I2C1_ADR_GPIO_GP12);
    gpio_init(I2C1_CSN_GPIO_GP13);
    gpio_set_dir(I2C1_ADR_GPIO_GP12, GPIO_OUT);
    gpio_set_dir(I2C1_CSN_GPIO_GP13, GPIO_OUT);
    gpio_put(I2C1_ADR_GPIO_GP12, false);
    gpio_put(I2C1_CSN_GPIO_GP13, true);

    // L3GD20H設定（CTRL1レジスタ PD=1：通常モード）
    iod_i2c_gyro_write(GYRO_REG_CTRL1, 0x0F);
    // L3GD20H設定（CTRL4レジスタ FS1-FS0=10：2000dps）
    iod_i2c_gyro_write(GYRO_REG_CTRL4, 0x20);
}

void iod_i2c_gyro_deinit() {
}

void iod_i2c_gyro_reinit() {
}

void iod_i2c_gyro_main_1ms() {
}

void iod_i2c_gyro_main_in() {
    uint8_t u8a_index;

    for (u8a_index = 0;u8a_index < GYRO_GROUP_NUM; u8a_index++) {
        iod_i2c_gyro_read(acu8s_gyro_address[u8a_index], (uint8_t *)&as16s_gyro_value[u8a_index], sizeof(int16_t));
    }
}

void iod_i2c_gyro_main_out() {
}

void iod_read_gyro_x_value(int16_t *ps16a_value) {
    *ps16a_value = as16s_gyro_value[GYRO_X];
}

void iod_read_gyro_y_value(int16_t *ps16a_value) {
    *ps16a_value = as16s_gyro_value[GYRO_Y];
}

void iod_read_gyro_z_value(int16_t *ps16a_value) {
    *ps16a_value = as16s_gyro_value[GYRO_Z];
}

// 内部関数
static void iod_i2c_gyro_read(uint8_t u8a_address, uint8_t *pu8a_buffer, uint8_t u8a_size) {
    // 読み出し操作コマンド（複数バイト読み出し）
    au8s_tx_buffer[0] = u8a_address | 0x80;
    // 読み出し操作
    i2c_write_blocking(I2C1_ID, GYRO_ADDRESS, au8s_tx_buffer, 1, true);
    i2c_read_blocking(I2C1_ID, GYRO_ADDRESS, pu8a_buffer, u8a_size, false);
}

static void iod_i2c_gyro_write(uint8_t u8a_address, uint8_t u8a_data) {
    // 書き込み操作コマンド（1バイト書き込み）
    au8s_tx_buffer[0] = u8a_address;
    au8s_tx_buffer[1] = u8a_data;
    // 書き込み操作
    i2c_write_blocking(I2C1_ID, GYRO_ADDRESS, au8s_tx_buffer, 2, false);
}
