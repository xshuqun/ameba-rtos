# Microwave Oven Example
This example is an implementation of the *Microwave Oven* device type. You can initialize any GPIO if necessary.
Note that these driver codes are meant to be just the skeleton, you should replace them and implement your own.

## ZAP
Since there is no example ZAP file for the Microwave Oven device type, we will use `microwaveoven-app.zap`.

## How it works
The Microwave Oven can be controlled in two ways, by the Matter controller, or by external means. 
In this example, we demonstrate both methods via Matter controller and external means.
If you wish to control by external means, you will need to use the `downlink` task shown in `matter_drivers.cpp`. Please feel free to add more based on your implementations. Meanwhile, controlling with Matter controller will trigger the `uplink` handler.

### Peripheral Initialization
The initializations are handled in `matter_drivers.cpp`.

### Matter Attribute Change Callback
Whenever the Matter controller changes the attribute of the LMicrowave Oven cluster, 2 types of callbacks will be invoked:
  1. MatterPreAttributeChangeCallback - Change the status/value before updating the attribute (TBD)
  2. MatterPostAttributeChangeCallback - Change the status/value after updating the attribute

These callbacks are defined in `core/matter_interaction.cpp`.
These callbacks will post an event to the uplink queue, which will be handled by `matter_driver_uplink_update_handler` in `matter_drivers.cpp`.
The driver codes will be called to carry out your actions depending on the Cluster and Attribute ID received.
You may add clusters and attributes handling in `matter_driver_uplink_update_handler` if they are not present. 

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
    make -C asdk microwaveoven_port

### Build the Final Firmware

    cd amebadplus_matter/amebadplus_gcc_project/project_km0
    make all
    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make EXAMPLE=microwaveoven

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
