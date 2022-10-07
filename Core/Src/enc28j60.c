
/* Includes ------------------------------------------------------------------*/
#include "enc28j60.h"

/** @addtogroup BSP
  * @{
  */

/** @addtogroup Components
  * @{
  */

/** @defgroup ENC28J60
  * @{
  */

/** @defgroup ENC28J60_Private_Types_Definitions
  * @{
  */

/** @defgroup ENC28J60_Private_Defines
  * @{
  */

#define ENC_POLLTIMEOUT 50

/**
  * @}
  */

/** @defgroup ENC28J60_Private_Macros
  * @{
  */

/* Работа с памятью ************************************************************/

/* расположение пакетов памяти  */

#define ALIGNED_BUFSIZE ((CONFIG_NET_ETH_MTU + 255) & ~255)

/*стек ФИФО*/

#  define PKTMEM_RX_START 0x0000                            /* RX буфер обнуление */
#  define PKTMEM_RX_END   (PKTMEM_END-ALIGNED_BUFSIZE)      /* Длина RX-буфера равна общей SRAM минус TX-буфер */
#  define PKTMEM_TX_START (PKTMEM_RX_END+1)                 /* Запустите буфер TX после */
#  define PKTMEM_TX_ENDP1 (PKTMEM_TX_START+ALIGNED_BUFSIZE) /* Разрешить буфер TX для двух кадров */

/*макросы вызова*/
#define enc_rdgreg(ctrlreg) \
  enc_rdgreg2(ENC_RCR | GETADDR(ctrlreg))					/*макрос вызова регистра для чтения данных*/
#define enc_wrgreg(ctrlreg, wrdata) \
  enc_wrgreg2(ENC_WCR | GETADDR(ctrlreg), wrdata)			/*макрос вызова регистра для записи данных*/
#define enc_bfcgreg(ctrlreg,clrbits) \
  enc_wrgreg2(ENC_BFC | GETADDR(ctrlreg), clrbits)			/*макрос установки битового поля*/
#define enc_bfsgreg(ctrlreg,setbits) \
  enc_wrgreg2(ENC_BFS | GETADDR(ctrlreg), setbits)			/*макрос отчистки битового поля*/

/**
  * @}
  */

/** @defgroup ENC28J60_Private_Variables
  * @{
  */

  /* Хранит количество итераций, которые микроконтроллер может выполнить за 1 мкс */
static uint32_t iter_per_us=0;

/**
  * @}
  */

/** @defgroup ENC28J60_Private_Function_Prototypes
  * @{
  */
/**
  * @}
  */

/** @defgroup ENC28J60_Private_Functions
  * @{
  */

/**
   Calibrate the constant time
 **/

static void calibrate(void)
{
    uint32_t time;
    volatile uint32_t i;

    iter_per_us = 1000000;

    time = HAL_GetTick();
    while (HAL_GetTick() == time) {
    }
    for (i=0; i<iter_per_us; i++) {
    }
    iter_per_us /= ((HAL_GetTick()-time)*1000);
}

/**
 * Программная задержка в мкс
 * us: количество ожидаемых мкс
 **/
__inline void up_udelay(uint32_t us)
{
    volatile uint32_t i;

    for (i=0; i<us*iter_per_us; i++) {
    }
}
/****************************************************************************
 * Function: enc_rdgreg2
 *
 * Description:
 * Прочитайте глобальный реестр (EIE, EIR, ESTATE, ECON 2 или ECON 1).  Команда cmd
 * включите CMD 'ИЛИ ' в глобальный регистр адресов.
 *
 * Parameters:
 *   cmd   - Полная команда для получения(cmd | address)
 *
 * Returned Value:
 *   Значение, считанное из регистра
 *
 * Assumptions:
 *
 ****************************************************************************/

static uint8_t enc_rdgreg2(uint8_t cmd)
{
    uint8_t cmdpdata[2];
    cmdpdata[0] = cmd;

/* Отправьте команду чтения и соберите данные.  Последовательность требует
   * 16-часы: 8 для синхронизации cmd + 8 для синхронизации данных.
   */
   
  ENC_SPI_SendBuf(cmdpdata, cmdpdata, 2);

  return cmdpdata[1];
}


/****************************************************************************
 * Function: enc_wrgreg2
 *
 * Description:
 * Запись в глобальный реестр (EIE, EIR, ESTATE, ECON 2 или ECON 1).  Команда cmd
 * включите CMD 'ИЛИ ' в глобальный регистр адресов.
 *
 * Parameters:
 *   cmd    - Полная команда для получения (cmd | address)
 *   wrdata - Данные для отправки
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

static void enc_wrgreg2(uint8_t cmd, uint8_t wrdata)
{
    uint8_t cmdpdata[2];
    cmdpdata[0] = cmd;
    cmdpdata[1] = wrdata;


/* Отправьте команду записи и данные.  Для выполнения последовательности требуется 16 тактов:
 * 8 для синхронизации cmd + 8 для синхронизации данных.
 */

    ENC_SPI_SendBuf(cmdpdata, NULL, 2);
}


/****************************************************************************
 * Function: enc_waitgreg
 *
 * Description:
 * Подождите, пока сгруппированные биты регистра не примут определенное значение (или тайм-аут
 * происходит).
 *
 * Parameters:
 *   ctrlreg - Битовый кодированный адрес банковского регистра для проверки
 *   bits    - Биты для проверки (маска)
 *   value   - Значение возвращаемых битов (значение под маской)
 *
 * Returned Value:
 *   ОК при успехе, отрицательная ошибка при неудаче
 *
 * Assumptions:
 *
 ****************************************************************************/

static bool enc_waitgreg(uint8_t ctrlreg,
                        uint8_t bits, uint8_t value)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;
  uint8_t  rddata;

	/* Цикл до тех пор, пока не будет выполнено условие выхода */
  do
    {
		/* Считывает байт из запрошенного банковского регистра */

      rddata  = enc_rdgreg(ctrlreg);
      elapsed = HAL_GetTick() - start;
    }
  while ((rddata & bits) != value && elapsed < ENC_POLLTIMEOUT);

  return (rddata & bits) == value;
}

/****************************************************************************
 * Function: enc_waitwhilegreg
 *
 * Description:
 * Подождите, пока сгруппированные биты регистра не получат определенное значение (или тайм-аут
 * происходит).
 *
 * Parameters:
 *   ctrlreg - Битовый кодированный адрес банковского регистра для проверки
 *   bits    - Биты для проверки (маска)
 *   value   - Значение возвращаемых битов (значение под маской)
 *
 * Returned Value:
 *   ОК при успехе, отрицательная ошибка при неудаче
 *
 * Assumptions:
 *
 ****************************************************************************/

#ifndef USE_PROTOTHREADS
static bool enc_waitwhilegreg(uint8_t ctrlreg,
                        uint8_t bits, uint8_t value)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;
  uint8_t  rddata;

	/* Цикл до тех пор, пока не будет выполнено условие выхода */
  do
    {
      /* Считывает байт из запрошенного банковского регистра */

      rddata  = enc_rdgreg(ctrlreg);
      elapsed = HAL_GetTick() - start;
    }
  while ((rddata & bits) == value && elapsed < ENC_POLLTIMEOUT);

  return (rddata & bits) != value;
}
#endif

/**
  * @brief  Выполните плавный сброс на enc28j60
  * Description:
  *   Отправьте однобайтовую команду сброса системы (SRC).
  *
  * "Команда сброса системы (SRC) позволяет хост-контроллеру выдать
  * Команда системного мягкого сброса.  В отличие от других команд SPI, SRC является
  * только однобайтовая команда и не работает ни с одним регистром. Команда
  * запускается путем опускания штифта CS. Код операции SRC - это
  * отправлено, за которым следует 5-битная константа команды плавного сброса, равная 1fH. Операция
  * SRC завершается поднятием PIN-кода CS."
  *
  * @param  None
  * @retval None
  */
void enc_reset(ENC_HandleTypeDef *handle) {

/* Отправьте команду сброса системы. */
  ENC_SPI_Send(ENC_SRC);

/* Проверьте бит CLKRDY, чтобы узнать, когда сброс будет завершен.  Есть опечатка
   * * это говорит о том, что ГОСПОДЬ может быть недействительным.  Мы подождем пару мс, чтобы
   * обходной путь этого условия.
   *
   * Кроме того, "После сброса системы все регистры PHY не должны считываться или
   * записывается до тех пор, пока с момента завершения сброса не пройдет не менее 50 мкс.
   * Все регистры вернутся к своим сброшенным значениям по умолчанию. Двойной
   * буферная память порта будет поддерживать состояние на протяжении всего сброса системы."
   */

  handle->bank = 0; /* Инициализировать трассировку для текущего выбранного банка */

  HAL_Delay(2); /* >1000 µs, соответствует ошибкам #2 */
}

/****************************************************************************
 * Function: enc_setbank
 *
 * Description:
 * Установите банк для следующего доступа к регистрам управления. *
 * Assumption:
 *   Вызывающий абонент имеет эксклюзивный доступ к шине SPI
 *
 * Parameters:
 *   handle - Ссылка на структуру состояния драйвера
 *   bank   - Выбор банка (0-3)
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

void enc_setbank(ENC_HandleTypeDef *handle, uint8_t bank) {

  if (bank != handle->bank) {
      /* Выберите банк 0 (просто так, чтобы все биты были очищены) */

      enc_bfcgreg(ENC_ECON1, ECON1_BSEL_MASK);

      /* Затем ИЛИ в битах, чтобы получить правильный банк */

      if (bank != 0)
        {
          enc_bfsgreg(ENC_ECON1, (bank << ECON1_BSEL_SHIFT));
        }

      /* Затем запомните настройки банка */

      handle->bank = bank;
    }
}

/****************************************************************************
 * Function: enc_rdbreg
 *
 * Description:
 * Считывание из накопленного управляющего регистра с помощью команды RCR. *
 
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   ctrlreg - Битовый кодированный адрес банковского регистра для чтения
 *
 * Returned Value:
 *   Байт, считанный из накопленного регистра
 *
 * Assumptions:
 *
 ****************************************************************************/

static uint8_t enc_rdbreg(ENC_HandleTypeDef *handle, uint8_t ctrlreg)
{
  uint8_t data[3];


  enc_setbank(handle, GETBANK(ctrlreg));

  /* Отправьте команду RCR и соберите данные.  Как мы собираем данные
   * зависит от того, является ли это PHY / CAN или нет.  Нормальная последовательность требует
   * 16-часы: 8 для синхронизации cmd и 8 для синхронизации данных.
   */

  data[0] = ENC_RCR | GETADDR(ctrlreg);

  /* Для последовательности PHY/MAC требуется 24 такта: 8 для синхронизации cmd,
   * 8 фиктивных битов и 8 для синхронизации данных PHY/MAC.
   */

  ENC_SPI_SendBuf(data, data, (ISPHYMAC(ctrlreg))?3:2);
  return (ISPHYMAC(ctrlreg))?data[2]:data[1];
}

/****************************************************************************
 * Function: enc_wrbreg
 *
 * Description:
 * Запись в накопительный регистр управления с помощью команды WCR.  В отличие от
 * чтение, эта же последовательность SPI работает для normal, MAC и PHY
 * регистры.
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   ctrlreg - Битовый кодированный адрес накопительного регистра для записи
 *   wrdata  - Данные для отправки
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

static void enc_wrbreg(ENC_HandleTypeDef *handle, uint8_t ctrlreg,
                       uint8_t wrdata)
{
  uint8_t data[2];


  enc_setbank(handle, GETBANK(ctrlreg));

  /* Отправьте команду WCR и данные.  Для выполнения последовательности требуется 16 тактов:
   * 8 для синхронизации cmd + 8 для синхронизации данных.
   */

  data[0] = ENC_WCR | GETADDR(ctrlreg);
  data[1] = wrdata;

  ENC_SPI_SendBuf(data, NULL, 2);
}

/****************************************************************************
 * Function: enc_waitbreg
 *
 * Description:
 * Подождите, пока бит(ы) накопленного регистра не примет определенное значение (или тайм-аут
 * происходит).
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   ctrlreg - Битовый кодированный адрес банковского регистра для проверки
 *   bits    - Биты для проверки (маска)
 *   value   - Значение возвращаемых битов (значение под маской)
 *
 * Returned Value:
 *   OK при успешном выполнении, отрицательная ошибка при сбое
 *
 * Assumptions:
 *
 ****************************************************************************/

static bool enc_waitbreg(ENC_HandleTypeDef *handle, uint8_t ctrlreg,
                        uint8_t bits, uint8_t value)
{
  uint32_t start = HAL_GetTick();
  uint32_t elapsed;
  uint8_t  rddata;

	/* Цикл до тех пор, пока не будет выполнено условие выхода */

  do
    {
		/* Считывает байт из запрошенного банковского регистра */

      rddata  = enc_rdbreg(handle, ctrlreg);
      elapsed = HAL_GetTick() - start;
    }
  while ((rddata & bits) != value && elapsed < ENC_POLLTIMEOUT);

  return (rddata & bits) == value;
}


/****************************************************************************
 * Function: enc_rdphy
 *
 * Description:
 *   Read 16-bits of PHY data.
 *
 * Parameters:
 *   priv    - Ссылка на структуру состояния драйвера
 *   phyaddr - Адрес регистра PHY
 *
 * Returned Value:
 *   16-битное значение, считанное с PHY
 *
 * Assumptions:
 *
 ****************************************************************************/

static uint16_t enc_rdphy(ENC_HandleTypeDef *handle, uint8_t phyaddr)
{
  uint16_t data = 0;

/* "Для чтения из регистра PHY:
   *
   * 1. Запишите адрес регистра PHY для чтения в MIREGADR
   * зарегистрируйтесь.
   */

  enc_wrbreg(handle, ENC_MIREGADR, phyaddr);

  /*  2 . Установите бит MIC MD.MIIRD. Начинается операция чтения, и
   * ОШИБКА.установлен бит ЗАНЯТОСТИ.
   */
   
  enc_wrbreg(handle, ENC_MICMD, MICMD_MIIRD);

/* 3. Подождите 10,24 мкс.Опросите ТУМАН В бит.BUSY, чтобы убедиться, что
   * операция завершена. Во время занятости хост-контроллер не должен
   * запуск любых операций MIISCAN или запись в регистр MIWRH.
   *
   * Когда MAC получит содержимое регистра, бит ЗАНЯТОСТИ будет
   * очистите себя.
   */

//  volatile int i;
//  for (i=0; i<12*17; i++) {
//  }

  up_udelay(12);

  if (enc_waitbreg(handle, ENC_MISTAT, MISTAT_BUSY, 0x00))
    {
/* 4. Очистите бит MIC MD.MIIRD. */

      enc_wrbreg(handle, ENC_MICMD, 0x00);

/* 5 . Считайте нужные данные из СРЕДНЕГО и ВЕСЕЛОГО регистров. Порядок
*, в котором осуществляется доступ к этим байтам, не имеет значения."
       */

      data  = (uint16_t)enc_rdbreg(handle, ENC_MIRDL);
      data |= (uint16_t)enc_rdbreg(handle, ENC_MIRDH) << 8;
    }

  return data;
}

/****************************************************************************
 * Function: enc_wrphy
 *
 * Description:
 *   write 16-bits of PHY data.
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   phyaddr - The PHY register address
 *   phydata - 16-bit данные для записи в PHY
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

static void enc_wrphy(ENC_HandleTypeDef *handle, uint8_t phyaddr,
                      uint16_t phydata)
{
/* "Для записи в регистр PHY:
   *
   * 1. Введите адрес регистра PHY для записи в
   * Регистрация MIREGADR.
   */

  enc_wrbreg(handle, ENC_MIREGADR, phyaddr);

/* 2. Запишите младшие 8 бит данных для записи в регистр MIWRL. */

  enc_wrbreg(handle, ENC_MIWRL, phydata);

/* 3. Запишите верхние 8 бит данных для записи в регистр MIWRH.
   * Запись в этот регистр автоматически запускает транзакцию MIIM,
   * таким образом, он должен быть записан после MIWRL. ТУМАН В бит.BUSY становится
   * набор.
   */

  enc_wrbreg(handle, ENC_MIWRH, phydata >> 8);

/* Регистр PHY будет записан после завершения операции MIIM,
* что занимает 10,24 мкс.Когда операция записи завершится, ЗАНЯТЫЙ
   * бит очистится сам по себе.
   *
   * Хост-контроллер не должен запускать какие-либо операции MIISCAN или MIIRD
   * пока занят."
   */

  /* wait for approx 12 µs */
//  volatile int i;
//  for (i=0; i<12*17; i++) {
//  }

  up_udelay(12);
  enc_waitbreg(handle, ENC_MISTAT, MISTAT_BUSY, 0x00);
}


/****************************************************************************
 * Function: enc_pwrfull
 *
 * Description:
* Когда требуется нормальная работа, главный контроллер должен выполнить
 * слегка измененная процедура:
 *
 * 1. Пробуждение путем очистки ECON2.PWRSV.
* 2. Подождите не менее 300 мкс, пока PHY стабилизируется.Чтобы выполнить
задержку *, хост-контроллер может опросить ESTAT.CLKRDY и дождаться его
 * чтобы стать установленным.
 * 3. Восстановите возможность приема, установив ECON1.RXEN.
 *
 * После выхода из спящего режима возникает задержка во много миллисекунд
 * до установления новой ссылки (при условии наличия соответствующей ссылки
 * присутствует партнер). Главный контроллер может пожелать подождать, пока
 * связь устанавливается перед попыткой передачи каких-либо пакетов.
 * Статус соединения может быть определен путем опроса бита PHSTAT2.LSTAT.
 * В качестве альтернативы может использоваться прерывание смены канала связи, если оно
 * включено.
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

#if 0
static void enc_pwrfull(ENC_HandleTypeDef *handle)
{
	/* 1. Пробуждение путем очистки ECON2.PWRSV. */

  enc_bfcgreg(ENC_ECON2, ECON2_PWRSV);
  
	/* 2. Подождите не менее 300 мкс, пока PHY стабилизируется.Для выполнения
   * задержка, хост-контроллер может опросить ESTAT.CLKRDY и ждать, пока он
   * станьте установленным.
   */
   
  /* wait for approx 350 µs */
//  volatile int i;
//  for (i=0; i<350*17; i++) {
//  }

  up_udelay(350);

  enc_waitbreg(handle, ENC_ESTAT, ESTAT_CLKRDY, ESTAT_CLKRDY);

	/* 3. Восстановите возможность приема, установив ECON1.RXEN.
   *
   * Вызывающий абонент сделает это, когда будет готов к приему пакетов
   */
}
#endif

/**
  * @brief  Инициализируйте enc28j60 и настройте необходимые аппаратные ресурсы
  * @param  handle: Обработайте конфигурацию данных.
  * @retval None
  */
bool ENC_Start(ENC_HandleTypeDef *handle)
{
	/* значение регистра */
    uint8_t regval;

    /* Калибровка постоянной времени */
    calibrate();

    /* Сброс системы */
	enc_reset(handle);

	/* Использовать банк 0 */
	enc_setbank(handle, 0);

	/* Проверьте, действительно ли мы общаемся с ENC28J60.  Если это
     * 0x00 или 0xff, тогда мы, вероятно, неправильно общаемся
     * через SPI.
     */

    regval = enc_rdbreg(handle, ENC_EREVID);
    if (regval == 0x00 || regval == 0xff) {
      return false;
    }

    /* Initialize ECON2: Включить автоматическое увеличение адреса.
     */

    enc_wrgreg(ENC_ECON2, ECON2_AUTOINC /* | ECON2_VRPS*/);

	/* Инициализируйте буфер приема.
     * Сначала установите начальный адрес буфера приема.
     */

    handle->nextpkt = PKTMEM_RX_START;
    enc_wrbreg(handle, ENC_ERXSTL, PKTMEM_RX_START & 0xff);
    enc_wrbreg(handle, ENC_ERXSTH, PKTMEM_RX_START >> 8);

    /* Установите указатель приема данных */

    /* Errata 14 */
    enc_wrbreg(handle, ENC_ERXRDPTL, PKTMEM_RX_END & 0xff);
    enc_wrbreg(handle, ENC_ERXRDPTH, PKTMEM_RX_END >> 8);
/*
    enc_wrbreg(handle, ENC_ERXRDPTL, PKTMEM_RX_START & 0xff);
    enc_wrbreg(handle, ENC_ERXRDPTH, PKTMEM_RX_START >> 8);
*/

    /* Установите конец буфера приема. */

    enc_wrbreg(handle, ENC_ERXNDL, PKTMEM_RX_END & 0xff);
    enc_wrbreg(handle, ENC_ERXNDH, PKTMEM_RX_END >> 8);

    /* Установите начало буфера передачи. */

    handle->transmitLength = 0;
    enc_wrbreg(handle, ENC_ETXSTL, PKTMEM_TX_START & 0xff);
    enc_wrbreg(handle, ENC_ETXSTH, PKTMEM_TX_START >> 8);

    /* Установите режим фильтра: одноадресная ИЛИ широковещательная рассылка И допустимый crc */

    enc_wrbreg(handle, ENC_ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);

	do {
		HAL_Delay(10); /* Подождите 10 мс, чтобы часы были готовы */
		regval = enc_rdbreg(handle, ENC_ESTAT);
	} while ((regval & ESTAT_CLKRDY) == 0);

    /* Включить прием MAC */

    enc_wrbreg(handle, ENC_MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);

    /* Включить автоматическое заполнение и операции CRC*/

    if (handle->Init.DuplexMode == ETH_MODE_HALFDUPLEX) {
      enc_wrbreg(handle, ENC_MACON3,
                 ((handle->Init.ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)?MACON3_PADCFG0 | MACON3_TXCRCEN:0) |
                 MACON3_FRMLNEN);
      enc_wrbreg(handle, ENC_MACON4, MACON4_DEFER);        /* Defer transmission enable */

      /* Установите Интервал между пакетами, Не Являющийся Обратным*/

      enc_wrbreg(handle, ENC_MAIPGL, 0x12);
      enc_wrbreg(handle, ENC_MAIPGH, 0x0c);

      /* Установите Взаимный разрыв Между пакетами */

      enc_wrbreg(handle, ENC_MABBIPG, 0x12);
    } else {
      /* Установите режим фильтра: одноадресная ИЛИ широковещательная передача, допустимый crc И Полный дуплекс.*/

      enc_wrbreg(handle, ENC_MACON3,
                ((handle->Init.ChecksumMode == ETH_CHECKSUM_BY_HARDWARE)?MACON3_PADCFG0 | MACON3_TXCRCEN:0) |
                MACON3_FRMLNEN | MACON3_FULDPX);

      /* Установите Интервал между пакетами, Не Являющийся Обратным*/

      enc_wrbreg(handle, ENC_MAIPGL, 0x12);

      /* Установите Взаимный разрыв Между пакетами*/

      enc_wrbreg(handle, ENC_MABBIPG, 0x15);
    }

    /* Установите максимальный размер пакета, который будет принимать контроллер */

    enc_wrbreg(handle, ENC_MAMXFLL, (CONFIG_NET_ETH_MTU+18) & 0xff);
    enc_wrbreg(handle, ENC_MAMXFLH, (CONFIG_NET_ETH_MTU+18) >> 8);

  /* Configure LEDs (No, just use the defaults for now) */
  /* enc_wrphy(priv, ENC_PHLCON, ??); */

    /* Setup up PHCON1 & 2 */

    if (handle->Init.DuplexMode == ETH_MODE_HALFDUPLEX) {
      enc_wrphy(handle, ENC_PHCON1, 0x00);
      enc_wrphy(handle, ENC_PHCON2, PHCON2_HDLDIS); /* errata 9 workaround */
    } else {
      enc_wrphy(handle, ENC_PHCON1, PHCON1_PDPXMD); /* errata 10 workaround */
      enc_wrphy(handle, ENC_PHCON2, 0x00);
    }

    /* Не используется Восстановление нормального режима работы
    enc_pwrfull(handle); */

    /* Настройки прерывания процесса */
    if (handle->Init.InterruptEnableBits & EIE_LINKIE) {
      /* Включить прерывание смены канала связи в модуле PHY */
      enc_wrphy(handle, ENC_PHIE, PHIE_PGEIE | PHIE_PLNKIE);
    }

    /* Поскольку мы не изменяем регистр PHALCON, мы не попадаем в случай ошибок 11 */

    /* Сбросить все флаги прерывания */
    enc_bfcgreg(ENC_EIR, EIR_ALLINTS);

    regval = handle->Init.InterruptEnableBits;
    if (regval) {
        /* Убедитесь, что значение INT установлено, когда выбрано хотя бы прерывание */
        regval |= EIE_INTIE;
    }
    /* Включить выбранные прерывания в модуле контроллера ethernet */
    enc_bfsgreg(ENC_EIE, regval);

    /* Включить приемник */
    enc_bfsgreg(ENC_ECON1, ECON1_RXEN);

    return true;
}

/**
  * @}
  */

/****************************************************************************
 * Function: ENC_SetMacAddr
 *
 * Description:
 * Установите MAC-адрес на заданное значение.  Это делается после ifup
 * или после тайм-аута передачи данных.  Обратите внимание, что это означает, что интерфейс должен
 * отключитесь перед настройкой MAC-адреса.
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

void ENC_SetMacAddr(ENC_HandleTypeDef *handle)
{
  /* Program the hardware with it's MAC address (for filtering).
   *   MAADR1  MAC Address Byte 1 (MAADR<47:40>), OUI Byte 1
   *   MAADR2  MAC Address Byte 2 (MAADR<39:32>), OUI Byte 2
   *   MAADR3  MAC Address Byte 3 (MAADR<31:24>), OUI Byte 3
   *   MAADR4  MAC Address Byte 4 (MAADR<23:16>)
   *   MAADR5  MAC Address Byte 5 (MAADR<15:8>)
   *   MAADR6  MAC Address Byte 6 (MAADR<7:0>)
   */

  enc_wrbreg(handle, ENC_MAADR1, handle->Init.MACAddr[0]);
  enc_wrbreg(handle, ENC_MAADR2, handle->Init.MACAddr[1]);
  enc_wrbreg(handle, ENC_MAADR3, handle->Init.MACAddr[2]);
  enc_wrbreg(handle, ENC_MAADR4, handle->Init.MACAddr[3]);
  enc_wrbreg(handle, ENC_MAADR5, handle->Init.MACAddr[4]);
  enc_wrbreg(handle, ENC_MAADR6, handle->Init.MACAddr[5]);
}


/****************************************************************************
 * Function: ENC_WriteBuffer
 *
 * Description:
 * Запись данных в буфер.
 
 * Parameters:
 *   buffer  - Указатель на буфер для записи из
 *   buflen  - Количество байтов для записи
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *   Указатель чтения установлен на правильный адрес
 *
 ****************************************************************************/

void ENC_WriteBuffer(void *buffer, uint16_t buflen)
{
  /* Отправьте команду WBM и скопируйте сам пакет в передающий
   * буфер в позиции регистра ЗАПИСИ.
   */

  /* Выберите микросхему ENC28J60
   *
   * "Команда WBM запускается путем опускания PIN-кода CS. ..."
   * Мы явно выбираем микросхему ENC28J60, потому что нам нужно передать несколько фрагментов
   * информация при сохранении низкого уровня CS
   *
   */
   
  ENC_SPI_Select(true);

  /* Отправьте команду записи в буферную память (игнорируя ответ)
   *
   * "...Затем [3-битный] код операции WBM должен быть отправлен в ENC28J60,
   * за которым следует 5-битная константа, 1Ah."
   */

  ENC_SPI_SendWithoutSelection(ENC_WBM);

/* Отправить буфер
   *
   * "... После отправки команды WBM и константы данные в
   * должны храниться в памяти, на которую указывает EWRPT, должны быть сдвинуты
   * сначала выведите MSb на ENC28J60. После получения 8 битов данных,
   * * Указатель записи будет автоматически увеличиваться, если AUTO INC установлен
   * набор. Главный контроллер может продолжать предоставлять часы на
   * PIN-код SCK и отправьте данные на pin-код SI, не поднимая CS, чтобы
   * продолжить запись в память. Таким образом, с помощью автоматического
   * включена возможность непрерывной записи последовательных байтов
   * в буферную память без какой-либо дополнительной команды SPI
   * накладные расходы.
   */
   
  ENC_SPI_SendBuf(buffer, NULL, buflen);

  /* Отменить выбор микросхемы ENC28J60
   *
   * "Команда WBM завершается выводом PIN-кода CS. ..."
   * выполнено в обратном вызове ENC_SPI_SendBuf
   */

}

/****************************************************************************
 * Function: enc_rdbuffer
 *
 * Description:
 *   Считывание буфера данных.
 *
 * Parameters:
 *   buffer  - Указатель на буфер для чтения
 *   buflen  - Количество байтов для чтения
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *   Указатель чтения установлен на правильный адрес
 *
 ****************************************************************************/

static void enc_rdbuffer(void *buffer, int16_t buflen)
{

  ENC_SPI_Select(true);

  /* Отправьте команду чтения буферной памяти (игнорируя ответ) */

  ENC_SPI_SendWithoutSelection(ENC_RBM);

  /* Затем считайте данные буфера */

  ENC_SPI_SendBuf(NULL, buffer, buflen);

  /* Отменить выбор микросхемы ENC28J60: сделано в обратном вызове ENC_SPI_SendBuf */
}

/****************************************************************************
 * Function: ENC_RestoreTXBuffer
 *
 * Description:
 *   Prepare TX buffer
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *   len     - размер буфера
 *
 * Returned Value:
 *    ERR_OK          0    No error, everything OK.
 *    ERR_MEM        -1    Out of memory error.
 *    ERR_TIMEOUT    -3    Timeout.
 *
 * Assumptions:
 *
 ****************************************************************************/

int8_t ENC_RestoreTXBuffer(ENC_HandleTypeDef *handle, uint16_t len)
{
  uint16_t txend;
  uint8_t control_write[2];

  if (!enc_waitgreg(ENC_ECON1, ECON1_TXRTS, 0)) {
    return ERR_TIMEOUT;
  }
  
  /* Убедитесь, что оборудование готово к отправке другого пакета.  Водитель
   * запускает процесс передачи, установив ECON1.TXRTS. Когда пакет будет
   * передача завершена или прервана из-за ошибки/отмены,
   * Бит ECON1.TXRTS будет очищен.
   *
   * ПРИМЕЧАНИЕ: Если мы попали сюда, значит, мы взяли на себя обязательство отправить пакет.
   * логика более высокого уровня должна быть уверена в том, что прерывания, связанные с TX, отключены.
   */

  /* Отправить пакет: адрес=priv->dev.d_buf, длина=priv->dev.d_len */

  /* Установите начало буфера передачи (необходимо ли это?). */

  enc_wrbreg(handle, ENC_ETXSTL, PKTMEM_TX_START & 0xff);
  enc_wrbreg(handle, ENC_ETXSTH, PKTMEM_TX_START >> 8);

/* Сбросьте указатель записи в начало буфера передачи */

  enc_wrbreg(handle, ENC_EWRPTL, PKTMEM_TX_START & 0xff);
  enc_wrbreg(handle, ENC_EWRPTH, PKTMEM_TX_START >> 8);

  /* Установите указатель конца передачи на основе размера отправляемого пакета. Примечание
   * что смещение учитывает управляющий байт в начале
   * буфер плюс размер пакетных данных.
   */

  txend = PKTMEM_TX_START + len;

  if (txend+8>PKTMEM_TX_ENDP1) {
    return ERR_MEM;
  }

  enc_wrbreg(handle, ENC_ETXNDL, txend & 0xff);
  enc_wrbreg(handle, ENC_ETXNDH, txend >> 8);

   /* Отправьте команду записи в буферную память (игнорируя ответ)
	*
	* "...Затем [3-битный] код операции WBM должен быть отправлен в ENC28J60,
	* за которым следует 5-битная константа, 1Ah."
	*
	* "...ENC28J60 требует одного управляющего байта на пакет для
	* предшествует пакету для передачи."
	*
	* * ПЕРЕОПРЕДЕЛЕНИЕ: Бит переопределения для каждого пакета (не установлен):
	* 1 = Значения PCRCEN, PPADEN и PHUGEEN переопределят
	* * конфигурация, определенная MACON 3.
	* * 0 = Значения в MACON 3 будут использоваться для определения того, как пакет
	* будет передано
	* * ПРОЦЕНТ: Бит включения CRC для каждого пакета (установлен, но не будет использоваться, поскольку
	* * ПЕРЕОПРЕДЕЛЕНИЕ равно нулю).
	* PPADEN: Бит включения заполнения для каждого пакета (установлен, но не будет использоваться, поскольку
	* * ПЕРЕОПРЕДЕЛЕНИЕ равно нулю).
	* PHUGEEN: Бит включения огромного фрейма для каждого пакета (установлен, но использоваться не будет
	* * потому что ПЕРЕОПРЕДЕЛЕНИЕ равно нулю).
	*/

  control_write[0] = ENC_WBM;
  control_write[1] = PKTCTRL_PCRCEN | PKTCTRL_PPADEN | PKTCTRL_PHUGEEN;
  ENC_SPI_SendBuf(control_write, control_write, 2);

  return ERR_OK;
}

/****************************************************************************
 * Function: ENC_Transmit
 *
 * Description:
 * Запустите аппаратную передачу.
 *
 *   -  pkif interrupt when an application responds to the receipt of data
 *      by trying to send something, or
 *   -  From watchdog based polling.
 *
 * Parameters:
 *   handle  - Reference to the driver state structure
 *   len     - length of buffer
 *
 * Returned Value:
 *   none
 *
 * Assumptions:
 *
 ****************************************************************************/

#ifdef USE_PROTOTHREADS
PT_THREAD(ENC_Transmit(struct pt *pt, ENC_HandleTypeDef *handle))
#else
void ENC_Transmit(ENC_HandleTypeDef *handle)
#endif
{
    PT_BEGIN(pt);

    if (handle->transmitLength != 0) {
		/* Кадр готов к передаче */
        /* Задайте ТЕКСТ для отправки пакета в буфере передачи */

        //enc_bfsgreg(ENC_ECON1, ECON1_TXRTS);
        /* Реализовать ошибки 12, 13 и 15 */
        /* Сброс логики передачи */
        handle->retries = 16;
        do {
            enc_bfsgreg(ENC_ECON1, ECON1_TXRST);
            enc_bfcgreg(ENC_ECON1, ECON1_TXRST);
            enc_bfcgreg(ENC_EIR, EIR_TXERIF | EIR_TXIF);

			/* Начать передачу */
            enc_bfsgreg(ENC_ECON1, ECON1_TXRTS);

#ifdef USE_PROTOTHREADS
            handle->startTime = HAL_GetTick();
            handle->duration = 20; /* Timeout after 20 ms */
            PT_WAIT_UNTIL(pt, (((enc_rdgreg(ENC_EIR) & (EIR_TXIF | EIR_TXERIF)) != 0) ||
                          (HAL_GetTick() - handle->startTime > handle->duration)));
#else
			/* Дождитесь окончания передачи */
            enc_waitwhilegreg(ENC_EIR, EIR_TXIF | EIR_TXERIF, 0);
#endif

            /* Остановить передачу*/
            enc_bfcgreg(ENC_ECON1, ECON1_TXRTS);

            {
                uint16_t addtTsv4;
                uint8_t tsv4, regval;

                /* чтение tsv */
                addtTsv4 = PKTMEM_TX_START + handle->transmitLength + 4;

                enc_wrbreg(handle, ENC_ERDPTL, addtTsv4 & 0xff);
                enc_wrbreg(handle, ENC_ERDPTH, addtTsv4 >> 8);

                enc_rdbuffer(&tsv4, 1);
                regval = enc_rdgreg(ENC_EIR);
                if (!(regval & EIR_TXERIF) || !(tsv4 & TSV_LATECOL)) {
                    break;
                }
            }
            handle->retries--;
        } while (handle->retries > 0);
		/* Передача завершена (но может быть неудачной) */
        handle->transmitLength = 0;
    }
    PT_END(pt);
}

/****************************************************************************
 * Function: ENC_GetReceivedFrame
 *
 * Description:
 *   Проверьте, получили ли мы пакет, и если да, извлеките их.
 *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   true, если доступен новый пакет; false в противном случае
 *
 * Assumptions:
 *
 ****************************************************************************/

bool ENC_GetReceivedFrame(ENC_HandleTypeDef *handle)
{
    uint8_t  rsv[6];
    uint16_t pktlen;
    uint16_t rxstat;

    uint8_t pktcnt;

    bool result = true;

    pktcnt = enc_rdbreg(handle, ENC_EPKTCNT);
    if (pktcnt == 0) {
        return false;
    };

	/* Установите указатель чтения на начало принятого пакета (ERDPT) */

    enc_wrbreg(handle, ENC_ERDPTL, (handle->nextpkt) & 0xff);
    enc_wrbreg(handle, ENC_ERDPTH, (handle->nextpkt) >> 8);

	/* Считывает указатель следующего пакета и 4-байтовый вектор состояния считывания (RSV)
    * в начале полученного пакета. (ERDPT должен автоматически увеличиваться
    * и перенос в начало буфера чтения по мере необходимости)
    */

    enc_rdbuffer(rsv, 6);

	/* Декодируйте новый указатель следующего пакета и RSV.  То
    * RSV кодируется как:
    *
    * Биты 0-15: указывает длину принятого кадра. Это включает в себя
    * адрес назначения, адрес источника, тип/длина, данные,
    * поля для заполнения и CRC. Это поле хранится в небольшом-
    * конечный формат.
    * Биты 16-31: Состояние RX в битовом кодировании.
    */

    handle->nextpkt = (uint16_t)rsv[1] << 8 | (uint16_t)rsv[0];
    pktlen        = (uint16_t)rsv[3] << 8 | (uint16_t)rsv[2];
    rxstat        = (uint16_t)rsv[5] << 8 | (uint16_t)rsv[4];

	/* Проверьте, был ли пакет получен нормально */

    if ((rxstat & RXSTAT_OK) == 0) {
#ifdef CONFIG_ENC28J60_STATS
        priv->stats.rxnotok++;
#endif
        result = false;
    } else { /* Проверьте допустимую длину пакета (добавлено 4 для CRC) */
        if (pktlen > (CONFIG_NET_ETH_MTU + 4) || pktlen <= (ETH_HDRLEN + 4)) {
    #ifdef CONFIG_ENC28J60_STATS
            priv->stats.rxpktlen++;
    #endif
            result = false;
        } else { /* В противном случае считайте и обработайте пакет */
            /* Сохраните длину пакета (без 4-байтового CRC) в handle->RxFrameInfos.длина*/

            handle->RxFrameInfos.length = pktlen - 4;
			
			/* Скопируйте данные из буфера приема в priv->dev.d_buf.
            * ERDPT должен быть правильно расположен с момента последнего вызова в
			буфер * * end_rd (выше).
            */

            enc_rdbuffer(handle->RxFrameInfos.buffer, handle->RxFrameInfos.length);

        }
    }

	/* Переместите указатель чтения RX в начало следующего принятого пакета.
    * Это освобождает память, которую мы только что прочитали.
    */

    /* Опечатка 14 (в se sert de rxstat comme variable temporaire */
    rxstat = handle->nextpkt;
    if (rxstat == PKTMEM_RX_START) {
        rxstat = PKTMEM_RX_END;
    } else {
        rxstat--;
    }
    enc_wrbreg(handle, ENC_ERXRDPTL, rxstat && 0xff);
    enc_wrbreg(handle, ENC_ERXRDPTH, rxstat >> 8);
/*
    enc_wrbreg(handle, ENC_ERXRDPTL, (handle->nextpkt));
    enc_wrbreg(handle, ENC_ERXRDPTH, (handle->nextpkt) >> 8);
*/

	/* Уменьшите счетчик пакетов, чтобы указать, что мы закончили с этим пакетом */
    enc_bfsgreg(ENC_ECON2, ECON2_PKTDEC);

    return result;
}

/****************************************************************************
 * Function: enc_linkstatus
 *
 * Description:
 * Текущее состояние связи можно получить из PHSTAT1.LLSTAT или
 * PHSTAT2.LSTAT.
 *
 * Parameters:
 *   priv    - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

static void enc_linkstatus(ENC_HandleTypeDef *handle)
{
  handle->LinkStatus = enc_rdphy(handle, ENC_PHSTAT2);
}

/****************************************************************************
 * Function: ENC_EnableInterrupts
 *
 * Description:
 *   Включение отдельных прерываний ENC28J60
 *
 * Parameters:
 *   bits - Отдельные биты, позволяющие
 *
 * Returned Value:
 *   None
 *
 * Assumptions:
 *
 ****************************************************************************/

void ENC_EnableInterrupts(uint8_t bits)
{
    enc_bfsgreg(ENC_EIE, bits);
}


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

void ENC_IRQHandler(ENC_HandleTypeDef *handle)
{
    uint8_t eir;

	/* Отключите дальнейшие прерывания, очистив бит разрешения глобального прерывания.
     * "После возникновения прерывания хост-контроллер должен очистить глобальную
     * включите бит для вывода прерывания перед обслуживанием прерывания. Очистка
     * бит разрешения приведет к возврату вывода прерывания к неустановленному
     * состояние (высокое). Это предотвратит пропуск хост-контроллером
     * падающий край, если произойдет другое прерывание, в то время как немедленное прерывание
     * находится на обслуживании."
     */

    enc_bfcgreg(ENC_EIE, EIE_INTIE);

    /* чтение EIR фрага
     */

    eir = enc_rdgreg(ENC_EIR) & EIR_ALLINTS;

	/* PKTIF ненадежен, вместо этого проверьте PKCNT */
    if (enc_rdbreg(handle, ENC_EPKTCNT) != 0) {
        /* Управление EIR_PKTIF с помощью программного обеспечения */
        eir |= EIR_PKTIF;
    }

    /* Хранить флаги прерывания в дескрипторе */
    handle->interruptFlags = eir;

    /* Если статус ссылки изменился, прочитайте его */
    if ((eir & EIR_LINKIF) != 0) /* Прерывание смены канала связи */
    {
        enc_linkstatus(handle);       /* Получить текущий статус ссылки */
        enc_rdphy(handle, ENC_PHIR);  /* Очистите ССЫЛКУ, ЕСЛИ прерывание */
    }

	/* Сбросить флаги прерывания ENC28J60, за исключением формы PKTIF, прерывание которой отменяется, когда PKTCNT достигает 0 */    enc_bfcgreg(ENC_EIR, EIR_ALLINTS);

	/* Включить прерывания Ethernet */
    /* done after effective process on interrupts enc_bfsgreg(ENC_EIE, EIE_INTIE); */
}

/****************************************************************************
 * Function: ENC_GetPkcnt
 *
 * Description:
 * Получить количество ожидающих приема пакетов *
 * Parameters:
 *   handle  - Ссылка на структуру состояния драйвера
 *
 * Returned Value:
 *   номер полученного пакета, который еще не обработан
 *
 * Assumptions:
 *
 ****************************************************************************/

void ENC_GetPkcnt(ENC_HandleTypeDef *handle)
{
    handle->pktCnt = enc_rdbreg(handle, ENC_EPKTCNT);
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

