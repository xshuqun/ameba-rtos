/**
  ******************************************************************************
  * @file    inic_def.h
  * @author
  * @version
  * @date
  * @brief
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
  ******************************************************************************
  */

#ifndef INIC_DEF_H
#define INIC_DEF_H

enum INIC_WIFI_C2H_API_ID {
	INIC_API_PROCESS_DONE = 0,
	INIC_API_SCAN_USER_CALLBACK,
	INIC_API_SCAN_EACH_REPORT_USER_CALLBACK,
	INIC_API_AUTO_RECONNECT,
	INIC_API_EAP_AUTO_RECONNECT,
	INIC_API_AP_CH_SWITCH,
	INIC_API_HDL,
	INIC_API_PROMISC_CALLBACK,
	INIC_API_GET_LWIP_INFO,
	INIC_API_SET_NETIF_INFO,
	INIC_API_CFG80211_SCAN_REPORT,
	INIC_API_IP_TABLE_CHK,

	/*NAN related*/
	INIC_API_CFG80211_NAN_REPORT_MATCH_EVENT,
	INIC_API_CFG80211_NAN_DEL_FUNC,
	INIC_API_CFG80211_NAN_CFGVENDOR_EVENT,
	INIC_API_CFG80211_NAN_CFGVENDOR_CMD_REPLY,
	/*P2P related*/
	INIC_API_CFG80211_P2P_CH_RDY,
};

enum INIC_WIFI_H2C_API_ID {
	//basic
	INIC_API_WIFI_CONNECT = 1,
	INIC_API_WIFI_DISCONNECT,
	INIC_API_WIFI_IS_CONNECTED_TO_AP,
	INIC_API_WIFI_IS_RUNNING,
	INIC_API_WIFI_SET_CHANNEL,
	INIC_API_WIFI_GET_CHANNEL,
	INIC_API_WIFI_SET_USR_CFG,
	INIC_API_WIFI_GET_DISCONN_REASCON,
	INIC_API_WIFI_ON,
	INIC_API_WIFI_INIT_AP,
	INIC_API_WIFI_DEINIT_AP,
	INIC_API_WIFI_START_AP,
	INIC_API_WIFI_STOP_AP,
	INIC_API_WIFI_SCAN_NETWROKS,
	INIC_API_WIFI_GET_SCANNED_AP_INFO,
	INIC_API_WIFI_SCAN_ABORT,
	//ext
	INIC_API_WIFI_SET_MAC_ADDR,
	INIC_API_WIFI_GET_MAC_ADDR,
	INIC_API_WIFI_DRIVE_IS_MP,
	INIC_API_WIFI_GET_ASSOCIATED_CLIENT_LIST,
	INIC_API_WIFI_GET_SETTING,
	INIC_API_WIFI_SET_IPS_EN,
	INIC_API_WIFI_SET_LPS_EN,
	INIC_API_WIFI_SET_LPS_LISTEN_INTERVAL,
	INIC_API_WIFI_SET_MFP_SUPPORT,
	INIC_API_WIFI_SET_GROUP_ID,
	INIC_API_WIFI_SET_PMK_CACHE_EN,
	INIC_API_WIFI_SAE_STATUS,
	INIC_API_WIFI_FT_STATUS,
	INIC_API_WIFI_GET_SW_STATISTIC,
	INIC_API_WIFI_GET_PHY_STATISTIC,
	INIC_API_WIFI_SET_NETWORK_MODE,
	INIC_API_WIFI_SET_WPS_PHASE,
	INIC_API_WIFI_SET_GEN_IE,
	INIC_API_WIFI_SET_EAP_PHASE,
	INIC_API_WIFI_GET_EAP_PHASE,
	INIC_API_WIFI_SET_EAP_METHOD,
	INIC_API_WIFI_SEND_EAPOL,
	INIC_API_WIFI_CONFIG_AUTORECONNECT,
	INIC_API_WIFI_GET_AUTORECONNECT,
	INIC_API_WIFI_CUS_IE,
	INIC_API_WIFI_SET_IND_MGNT,
	INIC_API_WIFI_SEND_MGNT,
	INIC_API_WIFI_SET_TXRATE_BY_TOS,
	INIC_API_WIFI_SET_EDCA_PARAM,
	INIC_API_WIFI_SET_TX_CCA,
	INIC_API_WIFI_SET_CTS2SEFL_DUR_AND_SEND,
	INIC_API_WIFI_GET_ANTENNA_INFO,
	INIC_API_WIFI_GET_BAND_TYPE,
	INIC_API_WIFI_DEL_STA,
	INIC_API_WIFI_AP_CH_SWITCH,
	INIC_API_WIFI_CONFIG_CSI,
	INIC_API_WIFI_GET_CSI_REPORT,
	INIC_API_WIFI_GET_CCMP_KEY,
	//inter
	INIC_API_WIFI_COEX_SET_PTA,
	INIC_API_WIFI_SET_WPA_MODE,
	INIC_API_WIFI_SET_PMF_MODE,
	INIC_API_WIFI_COEX_BT_RFK,
	INIC_API_WIFI_COEX_ZB_RFK,
	INIC_API_WIFI_SET_BT_SEL,
	INIC_API_WIFI_ADD_KEY,
	INIC_API_WIFI_GET_CHPLAN,
	INIC_API_WPA_4WAY_FAIL,
	INIC_API_WPA_PMKSA_OPS,
	INIC_API_WIFI_SET_OWE_PARAM,

	//promisc
	INIC_API_WIFI_PROMISC_INIT,
	//nan
	INIC_API_NAN_INIT,
	INIC_API_NAN_DEINIT,
	INIC_API_NAN_START,
	INIC_API_NAN_STOP,
	INIC_API_NAN_ADD_FUNC,
	INIC_API_NAN_DEL_FUNC,
	INIC_API_NAN_CFGVENFOR,
	//p2p
	INIC_API_P2P_ROLE,
	INIC_API_P2P_REMAIN_ON_CH,
	//misc
	INIC_API_WIFI_IP_UPDATE,
	INIC_API_WIFI_IWPRIV_INFO, //dbg cmd
	INIC_API_WIFI_MP_CMD, //mp cmd
	INIC_API_WIFI_GET_NETWORK_MODE,
	INIC_API_WIFI_MSG_TO,
	INIC_API_WIFI_SPEAKER,
	INIC_API_WIFI_SET_CHPLAN,
	INIC_API_WIFI_SET_EDCCA_MODE,
	INIC_API_WIFI_GET_EDCCA_MODE,
	INIC_API_WIFI_SET_COUNTRY_CODE,
	INIC_API_WIFI_GET_COUNTRY_CODE,

	INIC_API_WAR_OFFLOAD_CTRL,
};

enum IPC_LWIP_INFO_TYPE {
	INIC_WLAN_GET_IP,
	INIC_WLAN_GET_GW,
	INIC_WLAN_GET_GWMSK,
	INIC_WLAN_GET_HW_ADDR,
	INIC_WLAN_IS_VALID_IP
};

#endif

