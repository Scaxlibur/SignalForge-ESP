#pragma once

#include "driver/spi_master.h"
#include "driver/gpio.h"

typedef enum{
    Sine_Wave = 0x2008,     //正弦波
    Triangle_Wave = 0x2002,    //三角波
    Square_Wave = 0x2028,    //方波
}waveType_t;

typedef enum{
    FREQ_0 = 0,
    FREQ_1 = 1,
}freq_register_t;

#define AD9834_MCLK 75000000UL	//AD9834晶振频率

//连接引脚
#define AD9834_FSYNC  			    GPIO_NUM_4
#define AD9834_SCLK   			    GPIO_NUM_6
#define AD9834_SDATA  			    GPIO_NUM_5
#define AD9834_RESET				GPIO_NUM_20
#define AD9834_FS 					GPIO_NUM_7
#define AD9834_PS 					GPIO_NUM_8

#define AD9834_FSYNC_SET	gpio_set_level(AD9834_FSYNC, 1)
#define AD9834_FSYNC_CLR   	gpio_set_level(AD9834_FSYNC, 0) 
#define AD9834_SCLK_SET		gpio_set_level(AD9834_SCLK, 1) 
#define AD9834_SCLK_CLR		gpio_set_level(AD9834_SCLK, 0) 
#define AD9834_SDATA_SET   	gpio_set_level(AD9834_SDATA, 1) 
#define AD9834_SDATA_CLR   	gpio_set_level(AD9834_SDATA, 0) 
#define AD9834_RESET_SET   	gpio_set_level(AD9834_RESET, 1) 
#define AD9834_RESET_CLR   	gpio_set_level(AD9834_RESET, 0)

/** 
 * @brief 向AD9834写入16位数据
 * @param data 写入的数据
*/
void AD9834_Write_Data(unsigned int data);

/**
 * @brief 波形选择
 * @param waveType_t 波形种类
 */
void AD9834_Select_Wave(waveType_t waveType) ;

/**
 * @brief 初始化配置
 */
void AD9834_Init(void);

/**
 * @brief 频率写入
 * @param freq_register 频率寄存器选择
 * @param freq 频率，单位Hz
 */
void AD9834_Set_Freq(freq_register_t freq_register, uint32_t freq);
	
