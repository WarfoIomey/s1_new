/* сборка IwIP cчитывание с Eth  */

#ifndef INC_LWIP_H_
#define INC_LWIP_H_

#ifdef __cplusplus
 extern "C" {
#endif

 //IP адрес сервера
#define DEST_IP_ADDR0   (uint8_t) 	192
#define DEST_IP_ADDR1   (uint8_t) 	168
#define DEST_IP_ADDR2   (uint8_t) 	1
#define DEST_IP_ADDR3   (uint8_t) 	105

#define DEST_PORT       (uint16_t) 7

//IP устройства
 #define IP_ADDR0   	(uint8_t)  	192
 #define IP_ADDR1   	(uint8_t)  	168
 #define IP_ADDR2   	(uint8_t)  	1
 #define IP_ADDR3   	(uint8_t)  	50

//маска устройства
 #define NETMASK_ADDR0  (uint8_t) 	255
 #define NETMASK_ADDR1  (uint8_t) 	255
 #define NETMASK_ADDR2  (uint8_t) 	255
 #define NETMASK_ADDR3 	(uint8_t)  	0
//шлюз устройства
 #define GW_ADDR0  		(uint8_t) 	192
 #define GW_ADDR1  		(uint8_t) 	168
 #define GW_ADDR2  		(uint8_t) 	1
 #define GW_ADDR3   	(uint8_t) 	1


#include "lwip/opt.h"
#include "lwip/mem.h"
#include "lwip/memp.h"
#include "netif/etharp.h"
#include "lwip/dhcp.h"
#include "lwip/netif.h"
#include "lwip/timeouts.h"
#include "ethernetif.h"
#include "lwip/ip_addr.h"
#include "lwip/tcp.h"
//#include "lwip/lwip_timers.h"

#if WITH_RTOS
#include "lwip/tcpip.h"
#endif


void MX_LWIP_Init(void);
#if WITH_RTOS
#else
/* Функция, определенная в lwip.c для:
 * - Считывает принятый пакет из буферов Ethernet
 * - Отправьте его в стек LwIP для обработки
 * - Обрабатывать тайм-ауты, если NO_SYS_NO_TIMERS не установлено
 */

void MX_LWIP_Process(void);
#endif /* WITH_RTOS */

#ifdef __cplusplus
}
#endif
#endif /* INC_LWIP_H_ */

