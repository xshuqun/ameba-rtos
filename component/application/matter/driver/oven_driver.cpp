#include <oven_driver.h>
#include <support/logging/CHIPLogging.h>

void MatterOven::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t*) pvPortMalloc(sizeof(pwmout_t));
    mPwm_obj->pwm_idx               = 1;
    pwmout_init(mPwm_obj, pin);
}

void MatterOven::deInit(void)
{
    vPortFree(mPwm_obj);
}

void MatterOven::SetOpState(uint8_t state)
{
    if (mOpState == state)
        return;

    mOpState = state;
}

void MatterOven::SetMode(uint16_t mode)
{
    if (mMode == mode)
        return;

    mMode = mode;
}

int16_t MatterOven::GetMaxTemperature()
{
    return maxTemperature;
}

int16_t MatterOven::GetMinTemperature()
{
    return minTemperature;
}

int16_t MatterOven::GetTemperature()
{
    return localTemperature;
}

void MatterOven::SetTemperature(int16_t temp)
{
    if ((temp >= minTemperature) && (temp <= maxTemperature))
        localTemperature = temp;
    else
        ChipLogProgress(DeviceLayer, "Temperature must be set between %i and %i", minTemperature, maxTemperature);
}
