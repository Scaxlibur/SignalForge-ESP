#include "settingSave.hpp"

settingSave_class::settingSave_class(){
    
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}

void settingSave_class::dataWrite(const char* key, int16_t value){

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsdataHandle);
    // Write
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_set_i16(nvsdataHandle, key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvsdataHandle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(nvsdataHandle);

}

void settingSave_class::dataWrite(const char* key, uint32_t value){

    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsdataHandle);
    // Write
    printf("\n");
    printf("Opening Non-Volatile Storage (NVS) handle... ");
    err = nvs_set_u32(nvsdataHandle, key, value);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvsdataHandle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(nvsdataHandle);

}

void settingSave_class::dataRead(const char* key , int16_t* out_value){
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsdataHandle);
    // Read
    printf("Reading restart counter from NVS ... ");
    err = nvs_get_i16(nvsdataHandle, key, out_value);
    switch (err) {
        case ESP_OK:
            printf("Done\n");
            //printf("%hn\n", out_value);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvsdataHandle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(nvsdataHandle);
}

void settingSave_class::dataRead(const char* key , uint32_t* out_value){
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &nvsdataHandle);
    // Read
    printf("Reading restart counter from NVS ... ");
    err = nvs_get_u32(nvsdataHandle, key, out_value);
    switch (err) {
        case ESP_OK:
            printf("Done\n");
            //printf("%ln\n", out_value);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            printf("The value is not initialized yet!\n");
            break;
        default :
            printf("Error (%s) reading!\n", esp_err_to_name(err));
    }

    printf("Committing updates in NVS ... ");
    err = nvs_commit(nvsdataHandle);
    printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
    nvs_close(nvsdataHandle);
}

void settingSave_class::waveType_save(waveType_t waveType){
    int16_t waveSave = 0;
    switch (waveType)
    {
    case 0x2008:
        waveSave = 0;   //正弦
        break;
    case 0x2002:
        waveSave = 1;   //三角
        break;
    case 0x2028:
        waveSave = 2;   //方波
        break;
    }
    dataWrite("waveType", waveSave);
}

waveType_t settingSave_class::waveType_read(){
    waveType_t waveType = Sine_Wave;    //默认是正弦波
    int16_t waveSave = 0;
    dataRead("waveType", &waveSave);
    switch (waveSave)
    {
    case 0:
        waveType = Sine_Wave;   //正弦
        break;
    case 1:
        waveType = Triangle_Wave;   //三角
        break;
    case 2:
        waveType = Square_Wave;   //方波
        break;
    }
    return waveType;
}

void settingSave_class::freq_save(uint32_t freq){
    dataWrite("freq", freq);
}


uint32_t settingSave_class::freq_read(){
    uint32_t freq = 10000;
    dataRead("freq", &freq);
    return freq;
}
