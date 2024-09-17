#pragma once

#include <FreeRTOS.h>
#include <platform_stdlib.h>
#include "pwmout_api.h"

class MatterOven
{
public:
    void Init(PinName pin);
    void deInit(void);
    void SetOpState(uint8_t state);
    void SetMode(uint16_t mode);
    int16_t GetMaxTemperature();
    int16_t GetMinTemperature();
    int16_t GetTemperature();
    void SetTemperature(int16_t temp);

private:
    pwmout_t *mPwm_obj = NULL;
    uint8_t mOpState;
    uint16_t mMode;
    int16_t localTemperature;
    int16_t maxTemperature = 200;
    int16_t minTemperature = 100;
};
