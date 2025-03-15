#pragma once

#include "nvs_flash.h"
#include "AD9834.hpp"

class settingSave_class{
    private:
    nvs_handle_t nvsdataHandle;
    void dataWrite(const char* key, int16_t value);
    void dataWrite(const char* key, uint32_t value);
    void dataRead(const char* key , int16_t* out_value);
    void dataRead(const char* key , uint32_t* out_value);

    public:
    settingSave_class();
    ~settingSave_class();
    void waveType_save(waveType_t waveType);
    waveType_t waveType_read();
    void freq_save(uint32_t freq);
    uint32_t freq_read();
};