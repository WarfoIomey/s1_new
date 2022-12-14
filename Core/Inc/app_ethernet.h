/* настройка dhcp соединения */

#ifndef INC_APP_ETHERNET_H_
#define INC_APP_ETHERNET_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "lwip/netif.h"

#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

 void User_notification(struct netif *netif);
 #ifdef USE_DHCP
 void DHCP_thread(void const * argument);
 #endif

 #ifdef __cplusplus
 }
 #endif

#endif
