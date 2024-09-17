# Realtek IOT AmebaDPlus sdk-ameba-rtos_v1.0a Matter SDK Readme

## Apply Ameba Patch Orders
1. sdk-ameba-rtos_v1.0a.zip
2. sdk-ameba-rtos_v1.0a_amebadplus_support_matter_v1.3.zip

Connectedhomeip SHA: 70d9a61475d31686f0fde8e7b56f352a0f59b299

## Build Instruction for Matter v1.3
Please refer to [MATTER_GENERAL_BUILD.md](MATTER_GENERAL_BUILD.md) for the general build

## Matter OTA
Please refer to [MatterOTA_guide.md](tools/matter/ota/MatterOTA_guide.md) for the Matter OTA guide.

## Matter FactoryData
Please refer to [FactoryData_guide.md](tools/matter/factorydata/FactoryData_guide.md) for the Matter Factory Data guide.

## Matter TrustZone
Please refer to [MATTER_TRUSTZONE.md](MATTER_TRUSTZONE.md) for the Matter support TrustZone build.

## Matter Example Device Types
All the examples are be available at component/application/matter/example
1. All Clusters
2. Air Conditioner
3. Air Purifier
4. Dishwasher
5. Laundrywasher
6. Light
7. Microwave Oven
8. Oven
9. Refrigerator

Each of the [examples](component/application/matter/example) has its own readme, please refer to them for build guide

## Matter AT command
AT command implementation for Matter Application is located at component/application/matter/common/atcmd/atcmd_matter.c. How to run:

    ATmatter <command>

or

    ATmatter=<command>

* ATmatter factoryreset     : to factory reset the matter application
* ATmatter queryimage       : query image for matter ota requestor app
* ATmatter applyupdate      : apply update for matter ota requestor app
* ATmatter help             : to show other matter commands
* ATmatter secureheapstatus : to check secure heap status

For IP Mode, to connect device to existing wifi, you can run this AT command:

For new AT command **(default)** :

    AT+WLCONN=ssid,<ssid name>,pw,<password>

For old AT command:

    ATW0=<ssid name>
    ATW1=<password>
    ATWC