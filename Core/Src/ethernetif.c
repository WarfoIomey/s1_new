/******************************************************************************
  	  	  	  	  	  	  	  объявление библиотек
******************************************************************************/

#include "stm32f1xx_hal.h"
#include "lwip/opt.h"
#include "lwip/timeouts.h"
#include "netif/etharp.h"
#include "ethernetif.h"
#include <string.h>
#include "cmsis_os.h"
#include "enc28j60.h"
#include "spi.h"
#include "lwip/tcpip.h"
#include "stm32f1xx_hal_conf.h"


/* Время блокировки ожидания ввода для настройки интернет модуля */

#define TIME_WAITING_FOR_INPUT ( portMAX_DELAY )

/* Размер стека потока интерфейса */

#define INTERFACE_THREAD_STACK_SIZE ( 350 )

/* Имя сетевого интерфейса */

#define IFNAME0 'r'
#define IFNAME1 'u'

//#define ETH_MAX_PACKET_SIZE

/* буферы под ДМА */

//__ALIGN_BEGIN ETH_DMADescTypeDef  DMARxDscrTab[ETH_RXBUFNB] __ALIGN_END;/* Ethernet Rx MA Descriptor */
//__ALIGN_BEGIN ETH_DMADescTypeDef  DMATxDscrTab[ETH_TXBUFNB] __ALIGN_END;/* Ethernet Tx DMA Descriptor */

/* буфер под передачу данных */

#if defined ( __ICCARM__ )
#pragma data_alignment=4
__ALIGN_BEGIN uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE] __ALIGN_BEGIN;
#pragma data_alignment=4
__ALIGN_BEGIN__no_init uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE] __ALIGN_BEGIN;
#elif defined ( __CC_ARM   )
uint8_t Rx_Buff[ETH_RXBUFNB][ETH_RX_BUF_SIZE]  __attribute__((at(0x2000E200)));
uint8_t Tx_Buff[ETH_TXBUFNB][ETH_TX_BUF_SIZE]  __attribute__((at(0x2000FFC4)));
#endif




/* Семафор для передачи сигналов о входящих пакетах с использованием ОСРВ */
osSemaphoreId s_xSemaphore = NULL;

/*инициализация интернет соединения*/
ENC_HandleTypeDef EncHandle;

/*инициалищация spi*/
extern SPI_HandleTypeDef hspi2;

/*активация семафор по прерываниям*/
extern osSemaphoreId Netif_IRQSemaphore;

/******************************************************************************
                       	   	   ENC28J60 MSP Routines
*******************************************************************************/

void ENC_MSPInit (ENC_HandleTypeDef *heth)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();



    /* ENC28J60 pins configuration */
    /*
          ENC28J60_CS ----------------------> PD2
          ENC28J60_INT ---------------------> PC12
          ENC28J60_RST ---------------------> PB12
    */
	/*CS - D2 */
    GPIO_InitStructure.Pin = GPIO_PIN_2;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_MEDIUM;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStructure);

    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET);

    /*INT - C12 */
    GPIO_InitStructure.Pin = GPIO_PIN_12;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);

    /*RST - B12 */
    GPIO_InitStructure.Pin = GPIO_PIN_12;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);


    MX_SPI2_Init();


    HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/*******************************************************************************
                       LL Driver Interface ( LwIP stack --> ETH)
*******************************************************************************/

/**
 * В этой функции аппаратное обеспечение должно быть инициализировано.
 * Вызывается из ethernet if_init().
 *
 * @param netif уже инициализированная структура сетевого интерфейса lwip
 * для этого ethernetif
 */

/*вызов работы железа*/
static void low_level_init (struct netif *netif)
{
	/*инициалзация мас адреса */

	netif->hwaddr[0] =  MAC_ADDR0;
    netif->hwaddr[1] =  MAC_ADDR1;
    netif->hwaddr[2] =  MAC_ADDR2;
    netif->hwaddr[3] =  MAC_ADDR3;
    netif->hwaddr[4] =  MAC_ADDR4;
    netif->hwaddr[5] =  MAC_ADDR5;

    /*инициализация работы железа*/

    EncHandle.Init.MACAddr = netif -> hwaddr;					    //назначение мас адреса
    EncHandle.Init.DuplexMode = ETH_MODE_HALFDUPLEX;				//выбор редима работы
    EncHandle.Init.ChecksumMode = ETH_CHECKSUM_BY_HARDWARE;			//контрольсоединения - по железу
    EncHandle.Init.InterruptEnableBits =  EIE_LINKIE | EIE_PKTIE;	//бит кто разрешения прерываня

    /*инициализация протокола отправки сообщения между аппаратными узлами интрнет соединения*/

    ENC_MSPInit(&EncHandle);

    /*максимальное время ожидания пакетов */
    netif->mtu = 1500;

    /*установка флага  1 флаг - установка режима широковещательной передачи   ;
     * 				   2 флаг - Для определения MAC-адреса другого компьютера по известному IP-адресу */

    netif->flags |= NETIF_FLAG_BROADCAST  | NETIF_FLAG_ETHARP;

    /* Start the EN28J60 module */
    if(ENC_Start(&EncHandle))
    {
        /* установка мас адреса */
        ENC_SetMacAddr(&EncHandle);

        /* установка флага интернет соединения */
        netif->flags |= NETIF_FLAG_LINK_UP;
    }
}

/*******************************************************************************/

/**
 * Эта функция должна выполнять фактическую передачу пакета. Пакет является
 * содержится в pbuf, который передается функции.
 *
 * @param netif структура сетевого интерфейса lwip для этого ethernetif
 * @param p MAC-пакет для отправки (например, IP-пакет, включающий MAC-адреса и тип)
 * @return ERR_OK, если пакет может быть отправлен
 *         значение err_t, если пакет не может быть отправлен
 *
 * @note   Возврат ERR_MEM здесь, если очередь DMA вашего MAC заполнена, может привести к
 * 		   странным результатам. Вы могли бы подумать о том, чтобы дождаться свободного места в очереди DMA
 * 		   чтобы стать доступным, поскольку стек не повторяет попытку отправки пакета
* 		   удалено из-за сбоя памяти (за исключением таймеров TCP).
 */

/*передача пакета на уровне железа*/
static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
	err_t errval;
	struct pbuf *q;
	uint32_t framelength = 0;

	q=p;

	if (EncHandle.transmitLength != 0)
	{
			do
			{
				ENC_Transmit(&EncHandle);
			}
			while (EncHandle.transmitLength !=0 );
	}
	errval = ENC_RestoreTXBuffer(&EncHandle, p->tot_len);
	if (errval != ERR_OK)
    {
        return errval;
    }

    for(q = p; q != NULL; q = q->next)
    {
        ENC_WriteBuffer(q->payload, q->len);
        framelength += q->len;
    }

    if(framelength != p->tot_len)
    {
        return ERR_BUF;
    }

    EncHandle.transmitLength = p->tot_len;

#ifndef USE_PROTOTHREADS
    ENC_Transmit(&EncHandle);
#endif

    return ERR_OK;
}

/**
 * Следует выделить pbuf и передать байты входящего
 * пакет из интерфейса в pbuf.
 *
 * @param  netif структура сетевого интерфейса lwip для этого ethernetif
 * @return pbuf, заполненный полученным пакетом (включая заголовок MAC)
 * значение NULL при ошибке памяти
   */


static struct pbuf *low_level_input(struct netif *netif)
{
	struct pbuf *p = NULL;
	struct pbuf *q = NULL;
	uint16_t len;
	uint8_t *buffer;
	uint32_t bufferoffset = 0;

	if (!ENC_GetReceivedFrame (&EncHandle))
	{
		return NULL;
	}

	/* Получить размер пакета и поместить его в переменную "len". */
	len  = EncHandle.RxFrameInfos.length;
	buffer = (uint8_t *) EncHandle.RxFrameInfos.buffer;

	if (len >0)
	{
		/* Мы выделяем цепочку pbuf из pbuf-файлов из буферного пула Lwip */
		p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
	}

	if (p != NULL)
	{
		   if(p != NULL)
		    {
		        bufferoffset = 0;

		        for(q = p; q != NULL; q = q->next)
		        {
		            /* копировать данные в pbuf */
		            memcpy((uint8_t*)((uint8_t*)q->payload), (uint8_t*)((uint8_t*)buffer + bufferoffset), q->len);
		            bufferoffset = bufferoffset + q->len;
		        }
		    }
	}
	return p;
}


/**
  * @brief Эта функция должна вызываться, когда пакет готов к считыванию
  * из интерфейса. Он использует функцию low_level_input(), которая
  * должен обрабатывать фактический прием байтов из сети
  * интерфейс. Затем определяется тип принятого пакета и
  * вызывается соответствующая функция ввода.
  *
  * @param netif структура сетевого интерфейса lwip для этого ethernetif
  */
void ethernetif_input_do(struct netif *netif)
{
    struct pbuf *p;

    do {
        p = low_level_input(netif);
        if (p != NULL)
        {
          if (netif->input(p, netif) != ERR_OK )
          {
            pbuf_free(p);
          }
        }
    }while(p!=NULL);
}



/**
 * Эта функция должна вызываться, когда пакет готов к считыванию
 * из интерфейса. Он использует функцию low_level_input(), которая
 * должен обрабатывать фактический прием байтов из сети
 * интерфейс. Затем определяется тип принятого пакета и
 * вызывается соответствующая функция ввода.
 *
 * @param netif структура сетевого интерфейса lwip для этого ethernetif
 */

void ethernetif_input(void const *argument)
{
    struct pbuf *p;
    struct netif *netif = (struct netif *) argument;

    for(;;)
    {
        if(osSemaphoreWait(s_xSemaphore, TIME_WAITING_FOR_INPUT) == osOK)
        {
            do
            {
                p = low_level_input(netif);
                if(p != NULL)
                {
                    if(netif->input(p, netif) != ERR_OK)
                    {
                        pbuf_free(p);
                    }
                }
            }
            while(p!=NULL);
        }
    }
}



#if !LWIP_ARP
/**
 * Эта функция должна быть выполнена пользователем в случае отключения ARP.
 * *
 * @param netif структура сетевого интерфейса lwip для этого ethernetif
 * @return ERR_OK if ...
 */
static err_t low_level_output_arp_off(struct netif *netif, struct pbuf *q, ip_addr_t *ipaddr)
{
    err_t errval;
    errval = ERR_OK;

    /* USER CODE BEGIN 5 */

    /* USER CODE END 5 */

    return errval;

}
#endif /* LWIP_ARP */


/**
 * Должен быть вызван в начале программы для настройки
 * сетевого интерфейса. Он вызывает функцию low_level_init() для выполнения
 * фактической настройки оборудования.
 *
 * Эта функция должна быть передана в качестве параметра в netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t ethernetif_init(struct netif *netif)
{
    LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
    /* инициализация хоста */
    netif->hostname = "ruff1";
#endif /* LWIP_NETIF_HOSTNAME */

    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* Здесь мы напрямую используем etharp_output() для сохранения вызова функции.
     * Вместо этого вы можете объявить свою собственную функцию и вызвать etharp_output()
     * из него, если вам нужно выполнить некоторые проверки перед отправкой (например, если ссылка
     * доступно...) */
#if LWIP_ARP || LWIP_ETHERNET
#if LWIP_ARP
    netif->output = etharp_output;
#else
    /* Пользователь должен написать свой собственный код в функции low_level_output_arp_off */
    netif->output = low_level_output_arp_off;
#endif /* LWIP_ARP */
#endif  /* LWIP_ARP || LWIP_ETHERNET */

    netif->linkoutput = low_level_output;

    /* инициализировать аппаратное обеспечение */
    low_level_init(netif);

    return ERR_OK;
}

/* USER CODE BEGIN 6 */

/**
* @brief  Возвращает текущее время в миллисекундах
* 		  когда LWIP_TIMERS == 1 и NO_SYS == 1
* @param  нету
* @retval время
*/
u32_t sys_jiffies(void)
{
    return HAL_GetTick();
}

/**
* @brief  Возвращает текущее время в миллисекундах
* 		  когда LWIP_TIMERS == 1 и NO_SYS == 1
* @param  None
* @retval время
*/
u32_t sys_now(void)
{
    return HAL_GetTick();
}

#if LWIP_NETIF_LINK_CALLBACK
/**
  * @brief  Функция обратного вызова ссылки, эта функция вызывается при изменении статуса ссылки
  * для обновления конфигурации драйвера низкого уровня.
  * @param  netif: интернет соединение
  * @retval None
  */
void ethernetif_update_config(struct netif *netif)
{
  if(netif_is_link_up(netif)) {
      /* Restart the EN28J60 module */
      low_level_init(netif);
  }


  //    if(netif_is_link_up(netif))
  //    {
  //        /* Restart the auto-negotiation */
  //        if(heth.Init.AutoNegotiation != ETH_AUTONEGOTIATION_DISABLE)
  //        {
  //            /* Enable Auto-Negotiation */
  //            HAL_ETH_WritePHYRegister(&heth, PHY_BCR, PHY_AUTONEGOTIATION);
  //
  //            /* Get tick */
  //            tickstart = HAL_GetTick();
  //
  //            /* Wait until the auto-negotiation will be completed */
  //            do
  //            {
  //                HAL_ETH_ReadPHYRegister(&heth, PHY_BSR, &regvalue);
  //
  //                /* Check for the Timeout ( 1s ) */
  //                if((HAL_GetTick() - tickstart) > 1000)
  //                {
  //                    /* In case of timeout */
  //                    goto error;
  //                }
  //            }
  //            while(((regvalue & PHY_AUTONEGO_COMPLETE) != PHY_AUTONEGO_COMPLETE));
  //
  //            /* Read the result of the auto-negotiation */
  //            HAL_ETH_ReadPHYRegister(&heth, PHY_SR, &regvalue);
  //
  //            /* Configure the MAC with the Duplex Mode fixed by the auto-negotiation process */
  //            if((regvalue & PHY_DUPLEX_STATUS) != (uint32_t)RESET)
  //            {
  //                /* Set Ethernet duplex mode to Full-duplex following the auto-negotiation */
  //                heth.Init.DuplexMode = ETH_MODE_FULLDUPLEX;
  //            }
  //            else
  //            {
  //                /* Set Ethernet duplex mode to Half-duplex following the auto-negotiation */
  //                heth.Init.DuplexMode = ETH_MODE_HALFDUPLEX;
  //            }
  //            /* Configure the MAC with the speed fixed by the auto-negotiation process */
  //            if(regvalue & PHY_SPEED_STATUS)
  //            {
  //                /* Set Ethernet speed to 10M following the auto-negotiation */
  //                heth.Init.Speed = ETH_SPEED_10M;
  //            }
  //            else
  //            {
  //                /* Set Ethernet speed to 100M following the auto-negotiation */
  //                heth.Init.Speed = ETH_SPEED_100M;
  //            }
  //        }
  //        else /* AutoNegotiation Disable */
  //        {
  //        error :
  //            /* Check parameters */
  //            assert_param(IS_ETH_SPEED(heth.Init.Speed));
  //            assert_param(IS_ETH_DUPLEX_MODE(heth.Init.DuplexMode));
  //
  //            /* Set MAC Speed and Duplex Mode to PHY */
  //            HAL_ETH_WritePHYRegister(&heth, PHY_BCR, ((uint16_t)(heth.Init.DuplexMode >> 3) |
  //                                     (uint16_t)(heth.Init.Speed >> 1)));
  //        }
  //
  //        /* ETHERNET MAC Re-Configuration */
  //        HAL_ETH_ConfigMAC(&heth, (ETH_MACInitTypeDef *) NULL);
  //
  //        /* Restart MAC interface */
  //        HAL_ETH_Start(&heth);
  //    }
  //    else
  //    {
  //        /* Stop MAC interface */
  //        HAL_ETH_Stop(&heth);
  //    }

      ethernetif_notify_conn_changed(netif);
  }

/**
  * @brief  Эта функция уведомляет пользователя об изменении статуса ссылки.
  * @param  netif: интернет интерфейс
  * @retval None
  */
__weak void ethernetif_notify_conn_changed(struct netif *netif)
{
    /* NOTE : Эта функция может быть реализована в пользовательском файле
              когда требуется обратный вызов
    */

}
#endif /* LWIP_NETIF_LINK_CALLBACK */



/**
  * @brief  This function actually process pending IRQs.
  * @param  handler: Reference to the driver state structure
  * @retval None
  */
void ethernetif_process_irq_do(void const *argument)
{
    struct enc_irq_str *irq_arg = (struct enc_irq_str *)argument;

    /* Handle ENC28J60 interrupt */
    ENC_IRQHandler(&EncHandle);

    /* Check whether the link is up or down*/
    if((EncHandle.interruptFlags & EIE_LINKIE) != 0)
    {
        if((EncHandle.LinkStatus & PHSTAT2_LSTAT)!= 0)
        {
            netif_set_link_up(irq_arg->netif);
        }
        else
        {
            netif_set_link_down(irq_arg->netif);
        }
    }

    /* Check whether we have received a packet */
    if((EncHandle.interruptFlags & EIR_PKTIF) != 0)
    {
        ethernetif_input_do(irq_arg->netif);
    }

    ENC_EnableInterrupts(EIE_INTIE);
}


/**
  * @brief  This function triggers the interrupt service callback.
  * @param  netif: the network interface
  * @retval None
  */
void ethernetif_process_irq(void const *argument)
{
  struct enc_irq_str *irq_arg = (struct enc_irq_str *)argument;

  for(;;)
  {
    if (osSemaphoreWait(irq_arg->semaphore, TIME_WAITING_FOR_INPUT) == osOK)
    {
        tcpip_callback((tcpip_callback_fn) ethernetif_process_irq_do, (void *
) argument);
    }
  }
}

/**
  * @brief  This function unblocks ethernetif_process_irq when a new
interrupt is received
  * @param  netif: the network interface
  * @retval None
  */
void ethernet_irq_handler(osSemaphoreId Netif_IrqSemaphore)
{
    /* Release thread to check interrupt flags */
     osSemaphoreRelease(Netif_IrqSemaphore);
}


/**
  * Implement SPI single byte send and receive.
  * The ENC28J60 slave SPI must already be selected and wont be deselected
after transmission
  * Must be provided by user code
  * param  command: command or data to be sent to ENC28J60
  * retval answer from ENC28J60
  */

uint8_t ENC_SPI_SendWithoutSelection(uint8_t command)
{
    HAL_SPI_TransmitReceive(&hspi2, &command, &command, 1, 1000);
    return command;
}

/**
  * Implement SPI single byte send and receive. Must be provided by user code
  * param  command: command or data to be sent to ENC28J60
  * retval answer from ENC28J60
  */

uint8_t ENC_SPI_Send(uint8_t command)
{
    /* Select ENC28J60 module */
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_RESET);
    up_udelay(1);

    HAL_SPI_TransmitReceive(&hspi2, &command, &command, 1, 1000);

    /* De-select ENC28J60 module */
    HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_SET);
    up_udelay(1);

    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    return command;
}

/**
  * Implement SPI buffer send and receive. Must be provided by user code
  * param  master2slave: data to be sent from host to ENC28J60, can be NULL
if we only want to receive data from slave
  * param  slave2master: answer from ENC28J60 to host, can be NULL if we only
want to send data to slave
  * retval none
  */

void ENC_SPI_SendBuf(uint8_t *master2slave, uint8_t *slave2master, uint16_t
                     bufferSize)
{
    HAL_NVIC_DisableIRQ(EXTI3_IRQn);
    HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_RESET);
    up_udelay(1);

    /* Transmit or receuve data */
    if(slave2master == NULL)
    {
        if(master2slave != NULL)
        {
            HAL_SPI_Transmit(&hspi2, master2slave, bufferSize, 1000);
        }
    }
    else if(master2slave == NULL)
    {
        HAL_SPI_Receive(&hspi2, slave2master, bufferSize, 1000);
    }
    else
    {
        HAL_SPI_TransmitReceive(&hspi2, master2slave, slave2master, bufferSize, 1000);
    }

    /* De-select ENC28J60 module */
    HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_SET);
    up_udelay(1);
    HAL_NVIC_EnableIRQ(EXTI3_IRQn);
}

/**
  * Implement SPI Slave selection and deselection. Must be provided by user
code
  * param  select: true if the ENC28J60 slave SPI if selected, false otherwise
  * retval none
  */

void ENC_SPI_Select(bool select)
{
    /* Select or de-select ENC28J60 module */
    if(select)
    {
        HAL_NVIC_DisableIRQ(EXTI3_IRQn);
        HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_RESET);
        up_udelay(1);
    }
    else
    {
        HAL_GPIO_WritePin(GPIOD, CS_ETH_Pin, GPIO_PIN_SET);
        up_udelay(1);
        HAL_NVIC_EnableIRQ(EXTI3_IRQn);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == INT_ETH_Pin)
  {
      ethernet_irq_handler(Netif_IRQSemaphore);
  }
}

void HAL_GPIO_EXTI_Callback_RST (uint16_t GPIO_Pin)
{
	if (GPIO_Pin == RST_ETH_Pin)
	{
	   ethernet_irq_handler(Netif_IRQSemaphore);

	}
}
