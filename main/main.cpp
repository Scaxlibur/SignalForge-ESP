#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "AD9834.hpp"

extern "C" void app_main(void)
{

    AD9834_Init();
    uint32_t freq = 1000;
    while (true)
    {
        AD9834_Select_Wave(Sine_Wave);
        freq = 10000;
        while (freq <= 50000)
        {
            AD9834_Set_Freq(FREQ_0, freq);
            freq += 10000;
            vTaskDelay(500/ portTICK_PERIOD_MS);
        }
        AD9834_Select_Wave(Triangle_Wave);
        freq = 10000;
        while (freq <= 50000)
        {
            AD9834_Set_Freq(FREQ_0, freq);
            freq += 10000;
            vTaskDelay(500/ portTICK_PERIOD_MS);
        }
        AD9834_Select_Wave(Square_Wave);
        freq = 10000;
        while (freq <= 50000)
        {
            AD9834_Set_Freq(FREQ_0, freq);
            freq += 10000;
            vTaskDelay(500/ portTICK_PERIOD_MS);
        }
        vTaskDelay(50 / portTICK_PERIOD_MS);
    }
}