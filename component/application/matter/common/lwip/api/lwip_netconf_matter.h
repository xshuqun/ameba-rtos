/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MATTER_NETCONF_H
#define __MATTER_NETCONF_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "rtw_wifi_constants.h"
#include "lwipconf.h"
#include "lwip/netifapi.h"
#include "lwip/dhcp6.h"
#include "lwip/prot/dhcp6.h"

// macros
/* Give default value if not defined */
#define IP_ADDR_INVALID 0x00000000

/* Private typedef -----------------------------------------------------------*/

#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 1
#if LWIP_IPV6
typedef enum {
	DHCP6_START=0,
	DHCP6_WAIT_ADDRESS,
	DHCP6_ADDRESS_ASSIGNED,
	DHCP6_RELEASE_IP,
	DHCP6_STOP,
	DHCP6_TIMEOUT
} DHCP6_State_TypeDef;
#endif
#endif

/* Extern functions ------------------------------------------------------------*/
#ifndef CONFIG_AS_INIC_AP
void rtw_wakelock_timeout(uint32_t timeoutms);
#endif
/** Release a DHCP lease. */
err_t dhcp_release_unicast(struct netif *netif);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 1
#if LWIP_IPV6
uint8_t LwIP_DHCP6(uint8_t idx, uint8_t dhcp6_state);
uint8_t* LwIP_GetIPv6_linklocal(uint8_t idx);
uint8_t* LwIP_GetIPv6_global(uint8_t idx);
extern struct netif xnetif[];
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MATTER_NETCONF_H */

