
/*
 * < CONFIG TrustZone
 */

/*
 * < CONFIG Link Option
 */
#define CONFIG_IMG1_FLASH 1
#undef  CONFIG_IMG1_SRAM
#define CONFIG_IMG2_FLASH 1
#undef  CONFIG_IMG2_PSRAM
#undef  CONFIG_PSRAM_AS_HEAP

/*
 * < CONFIG Mass Production
 */
#undef  CONFIG_MP_INCLUDED

/*
 * < CONFIG Shell CMD
 */
#define CONFIG_ATCMD_IO_LOGUART 1
#undef  CONFIG_ATCMD_IO_UART
#undef  CONFIG_LONGER_CMD

/*
 * < CONFIG VFS
 */
#define CONFIG_VFS_LITTLEFS_INCLUDED 1
#undef  CONFIG_VFS_FATFS_INCLUDED

/*
 * < CONFIG OTA OPTION
 */
#undef  CONFIG_UPGRADE_BOOTLOADER

/*
 * < CONFIG WIFI
 */
#define CONFIG_WLAN_MENU 1
#define CONFIG_KM0_NP_KM4_AP 1
#undef  CONFIG_SDIO_FULLMAC
#undef  CONFIG_HIGH_TP_TEST
#define CONFIG_INIC_INTF_IPC 1
#define CONFIG_WLAN 1
#define CONFIG_AS_INIC_NP 1
#define CONFIG_FW_DRIVER_COEXIST 1
#define CONFIG_WIFI_FW_EN 1
#undef  CONFIG_WIFI_CSI_ENABLE
#undef  CONFIG_WIFI_ANTDIV_ENABLE
#undef  CONFIG_WIFI_11K_ENABLE
#undef  CONFIG_WIFI_11V_ENABLE
#undef  CONFIG_WIFI_11R_ENABLE
#undef  CONFIG_WIFI_SPEAKER_ENABLE

/*
 * < CONFIG BT
 */
#undef  CONFIG_BT_MENU

/*
 * < CONFIG LWIP
 */
#undef  CONFIG_LWIP_DEBUG


/*
 * < MENUCONFIG FOR KM0 CONFIG
 */

/*
 * < CONFIG CHIP
 */
#define CONFIG_AMEBADPLUS 1
#define ARM_CORE_CM0 1
#undef  CONFIG_FPGA
#define CONFIG_AMEBADPLUS_A_CUT 1

/*
 * < CONFIG OS
 */
#define CONFIG_KERNEL 1
#define PLATFORM_FREERTOS 1
#define TASK_SCHEDULER_DISABLED (0)

/*
 * < CONFIG FUNCTION TEST
 */
#undef  CONFIG_PER_TEST

/*
 * < Build Option
 */
#define CONFIG_TOOLCHAIN_ASDK 1
#undef  CONFIG_TOOLCHAIN_ARM_GCC
#undef  CONFIG_LINK_ROM_LIB
#define CONFIG_LINK_ROM_SYMB 1

