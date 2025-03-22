#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   初始化函数，填入相应引脚参数即可
 * @param   _MOSIpin    芯片DI引脚，即数据输入引脚
 * @param   _SCLKpin    时钟引脚
 * @param   _SYNCpin    片选引脚
 * @param   _LDACpin    DAC异步更新引脚
 * @param   _SPI_HOST   使用的SPI外设
 */
void AD5626_init(gpio_num_t _MOSIpin  , gpio_num_t _SCLKpin, gpio_num_t _SYNCpin, gpio_num_t _LDACpin, spi_host_device_t _SPI_HOST);

/**
 * @brief   输出值设置
 * @param   value     电压值
 * @note    范围是0-4095
 * @note    作者使用双极性输出，0x000是正最大量程，0xfff是负最大量程
 */
void AD5626_writeValue(uint16_t value);

/**
 * @brief   发送前回调函数，拉高LDAC
 * @param   *spi_trans  不要更改，和spi传递的结构体相同即可
 */
void setLDAC(spi_transaction_t *spi_trans);

/**
 * @brief   发送后回调函数，拉低LDAC
 * @param   *spi_trans  不要更改，和spi传递的结构体相同即可
 */
void resetLDAC(spi_transaction_t *spi_trans);

#ifdef __cplusplus
}
#endif