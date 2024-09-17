# Bridge-app using Ameba Data Model Example
This example is an implementation of the *Bridge* device type. The Bridge will be communicating with the other non-Matter peripherals via TCP sockets.
You will need 2 non-Matter peripherals running TCP client socket.

## ZAP
We will use `bridge-app.zap` instead of the zap file within the connectedhomeip example.

## Ameba Data Model
This example demonstrates adding and removing endpoints dynamically using the *Ameba Data Model*.
A `Root Node` device type will be created on Endpoint0, a `Aggregator` device type on Endpoint1. User can add or remove dimmable light using the related AT command.

Additionally a new thread will be created for user to input their code to communicate with non-matter device based on the protocol (e.g., IP-based (TCP,UDP), BLE, zigbee and etc) they wish to use.

## Related AT commands

You can run `ATmatter bridge_dm_add` to add one dummy bridged device dynamic endpoint. You can also run `ATmatter bridge_dm_remove` to remove one dummy bridged device dynamic endpoint.

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
    make -C asdk bridge_dm

### Build the Final Firmware

    cd amebadplus_matter/amebadplus_gcc_project/project_km0
    make all
    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make EXAMPLE=bridge_dm

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