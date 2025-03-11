#include "AD9834.hpp"

/*
函数名：AD9833_Write_Data

功  能：向AD9834写入16位数据

参  数：data -- 16位数据

返回值：无
*/

void AD9834_Write_Data(unsigned int data)
{
	unsigned char i =0;
	
	AD9834_SCLK_SET;
	AD9834_FSYNC_CLR;
	
	for(i= 0;i < 16;	i++)
	{
		if(data & 0x8000)
			AD9834_SDATA_SET;
		else
			AD9834_SDATA_CLR;
		
		AD9834_SCLK_CLR;
		data <<= 1;
		AD9834_SCLK_SET;
	}
	AD9834_SDATA_SET;
	AD9834_FSYNC_SET;
}
/*
函数名：AD9834_Init

功  能：AD9834初始化

参  数：无

返回值：无
*/

void AD9834_Init(void)
{
    /*
	GPIO_InitTypeDef GPIO_InitStruct;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOC,ENABLE);
	
	GPIO_InitStruct.GPIO_Pin = AD9834_FSYNC|AD9834_SDATA|AD9834_RESET;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = AD9834_SCLK;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = AD9834_FS|AD9834_PS;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	*/
    gpio_config(&AD9834_controlPins_conf);
	
    //GPIO_ResetBits(GPIOC,AD9834_FS);
    gpio_set_level(AD9834_FS, 0);

	//GPIO_ResetBits(GPIOC,AD9834_PS);
    gpio_set_level(AD9834_PS, 0);
	
	AD9834_Write_Data(0x2100);
	AD9834_Write_Data(0x2038);
	AD9834_Write_Data(0XC000);
	AD9834_Write_Data(0x2100);
}
/*
函数名：AD9834_Select_Wave

功  能：波形选择		？？为什么要进行拉高RESET复位

参  数：initdata -- 波形数据

返回值：无
*/

void AD9834_Select_Wave(unsigned int initdata) 

{ 

    AD9834_FSYNC_SET; 

    AD9834_SCLK_SET; 

     

    AD9834_RESET_SET; 

    AD9834_RESET_SET; 

    AD9834_RESET_CLR;


    AD9834_Write_Data(initdata); 

} 

/*
函数名：AD9834_Set_Freq

功  能：频率写入

参  数：freq_number -- 频率寄存器选择
				freq -- 频率值

返回值：无
*/

void AD9834_Set_Freq(unsigned char freq_number, unsigned long freq) 
{
    unsigned long FREQREG = (unsigned long)(268435456.0/AD9834_MCLK*freq); 
    unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG; 
    unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG>>14); 
	
    if(freq_number == FREQ_0) 
    { 
        FREQREG_LSB_14BIT &= ~(1U<<15); 
        FREQREG_LSB_14BIT |= 1<<14; 
        FREQREG_MSB_14BIT &= ~(1U<<15); 
        FREQREG_MSB_14BIT |= 1<<14; 
    }
    else
    {
        FREQREG_LSB_14BIT &= ~(1<<14); 
        FREQREG_LSB_14BIT |= 1U<<15; 
        FREQREG_MSB_14BIT &= ~(1<<14); 
        FREQREG_MSB_14BIT |= 1U<<15; 
    } 
    AD9834_Write_Data(FREQREG_LSB_14BIT); 
    AD9834_Write_Data(FREQREG_MSB_14BIT);      
} 