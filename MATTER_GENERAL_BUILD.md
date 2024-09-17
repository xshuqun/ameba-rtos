# Matter (previously CHIP) on AmebaDPlus

## Reminder

### For v1.3.0.0 onwards, all-clusters-app.zap location change

If you need to change all-clusters-app.zap, please change it in the following directory:

	component/application/matter/example/chiptest/all-clusters-app.zap

This is because we have added all files required for all supported clusters in `component/application/matter/project/amebadplus/make/chip_main/all_clusters_app/Makefile`, while the default all-clusters-app.zap in connectedhomeip does not include some clusters supported by Ameba. Therefore, build errors occur.

The only solution is to add Ameba's all-clusters-app.zap. 

In `component/application/matter/project/amebadplus/Makefile`, the default all-clusters-app.zap in connectedhomeip will be replaced by Ameba's all-clusters-app.zap

	@cp $(MATTER_DIR)/example/chiptest/all-clusters-app.zap $(ALL_CLUSTERS_ZAP)

## Get Ameba SDK & Matter SDK

	Tested on Ubuntu 22.04

Create a common directory for Ameba and Matter SDK

	mkdir dev
	cd dev

To check out Matter repository:

	git clone --recurse-submodules https://github.com/project-chip/connectedhomeip.git

	cd connectedhomeip

	git checkout 70d9a61475d31686f0fde8e7b56f352a0f59b299 #v1.3-release

Make sure sdk-ameba-rtos_v1.0a and connectedhomeip are on the same directory level

	dev/
	├── sdk-ameba-rtos_v1.0a
	└── connectedhomeip

## Set Matter Build Environment 

	> Follow below guide to install prerequisites
	> https://github.com/project-chip/connectedhomeip/blob/master/docs/guides/BUILDING.md

	cd sdk-ameba-rtos_v1.0a

	chmod u+x matter_setup.sh
	
	./matter_setup.sh

	cd connectedhomeip

	git submodule sync

	git submodule update --init --recursive

	source scripts/bootstrap.sh

	source scripts/activate.sh

## Project Configurations

### Matter Configurations

#### BLE Mode (default)

1. In `connectedhomeip/config/ameba/args.gni`

    - Set `chip_config_network_layer_ble = true`

2. In `connectedhomeip/src/platform/Ameba/CHIPDevicePlatformConfig.h`

    - Set `#define CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE 1`

#### IP Mode (without BLE)

1. In `connectedhomeip/config/ameba/args.gni`

    - Set `chip_config_network_layer_ble = false`

2. In `connectedhomeip/src/platform/Ameba/CHIPDevicePlatformConfig.h`

    - Set `#define CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE 0`

### Ameba Configurations

    cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project

    chmod u+x menuconfig/scripts/Menuconfig

    make menuconfig

    For BLE Mode, at Config BT, Enable BT and enable BLE_Matter_Adapter

    At Menuconfig for KM0 Config, go to Matter Config and enable Matter

    At Menuconfig for KM4 Config, go to Matter Config and enable Matter

    Save configuration

## Make project_km0

	cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project/project_km0

	make all

## Make Matter Libraries

	cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project/project_km4

	make -C asdk all_clusters

## Make project_km4

in the same project_km4 directory,

	make EXAMPLE=chiptest

## Notes

1. Please try to `make clean` if faced with any build issue
2. If met with issue during `Set Matter Build Environment` (e.g., pw command issue), please remove the environment with `rm -rf .environment/*` in connectedhomeip directory and `source scripts/bootstrap.sh` again.
3. Some bash scripts may require permission to run, enter the following command to make it executable: `chmod u+x <path to script>`
