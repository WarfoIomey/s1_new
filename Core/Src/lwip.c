
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "app_ethernet.h"
#include "tcp_client.h"



/* ETH Инициализация----------------------------------------------*/
void _Error_Handler(char * file, int line);

static void Netif_Config(void);

/* Инициализация переменных */
struct netif gnetif;
ip_addr_t ipaddr;
ip_addr_t netmask;
ip_addr_t gw;

/* Семафор для передачи сигнала об обновлении состояния канала Ethernet */
osSemaphoreId Netif_IRQSemaphore = NULL;
struct enc_irq_str irq_arg;

void BSP_Config()
{
    // set interrupt pin for enc28j60
}


/* init function */
void MX_LWIP_Init(void)
{

    printf("MX_Lwip_init()\n");
    printf("BSP_config\n");
    tcpip_init(NULL, NULL);
    printf("tcpip_init()\n");
    printf("netif_config()\n");
    /* Инициализируйте стек LwIP */
    Netif_Config();
    tcp_server_init();
    /* Уведомлять пользователя о конфигурации сетевого интерфейса */
    User_notification(&gnetif);

#ifdef USE_DHCP
    osThreadDef(DHCP, DHCP_thread, osPriorityBelowNormal, 0, configMINIMAL_STACK_SIZE * 2);
    osThreadCreate(osThread(DHCP), &gnetif);
#endif
}

/**
  * @brief  Инициализация lwIP
  * @param  None
  * @retval None
  */
static void Netif_Config(void)
{
	ip_addr_t ipaddr;
	ip_addr_t netmask;
	ip_addr_t gw;

    // получение статического ip-адреса из eeprom
    IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
    IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,NETMASK_ADDR3);
    IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

    /* создайте двоичный семафор, используемый для информирования ethernetif о фрейме  прием */
    osSemaphoreDef(Netif_SEM);
    Netif_IRQSemaphore = osSemaphoreCreate(osSemaphore(Netif_SEM), 1);

    /* - netif_add(struct netif *netif, struct ip_addr *ipaddr,
    struct ip_addr *netmask, struct ip_addr *gw,
    void *state, err_t (* init)(struct netif *netif),
    err_t (* input)(struct pbuf *p, struct netif *netif))

 	Добавляет ваш сетевой интерфейс в список net if_list. Выделите структуру
    netif и передайте указатель на эту структуру в качестве первого аргумента.
    Указывайте указатели на очищенные структуры ip_addr при использовании DHCP,
    или заполните их вменяемыми цифрами в противном случае. Указатель состояния может быть НУЛЕВЫМ.

    Указатель функции инициализации должен указывать на функцию инициализации для
	вашего интерфейса ethernet netif. Следующий код иллюстрирует его использование.*/

    netif_add(&gnetif, &ipaddr, &netmask, &gw, NULL, &ethernetif_init, &tcpip_input);

    /* Регистрирует сетевой интерфейс по умолчанию. */
    netif_set_default(&gnetif);

    if(netif_is_link_up(&gnetif))
    {
    	/* Когда netif полностью настроен, эта функция должна быть вызвана.*/
        netif_set_up(&gnetif);
    }
    else
    {
    	/* Когда связь net if отключена, эта функция должна быть вызвана */
        netif_set_down(&gnetif);
    }

    /* Установите функцию обратного вызова ссылки, эта функция вызывается при изменении статуса  ссылки*/
    netif_set_link_callback(&gnetif, ethernetif_update_config);

    irq_arg.netif = &gnetif;
    irq_arg.semaphore = Netif_IRQSemaphore;



    /* Создайте поток обработчика Ethernet IRQ */
    osThreadDef(IrqThr, ethernetif_process_irq, osPriorityRealtime, 0, configMINIMAL_STACK_SIZE * 2);

    osThreadCreate(osThread(IrqThr), &irq_arg);
}
