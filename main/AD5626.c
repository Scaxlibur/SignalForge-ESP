#include "AD5626.h"

 gpio_num_t LDAC_pin;
 spi_transaction_t spi_trans;
 spi_host_device_t spi_host;
 spi_bus_config_t spi_bus_config;
 spi_device_interface_config_t interface_config;
 gpio_config_t LDAC_config;
 spi_device_handle_t AD5626handle = NULL;
 uint8_t txbuffer[2];

 dedic_gpio_bundle_handle_t LDAC_asm_handle = NULL;
 uint32_t LDACmask;

 int output_array[1];    //LDACpin
 dedic_gpio_bundle_config_t LDAC_asm_config = {
    .gpio_array = output_array,
    .array_size = 1,
    .flags = {
        .out_en = 1
    }
 };
 
 void GPIO_bundle_init(){
    output_array[0] = LDAC_pin;
    ESP_ERROR_CHECK(dedic_gpio_new_bundle(&LDAC_asm_config, &LDAC_asm_handle));
    dedic_gpio_get_out_mask(LDAC_asm_handle, &LDACmask);
 }

IRAM_ATTR void setLDAC_asm(spi_transaction_t *spi_trans){
    dedic_gpio_cpu_ll_write_mask(LDACmask, 1);
}

IRAM_ATTR void resetDAC_asm(spi_transaction_t *spi_trans){
    dedic_gpio_cpu_ll_write_mask(LDACmask, 0);
}

void setLDAC(spi_transaction_t *spi_trans){
    gpio_set_level(LDAC_pin, 1);
}

void resetLDAC(spi_transaction_t *spi_trans){
    gpio_set_level(LDAC_pin, 0);
}

IRAM_ATTR void AD5626_writeValue(uint16_t value){
    //txbuffer[0] = (uint8_t)(value >> 8);
    //txbuffer[1] = (uint8_t)(value >> 0); 
    spi_trans.flags = SPI_TRANS_USE_TXDATA;
    spi_trans.length = 16;
    //spi_trans.tx_buffer = txbuffer;
    spi_trans.tx_data[0] = (uint8_t)(value >> 8);
    spi_trans.tx_data[1] = (uint8_t)(value >> 0); 
    spi_device_polling_transmit(AD5626handle, &spi_trans);
}


void AD5626_init(gpio_num_t _MOSIpin  , gpio_num_t _SCLKpin, gpio_num_t _SYNCpin, gpio_num_t _LDACpin, spi_host_device_t _SPI_HOST){
    spi_bus_config.mosi_io_num = _MOSIpin;
    spi_bus_config.miso_io_num = -1;
    spi_bus_config.sclk_io_num = _SCLKpin;
    spi_bus_config.flags = SPICOMMON_BUSFLAG_MASTER;
    spi_bus_config.isr_cpu_id = ESP_INTR_CPU_AFFINITY_0;    //中断cpu亲和性为cpu0

    interface_config.command_bits = 0;
    interface_config.address_bits = 0;
    interface_config.mode = 3;                              //手册要求
    // interface_config.clock_speed_hz = SPI_MASTER_FREQ_20M;
    interface_config.clock_speed_hz = SPI_MASTER_FREQ_40M;
    interface_config.clock_source = SOC_MOD_CLK_SPLL;
    interface_config.spics_io_num = _SYNCpin;
    interface_config.queue_size = 1024;
    interface_config.pre_cb = setLDAC_asm;
    interface_config.post_cb = resetDAC_asm;

    LDAC_pin = _LDACpin;
    LDAC_config.pin_bit_mask = (1ULL << _LDACpin),
    LDAC_config.mode = GPIO_MODE_OUTPUT,
    LDAC_config.pull_up_en = GPIO_PULLUP_ENABLE,
    LDAC_config.pull_down_en = GPIO_PULLDOWN_ENABLE,
    LDAC_config.intr_type = GPIO_INTR_DISABLE,

    spi_host = _SPI_HOST;

    ESP_ERROR_CHECK(spi_bus_initialize(spi_host, &spi_bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(spi_host, &interface_config, &AD5626handle));
    //ESP_ERROR_CHECK(gpio_config(&LDAC_config));
    GPIO_bundle_init();
    spi_device_acquire_bus(AD5626handle, portMAX_DELAY);
}

