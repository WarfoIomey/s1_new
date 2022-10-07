#include "esp8266.h"

static WIFI_StateTypeDef wifi_state = _OFFLINE;
static WIFI_StateTypeDef trans_state = _UNKNOWN_STATE;
static uint8_t wifi_config_step = 0;
/*********************ПРОВЕРКА ОТВЕТА ESP***********************************/
WIFI_StateTypeDef esp8266_CheckRespond(uint8_t *str) {
	u2_printf("(DBG:) USART2 CHECKING THE RESPONSE :\r\n%s\r\n", USART3_RxBUF); /*ПРОВЕРКА ОТВЕТА*/
	printf("(DBG:) USART2 CHECKING THE RESPONSE :\r\n%s\r\n", USART3_RxBUF);
	if (strstr((const char*) USART3_RxBUF, (const char*) str) != NULL) {
		u2_printf("(DBG:) CONNECTION SUCCESSFUL\r\n");
		printf("(DBG:) CONNECTION SUCCESSFUL\r\n");
		memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
		return _MATCHOK;
	}

	memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
	return _MATCHERROR;
}
/*********************ПРОВЕРКА ОТВЕТА ESP***********************************/
/*********************ПРОВЕРКА  ОТПРАВКИ ESP***********************************/
WIFI_StateTypeDef esp8266_TransmitCmd(uint8_t *cmd, uint8_t *ack,
		uint32_t waitms, uint8_t newline) {
	int timeout = waitms;
	uint8_t check = 0;
	memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
	u2_printf("\r\n(DBG:) TRY SENDING cmd: %s\r\n", cmd);
	printf("\r\n(DBG:) TRY SENDING cmd: %s\r\n", cmd);
	if (newline == 0)
		u3_transmit("%s", cmd);      // ОТПРАВКА ПО КОНСОЛИ ЧЕРЕЗ USART1
	else
		u3_transmit("%s\r\n", cmd); // ОТПРАВКА ПО КОНСОЛИ ЧЕРЕЗ USART1

	u2_printf("(DBG:) WAITING FOR A RESPONSE\r\n"); /*ожидание ответа*/
	printf("(DBG:) WAITING FOR A RESPONSE \r\n");
	while (timeout--) {
		// u2_printf("%d ", timeout);
		// finish dma receive
		if (USART3_RecvEndFlag == 1) {
			check = esp8266_CheckRespond(ack);
			if (check == _MATCHOK) {
				u2_printf("(DBG:) The CYCLE is COMPLETED\r\n"); /*цИКЛ ЗАВЕРШЕН*/
				printf("(DBG:)The CYCLE is COMPLETED\r\n");
			}

			USART3_RxLen = 0;
			USART3_RecvEndFlag = 0;
			HAL_UART_Receive_DMA(&huart3, USART3_RxBUF, USART3_MAX_RECVLEN);

			break;
		}
		HAL_Delay(1);
	}

	if (check == _MATCHERROR) {
		u2_printf("\r\n(DBG:) Cmd error\r\n");
		printf("\r\n(DBG:) Cmd error\r\n");
		return check;
	}
	if (timeout <= 0) {
		u2_printf("(DBG:) DOWNLOAD COMPLETED\r\n");
		u2_printf("\r\n(DBG:) break\r\n");
		printf("(DBG:) DOWNLOAD COMPLETED\r\n");
		printf("\r\n(DBG:) break\r\n");
		return _TIMEOUT;
	}
	u2_printf("(DBG:) ok\r\n");
	printf("(DBG:) ok\r\n");
	return _SUCCEED;
}
/*********************ПРОВЕРКА ОТПРАВКИ ESP***********************************/
/******************************АППАРАТНЫЙ СБРОС****************************************/

WIFI_StateTypeDef esp8266_initHard()
{
	WIFI_CH_PD_Enable();
}

WIFI_StateTypeDef esp8266_offHard()
{
	WIFI_CH_PD_Disable();
}

WIFI_StateTypeDef esp8266_HardwareReset(uint32_t waitms)
{
	int timeout = waitms;
	WIFI_RST_Enable();
	HAL_Delay(500);
	WIFI_RST_Disable();
	printf("попытка hardware reset\r\n");
	while (timeout--)
	{
		if (USART3_RecvEndFlag == 1)
		{

			u2_printf("(DBG:) HARDWARE RESET OK!\r\n");
			printf("(DBG:) HARDWARE RESET OK!\r\n");
			HAL_Delay(100);

			USART3_RxLen = 0;
			USART3_RecvEndFlag = 0;
			HAL_UART_Receive_DMA(&huart3, USART3_RxBUF, USART3_MAX_RECVLEN);

			return _SUCCEED;
		}
		HAL_Delay(1);
	}
	if (timeout <= 0)
	{
		u2_printf("(DBG:) DOWNLOAD COMPLETED\r\n");
		u2_printf("\r\n(DBG:) break\r\n");
		printf("(DBG:) DOWNLOAD COMPLETED \r\n");
		printf("\r\n(DBG:) break\r\n");
		return _TIMEOUT;
	}
	printf("Сброс не удался\r\n");
	return _UNKNOWN_ERROR;
}
/*************************АППАРАТНЫЙ СБРОС************************************/
/*************************ПОДКЛЮЧЕНИЕ К ТОЧКЕ ДОСТУПА****************************************************/
WIFI_StateTypeDef esp8266_ConnectAP() {
	uint16_t cmd_len = strlen(AP_SSID) + strlen(AP_PSWD) + 30;
	uint8_t *cmd = (uint8_t*) malloc(cmd_len * sizeof(uint8_t));
	memset(cmd, 0, cmd_len);
	sprintf((char*) cmd, "AT+CWJAP_CUR=\"%s\",\"%s\"", AP_SSID, AP_PSWD); // ПОДКЛЮЧЕНИЕ К ТОЧКЕ ДОСТУПА

	if (esp8266_TransmitCmd(cmd, (uint8_t*) "WIFI CONNECT",
			3 * ESP8266_MAX_TIMEOUT, WITH_NEWLINE) == _SUCCEED)
		wifi_state = _ONLINE;
	else
		wifi_state = _OFFLINE;

	return wifi_state;
}
/*************************ПОДКЛЮЧЕНИЕ К ТОЧКЕ ДОСТУПА****************************************************/
/*********************************УСТАНОВКА TCP СОЕДИНЕНИЯ**************************************************/
WIFI_StateTypeDef esp8266_ConnectServer() {
	uint16_t cmd_len = strlen(IpServer) + strlen(ServerPort) + 30;
	uint8_t *cmd = (uint8_t*) malloc(cmd_len * sizeof(uint8_t));
	memset(cmd, 0, cmd_len);
	u2_printf((char*) cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", IpServer, ServerPort);
	printf((char*) cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s", IpServer, ServerPort);
	if (esp8266_TransmitCmd(cmd, (uint8_t*) "connect", 3 * ESP8266_MAX_TIMEOUT,
	WITH_NEWLINE) == _SUCCEED)
		wifi_state = _CONNECTED;
	else
		wifi_state = _DISCONNECTED;

	return wifi_state;
}

WIFI_StateTypeDef esp8266_SetUpTCPConnection() {
	uint8_t retry_count = 0;

	/* СБРОС esp8266 */
	u2_printf("(DBG:) ATTEMPT TO RESET esp8266\r\n");
	printf("(DBG:) ATTEMPT TO RESET esp8266\r\n");
	wifi_config_step++;
	while (esp8266_HardwareReset(10) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) RESET ATTEMPT FAILED\r\n");
			printf("(DBG:) RESET ATTEMPT FAILED\r\n");
			retry_count = 0;
			trans_state = _UNKNOWN_STATE;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(2000);		// ЖДЕМ 2 СЕКУНДУ
	retry_count = 0;		//СБРОС СЧЁТЧИК

	/* ОТКЛЮЧЕНИЕ ПРОЗРАЧНОЙ ПЕРЕДАЧИ */
	u2_printf("(DBG:) ATTEMPT TO DISABLE TRANSPARENT TRANSMISSION\r\n");
	printf("(DBG:) ATTEMPT TO DISABLE TRANSPARENT TRANSMISSION\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd(TRANS_QUIT_CMD, TRANS_QUIT_CMD,
	ESP8266_MAX_TIMEOUT,
	WITHOUT_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1500);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) DISABLING TRANSPARENT TRANSMISSION REJECTED\r\n");
			printf("(DBG:) DISABLING TRANSPARENT TRANSMISSION REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	trans_state = _TRANS_DISABLE;
	HAL_Delay(1500);
	retry_count = 0;		// СБРОС СЧЁТЧИК

	/* ЗАКРЫТИЕ echo */
	u2_printf("(DBG:) ATTEMPT TO CLOSE ECHO\r\n");
	printf("(DBG:) ATTEMPT TO CLOSE ECHO\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "ATE0", OK_ACK, 500, WITH_NEWLINE)
			!= _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) CLOSE echo Rejected\r\n");
			printf("(DBG:) CLOSE echo Rejected\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(100);
	retry_count = 0;		//СБРОС СЧЁТЧИК

	/* УСТАНОВКА WIFI РЕЖИМА 0:AP 1:STA 2:AP+STA */
	u2_printf("(DBG:) ATTEMPT TO SET WI FI MODE\r\n");
	printf("(DBG:) ATTEMPT TO SET WI FI MODE\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CWMODE_CUR=1", OK_ACK, 500,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) WI FI MODE SETTING IS REJECTED\r\n");
			printf("(DBG:) WI FI MODE SETTING IS REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(100);
	retry_count = 0;		//СБРОС СЧЁТЧИК

	/* ОТКЛЮЧЕНО АВТОПОДКЛЮЧЕНИЕ */
	u2_printf("(DBG:) ATTEMPT TO DISABLE AUTO-CONNECTION\r\n");
	printf("(DBG:) ATTEMPT TO DISABLE AUTO-CONNECTION\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CWAUTOCONN=0", OK_ACK, 500,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) DISABLING AUTO-CONNECTION REJECTED\r\n");
			printf("(DBG:) DISABLING AUTO-CONNECTION REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(100);
	retry_count = 0;		// СБРОС СЧЁТЧИК

	/* Connect to AP(Wifi) */
	u2_printf("(DBG:) Trying to connect to AP\r\n");
	printf("(DBG:) Trying to connect to AP\r\n");
	wifi_config_step++;
	while (esp8266_ConnectAP() != _ONLINE) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) Connect to AP failed\r\n");
			printf("(DBG:) Connect to AP failed\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(1000);
	retry_count = 0;		// СБРОС СЧЁТЧИК

	/* ПОПЫТКА ПОЛУЧЕНИЯ AP ИНФОРМАЦИИ */
	if (wifi_state == _ONLINE) {
		while (esp8266_TransmitCmd((uint8_t*) "AT+CWJAP_CUR?", OK_ACK,
		ESP8266_MAX_TIMEOUT, WITH_NEWLINE) != _SUCCEED) {
			retry_count++;
			HAL_Delay(1000);
			if (retry_count > ESP8266_MAX_RETRY_TIME / 2) {
				u2_printf("(DBG:) RECEIVING AP MESSAGE REJECTED\r\n");
				u2_printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED");
				printf("(DBG:) ПОЛУЧЕНИЕ AP СООБЩЕНИЕ ОТКЛОНЕНО\r\n");
				printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED");
				retry_count = 0;
				wifi_config_step--;
				break;
			}
		}
	}

	HAL_Delay(1000);
	retry_count = 0;		// СБРОС СЧЁТЧИКА

	/* ПОПЫТКА УСТАНОВИТЬ IP ИНФОРМАЦИИ */
	if (wifi_state == _ONLINE) {
		while (esp8266_TransmitCmd((uint8_t*) "	AT+CIPSTA_CUR?", OK_ACK,
		ESP8266_MAX_TIMEOUT, WITH_NEWLINE) != _SUCCEED) {
			retry_count++;
			HAL_Delay(1000);
			if (retry_count > ESP8266_MAX_RETRY_TIME / 2) {
				u2_printf("(DBG:) INSTALLATION OF IP INFORMATION IS REJECTED\r\n");	//УСТАНОВКА IP ИНФОРМАЦИИ ОТКЛОНЕНА
				u2_printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED");	//ПРОЦЕСС ПОДКЛЮЧЕНИЯ СЕРВЕРА НЕ БУДЕТ ЗАВЕРШЕН
				printf("(DBG:) INSTALLATION OF IP INFORMATION IS REJECTED\r\n");
				printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED");
				retry_count = 0;
				wifi_config_step--;
				break;
			}
		}
	}

	HAL_Delay(1000);
	retry_count = 0;		// СБРОС СЧЁТЧИКА

	/* УСТАНОВКА  DHCP */
	u2_printf("(DBG:) Trying to set DHCP mode\r\n");
	printf("(DBG:) Trying to set DHCP mode\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CWDHCP_CUR=1,1", OK_ACK, 1000,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) THE INSTALLATION OF THE DHCP MODEL IS REJECTED\r\n");	// УСТАНОВКА DHCP МОДЕЛИ ОТКЛОНЕНА
			printf("(DBG:) THE INSTALLATION OF THE DHCP MODEL IS REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(1000);
	retry_count = 0;

	/* УСТАНОВКА ОДИНОЧНОГО СОЕДИНЕНИЕ */
	u2_printf("(DBG:) ATTEMPT TO ESTABLISH A SINGLE CONNECTION\r\n");	//ПОПЫТКА УСТАНОВКИ ОДИНОЧНОГО СОЕДИНЕНИЯ
	printf("(DBG:) ATTEMPT TO ESTABLISH A SINGLE CONNECTION\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CIPMUX=0", OK_ACK, 1000,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) SINGLE CONNECTION SETUP REJECTED\r\n");	//УСТАНОВКА ОДИНОЧНОГО СОЕДИНЕНИЯ ОТКЛОНЕНО
			printf("(DBG:) SINGLE CONNECTION SETUP REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(1000);
	retry_count = 0;

	/* УСТАНОВКА РЕЖИМА ПРОЗРАЧНОЙ ПЕРЕДАЧИ */
	u2_printf("(DBG:) ATTEMPT TO SET TRANSPARENT TRANSMISSION MODE\r\n"); // ПОПЫТКА УСТАНОВИТЬ РЕЖИМ ПРОЗРАЧНОЙ ПЕРЕДАЧИ
	printf("(DBG:) ATTEMPT TO SET TRANSPARENT TRANSMISSION MODE\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CIPMODE=1", OK_ACK, 1000,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED\r\n");	//ПРОЦЕСС ПОДКЛЮЧЕНИЯ СЕРВЕРА НЕ БУДЕТ ЗАВЕРШЕН
			printf("(DBG:) THE SERVER CONNECTION PROCESS WILL NOT BE COMPLETED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(1000);
	retry_count = 0;

	/* ПОДКЛЮЧЕНИЕ К TCP СЕРВЕРУ */
	u2_printf("(DBG:) ATTEMPT TO CONNECT TO A TCP SERVER\r\n"); //ПОПЫТКА ПОДКЛЮЧИТЬСЯ К TCP СЕРВЕРУ
	printf("(DBG:) ATTEMPT TO CONNECT TO A TCP SERVER\r\n");
	wifi_config_step++;
	while (esp8266_ConnectServer() != _CONNECTED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) CONNECTION TO TCP SERVER REJECTED\r\n");	//ПОДКЛЮЧЕНИЕ К TCP СЕРВЕРУ ОТКЛОНЕНО
			printf("(DBG:) CONNECTION TO TCP SERVER REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	HAL_Delay(1000);
	retry_count = 0;

	/* ВКЛЮЧЕНИЕ ПЕРЕДАЧИ ДАННЫХ (ПРОЗРАЧНЫЙ РЕЖИМ ПЕРЕДАЧИ) */
	u2_printf("(DBG:) ATTEMPT TO ENABLE DATA TRANSMISSION\r\n");	//ПОПЫТКА ВКЛЮЧЕНИЕЯ ПЕРЕДАЧИ ДАННЫХ
	printf("(DBG:) ATTEMPT TO ENABLE DATA TRANSMISSION\r\n");
	wifi_config_step++;
	while (esp8266_TransmitCmd((uint8_t*) "AT+CIPSEND", OK_ACK, 1000,
	WITH_NEWLINE) != _SUCCEED) {
		retry_count++;
		HAL_Delay(1000);
		if (retry_count > ESP8266_MAX_RETRY_TIME) {
			u2_printf("(DBG:) SETTING TRANSPARENT TRANSMISSION MODE REJECTED\r\n");	//УСТАНОВКА ПРОЗРАЧНОГО РЕЖИМА ПЕРЕДАЧИ ОТКЛОНЕНО
			printf("(DBG:) SETTING TRANSPARENT TRANSMISSION MODE REJECTED\r\n");
			retry_count = 0;
			wifi_config_step--;
			return _FAILED;
		}
	}

	trans_state = _TRANS_ENBALE;
	HAL_Delay(1000);
	retry_count = 0;

	/* send test msg */
//	u2_printf("Test msg is sending to TCP Server\r\n");
//	u2_transmit("This msg means TCP connection has been set up\r\n");
//	u2_printf("Test msg has been send to TCP Server\r\n");
	return _SUCCEED;
}
/*********************************УСТАНОВКА TCP СОЕДИНЕНИЯ**************************************************/
