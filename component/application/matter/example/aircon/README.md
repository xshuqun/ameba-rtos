# Aircon Example
This example is an implementation of the *Room Air Conditioner* device type. You will need a PWM fan and a temperature/humidity sensor.

| Peripheral | Pin |
| ----------- | ----------- |
| Fan | PB_18 |
| Temp/Hum Sensor | Depends on type of sensor |

## ZAP
Since there is no example ZAP file for the aircon device type, we will use `aircon-app.zap`.

## How it works
The fan can be controlled in two ways, by the Matter controller, or by external means. In this example, we only demonstrate control via Matter controller. If you wish to add more methods to control (eg. a push button), you will need to implement the `downlink` task and handler. See `lighting-app` for button example.
Thus, we only use 1 Uplink queue to for the fan to be controlled by the Matter controller.

### Peripheral Initialization
Both the initializations of the fan and the temperature/humidity sensor are handled in `matter_drivers.cpp`.

### Fan Attribute Change
Whenever the Matter controller changes the Fanmode/Fanspeed attribute of the fan, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the Fanmode/Fanspeed before updating the Fanmode/Fanspeed attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the Fanmode/Fanspeed after updating the Fanmode/Fanspeed attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

### Temperature/Humidity Sensor Attribute Change
By calling `matter_driver_temphumsensor_start`, a task will be created to poll the temperature and humidity periodically.
After obtaining the temperature and humidity measurements, the task will update the respective attributes on the Matter data model by invoking the `Set()` function. 

## How to build

### Configurations

    cd amebadplus_matter/amebadplus_gcc_project
    chmod u+x menuconfig/scripts/Menuconfig
    make menuconfig
    At Config BT, Enable BT and enable BLE_Matter_Adapter
    At Menuconfig for KM0 Config, go to Matter Config and enable Matter
    At Menuconfig for KM4 Config, go to Matter Config and enable Matter
    Save configuration

### Setup the Build Environment

    cd connectedhomeip
    source scripts/activate.sh

### Build Matter Libraries

    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make -C asdk aircon_port

### Build the Final Firmware

    cd amebadplus_matter/amebadplus_gcc_project/project_km0
    make all
    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make EXAMPLE=aircon

### Flash the Image
When the build finishes, downloading images into flash by [AmebaImageTool](tools/ameba/ImageTool/AmebaImageTool.exe):

See the ApplicationNote chapter **Image Tool** from documentation links for more details.

* Environment Requirements: EX. WinXP, Win 7 or later, Microsoft .NET Framework 4.0.
* Connect chip and PC with USB wire.
* Choose the Device profiles according to the chip you use.
* Select the corresponding serial port and transmission baud rate. The default baud rate is 1500000.
* Select the images to be programmed and set the start address and end address according to the flash layout, refer to [ameba_flashcfg.c/Flash_layout].
* Click the Download button and start. The progress bar will show the download progress of each image and the log window will show the operation status.

**Note:** For an empty chip, the bootloader and app image shall be downloaded.

### Clean Matter Libraries and Firmware

    cd amebadplus_matter/amebadplus_gcc_project/project_km0
    make clean
    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make clean
