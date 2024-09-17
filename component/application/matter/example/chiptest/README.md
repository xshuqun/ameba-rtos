# All-Clusters-App Example
This example is an implementation of all clusters app.

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
    make -C asdk all_clusters

### Build the Final Firmware

    cd amebadplus_matter/amebadplus_gcc_project/project_km0
    make all
    cd amebadplus_matter/amebadplus_gcc_project/project_km4
    make EXAMPLE=chiptest

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
