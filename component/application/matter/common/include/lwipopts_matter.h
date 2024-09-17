/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef LWIP_HDR_LWIPOPTS_MATTER_H
#define LWIP_HDR_LWIPOPTS_MATTER_H

#include "platform_stdlib.h"
#include "basic_types.h"

#undef PBUF_POOL_SIZE
#undef PBUF_POOL_BUFSIZE
#undef  MEMP_NUM_SYS_TIMEOUT
#define PBUF_POOL_SIZE              20
#define PBUF_POOL_BUFSIZE           1500
#define MEMP_NUM_SYS_TIMEOUT        14

#undef LWIP_TCPIP_CORE_LOCKING
#undef LWIP_SOCKET_SET_ERRNO
#define LWIP_TCPIP_CORE_LOCKING     1
#define LWIP_SOCKET_SET_ERRNO       1
#define LWIP_COMPAT_MUTEX_ALLOWED   1
#define LWIP_IPV6_ND                1
#define LWIP_IPV6_SCOPES            0
#define LWIP_PBUF_FROM_CUSTOM_POOLS 0

#undef LWIP_IPV6
#define LWIP_IPV6                   1
#define LWIP_IPV6_MLD               1
#define LWIP_IPV6_AUTOCONFIG        1
#define LWIP_ICMP6                  1
#define LWIP_IPV6_DHCP6             1

#undef  MEMP_NUM_SYS_TIMEOUT
#undef  MEMP_NUM_MLD6_GROUP
#define MEMP_NUM_SYS_TIMEOUT        13
#define MEMP_NUM_MLD6_GROUP         6

#endif /* LWIP_HDR_LWIPOPTS_MATTER_H */
