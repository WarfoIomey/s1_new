/*библиотека интернет соединения*/

#ifndef ENC28J60_H_INCLUDED
#define ENC28J60_H_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#ifdef USE_PROTOTHREADS
#include "pt.h"
#else
#define PT_BEGIN(x)
#define PT_END(x)
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

/* Ethernet frames are between 64 and 1518 bytes long */

#define MIN_FRAMELEN      64
#define MAX_FRAMELEN      1518


/* External functions --------------------------------------------------------*/
void HAL_Delay(volatile uint32_t Delay);
uint32_t HAL_GetTick(void);

/* Callback  functions  *********************************************************/

/**
  * Реализовать выбор и отмену выбора ведомого устройства SPI.
  * Должно быть предоставлено пользовательским кодом
  * выбор параметра: true, если выбран slave SPI ENC28J60, в противном случае false
  * ретвал отсутствует
  */

void ENC_SPI_Select(bool select);

/**
* Реализовать однобайтовую отправку и прием SPI.
  * Ведомый SPI ENC28J60 уже должен быть выбран и не будет отменен после передачи
  * Должно быть предоставлено пользовательским кодом
  * команда param: команда или данные, которые должны быть отправлены в ENC28J60
  * ответ retval от ENC28J60
  */

uint8_t ENC_SPI_SendWithoutSelection(uint8_t command);

/**
  * Реализовать однобайтовую отправку и прием SPI. Должно быть предоставлено пользовательским кодом
  * команда param: команда или данные, которые должны быть отправлены в ENC28J60
  * ответ retval от ENC28J60
  */

uint8_t ENC_SPI_Send(uint8_t command);

/**
  * Реализовать отправку и прием буфера SPI. Должно быть предоставлено пользовательским кодом
  * параметр master 2 slave: данные, которые должны быть отправлены с хоста на ENC28J60,
  * могут быть равны нулю, если мы хотим получать данные только от ведомого устройства.
  * параметр slave 2 master: ответ от ENC28J60 на хост может быть нулевым, если мы хотим
  * отправить данные только на slave
  * ретвал отсутствует
  */

void ENC_SPI_SendBuf(uint8_t *master2slave, uint8_t *slave2master, uint16_t bufferSize);

/** @defgroup ETH_Exported_Types ETH Exported Types
  * @{
  */

/**
  * @brief  ETH Init Structure definition
  */

typedef struct
{
   uint32_t             DuplexMode;                /*!< Выбор дуплексного режима MAC: Полудуплексный или полнодуплексный
													* режим Этот параметр может иметь значение @ref ETH_Duplex_Mode */

   uint8_t              *MACAddr;                   /*!< MAC-адрес используемого оборудования: должен быть
													 * указателем на массив из 6 байт*/

   uint32_t             ChecksumMode;              /*!< Выбирает, проверяется ли контрольная сумма аппаратным или программным обеспечением.
                                                         Этот параметр может быть значением @ref ETH_Checksum_Mode */

   uint8_t              InterruptEnableBits;       /*!< Выбирает разрешенные прерывания*/
} ENC_InitTypeDef;


/**
  * @brief  Определение структуры полученной информации о кадре
  */
typedef struct
{
  uint32_t length;                       			/*!< Длина кадра */

  uint8_t buffer[MAX_FRAMELEN+20];             		/*!< Буфер кадров */

} ENC_RxFrameInfos;


/**
  * @brief  Определение структуры дескриптора ENC28J60
  */

typedef struct
{
  ENC_InitTypeDef           Init;          /*!< Конфигурация инициализации Ethernet  		  */

  uint8_t                   bank;          /*!< Выбранный в настоящее время банк     		  */
  uint8_t                   interruptFlags;/*!< Последнее значение флагов прерываний 		  */
  uint8_t                   pktCnt;        /*!< Количество ожидающих приема пакетов  		  */
  uint16_t                  nextpkt;       /*!< Адрес следующего пакета 			 		  */
  uint16_t                  LinkStatus;    /*!< Состояние канала Ethernet     				  */
  uint16_t                  transmitLength;/*!< Длина ip-кадра для передачи	 		 		  */
  uint32_t                  startTime;     /*!< Время начала текущего времени		 		  */
  uint32_t                  duration;      /*!< Длительность текущего времени в мс			  */
  uint16_t                  retries;       /*!< Количество оставшихся попыток передачи		  */

  ENC_RxFrameInfos          RxFrameInfos;  /*!< информация о последнем кадре Rx		          */
} ENC_HandleTypeDef;

 /**
  * @}
  */

/* Exported constants --------------------------------------------------------*/
/** @defgroup ETH_Exported_Constants Экспортированные константы ETH
  * @{
  */
/* Размер заголовка Ethernet */
#define ETH_HDRLEN      14   /* Minimum size: 2*6 + 2 	   */


#define ERR_OK          0    /* Ошибок нет, все в порядке. */
#define ERR_MEM        -1    /* Ошибка нехватки памяти.    */
#define ERR_BUF        -2    /* Ошибка буфера.             */
#define ERR_TIMEOUT    -3    /* Ошибка времени.            */

/* ENC28J60 Commands ********************************************************/
/* В общей сложности на ENC28J60 реализовано семь инструкций.  Где:
*
* 	aaaaaa - это 5-разрядный адрес управляющего регистра, а
* 	dddddddd - это один или несколько байтов данных, которые могут сопровождать команду.
 */

#define ENC_RCR             (0x00)    /* Считывание Управляющего регистра
                                       * 000 | aaaaa | (Register value returned)) */
#define ENC_RBM             (0x3a)    /* Чтение Буферной Памяти
                                       * 001 | 11010 | (Read buffer data follows) */
#define ENC_WCR             (0x40)    /* Регистр Управления записьюr
                                       * 010 | aaaaa | dddddddd */
#define ENC_WBM             (0x7a)    /* Запись в Буферную память
                                       * 011 | 11010 | (Write buffer data follows) */
#define ENC_BFS             (0x80)    /* Набор Битовых полей
                                       * 100 | aaaaa | dddddddd */
#define ENC_BFC             (0xa0)    /* Битовое поле Очищено
                                       * 101 | aaaaa | dddddddd */
#define ENC_SRC             (0xff)    /* Сброс системы
                                       * 111 | 11111 | (No data) */

/* Global Control Registers *************************************************/
/* Доступ к управляющим регистрам осуществляется с помощью RCR, RBM, WCR, BFS и BFC
 * команды. Ниже указаны все управляющие регистры ENC28J60.
Память регистра управления * разделена на четыре
блока, выбираемых битами выбора * банка, BSEL1:BSEL0, в регистре ECON1.
 *
 * Последние пять местоположений (от 0x1b до 0x1f) всех банков указывают на общий набор
 * * реестров: ОВОС, EIR, НЕДВИЖИМОСТЬ, ECON 2 и ECON 1. Это ключевые регистры
 * используется для управления и мониторинга работы устройства. Их
 * общее отображение обеспечивает легкий доступ без переключения банка.
 *
 * Управляющие регистры для ENC28J60 обычно сгруппированы как ETH, MAC и
 * Регистры MII. Имена регистров, начинающиеся с E, принадлежат к группе ETH.
 * Аналогичным образом, имена регистров, начинающиеся с MA, принадлежат группе MAC и
 * регистры с префиксом MI принадлежат к группе MII.
 */

#define ENC_EIE             (0x1b)   /* Регистр включения прерывания Ethernet*/
#define ENC_EIR             (0x1c)   /* Регистр запросов на прерывание Ethernet */
#define ENC_ESTAT           (0x1d)   /* Регистр состояния Ethernet */
#define ENC_ECON2           (0x1e)   /* Регистр управления Ethernet 2 */
#define ENC_ECON1           (0x1f)   /* Регистр управления Ethernet 1 */

/* Прерывание Ethernet Позволяет Определять Регистровые разряды*/

#define EIE_RXERIE          (1 << 0) /* Bit 0: Разрешить прерывание приема с ошибкой */
#define EIE_TXERIE          (1 << 1) /* Bit 1: Включить Прерывание передачи с ошибкой*/
                                     /* Bit 2: Зарезервированный */
#define EIE_TXIE            (1 << 3) /* Bit 3: Включить передачу */
#define EIE_LINKIE          (1 << 4) /* Bit 4: Изменение статуса Соединения Включение Прерывания */
#define EIE_DMAIE           (1 << 5) /* Bit 5: Включение прерывания DMA */
#define EIE_PKTIE           (1 << 6) /* Bit 6: Разрешить Прерывание в ожидании приема пакета */
#define EIE_INTIE           (1 << 7) /* Bit 7: Включение глобального прерывания INT */

/* Определения битов Регистра Запроса прерывания Ethernet */

#define EIR_RXERIF          (1 << 0) /* Bit 0: Прерывание приема с ошибкой */
#define EIR_TXERIF          (1 << 1) /* Bit 1: Ошибка передачи Прерывание */
                                     /* Bit 2: Зарезервированный */
#define EIR_TXIF            (1 << 3) /* Bit 3: Прерывание передачи */
#define EIR_LINKIF          (1 << 4) /* Bit 4: Прерывание смены канала связи */
#define EIR_DMAIF           (1 << 5) /* Bit 5: DMA Прерывать */
#define EIR_PKTIF           (1 << 6) /* Bit 6: Ожидающее Прерывания Приема Пакета */
                                     /* Bit 7: Зарезервированный */
#define EIR_ALLINTS         (EIR_RXERIF | EIR_TXERIF | EIR_TXIF | EIR_LINKIF | EIR_DMAIF)   /* All interrupts bar EIR_PKTIF*/

/* Определения битов Регистра состояния Ethernet */

#define ESTAT_CLKRDY        (1 << 0) /* Bit 0: Часы Готовы */
#define ESTAT_TXABRT        (1 << 1) /* Bit 1: Ошибка Прерывания Передачи */
#define ESTAT_RXBUSY        (1 << 2) /* Bit 2: Прием Занят */
                                     /* Bit 3: Зарезервированный */
#define ESTAT_LATECOL       (1 << 4) /* Bit 4: Ошибка Позднего Столкновения */
                                     /* Bit 5: Зарезервированный */
#define ESTAT_BUFER         (1 << 6) /* Bit 6: Состояние ошибки буфера Ethernet */
#define ESTAT_INT           (1 << 7) /* Bit 7: Прерывание INT */

/* Управление Ethernet 1 Определение битов регистра */

#define ECON1_BSEL_SHIFT    (0)      /* Биты 0-1: Выбор банка */
#define ECON1_BSEL_MASK     (3 << ECON1_BSEL_SHIFT)
#  define ECON1_BSEL_BANK0  (0 << ECON1_BSEL_SHIFT) /* Bank 0 */
#  define ECON1_BSEL_BANK1  (1 << ECON1_BSEL_SHIFT) /* Bank 1 */
#  define ECON1_BSEL_BANK2  (2 << ECON1_BSEL_SHIFT) /* Bank 2 */
#  define ECON1_BSEL_BANK3  (3 << ECON1_BSEL_SHIFT) /* Bank 3 */
#define ECON1_RXEN          (1 << 2) /* Bit 2: Прием Позволяет */
#define ECON1_TXRTS         (1 << 3) /* Bit 3: Передать запрос на отправку*/
#define ECON1_CSUMEN        (1 << 4) /* Bit 4: DMA Контрольная сумма Включена */
#define ECON1_DMAST         (1 << 5) /* Bit 5: DMA Состояние запуска и занятости */
#define ECON1_RXRST         (1 << 6) /* Bit 6: Логический сброс приема */
#define ECON1_TXRST         (1 << 7) /* Bit 7: Сброс логики передачи */

/* Регистр управления Ethernet 2 */
                                     /* Bits 0-2: Зарезервированный */
#define ECON2_VRPS          (1 << 3) /* Bit 3: Регулятор Напряжения Включение Энергосбережения */
                                     /* Bit 4: Зарезервированный */
#define ECON2_PWRSV         (1 << 5) /* Bit 5: Включение Энергосбережения */
#define ECON2_PKTDEC        (1 << 6) /* Bit 6: Уменьшение размера пакета */
#define ECON2_AUTOINC       (1 << 7) /* Bit 7: Включить Автоматическое Увеличение Указателя Буфера */

/* Регистры Управления с Наклоном *************************************************/
/* Остальные управляющие регистры идентифицируются с помощью 5-разрядного адреса и
 * выбор банка.  Мы упаковываем номер банка и указание, является ли это
 * доступом к регистру MAC/PHY вместе с адресом регистра управления.
 * вместе, чтобы упростить дизайн.
 */

#define ENC_ADDR_SHIFT      (0)      /* Bits 0-4: Адрес регистрации*/
#define ENC_ADDR_MASK       (0x1f << ENC_ADDR_SHIFT)
#define ENC_BANK_SHIFT      (5)      /* Bits 5-6: Номер банка */
#define ENC_BANK_MASK       (3 << ENC_BSEL_SHIFT)
#  define ENC_BANK0         (0 << ENC_BSEL_SHIFT)
#  define ENC_BANK1         (1 << ENC_BSEL_SHIFT)
#  define ENC_BANK2         (2 << ENC_BSEL_SHIFT)
#  define ENC_BANK3         (3 << ENC_BSEL_SHIFT)
#define ENC_PHYMAC_SHIFT    (7)      /* Bit 7: Это команда PHY/MAC */
#define ENC_PHYMAC          (1 << ENC_PHYMAC_SHIFT)

#define REGADDR(a,b,m)      ((m) << ENC_PHYMAC_SHIFT | (b) << ENC_BANK_SHIFT | (a))
#define GETADDR(a)          ((a) & ENC_ADDR_MASK)
#define GETBANK(a)          (((a) >> ENC_BANK_SHIFT) & 3)
#define ISPHYMAC(a)         (((a) & ENC_PHYMAC) != 0)

/* Адреса Регистра управления банка 0 */

#define ENC_ERDPTL      REGADDR(0x00, 0, 0) /* Младший Байт Указателя Чтения(ERDPT<7:0> */
#define ENC_ERDPTH      REGADDR(0x01, 0, 0) /* Старший Pointer High Byte (ERDPT<12:8>) */
#define ENC_EWRPTL      REGADDR(0x02, 0, 0) /* Младший Байт Указателя Записи(EWRPT<7:0>) */
#define ENC_EWRPTH      REGADDR(0x03, 0, 0) /* Старший Байт Указателя Записи (EWRPT<12:8>) */
#define ENC_ETXSTL      REGADDR(0x04, 0, 0) /* TX Start Младший байт(ETXST<7:0>) */
#define ENC_ETXSTH      REGADDR(0x05, 0, 0) /* TX Start Старший байт (ETXST<12:8>) */
#define ENC_ETXNDL      REGADDR(0x06, 0, 0) /* TX End Младший байт (ETXND<7:0>) */
#define ENC_ETXNDH      REGADDR(0x07, 0, 0) /* TX End Старший байт (ETXND<12:8>) */
#define ENC_ERXSTL      REGADDR(0x08, 0, 0) /* RX Start Младший байт (ERXST<7:0>) */
#define ENC_ERXSTH      REGADDR(0x09, 0, 0) /* RX Start Старший байт (ERXST<12:8>) */
#define ENC_ERXNDL      REGADDR(0x0a, 0, 0) /* RX End Младший байт (ERXND<7:0>) */
#define ENC_ERXNDH      REGADDR(0x0b, 0, 0) /* RX End Старший байт (ERXND<12:8>) */
#define ENC_ERXRDPTL    REGADDR(0x0c, 0, 0) /* RX RD Pointer Младший байт (ERXRDPT<7:0>) */
#define ENC_ERXRDPTH    REGADDR(0x0d, 0, 0) /* RX RD Pointer Старший байт (ERXRDPT<12:8>) */
#define ENC_ERXWRPTL    REGADDR(0x0e, 0, 0) /* RX WR Pointer Младший байт (ERXWRPT<7:0>) */
#define ENC_ERXWRPTH    REGADDR(0x0f, 0, 0) /* RX WR Pointer Старший байт (ERXWRPT<12:8>) */
#define ENC_EDMASTL     REGADDR(0x10, 0, 0) /* DMA Start Младший байт (EDMAST<7:0>) */
#define ENC_EDMASTH     REGADDR(0x11, 0, 0) /* DMA Start Старший байт(EDMAST<12:8>) */
#define ENC_EDMANDL     REGADDR(0x12, 0, 0) /* DMA End Младший байт (EDMAND<7:0>) */
#define ENC_EDMANDH     REGADDR(0x13, 0, 0) /* DMA End Старший байт(EDMAND<12:8>) */
#define ENC_EDMADSTL    REGADDR(0x14, 0, 0) /* DMA Место назначения Младший байт (EDMADST<7:0>) */
#define ENC_EDMADSTH    REGADDR(0x15, 0, 0) /* DMA Место назначения Старший байт (EDMADST<12:8>) */
#define ENC_EDMACSL     REGADDR(0x16, 0, 0) /* DMA Контрольная сумма Младший байт (EDMACS<7:0>) */
#define ENC_EDMACSH     REGADDR(0x17, 0, 0) /* DMA Контрольная сумма Старший байт (EDMACS<15:8>) */
                                            /* 0x18-0x1a: Зарезервированный */
                                            /* 0x1b-0x1f: EIE, EIR, ESTAT, ECON2, ECON1 */
/* Bank 1 Адреса управляющих Регистров */

#define ENC_EHT0        REGADDR(0x00, 1, 0) /* Байт хэш-таблицы 0 (EHT<7:0>) */
#define ENC_EHT1        REGADDR(0x01, 1, 0) /* Байт хэш-таблицы 1 (EHT<15:8>) */
#define ENC_EHT2        REGADDR(0x02, 1, 0) /* Байт хэш-таблицы 2 (EHT<23:16>) */
#define ENC_EHT3        REGADDR(0x03, 1, 0) /* Байт хэш-таблицы 3 (EHT<31:24>) */
#define ENC_EHT4        REGADDR(0x04, 1, 0) /* Байт хэш-таблицы 4 (EHT<39:32>) */
#define ENC_EHT5        REGADDR(0x05, 1, 0) /* Байт хэш-таблицы 5 (EHT<47:40>) */
#define ENC_EHT6        REGADDR(0x06, 1, 0) /* Байт хэш-таблицы 6 (EHT<55:48>) */
#define ENC_EHT7        REGADDR(0x07, 1, 0) /* Байт хэш-таблицы 7 (EHT<63:56>) */
#define ENC_EPMM0       REGADDR(0x08, 1, 0) /* Байт Маски Соответствия Шаблону 0 (EPMM<7:0>) */
#define ENC_EPMM1       REGADDR(0x09, 1, 0) /* Байт Маски Соответствия Шаблону 1 (EPMM<15:8>) */
#define ENC_EPMM2       REGADDR(0x0a, 1, 0) /* Байт Маски Соответствия Шаблону 2 (EPMM<23:16>) */
#define ENC_EPMM3       REGADDR(0x0b, 1, 0) /* Байт Маски Соответствия Шаблону 3 (EPMM<31:24>) */
#define ENC_EPMM4       REGADDR(0x0c, 1, 0) /* Байт Маски Соответствия Шаблону 4 (EPMM<39:32>) */
#define ENC_EPMM5       REGADDR(0x0d, 1, 0) /* Байт Маски Соответствия Шаблону 5 (EPMM<47:40>) */
#define ENC_EPMM6       REGADDR(0x0e, 1, 0) /* Байт Маски Соответствия Шаблону 6 (EPMM<55:48>) */
#define ENC_EPMM7       REGADDR(0x0f, 1, 0) /* Байт Маски Соответствия Шаблону 7 (EPMM<63:56>) */
#define ENC_EPMCSL      REGADDR(0x10, 1, 0) /* Контрольная Сумма Соответствия Шаблону Младший Байт (EPMCS<7:0>) */
#define ENC_EPMCSH      REGADDR(0x11, 1, 0) /* Контрольная Сумма Соответствия Шаблону Старший Байт (EPMCS<15:0>) */
                                            /* 0x12-0x13: Зарезервированный */
#define ENC_EPMOL       REGADDR(0x14, 1, 0) /* Смещение Соответствия Шаблону Младший Байт (EPMO<7:0>) */
#define ENC_EPMOH       REGADDR(0x15, 1, 0) /* Совпадение с Шаблоном Смещение Старшего Байта(EPMO<12:8>) */
                                            /* 0x16-0x17: Зарезервированный */
#define ENC_ERXFCON     REGADDR(0x18, 1, 0) /* Конфигурация фильтра Приема */
#define ENC_EPKTCNT     REGADDR(0x19, 1, 0) /* Количество пакетов Ethernet */
                                            /* 0x1a: Зарезервированный */
                                            /* 0x1b-0x1f: EIE, EIR, ESTAT, ECON2, ECON1 */

/* Получать Определения Битов Конфигурации фильтра */

#define ERXFCON_BCEN    (1 << 0) /* Bit 0: Включить Широковещательный фильтр */
#define ERXFCON_MCEN    (1 << 1) /* Bit 1: Включить фильтр Многоадресной Рассылки */
#define ERXFCON_HTEN    (1 << 2) /* Bit 2: Включить фильтр Хэш-таблицы */
#define ERXFCON_MPEN    (1 << 3) /* Bit 3: Включение фильтра Волшебных Пакетов */
#define ERXFCON_PMEN    (1 << 4) /* Bit 4: Включить фильтр Соответствия Шаблону */
#define ERXFCON_CRCEN   (1 << 5) /* Bit 5: Включить проверку CRC после фильтрации */
#define ERXFCON_ANDOR   (1 << 6) /* Bit 6: AND/OR Выбор фильтра */
#define ERXFCON_UCEN    (1 << 7) /* Bit 7: Включить фильтр Одноадресной Рассылки */

/* Bank 2 Адреса управляющих Регистров*/

#define ENC_MACON1      REGADDR(0x00, 2, 1) /* MAC Control 1 */
                                            /* 0x01: Зарезервированный */
#define ENC_MACON3      REGADDR(0x02, 2, 1) /* MAC Control 3 */
#define ENC_MACON4      REGADDR(0x03, 2, 1) /* MAC Control 4 */
#define ENC_MABBIPG     REGADDR(0x04, 2, 1) /* Взаимный разрыв Между пакетами(BBIPG<6:0>) */
                                            /* 0x05: Reserved */
#define ENC_MAIPGL      REGADDR(0x06, 2, 1) /* Низкий байт Межпакетного Промежутка Без Обратной связи (MAIPGL<6:0>) */
#define ENC_MAIPGH      REGADDR(0x07, 2, 1) /* Высокий байт Межпакетного разрыва Без Обратной связи (MAIPGH<6:0>) */
#define ENC_MACLCON1    REGADDR(0x08, 2, 1) /* MAC Collision Control 1 */
#define ENC_MACLCON2    REGADDR(0x09, 2, 1) /* MAC Collision Control 2 */
#define ENC_MAMXFLL     REGADDR(0x0a, 2, 1) /* Максимальная Длина Кадра Младший Байт (MAMXFL<7:0>) */
#define ENC_MAMXFLH     REGADDR(0x0b, 2, 1) /* Максимальная Длина Кадра Старший Байт (MAMXFL<15:8>) */
                                            /* 0x0c-0x11: Reserved */
#define ENC_MICMD       REGADDR(0x12, 2, 1) /* MII Регистр команд */
                                            /* 0x13: Зарезервированный */
#define ENC_MIREGADR    REGADDR(0x14, 2, 1) /* MII Адрес регистрации */
                                            /* 0x15: Зарезервированный */
#define ENC_MIWRL       REGADDR(0x16, 2, 1) /* MII Запись Данных Младшим Байтом(MIWR<7:0>) */
#define ENC_MIWRH       REGADDR(0x17, 2, 1) /* MII Запись Данных Старший Байтом (MIWR<15:8>) */
#define ENC_MIRDL       REGADDR(0x18, 2, 1) /* MII Считывание Данных Младший Байт(MIRD<7:0>) */
#define ENC_MIRDH       REGADDR(0x19, 2, 1) /* MII Считывание Данных Старший Байт(MIRD<15:8>) */
                                            /* 0x1a: Зарезервированный */
                                            /* 0x1b-0x1f: EIE, EIR, ESTAT, ECON2, ECON1 */

/* MAC Control 1 Определения битов регистра */

#define MACON1_MARXEN   (1 << 0) /* Bit 0: Включение приема MAC */
#define MACON1_PASSALL  (1 << 1) /* Bit 1: Передать Все Полученные Кадры Включить */
#define MACON1_RXPAUS   (1 << 2) /* Bit 2: Включение приема Управляющего Кадра Паузы */
#define MACON1_TXPAUS   (1 << 3) /* Bit 3: Включение передачи Управляющего Кадра На Паузу */
                                 /* Bits 4-7: Неосуществленный или Зарезервированный */

/* MAC Control 1 Определения битов регистра*/

#define MACON3_FULDPX   (1 << 0) /* Bit 0: Включение полнодуплексного режима MAC */
#define MACON3_FRMLNEN  (1 << 1) /* Bit 1: Проверка длины Кадра Позволяет*/
#define MACON3_HFRMLEN  (1 << 2) /* Bit 2: Огромная Рамка Позволяет */
#define MACON3_PHDRLEN  (1 << 3) /* Bit 3: Проприетарный Заголовок Позволяет */
#define MACON3_TXCRCEN  (1 << 4) /* Bit 4: Передача CRC Позволяет */
#define MACON3_PADCFG0  (1 << 5) /* Bit 5: Автоматическая настройка Pad и CRC */
#define MACON3_PADCFG1  (1 << 6) /* Bit 6: "       " " " " " " " "           " */
#define MACON3_PADCFG2  (1 << 7) /* Bit 7: "       " " " " " " " "           " */

/* MAC Control 1 Определения битов регистра */

#define MACON4_NOBKOFF  (1 << 4) /* Bit 4: Нет Возможности Отключения*/
#define MACON4_BPEN     (1 << 5) /* Bit 5: Отсутствие Обратного Хода При Включении Противодавления */
#define MACON4_DEFER    (1 << 6) /* Bit 6: Бит разрешения отсрочки передачи */

/* MII Command Register Bit Definitions */

#define MICMD_MIIRD     (1 << 0) /* Bit 0: MII Включить чтение */
#define MICMD_MIISCAN   (1 << 1) /* Bit 1: MII Включить сканирование */

/* Bank 3 Control Register Addresses */

#define ENC_MAADR5      REGADDR(0x00, 3, 1) /* MAC Address Byte 5 (MAADR<15:8>) */
#define ENC_MAADR6      REGADDR(0x01, 3, 1) /* MAC Address Byte 6 (MAADR<7:0>) */
#define ENC_MAADR3      REGADDR(0x02, 3, 1) /* MAC Address Byte 3 (MAADR<31:24>), OUI Byte 3 */
#define ENC_MAADR4      REGADDR(0x03, 3, 1) /* MAC Address Byte 4 (MAADR<23:16>) */
#define ENC_MAADR1      REGADDR(0x04, 3, 1) /* MAC Address Byte 1 (MAADR<47:40>), OUI Byte 1 */
#define ENC_MAADR2      REGADDR(0x05, 3, 1) /* MAC Address Byte 2 (MAADR<39:32>), OUI Byte 2 */
#define ENC_EBSTSD      REGADDR(0x06, 3, 0) /* Встроенный Самотестирующий заполнитель семян (EBSTSD<7:0>) */
#define ENC_EBSTCON     REGADDR(0x07, 3, 0) /* Встроенный контроль Самотестирования */
#define ENC_EBSTCSL     REGADDR(0x08, 3, 0) /* Встроенная Контрольная сумма Самопроверки Младший байт (EBSTCS<7:0>) */
#define ENC_EBSTCSH     REGADDR(0x09, 3, 0) /* Встроенный Высокий Байт Контрольной суммы Самопроверки(EBSTCS<15:8>) */
#define ENC_MISTAT      REGADDR(0x0a, 3, 1) /* MII Status Register */
                                            /* 0x0b-0x11: Reserved */
#define ENC_EREVID      REGADDR(0x12, 3, 0) /* Идентификатор версии Ethernet*/
                                            /* 0x13-0x14: Reserved */
#define ENC_ECOCON      REGADDR(0x15, 3, 0) /* Управление Тактовым Выходом*/
                                            /* 0x16: Reserved */
#define ENC_EFLOCON     REGADDR(0x17, 3, 0) /* Управление потоком Ethernet */
#define ENC_EPAUSL      REGADDR(0x18, 3, 0) /* Значение Таймера Паузы Младший Байт (EPAUS<7:0>) */
#define ENC_EPAUSH      REGADDR(0x19, 3, 0) /* Значение Таймера Паузы Старший Байт(EPAUS<15:8>) */
                                            /* 0x1a: Reserved */
                                            /* 0x1b-0x1f: EIE, EIR, ESTAT, ECON2, ECON1 */

/* Встроенные Определения битов Регистра управления Самопроверкой */

#define EBSTCON_BISTST  (1 << 0) /* Bit 0: Встроенный Запуск/Занятость самотестирования */
#define EBSTCON_TME     (1 << 1) /* Bit 1: Включение тестового режима */
#define EBSTCON_TMSEL0  (1 << 2) /* Bit 2: Выбор тестового режима */
#define EBSTCON_TMSEL1  (1 << 3) /* Bit 3: "  " "  " "    " */
#define EBSTCON_PSEL    (1 << 4) /* Bit 4: Выбор порта */
#define EBSTCON_PSV0    (1 << 5) /* Bit 5: Значение Сдвига шаблона */
#define EBSTCON_PSV1    (1 << 6) /* Bit 6: "     " "   "     " */
#define EBSTCON_PSV2    (1 << 7) /* Bit 7: "     " "   "     " */

/* MII Регистр состояния Определения битов регистра */

#define MISTAT_BUSY     (1 << 0) /* Bit 0: MII Руководство Занято */
#define MISTAT_SCAN     (1 << 1) /* Bit 1: MII Управление Операцией сканирования */
#define MISTAT_NVALID   (1 << 2) /* Bit 2: MII Недопустимые Данные Для Чтения Управлением */
                                 /* Bits 3-7: Зарезервировано или не реализовано */

/* Определения битов Регистра Управления потоком Ethernet */

#define EFLOCON_FCEN0   (1 << 0) /* Bit 0: Управление Потоком Позволяет */
#define EFLOCON_FCEN1   (1 << 1) /* Bit 1: "  " "     " "    " */
#define EFLOCON_FULDPXS (1 << 2) /* Bit 2: Полнодуплексная тень MAC Только для чтения */
                                 /* Bits 3-7: Reserved or unimplemented */

/* PHY Регистр ************************************************************/

#define ENC_PHCON1        (0x00)    /* PHY Регистр управления 1 */
#define ENC_PHSTAT1       (0x01)    /* PHY Status 1 */
#define ENC_PHID1         (0x02)    /* PHY ID Register 1 */
#define ENC_PHID2         (0x03)    /* PHY ID Register 2 */
#define ENC_PHCON2        (0x10)    /* PHY Control Register 2 */
#define ENC_PHSTAT2       (0x11)    /* PHY Status 2 */
#define ENC_PHIE          (0x12)    /* PHY Регистр включения прерывания */
#define ENC_PHIR          (0x13)    /* PHY Регистр запросов на прерывание */
#define ENC_PHLCON        (0x14)

/* PHY Управляющий Регистр 1 Определения битов регистра*/

#define PHCON1_PDPXMD     (1 << 8)  /* Bit 8:  PHY Дуплексный режим */
#define PHCON1_PPWRSV     (1 << 11) /* Bit 11: PHY Отключение питания */
#define PHCON1_PLOOPBK    (1 << 14) /* Bit 14: PHY Обратная петля */
#define PHCON1_PRST       (1 << 15) /* Bit 15: PHY Сброс программного обеспечения */

/* PHY Status 1 Register Bit Definitions */

#define PHSTAT1_JBSTAT    (1 << 1)  /* Bit 1: PHY Фиксация статуса Jabber */
#define PHSTAT1_LLSTAT    (1 << 2)  /* Bit 2: PHY Состояние соединения с Фиксацией*/
#define PHSTAT1_PHDPX     (1 << 11) /* Bit 11: PHY Возможность полудуплексного подключения */
#define PHSTAT1_PFDPX     (1 << 12) /* Bit 12: PHY Возможность полного Дуплекса */

/* PHY Управляющий Регистр 2 Определения Битов регистра*/

#define PHCON2_HDLDIS     (1 << 8)  /* Bit 8:  PHY Отключение Полудуплексной Обратной связи */
#define PHCON2_JABBER     (1 << 10) /* Bit 10: Отключение коррекции Jabber */
#define PHCON2_TXDIS      (1 << 13) /* Bit 13: Отключение передатчика по витой паре */
#define PHCON2_FRCLINK    (1 << 14) /* Bit 14: PHY Соединение сил */

/* PHY Status 2 Register Bit Definitions */

#define PHSTAT2_PLRITY    (1 << 5)  /* Bit 5:  Состояние полярности */
#define PHSTAT2_DPXSTAT   (1 << 9)  /* Bit 9:  PHY Дуплексный статус */
#define PHSTAT2_LSTAT     (1 << 10) /* Bit 10: PHY Статус ссылки */
#define PHSTAT2_COLSTAT   (1 << 11) /* Bit 11: PHY Статус столкновения */
#define PHSTAT2_RXSTAT    (1 << 12) /* Bit 12: PHY Получение статуса*/
#define PHSTAT2_TXSTAT    (1 << 13) /* Bit 13: PHY Статус передачи */

/* PHY Прерывание Позволяет Определять Регистровые разряды */

#define PHIE_PGEIE        (1 << 1)  /* Bit 1:  PHY Включение глобального прерывания */
#define PHIE_PLNKIE       (1 << 4)  /* Bit 4:  PHY Включение прерывания Изменения связи */

/* PHIR Определения битов регистра */

#define PHIR_PGIF         (1 << 2)  /* Bit 2:  PHY Глобальное прерывание */
#define PHIR_PLNKIF       (1 << 4)  /* Bit 4:  PHY Прерывание смены канала связи */

/* PHLCON Regiser Bit Definitions */
                                    /* Bit 0:  Reserved */
#define PHLCON_STRCH      (1 << 1)  /* Bit 1:  LED Импульсное Растяжение  */
#define PHLCON_LFRQ0      (1 << 2)  /* Bit 2:  LED Конфигурация Времени Растяжения Импульса */
#define PHLCON_LFRQ1      (1 << 3)  /* Bit 3:  " " "   " "     " "  " " */
#define PHLCON_LBCFG0     (1 << 4)  /* Bit 4:  LEDB Configuration */
#define PHLCON_LBCFG1     (1 << 5)  /* Bit 5:  "  " "           " */
#define PHLCON_LBCFG2     (1 << 6)  /* Bit 6:  "  " "           " */
#define PHLCON_LBCFG3     (1 << 7)  /* Bit 7:  "  " "           " */
#define PHLCON_LACFG0     (1 << 8)  /* Bit 8:  LEDA Configuration */
#define PHLCON_LACFG1     (1 << 9)  /* Bit 9:  "  " "           " */
#define PHLCON_LACFG2     (1 << 10) /* Bit 10: "  " "           " */
#define PHLCON_LACFG3     (1 << 11) /* Bit 11: "  " "           " */

/* Packet Memory ************************************************************/

/* 8-Килобайтный Пакет Передачи/Приема С Двумя Портами SRAM */

#define PKTMEM_START      0x0000
#define PKTMEM_END        0x1fff

/* максимальная единица передачи */
#define CONFIG_NET_ETH_MTU 1500

/* Packet Control Bits Definitions ******************************************/

#define PKTCTRL_POVERRIDE (1 << 0)  /* Bit 0:  Переопределение для каждого пакета */
#define PKTCTRL_PCRCEN    (1 << 1)  /* Bit 1:  Для каждого Пакета CRC Включить */
#define PKTCTRL_PPADEN    (1 << 2)  /* Bit 2:  Включение Заполнения Для Каждого Пакета */
#define PKTCTRL_PHUGEEN   (1 << 3)  /* Bit 3:  За Пакет Огромный Фрейм Позволяет */

/* RX Определения битов состояния ************************************************/

#define RXSTAT_LDEVENT    (1 << 0)  /* Bit 0:  Длинное событие или сброшенный пакет */
                                    /* Bit 1:  Reserved */
#define RXSTAT_CEPS       (1 << 2)  /* Bit 2:  Событие перевозчика, ранее замеченное */
                                    /* Bit 3:  Reserved */
#define RXSTAT_CRCERROR   (1 << 4)  /* Bit 4:  Плохое поле CRC кадра */
#define RXSTAT_LENERROR   (1 << 5)  /* Bit 5:  Длина пакета != длина данных */
#define RXSTAT_LENRANGE   (1 << 6)  /* Bit 6:  Поле типа/длины > 1500 байт */
#define RXSTAT_OK         (1 << 7)  /* Bit 7:  Пакет с действительным CRC и без ошибок символов */
#define RXSTAT_MCAST      (1 << 8)  /* Bit 8:  Пакет с адресом многоадресной рассылки */
#define RXSTAT_BCAST      (1 << 9)  /* Bit 9:  Пакет с широковещательным адресом */
#define RXSTAT_DRIBBLE    (1 << 10) /* Bit 10: Дополнительные биты, полученные после пакета */
#define RXSTAT_CTRLFRAME  (1 << 11) /* Bit 11: Рамка управления с допустимым типом/длиной */
#define RXSTAT_PAUSE      (1 << 12) /* Bit 12: Управляющий кадр с кодом операции кадра паузы */
#define RXSTAT_UNKOPCODE  (1 << 13) /* Bit 13: Управляющий кадр с неизвестным кодом операции */
#define RXSTAT_VLANTYPE   (1 << 14) /* Bit 14: Текущий кадр - это кадр с тегом VLAN */
                                    /* Bit 15: Zero */
/* TSV (txt формат БД) определения битов*/
#define TSV_LATECOL       (1 << 5) /* Bit 5: Ошибка позднего столкновения, байт RSV 3 */



/** @defgroup ETH_Duplex_Mode ETH Duplex Mode - дуплексный режим работы
  * @{
  */
#define ETH_MODE_FULLDUPLEX       ((uint32_t)0x00000800)
#define ETH_MODE_HALFDUPLEX       ((uint32_t)0x00000000)
/**
  * @}
  */

/** @defgroup ETH_Rx_Mode ETH Rx Mode - работа на чтение данных
  * @{
  */
#define ETH_RXPOLLING_MODE      ((uint32_t)0x00000000)
#define ETH_RXINTERRUPT_MODE    ((uint32_t)0x00000001)
/**
  * @}
  */

/** @defgroup ETH_Checksum_Mode ETH Checksum Mode - вкл контрольную сумму
  * @{
  */
#define ETH_CHECKSUM_BY_HARDWARE      ((uint32_t)0x00000000)
#define ETH_CHECKSUM_BY_SOFTWARE      ((uint32_t)0x00000001)
/**
  * @}
  */

/* Экспортируемые функции--------------------------------------------------------*/
/** @addtogroup SPI_Exported_Functions	- вкл SPI
  * @{
  */

/** @addtogroup SPI_Exported_Functions_Group1
  * @{
  */
/* Функции инициализации/деинициализации  **********************************/


/**
  * Инициализируйте enc28j60 и настройте необходимые аппаратные ресурсы
  * дескриптор параметров: Дескриптор конфигурации данных.
  * ретвал Отсутствует
  */

bool ENC_Start(ENC_HandleTypeDef *handle);

/****************************************************************************
* Функция: ENC_SetMacAddr
 *
 * Описание:
 * Установите MAC-адрес на заданное значение.  Это делается после ifup
 * или после тайм-аута передачи данных.  Обратите внимание, что это означает, что интерфейс должен
 * отключитесь перед настройкой MAC-адреса.
 *
 * Параметры:
 * handle - Ссылка на структуру состояния драйвера
 *
 * Возвращаемое значение:
 * Нет
 *
 * Допущения:
 *
 ****************************************************************************/

void ENC_SetMacAddr(ENC_HandleTypeDef *handle);

/****************************************************************************
 * Функция: ENC_RestoreTXBuffer
 *
* Описание:
* Запись данных в буфер.
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   len     - длина буфера
 *
 * Returned Value:
 *    ERR_OK          0    Ошибок нет, все в порядке.
 *    ERR_MEM        -1    Ошибка нехватки памяти.
 *    ERR_TIMEOUT    -3    Ошибка Timeout.
 *
 * Допущения:
 *
 ****************************************************************************/

int8_t ENC_RestoreTXBuffer(ENC_HandleTypeDef *handle, uint16_t len);

/****************************************************************************
 * Функция: ENC_WriteBuffer
 *
 * Description:
 *   Запись данных в буфер.
 *
 * Параметры:
 *   buffer  - Указатель на буфер для записи
 *   buflen  - Количество байтов для записи
 *
 * Returned Value:
 *   None
 *
 * Допущения:
 *   Указатель чтения установлен на правильный адрес
 *
 ****************************************************************************/

void ENC_WriteBuffer(void *buffer, uint16_t buflen);

/****************************************************************************
 * Function: ENC_Transmit
 *
 * Description:
 *   Запустите аппаратную передачу. Звонил либо из:
 *
 * - прерывание pkif, когда приложение отвечает на
 * получение данныхпри попытке отправить что-либо, или
 * - Из опроса на основе вызова.
 *
 * Параметры:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   none
 *
 * Допущения:
 *
 ****************************************************************************/

#ifdef USE_PROTOTHREADS
PT_THREAD(ENC_Transmit(struct pt *pt, ENC_HandleTypeDef *handle));
#else
void ENC_Transmit(ENC_HandleTypeDef *handle);
#endif

 /**
  * @}
  */

 /**
  * @}
  */

/****************************************************************************
 * Function: ENC_GetReceivedFrame
 *
 * Description:
 *   Проверьте, получили ли мы пакет, и если да, извлеките его.
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 * true, если доступен новый пакет; false в противном случае
 *
 * Assumptions:
 *
 ****************************************************************************/

bool ENC_GetReceivedFrame(ENC_HandleTypeDef *handle);

/****************************************************************************
 * Function: ENC_IRQHandler
 *
 * Description:
 * Выполните логику обработки прерываний вне обработчика прерываний (в
 * потоке рабочей очереди).
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

void ENC_IRQHandler(ENC_HandleTypeDef *handle);

/****************************************************************************
 * Function: ENC_EnableInterrupts
 *
 * Description:
 *   Включение отдельных прерываний ENC28J60
 *
 * Parameters:
 *   bits - Разрешить биты прерывания
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

void ENC_EnableInterrupts(uint8_t bits);

/****************************************************************************
 * Function: ENC_GetPkcnt
 *
 * Description:
 * Получить количество ожидающих приема пакетов
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   номер полученного пакета, который еще не обработан
 *
 * Assumptions:
 *
 ****************************************************************************/

void ENC_GetPkcnt(ENC_HandleTypeDef *handle);


/****************************************************************************
 * Function: up_udelay
 *
 * Description:
 *   ожидание
 *
 * Parameters:
 *   us  - Время ожидания в мкс
 *
 * Returned Value:
 *   none
 *
 * Assumptions:
 *
 ****************************************************************************/

__inline void up_udelay(uint32_t us);

#endif /* ENC28J60_H_INCLUDED */
