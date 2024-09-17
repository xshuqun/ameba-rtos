/******************************************************************************
 * Copyright (c) 2013-2016 Realtek Semiconductor Corp.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
  ******************************************************************************
  * @file    bt_matter_adapter_wifi.h
  * @author
  * @version
  * @brief   This file provides user interface for Wi-Fi station and AP mode configuration 
  *             base on the functionalities provided by Realtek Wi-Fi driver.
  ******************************************************************************
  */
#ifndef __BT_MATTER_ADAPTER_WIFI_H_
#define __BT_MATTER_ADAPTER_WIFI_H_

/*============================================================================*
 *                              Header Files
 *============================================================================*/
#include "platform_stdlib.h"
#include "rtw_wifi_constants.h"
#include "wifi_conf.h"
#if defined(CONFIG_PLATFORM_8721D)
#include "ameba_soc.h"
#endif
// #include "wifi_structures.h"
#if defined(CONFIG_PLATFORM_8710C)
#include <platform_opts_bt.h>
#endif

#define DEBUG 0

#if DEBUG
	#define BC_DBG_PREFIX	"\n\r[BT Config Wifi][DBG] "
	#define	BC_DBG(...)		DiagPrintf(BC_DBG_PREFIX __VA_ARGS__);
#else
	#define BC_DBG(...)
#endif

#define BC_PREFIX		"\n\r[BT Config Wifi] "
#define	BC_printf(...)	DiagPrintf(BC_PREFIX __VA_ARGS__);

#define BC_BSSID_LEN					(6)
#define BC_MAX_SSID_LEN					(32)
#define BC_MAX_WIFI_SCAN_AP_NUM			(64)
#define BC_MAX_WIFI_SCAN_AP_NUM_AIRSYNC	(32)

typedef enum {
	BC_DEV_DISABLED            = 0x0,
	BC_DEV_INIT                = 0x1,
	BC_DEV_IDLE                = 0x2,
	BC_DEV_BT_CONNECTED        = 0x3,
	BC_DEV_DEINIT              = 0x4,
} BC_device_state_t;

typedef enum {
	BC_BAND_UNKNOWN       = 0x00,
	BC_BAND_2G            = 0x01,
	BC_BAND_5G            = 0x10,
	BC_BAND_2G_5G         = BC_BAND_2G | BC_BAND_5G,
} BC_band_t;

typedef enum {
	BC_STATE_DISABLED            = 0x0,
	BC_STATE_IDLE                = 0x1,
	BC_STATE_CONNECTED           = 0x2,
	BC_STATE_WRONG_PASSWORD      = 0x3,
} BC_status_t;

struct BC_wifi_scan_result {
	rtw_scan_result_t ap_info[BC_MAX_WIFI_SCAN_AP_NUM];
	uint32_t          ap_num;
};

#endif // __BT_MATTER_ADAPTER_WIFI_H_
