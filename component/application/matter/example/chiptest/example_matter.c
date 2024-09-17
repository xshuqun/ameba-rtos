#include <platform_stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "rtw_wifi_constants.h"
#include "wifi_intf_drv_to_app_basic.h"

extern void ChipTest(void);

static void example_matter_task_thread(void *pvParameters)
{
    while(!(wifi_is_running(WLAN0_IDX))) {
        //waiting for Wifi to be initialized
    }

    ChipTest();
    DiagPrintf("MATTER TEST\n");

    vTaskDelete(NULL);
    return;
}

void example_matter_task(void)
{
    if (xTaskCreate(example_matter_task_thread, ((const char *)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		DiagPrintf("\n\r%s xTaskCreate(example_matter_task_thread) failed", __FUNCTION__);
	}
}
