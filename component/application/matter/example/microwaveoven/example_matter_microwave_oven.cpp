#include "FreeRTOS.h"
#include "task.h"
#include "platform_stdlib.h"
#include "basic_types.h"
#include "section_config.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"
#include "chip_porting.h"
#include "matter_core.h"
#include "matter_drivers.h"
#include "matter_interaction.h"

static void example_matter_microwaveoven_task(void *pvParameters)
{
    while(!(wifi_is_running(WLAN0_IDX))) {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Microwave Oven example!\n");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS
    //
    err = matter_core_start();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_core_start failed!\n");

    err = matter_driver_microwave_oven_init();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_driver_microwave_oven_init failed!\n");

    err = matter_driver_microwave_oven_set_startup_value();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_driver_microwave_oven_set_startup_value failed!\n");

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!\n");

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!\n");

    vTaskDelete(NULL);
}

extern "C" void example_matter_microwaveoven(void)
{
    if(xTaskCreate(example_matter_microwaveoven_task, ((const char*)"example_matter_microwaveoven_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        ChipLogProgress(DeviceLayer, "\n\r%s xTaskCreate(example_matter_microwaveoven) failed", __FUNCTION__);
}

