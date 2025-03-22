#include "AD5626.h"

 gpio_num_t LDAC_pin;
 spi_transaction_t spi_trans;
 spi_host_device_t spi_host;
 spi_bus_config_t spi_bus_config;
 spi_device_interface_config_t interface_config;
 gpio_config_t LDAC_config;
 spi_device_handle_t AD5626handle = NULL;
 uint8_t txbuffer[2];

void setLDAC(spi_transaction_t *spi_trans){
    gpio_set_level(LDAC_pin, 1);
}

void resetLDAC(spi_transaction_t *spi_trans){
    gpio_set_level(LDAC_pin, 0);
}


void AD5626_writeValue(uint16_t value){
    txbuffer[0] = (uint8_t)(value >> 8);
    txbuffer[1] = (uint8_t)(value >> 0); 
    spi_trans.length = 16;
    spi_trans.tx_buffer = txbuffer;
    spi_device_polling_transmit(AD5626handle, &spi_trans);
}


void AD5626_init(gpio_num_t _MOSIpin  , gpio_num_t _SCLKpin, gpio_num_t _SYNCpin, gpio_num_t _LDACpin, spi_host_device_t _SPI_HOST){
    spi_bus_config.mosi_io_num = _MOSIpin;
    spi_bus_config.miso_io_num = -1;
    spi_bus_config.sclk_io_num = _SCLKpin;
    spi_bus_config.flags = SPICOMMON_BUSFLAG_MASTER;

    interface_config.command_bits = 0;
    interface_config.address_bits = 0;
    interface_config.mode = 3;                              //手册要求
    interface_config.clock_speed_hz = SPI_MASTER_FREQ_8M;
    interface_config.spics_io_num = _SYNCpin;
    interface_config.queue_size = 6;
    interface_config.pre_cb = setLDAC;
    interface_config.post_cb = resetLDAC;

    LDAC_pin = _LDACpin;
    LDAC_config.pin_bit_mask = (1ULL << _LDACpin),
    LDAC_config.mode = GPIO_MODE_OUTPUT,
    LDAC_config.pull_up_en = GPIO_PULLUP_ENABLE,
    LDAC_config.pull_down_en = GPIO_PULLDOWN_ENABLE,
    LDAC_config.intr_type = GPIO_INTR_DISABLE,

    spi_host = _SPI_HOST;

    ESP_ERROR_CHECK(spi_bus_initialize(spi_host, &spi_bus_config, SPI_DMA_CH_AUTO));
    ESP_ERROR_CHECK(spi_bus_add_device(spi_host, &interface_config, &AD5626handle));
    ESP_ERROR_CHECK(gpio_config(&LDAC_config));

}

