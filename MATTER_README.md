# Matter Guide

Note: This patch only supports AmebaDplus Matter.

## Build Instruction

Environment: Ubuntu 22.04 LTS

### Create and enter new directory

	mkdir dev
	cd dev

### Unzip (or copy paste) Ameba SDK into `dev` directory.

	git clone https://github.com/Ameba-AIoT/ameba-rtos.git

### Clone connectedhomeip sdk into `dev` directory 

	git clone --recurse-submodules https://github.com/project-chip/connectedhomeip.git

### Make sure sdk-RTL871xEC_v10.1e_beta and connectedhomeip are on the same directory level

	dev/
	├── ameba-rtos
	└── connectedhomeip

### Setup Ameba SDK

	cd ameba-rtos

	chmod u+x matter_setup.sh

	./matter_setup.sh

### Set Matter Build Environment

	cd connectedhomeip

	git checkout v1.4-branch

	git submodule sync
	
	git submodule update --init --recursive

	source scripts/bootstrap.sh

### Menuconfig

	cd ameba-rtos/amebadplus_gcc_project

	./menuconfig.py

(1) Under `CONFIG APPLICATION`, select `Matter Config` and set `Enable Matter`.

**Please do not change the Ameba RTOS SDK Version. You shall use development verion.**

(2) Under `CONFIG BT`, set 'Enable BT` and under `BT Example Demo`, set `BLE Matter Adapter`.

### Build all-clusters-app

	cd ameba-rtos/amebadplus_gcc_project

	./build.py -D MATTER_EXAMPLE=all_clusters

### Build lighting-app

	cd ameba-rtos/amebadplus_gcc_project

	./build.py -D MATTER_EXAMPLE=light_port

### Final Firmware

Under ameba-rtos/amebadplus_gcc_project, you will see `km4_boot_all.bin` and `km0_km4_app.bin`.
