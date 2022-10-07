/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "tim.h"
/* USER CODE END Includes */

extern UART_HandleTypeDef huart2;

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */
#define esp8266_uart     huart3
#define rs232_monitor	 huart2

#define USART3_MAX_SENDLEN  1024
#define USART3_MAX_RECVLEN  1024

#define USART2_MAX_SENDLEN  1024
#define USART2_MAX_RECVLEN  1024


/* USER CODE BEGIN Private defines */
/* 1 (2) - WF
 * 3 - RS232*/
/* USER CODE END Private defines */

void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
extern uint8_t USART2_TxBUF[USART2_MAX_SENDLEN];
extern uint8_t USART2_RxBUF[USART2_MAX_RECVLEN];

extern volatile uint8_t USART3_RxLen;
extern volatile uint8_t USART3_RecvEndFlag;

extern uint8_t USART3_TxBUF[USART3_MAX_SENDLEN];
extern uint8_t USART3_RxBUF[USART3_MAX_RECVLEN];

void u2_printf(char *fmt, ...);
void u3_transmit(char *fmt, ...);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

