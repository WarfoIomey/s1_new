#ifndef __ESP8266_H
#define __ESP8266_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "net_conf.h"
#include <string.h>
#include <stdlib.h>
#include "usart.h"

#define OK_ACK            (uint8_t*)"OK"
#define TRANS_QUIT_CMD    (uint8_t*)"+++"

#define WITH_NEWLINE     1
#define WITHOUT_NEWLINE  0
/**************СОСТОЯНИЯ WIFI*********************/
typedef enum __WIFI_StateTypDef {
	_FAILED = 0,        // failed
	_SUCCEED = 1,       // succeed
	_TIMEOUT = 2,       // ВРЕМЯ ОЖИДАНИЯ
	_MATCHERROR = 3,    // cmd match error
	_MATCHOK = 4,       // cmd match ok
	_ONLINE = 5,        // ПОДКЛЮЧЕН К WIFI
	_OFFLINE = 6,       // ОТКЛЮЧЁН ОТ WIFI
	_CONNECTED = 7,     // ПОДЛКЛЮЧЕН К СЕРВЕРУ
	_DISCONNECTED = 8,  // ОТКЛЮЧЕН ОТ СЕРВЕРА
	_TRANS_ENBALE = 9,  // ВКЛЮЧЕНА ПЕРЕДАЧА
	_TRANS_DISABLE = 10,// ОТКЛЮЧЕНА ПЕРЕДАЧА
	_UNKNOWN_STATE = 0xee,
	_UNKNOWN_ERROR = 0xff
} WIFI_StateTypeDef;
/**************СОСТОЯНИЯ WIFI*********************/
#define ESP8266_MAX_TIMEOUT     (uint16_t)0xffff
#define ESP8266_MAX_RETRY_TIME  1
/**************МАКРОС НА WIFI******************************/
#define RST_WF_Pin	           GPIO_PIN_7
#define RST_WF_GPIO_Port  	   GPIOB
#define WIFI_RST_Enable()      HAL_GPIO_WritePin(RST_WF_GPIO_Port, RST_WF_Pin, RESET);
#define WIFI_RST_Disable() 	   HAL_GPIO_WritePin(RST_WF_GPIO_Port, RST_WF_Pin, SET);

/**/
#define CH_PD_WF_Pin 		   GPIO_PIN_6
#define CH_PD_WF_GPIO_Port 	   GPIOB
#define WIFI_CH_PD_Enable()    HAL_GPIO_WritePin(CH_PD_WF_GPIO_Port, CH_PD_WF_Pin, RESET)
#define WIFI_CH_PD_Disable()   HAL_GPIO_WritePin(CH_PD_WF_GPIO_Port, CH_PD_WF_Pin, SET);


/**************МАКРОС НА WIFI******************************/
extern uint8_t *esp8266_ack;

WIFI_StateTypeDef esp8266_initHard(void);
WIFI_StateTypeDef esp8266_offHard(void);

WIFI_StateTypeDef esp8266_CheckRespond(uint8_t *str);
WIFI_StateTypeDef esp8266_TransmitCmd(uint8_t *cmd, uint8_t *ack,
		uint32_t waittime, uint8_t newline);
WIFI_StateTypeDef esp8266_HardwareReset(uint32_t timeout);
WIFI_StateTypeDef esp8266_initHard(void);
WIFI_StateTypeDef esp8266_offHard(void);
WIFI_StateTypeDef esp8266_QuitTrans(void);
WIFI_StateTypeDef esp8266_ConnectAP(void);
WIFI_StateTypeDef esp8266_ConnectServer(void);
WIFI_StateTypeDef esp8266_SetUpTCPConnection(void);

#ifdef __cplusplus
}
#endif

#endif
