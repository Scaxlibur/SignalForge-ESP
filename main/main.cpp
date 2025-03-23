#include "settingSave.hpp"
#include "wave_gen.hpp"
#include "AD5626.h"

typedef struct{
    WAVE_TYPE_t wavetype;
    uint16_t freq;
}waveSetting;

TaskHandle_t wavegen_task_handle = NULL;
TaskHandle_t settingSave_task_handle = NULL;

QueueHandle_t wavegen_settingSave_com_handle = NULL;    //波形发生器和掉电存储的通信队列

void settingSave_task(void *arg){
    settingSave_class settingSave;
    while (true)
    {
        /* code */
    }
    
}

IRAM_ATTR void wavegen_task(void *arg){
    const gpio_num_t _mosiPIN = GPIO_NUM_25;
    const gpio_num_t _SCLKpin = GPIO_NUM_5;
    const gpio_num_t _SYNCpin = GPIO_NUM_2;
    const gpio_num_t _LDACpin = GPIO_NUM_21;
    spi_host_device_t _SPI_HOST = SPI2_HOST;
    AD5626_init(_mosiPIN, _SCLKpin, _SYNCpin, _LDACpin, _SPI_HOST);
    //WAVE_GEN wavegen(6, 0, 50, 10000, SIN);
    //wavegen.initTimer();
    //wavegen.updateTimer();
    //wavegen.waveGen(wavegen.wave_type);
    static DRAM_ATTR uint16_t waveTab[SAMPLE_PER_CYCLE];
    static DRAM_ATTR uint16_t waveTab1[SAMPLE_PER_CYCLE];
    double sineValue = 0.0;
    for (int i = 0; i < SAMPLE_PER_CYCLE; i++)
        {
        // sineValue = sin(((2 * PI) / SAMPLE_PER_CYCLE) * i) * (uMaxValue / 2) + offSetValue;
        sineValue = sin(((2 * PI) / SAMPLE_PER_CYCLE) * i) + 1; //这里出来的范围是0-2

        // waveTab1[i] = (((int)(sineValue * ADC_MAX_VALUE / ADC_MAX_VOLTAGE)));
        waveTab1[i] = ((uint16_t)(sineValue * (ADC_MAX_VALUE / 4)+1024));//控制到1020-3080

        }
    for (int i = 0; i < SAMPLE_PER_CYCLE; i++)
    {
        if (waveTab1[i] > ADC_MAX_VALUE)
        {
        waveTab1[i] = ADC_MAX_VALUE;
        }
        if (waveTab1[i] < 0)
        {
        waveTab1[i] = 0;
        }
        waveTab[i] = waveTab1[i];
    }
    static DRAM_ATTR uint16_t waveindex = 0;
    while (true)
    {
        if (waveindex >= SAMPLE_PER_CYCLE)
        {
            waveindex = 0;
        }
        AD5626_writeValue(waveTab[waveindex]);
        waveindex += 1;
        //AD5626_writeValue((uint16_t) 3072u);
        //vTaskDelay(100 / portTICK_PERIOD_MS);
        //vTaskDelay(100 / portTICK_PERIOD_MS);
        //vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    xTaskCreatePinnedToCore(wavegen_task, "wavegen_task", 64 * 1024, NULL, 20, &wavegen_task_handle, 0);
    // xTaskCreate(settingSave_task, "settingSave_task", 1 * 1024, NULL , 10, &settingSave_task_handle);
    
}