# Matter Trust Zone Support

## Introduction

The Matter Trust Zone Support is enhanced security feature for Matter Project. It will locate the DAC private key **(kSecureDacPrivateKey)**, DAC keypair **(DacKey)**, and Node Operational Keypair **(OpKey)** within the secure world region. This implementation can be found in the component/ssl/mbedtls-2.28.1/library_s/mbedtls_nsc.c. This feature is also activated together with ameba factory data and ameba crypto.

## Project Configurations

### Factory Data Configurations

Please refer to [Ameba Factory Data](tools/matter/factorydata/FactoryData_guide.md) on how to create the factory data and flashing it to the device.

### E-fuse Configurations

Before flashing the image, run the following command in the AT command:

    efuse rmap

Check the value in 0x3, by default it should be `e0`. After that, run the following command in the AT command:

    efuse wmap 0x3 1 f0

It will print out succesfully written the value to the efuse map. You can run `efuse rmap` to check if the value is updated to f0. If you reset the device, it will fail to boot the non TrustZone image with the message:

    OTA Certificate & IMG2 invalid, boot fail!

**For development phase, there is no need to update the efuse raw values and [manifest.json](amebadplus_gcc_project/manifest.json). Please refer to the Application Note for more details.**

If you want to run non trustzone image back again, run the following command in the AT command:

    efuse wmap 0x3 1 e0

It will print out succesfully written the value to the efuse map. You can run `efuse rmap` to check if the value is updated to e0. If you reset the device, it will fail to boot the TrustZone image with the message:

    RDP Shall En when TZ Configure

### Ameba Configurations

    cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project

    make menuconfig

    Under general config, go to CONFIG Trustzone, Enable TrustZone, use RDP_BASIC

    At Menuconfig for KM4 Config, go to Matter Config, under enable Matter, enable Matter Secure

    Save configuration

    Please also update the kSecureDacPrivateKey (located in mbedtls_nsc.c) based on your generated DAC private key!

## Make project_km0

	cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project/project_km0

	make all

## Make Matter Libraries

	cd sdk-ameba-rtos_v1.0a/amebadplus_gcc_project/project_km4

	make -C asdk all_clusters

## Make project_km4

in the same project_km4 directory,

	make EXAMPLE=chiptest

