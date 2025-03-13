#include "AD9834.hpp"

void AD9834_Write_Data(unsigned int data)
{

	unsigned char i = 0;

	AD9834_SCLK_SET;
	AD9834_FSYNC_CLR;

	for (i = 0; i < 16; i++)
	{
		if (data & 0x8000)
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

void AD9834_Init(void)
{
	gpio_config_t AD9834_controlPins_conf = {
		.pin_bit_mask = (1ULL << AD9834_FSYNC) |
						(1ULL << AD9834_SCLK) |
						(1ULL << AD9834_SDATA) |
						(1ULL << AD9834_RESET) |
						(1ULL << AD9834_FS) |
						(1ULL << AD9834_PS),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_ENABLE,
		.pull_down_en = GPIO_PULLDOWN_ENABLE,
		.intr_type = GPIO_INTR_DISABLE,
	};
	gpio_config(&AD9834_controlPins_conf);
	gpio_set_level(AD9834_FS, 0);
	gpio_set_level(AD9834_PS, 0);

	AD9834_Write_Data(0x2100);
	AD9834_Write_Data(0x2038);
	AD9834_Write_Data(0XC000);
	AD9834_Write_Data(0x2100);
}

void AD9834_Select_Wave(waveType_t waveType)

{
	AD9834_FSYNC_SET;
	AD9834_SCLK_SET;
	AD9834_RESET_SET;
	AD9834_RESET_SET;
	AD9834_RESET_CLR;
	// unsigned int initdata = waveType;
	AD9834_Write_Data(waveType);
}

void AD9834_Set_Freq(freq_register_t freq_register, uint32_t freq)
{
	unsigned long FREQREG = (unsigned long)(268435456.0 / AD9834_MCLK * freq);
	unsigned int FREQREG_LSB_14BIT = (unsigned int)FREQREG;
	unsigned int FREQREG_MSB_14BIT = (unsigned int)(FREQREG >> 14);

	if (freq_register == FREQ_0)
	{
		FREQREG_LSB_14BIT &= ~(1U << 15);
		FREQREG_LSB_14BIT |= 1 << 14;
		FREQREG_MSB_14BIT &= ~(1U << 15);
		FREQREG_MSB_14BIT |= 1 << 14;
	}
	else
	{
		FREQREG_LSB_14BIT &= ~(1 << 14);
		FREQREG_LSB_14BIT |= 1U << 15;
		FREQREG_MSB_14BIT &= ~(1 << 14);
		FREQREG_MSB_14BIT |= 1U << 15;
	}
	AD9834_Write_Data(FREQREG_LSB_14BIT);
	AD9834_Write_Data(FREQREG_MSB_14BIT);
}