/*установка инернет соединения на eth и rs*/

#ifndef INC_ETHERNETIF_H_
#define INC_ETHERNETIF_H_

#include "lwip/err.h"
#include "lwip/netif.h"
#include "cmsis_os.h"
#include "lwip/tcpip.h"

/*INT*/
#define INT_ETH_Pin GPIO_PIN_12
#define INT_ETH_GPIO_Port GPIOC
/*RST*/
#define RST_ETH_Pin GPIO_PIN_12
#define RST_ETH_GPIO_Port GPIOB
/*CS*/
#define CS_ETH_Pin GPIO_PIN_2
#define CS_ETH_GPIO_Port GPIOD

#define ENC28J60_INTERRUPT

struct enc_irq_str {
  struct netif *netif;
  osSemaphoreId semaphore;
};

/* Exported functions ------------------------------------------------------- */
err_t ethernetif_init(struct netif *netif);

//void ethernetif_input( void const * argument );
//void ethernetif_update_config(struct netif *netif);
//void ethernetif_notify_conn_changed(struct netif *netif);

void ethernetif_process_irq(void const *argument);
void ethernetif_update_config(struct netif *netif);
void ethernetif_notify_conn_changed(struct netif *netif);
void ethernet_transmit(void);
void ethernet_irq_handler(osSemaphoreId Netif_LinkSemaphore);

#endif /* INC_ETHERNETIF_H_ */
