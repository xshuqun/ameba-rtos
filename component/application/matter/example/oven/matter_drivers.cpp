#include "matter_drivers.h"
#include "matter_interaction.h"
#include "oven_driver.h"

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <protocols/interaction_model/StatusCode.h>
#include <oven-operational-state-delegate.h>
#include <oven-modes.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OvenCavityOperationalState;
using namespace chip::app::Clusters::OvenMode;
using chip::Protocols::InteractionModel::Status;

using ErrorStateStructType = chip::app::Clusters::detail::Structs::ErrorStateStruct::Type;
using Status               = Protocols::InteractionModel::Status;

#if defined (CONFIG_AMEBASMART)
#define PWM_PIN       PA_5
#elif defined (CONFIG_AMEBALITE)
#define PWM_PIN       PA_31
#elif defined (CONFIG_AMEBADPLUS)
#define PWM_PIN       PB_18
#endif

MatterOven Oven;

CHIP_ERROR matter_driver_oven_init()
{
    Oven.Init(PWM_PIN);
    return CHIP_NO_ERROR;
}

CHIP_ERROR matter_driver_oven_set_startup_value()
{
    CHIP_ERROR err = CHIP_NO_ERROR;
    Status status;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
    
    err = OvenCavityOperationalState::GetOperationalStateInstance()->SetOperationalState(
            to_underlying(Clusters::OvenCavityOperationalState::OperationalStateEnum::kStopped));
    VerifyOrExit(err == CHIP_NO_ERROR, 
        ChipLogProgress(DeviceLayer, "Failed to set Oven Cavity State!\n"));
    Oven.SetOpState((uint8_t) Clusters::OvenCavityOperationalState::OperationalStateEnum::kStopped);
    
    status = OvenMode::Instance()->UpdateCurrentMode(to_underlying(Clusters::OvenMode::ModeTag::kBake));
    VerifyOrExit(status == Status::Success, {
        ChipLogError(DeviceLayer, "Failed to set Oven Mode!\n");
        err = CHIP_ERROR_INTERNAL;
    });
    Oven.SetMode((uint16_t) Clusters::OvenMode::ModeTag::kBake);

    status = Clusters::TemperatureControl::Attributes::MaxTemperature::Set(1, Oven.GetMaxTemperature());
    VerifyOrExit(status == Status::Success, {
        ChipLogError(DeviceLayer, "Failed to set MaxTemperature!\n");
        err = CHIP_ERROR_INTERNAL;
    });

    status = Clusters::TemperatureControl::Attributes::MinTemperature::Set(1, Oven.GetMinTemperature());
    VerifyOrExit(status == Status::Success, {
        ChipLogError(DeviceLayer, "Failed to set MinTemperature!\n");
        err = CHIP_ERROR_INTERNAL;
    });

    Oven.SetTemperature(150); // Set Oven temperature
    status = Clusters::TemperatureControl::Attributes::TemperatureSetpoint::Set(1, Oven.GetTemperature());
    VerifyOrExit(status == Status::Success, {
        ChipLogError(DeviceLayer, "Failed to set TemperatureSetpoint!\n");
        err = CHIP_ERROR_INTERNAL;
    });

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

exit:
    if (err != CHIP_NO_ERROR)
    {
        chip::DeviceLayer::PlatformMgr().UnlockChipStack();
    }
    return err;
}

void matter_driver_set_ovencavity_opstate_state_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type     = AppEvent::kEventType_Downlink_OvenCavity_State;
    downlink_event.value._u8 = (uint8_t) id; // 0: Stop; 1:Running ,2:Paused; 3: Error
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_set_ovencavity_opstate_error_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type     = AppEvent::kEventType_Downlink_OvenCavity_Error_State;
    downlink_event.value._u8 = (uint8_t) id; // 0: No Error; 1:UnableToStartOrResume ,2:UnableToCompleteOperation; 3: CommandInvalidInState
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_set_oven_mode_callback(uint32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type     = AppEvent::kEventType_Downlink_Oven_Mode;
    downlink_event.value._u16 = (uint16_t) id; // 0x4000: Bake; 0x4001: Convection; 0x4002: Grill; 0x4003: Roast;
    // 0x4004: Clean; 0x4005: ConvectionBake; 0x4006: ConvectionRoast; 0x4007: kWarming; 0x4008: Proofing;
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_set_temperature_callback(int32_t id)
{
    AppEvent downlink_event;
    downlink_event.Type     = AppEvent::kEventType_Downlink_TempControl_SetPoint;
    downlink_event.value._i16 = (int16_t) id;
    downlink_event.mHandler = matter_driver_downlink_update_handler;
    PostDownlinkEvent(&downlink_event);
}

void matter_driver_uplink_update_handler(AppEvent *event)
{
    chip::app::ConcreteAttributePath path = event->path;

    // this example only considers endpoint 1
    VerifyOrExit(event->path.mEndpointId == 1,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", path.mEndpointId));

    switch(path.mClusterId)
    {
    case Clusters::OvenCavityOperationalState::Id:
        {
            ChipLogProgress(DeviceLayer, "OvenOperationalState(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            if(path.mAttributeId == Clusters::OvenCavityOperationalState::Attributes::OperationalState::Id)
                Oven.SetOpState(event->value._u8);
        }
        break;
    case Clusters::OvenMode::Id:
        {
            ChipLogProgress(DeviceLayer, "OvenMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            if(path.mAttributeId == Clusters::OvenMode::Attributes::CurrentMode::Id)
                Oven.SetMode(event->value._u16);
        }
        break;
    case Clusters::TemperatureControl::Id:
        {
            ChipLogProgress(DeviceLayer, "TemperatureControl(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
            if(path.mAttributeId == Clusters::TemperatureControl::Attributes::TemperatureSetpoint::Id);
                Oven.SetTemperature(event->value._i16); 
        }
        break;
    default:
        break;
    }
exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *event)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();
    switch (event->Type)
    {
    case AppEvent::kEventType_Downlink_OvenCavity_State:
        {
            ChipLogProgress(DeviceLayer, "Set Oven Cavity Operational State 0x%x", event->value._u8);
            CHIP_ERROR err;
            err = OvenCavityOperationalState::GetOperationalStateInstance()->SetOperationalState(event->value._u8);
            if (err != CHIP_NO_ERROR)
            {
                ChipLogError(DeviceLayer, "Set Oven Cavity Operational State Failed!\r\n");
            }
        }
        break;
    case AppEvent::kEventType_Downlink_OvenCavity_Error_State:
        {
            ChipLogProgress(DeviceLayer, "Set Oven Cavity Operational Error State 0x%x", event->value._u8);
            ErrorStateStructType err;
            err.errorStateID = event->value._u8;
            OvenCavityOperationalState::GetOperationalStateInstance()->OnOperationalErrorDetected(err);
        }
        break;
    case AppEvent::kEventType_Downlink_Oven_Mode:
        {
            ChipLogProgress(DeviceLayer, "Set Oven Mode 0x%x", event->value._u16);
            Status status;
            status = OvenMode::Instance()->UpdateCurrentMode(event->value._u16);
            if (status != Status::Success)
            {
                ChipLogError(DeviceLayer, "Set Oven Mode Failed!\r\n");
            }
        }
        break;
    case AppEvent::kEventType_Downlink_TempControl_SetPoint:
        {
            if ((event->value._i16 >= Oven.GetMinTemperature()) && (event->value._i16 <= Oven.GetMaxTemperature()))
            {
                ChipLogProgress(DeviceLayer, "Set TemperatureSetpoint %i", event->value._i16);
                Status status;
                status = Clusters::TemperatureControl::Attributes::TemperatureSetpoint::Set(1, event->value._i16);
                if (status != Status::Success)
                {
                    ChipLogProgress(DeviceLayer, "Failed to set TemperatureSetpoint!\n");
                }
            }
            else
                ChipLogProgress(DeviceLayer, "Temperature must be set between %i and %i", Oven.GetMinTemperature(), Oven.GetMaxTemperature());
        }
        break;
    default:
        break;
    }
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
}
