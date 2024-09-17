#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <basic_types.h>
#include <rtw_wifi_constants.h>
#include <wifi_intf_drv_to_app_basic.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_data_model.h>
#include <matter_data_model_presets.h>
#include <matter_drivers.h>
#include <matter_interaction.h>
#include <bridge_dm_driver.h>

#include <app/ConcreteAttributePath.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace ::chip;
using namespace ::chip::DeviceLayer;
using namespace ::chip::Platform;
using namespace ::chip::app::Clusters;

MatterBridge bridge;
Node& node = Node::getInstance();

EmberAfDeviceType gBridgedDimmableLightTypes[] = {
    { DEVICE_TYPE_LO_DIMMABLE_LIGHT, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT },
};

static void example_matter_bridge_task(void *pvParameters)
{
    while(!(wifi_is_running(WLAN0_IDX))) {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "\nBridge Dynamic Endpoint example!\n");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS

    err = matter_driver_bridge_light_init();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_driver_bridge_light_init failed!\n");

    err = matter_core_start();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_core_start failed!\n");

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!\n");

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR)
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!\n");

    vTaskDelay(1000);

    bridge.Init(node);

    if(xTaskCreate(matter_customer_bridge_code, ((const char*)"matter_customer_bridge_code"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        printf("\n\r%s xTaskCreate(matter_customer_bridge_code) failed\n", __FUNCTION__);

    vTaskDelete(NULL);
}

extern "C" void matter_bridge_add_device(void)
{
    ChipLogProgress(DeviceLayer, "Adding dimmable light [Endpoint 0x%x]", node.getNextEndpointId());
    bridge.addBridgedEndpoint(&Presets::Endpoints::matter_dimmable_light_endpoint, Span<const EmberAfDeviceType>(gBridgedDimmableLightTypes));
}

extern "C" void matter_bridge_remove_device(void)
{
    Endpoint *endpoint = NULL;
    chip::EndpointId endpointId = 2, maxEndpoint = node.getNextEndpointId() - 1;
    while ((endpoint == NULL) && (endpointId <= maxEndpoint))
    {
        endpoint = node.getEndpoint(endpointId++);
    }
    if (endpoint)
    {
        bridge.removeBridgedEndpoint(--endpointId);
        ChipLogProgress(DeviceLayer, "Removed Endpoint 0x%x", endpointId);
    }
    else
    {
        ChipLogProgress(DeviceLayer, "No bridged device!");
    }
}

extern "C" void example_matter_bridge(void)
{
    if(xTaskCreate(example_matter_bridge_task, ((const char*)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
        ChipLogProgress(DeviceLayer, "\n\r%s xTaskCreate(example_matter_light) failed", __FUNCTION__);
}