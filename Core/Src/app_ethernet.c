
#include "lwip/opt.h"
#include "main.h"
#include "lwip/dhcp.h"
#include "lwip/tcpip.h"
#include "app_ethernet.h"
#include "ethernetif.h"
#include "lwip.h"
#include "usart.h"

extern uint8_t USART2_TxBUF[USART2_MAX_SENDLEN];
extern uint8_t USART2_RxBUF[USART2_MAX_RECVLEN];

#ifdef USE_DHCP
#define MAX_DHCP_TRIES  4
__IO uint8_t DHCP_state;
#endif

uint8_t IP_ADDRESS[4];

void User_notification(struct netif *netif)
{
    if(netif_is_up(netif))
    {
#ifdef USE_DHCP
        DHCP_state = DHCP_START;
#else

        uint8_t iptxt[20];
    //	u2_printf("(DBG:) ПОПЫТКА ПОДКЛЮЧИТЬСЯ MQTT СЕРВЕРУ\r\n\n");
    //  u2_printf("(DBG:)"  (char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3 "\r\n\n");
        sprintf((char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);

        u2_printf("Static IP address: %s\n", iptxt);

#endif /* USE_DHCP */
    }
    else
    {
#ifdef USE_DHCP
        DHCP_state = DHCP_LINK_DOWN;
#endif  /* USE_DHCP */

        u2_printf("no ethernet connect\n");
        /* USE_LCD */
    }
}

/*отслеживание изменение IP*/
void ethernetif_notify_conn_changed(struct netif *netif)
{
	     ip_addr_t ipaddr;
	     ip_addr_t netmask;
	     ip_addr_t gw;

    if(netif_is_link_up(netif))
    {
#ifdef USE_DHCP
#ifdef USE_LCD
        u2_printf("Current ethernet connection  \n");/*Нынешнее интернет соединение*/


#endif /* USE_LCD */
        DHCP_state = DHCP_START;
#else
        IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
        IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

#ifdef USE_LCD
        uint8_t iptxt[20];

        sprintf((char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
        u2_printf("Eth connect\n");
        u2_printf("Static IP address: %s\n", iptxt);

#endif /* USE_LCD */
#endif /* USE_DHCP */

        netif_set_addr(netif, &ipaddr, &netmask, &gw);

        netif_set_up(netif);
    }
    else
    {
#ifdef USE_DHCP
        DHCP_state = DHCP_LINK_DOWN;
#endif /* USE_DHCP */

        netif_set_down(netif);

#ifdef USE_LCD
        u2_printf("No eth Connect\n");


#endif /* USE_LCD */
    }
}

#ifdef USE_DHCP
/*Рабочий процесс, вызываемый потоком tcpip*/
void dhcp_do(struct netif *netif)
{
    struct ip_addr ipaddr;
    struct ip_addr netmask;
    struct ip_addr gw;
    uint32_t IPaddress;

    switch(DHCP_state)
    {
        case DHCP_START:
        {
            netif->ip_addr.addr = 0;
            netif->netmask.addr = 0;
            netif->gw.addr = 0;
            IPaddress = 0;
            dhcp_start(netif);
            DHCP_state = DHCP_WAIT_ADDRESS;
#ifdef USE_LCD
            u2_printf("Condition: I'm looking for a DHCP server...\n");/*Состояние: Ищу DHCP-сервер...*/
#endif
        }
        break;

        case DHCP_WAIT_ADDRESS:
        {
        	/* Считайте новый IP-адрес */
        	IPaddress = netif->ip_addr.addr;

            if(IPaddress!=0)
            {
                DHCP_state = DHCP_ADDRESS_ASSIGNED;

                dhcp_stop(netif);

#ifdef USE_LCD
                uint8_t iptab[4];
                uint8_t iptxt[20];

                iptab[0] = (uint8_t)(IPaddress >> 24);
                iptab[1] = (uint8_t)(IPaddress >> 16);
                iptab[2] = (uint8_t)(IPaddress >> 8);
                iptab[3] = (uint8_t)(IPaddress);

                sprintf((char*)iptxt, "%d.%d.%d.%d", iptab[3], iptab[2], iptab[1], iptab[0]);

                u2_printf("IP-адрес, назначенный DHCP-сервером: %s\n", iptxt);
                IP_ADDRESS[0] = iptab[3];
                IP_ADDRESS[1] = iptab[2];
                IP_ADDRESS[2] = iptab[1];
                IP_ADDRESS[3] = iptab[0];

#endif
            }
            else
            {
                if(netif->dhcp->tries > MAX_DHCP_TRIES)
                {
                    DHCP_state = DHCP_TIMEOUT;

                    dhcp_stop(netif);

                    /*Используемый статический адрес*/
                    IP4_ADDR(&ipaddr, IP_ADDR0,IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2, NETMASK_ADDR3);
                    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);
                    netif_set_addr(netif, &ipaddr, &netmask, &gw);

#ifdef USE_LCD
                    uint8_t iptxt[20];

                    sprintf((char*)iptxt, "%d.%d.%d.%d", IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
                    u2_printf("DHCP download !!\n");
                    u2_printf("Static IP address  : %s\n", iptxt);

                    IP_ADDRESS[0] =IP_ADDR0;
                    IP_ADDRESS[1] =IP_ADDR1;
                    IP_ADDRESS[2] = IP_ADDR2;
                    IP_ADDRESS[3] = IP_ADDR3;

#endif
                }
            }
        }
        break;

        default:
            break;
    }
}

void DHCP_thread(void const * argument)
{
    for(;;)
    {
        tcpip_callback((tcpip_callback_fn) dhcp_do, (void *) argument);
        osDelay(250);
    }
}
#endif  /* USE_DHCP */
