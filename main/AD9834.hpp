#ifndef __AD9834_HPP__
#define __AD9834_HPP__

#include "driver/spi_master.h"
#include "driver/gpio.h"



#define Sine_Wave     0x2008	//正弦波
#define Triangle_Wave 0x2002	//三角波
#define Square_Wave   0x2028	//方波

#define AD9834_MCLK 75000000UL	//AD9834晶振频率

//控制脚A3-A6，FS，PS即B0,B1直接置低，由控制位控制
#define AD9834_FSYNC  			    GPIO_NUM_11
#define AD9834_SCLK   			    GPIO_NUM_12
#define AD9834_SDATA  			    GPIO_NUM_13
#define AD9834_RESET				GPIO_NUM_14
#define AD9834_FS 					GPIO_NUM_15
#define AD9834_PS 					GPIO_NUM_16

/*
#define AD9834_FSYNC_SET	GPIO_SetBits(AD9834_Control_Port,AD9834_FSYNC)
#define AD9834_FSYNC_CLR   	GPIO_ResetBits(AD9834_Control_Port ,AD9834_FSYNC) 
#define AD9834_SCLK_SET		GPIO_SetBits(GPIOC ,AD9834_SCLK) 
#define AD9834_SCLK_CLR		GPIO_ResetBits(GPIOC ,AD9834_SCLK) 
#define AD9834_SDATA_SET   	GPIO_SetBits(AD9834_Control_Port ,AD9834_SDATA) 
#define AD9834_SDATA_CLR   	GPIO_ResetBits(AD9834_Control_Port ,AD9834_SDATA) 
#define AD9834_RESET_SET   	GPIO_SetBits(AD9834_Control_Port ,AD9834_RESET) 
#define AD9834_RESET_CLR   	GPIO_ResetBits(AD9834_Control_Port ,AD9834_RESET)
*/

#define AD9834_FSYNC_SET	gpio_set_level(AD9834_FSYNC, 1)
#define AD9834_FSYNC_CLR   	gpio_set_level(AD9834_FSYNC, 0) 
#define AD9834_SCLK_SET		gpio_set_level(AD9834_SCLK, 1) 
#define AD9834_SCLK_CLR		gpio_set_level(AD9834_SCLK, 0) 
#define AD9834_SDATA_SET   	gpio_set_level(AD9834_SDATA, 1) 
#define AD9834_SDATA_CLR   	gpio_set_level(AD9834_SDATA, 0) 
#define AD9834_RESET_SET   	gpio_set_level(AD9834_RESET, 1) 
#define AD9834_RESET_CLR   	gpio_set_level(AD9834_RESET, 0)

//频率寄存器
#define FREQ_0      0 
#define FREQ_1      1 

gpio_config_t AD9834_controlPins_conf ={
    .pin_bit_mask = (1ULL<<AD9834_FSYNC) | 
                    (1ULL<<AD9834_SCLK) |
                    (1ULL<<AD9834_SDATA) |
                    (1ULL<<AD9834_RESET) |
                    (1ULL<<AD9834_FS) |
                    (1ULL<<AD9834_PS),
    .mode = GPIO_MODE_OUTPUT,
    .pull_up_en = GPIO_PULLUP_ENABLE,
    .pull_down_en = GPIO_PULLDOWN_ENABLE,
    .intr_type = GPIO_INTR_DISABLE,
};

/* AD9834函数声明 */ 

void AD9834_Write_Data(unsigned int data);			                    //写16位数据

void AD9834_Select_Wave(unsigned int initdata) ;                        //选择输出波形 

void AD9834_Init(void);  										        //初始化配置 

void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq) ;	//选择输出寄存器和输出频率值 
	
#endif