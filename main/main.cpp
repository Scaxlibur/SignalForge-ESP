#include "settingSave.hpp"
#include "wave_gen.hpp"
#include "AD5626.h"

typedef struct{
    WAVE_TYPE_t wavetype;
}waveSetting;

TaskHandle_t wavegen_task_handle = NULL;

void wavegen_task(void *arg){
    const gpio_num_t _mosiPIN = GPIO_NUM_25;
    const gpio_num_t _SCLKpin = GPIO_NUM_5;
    const gpio_num_t _SYNCpin = GPIO_NUM_2;
    const gpio_num_t _LDACpin = GPIO_NUM_21;
    spi_host_device_t _SPI_HOST = SPI2_HOST;
    AD5626_init(_mosiPIN, _SCLKpin, _SYNCpin, _LDACpin, _SPI_HOST);
    WAVE_GEN wavegen(6, 0, 50, 1000, SIN);
    wavegen.initTimer();
    wavegen.updateTimer();
    while (true)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

extern "C" void app_main(void)
{
    xTaskCreate(wavegen_task, "wavegen_task", 64 * 1024, NULL, 20, &wavegen_task_handle);
    
}