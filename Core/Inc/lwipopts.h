/*настройка LwIP*/

#ifndef INC_LWIPOPTS_H_
#define INC_LWIPOPTS_H_

#include "stm32f1xx_hal.h"

#define WITH_RTOS 1
#define NO_SYS                  0		// есть два режима работы с ОС и без "NO-SYS" без ос  OS mode с ОС
//#define SYS_LIGHTWEIGHT_PROT    0		// отключение межзадачной защиты

/* ---------- настройка памяти ---------- */
#define MEM_ALIGNMENT           4		// макрос подстройки бита под размер вывода
#define MEM_SIZE        (10*1024)		// размер памяти
#define MEMP_NUM_PBUF           10		// размер буфера
#define MEMP_NUM_UDP_PCB        6		// кодичество одновременных связей
#define MEMP_NUM_TCP_PCB        10
#define MEMP_NUM_TCP_PCB_LISTEN 5		// количество прослушиваний по ТСР связей
#define MEMP_NUM_TCP_SEG        8		// количество одновременно поставленных в очередь TCP сегменты.
#define MEMP_NUM_SYS_TIMEOUT    10		// количество одновременно активных тайм-ауты.

/* ---------- буферный пул для кэширования ---------- */
#define PBUF_POOL_SIZE          8		// количество буферов в пуле pbuf
#define PBUF_POOL_BUFSIZE       512		// размер каждого pbuf в пуле pbuf.

/* ---------- настройка ТСР ---------- */
#define LWIP_TCP                1							// настройка тср канала
#define TCP_TTL                 255							// время жизни ип пакета
#define TCP_QUEUE_OOSEQ         0							// Определяет, должен ли TCP ставить в очередь сегменты, поступающие не по порядку. Установите значение 0, если на вашем устройстве мало памяти. */
#define TCP_MSS                 (1500 - 40)	  				// Максимальный размер сегмента TCP. определяется TCP_MSS = (Ethernet MTU (максимальный размер пакета) - IP header size (рамер ип) -  TCP header size (размер настройки тср)) */
#define TCP_SND_BUF             (4*TCP_MSS)					// Пространство буфера TCP-отправителя (байты).
#define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS) 	// буферное пространство TCP-отправителя (pbufs). Это должно быть по крайней мере столько же, сколько (2 * TCP_SND_BUF/TCP_MSS), чтобы все работало. */
#define TCP_WND                 (2*TCP_MSS)					// окно приема

/* ---------- ARP ---------- */
#define LWIP_ARP                1

/* ---------- ICMP ---------- */
#define LWIP_ICMP               1

/* ---------- DHCP ---------- */
#define LWIP_DHCP               0

/* ---------- UDP ---------- */
#define LWIP_UDP                1
#define UDP_TTL                 255

/* ---------- отчетность по lwip ---------- */
#define LWIP_STATS 0
#define LWIP_DEBUG 1

/* ---------- параметры обратного вызова ссылки ---------- */
#define LWIP_NETIF_LINK_CALLBACK        1					// обратный вызов для контроля передачи данных

/* ---------- параметры контрольнной суммы ---------- */
#define CHECKSUM_BY_HARDWARE

#ifdef CHECKSUM_BY_HARDWARE
  #define CHECKSUM_GEN_IP                 0
  #define CHECKSUM_GEN_UDP                0
  #define CHECKSUM_GEN_TCP                0
  #define CHECKSUM_CHECK_IP               0
  #define CHECKSUM_CHECK_UDP              0
  #define CHECKSUM_CHECK_TCP              0
  #define CHECKSUM_GEN_ICMP               0
#else
  #define CHECKSUM_GEN_IP                 1
  #define CHECKSUM_GEN_UDP                1
  #define CHECKSUM_GEN_TCP                1
  #define CHECKSUM_CHECK_IP               1
  #define CHECKSUM_CHECK_UDP              1
  #define CHECKSUM_CHECK_TCP              1
  #define CHECKSUM_GEN_ICMP               1
#endif

/* ---------- API ---------- */

#define LWIP_NETCONN                    0
#define LWIP_SOCKET                     0


#endif /* INC_LWIPOPTS_H_ */
