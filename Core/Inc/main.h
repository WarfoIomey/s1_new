/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DEV3_Pin GPIO_PIN_13
#define DEV3_GPIO_Port GPIOC
#define Y1_Pin GPIO_PIN_0
#define Y1_GPIO_Port GPIOA
#define V24_Pin GPIO_PIN_1
#define V24_GPIO_Port GPIOA
#define TX_RS232_Pin GPIO_PIN_2
#define TX_RS232_GPIO_Port GPIOA
#define RX_RS232_Pin GPIO_PIN_3
#define RX_RS232_GPIO_Port GPIOA
#define X1_Pin GPIO_PIN_4
#define X1_GPIO_Port GPIOA
#define Z1_Pin GPIO_PIN_5
#define Z1_GPIO_Port GPIOA
#define Y2_Pin GPIO_PIN_6
#define Y2_GPIO_Port GPIOA
#define X2_Pin GPIO_PIN_7
#define X2_GPIO_Port GPIOA
#define Z2_Pin GPIO_PIN_4
#define Z2_GPIO_Port GPIOC
#define Y3_Pin GPIO_PIN_5
#define Y3_GPIO_Port GPIOC
#define X3_Pin GPIO_PIN_0
#define X3_GPIO_Port GPIOB
#define Z3_Pin GPIO_PIN_1
#define Z3_GPIO_Port GPIOB
#define IN1_Pin GPIO_PIN_10
#define IN1_GPIO_Port GPIOB
#define IN2_Pin GPIO_PIN_11
#define IN2_GPIO_Port GPIOB
#define RST_ETH_Pin GPIO_PIN_12
#define RST_ETH_GPIO_Port GPIOB
#define CK_ETH_Pin GPIO_PIN_13
#define CK_ETH_GPIO_Port GPIOB
#define MISO_ETH_Pin GPIO_PIN_14
#define MISO_ETH_GPIO_Port GPIOB
#define MOSI_ETH_Pin GPIO_PIN_15
#define MOSI_ETH_GPIO_Port GPIOB
#define IN3_Pin GPIO_PIN_6
#define IN3_GPIO_Port GPIOC
#define IN4_Pin GPIO_PIN_7
#define IN4_GPIO_Port GPIOC
#define IN5_Pin GPIO_PIN_8
#define IN5_GPIO_Port GPIOC
#define IN6_Pin GPIO_PIN_9
#define IN6_GPIO_Port GPIOC
#define IN7_Pin GPIO_PIN_8
#define IN7_GPIO_Port GPIOA
#define IN8_Pin GPIO_PIN_9
#define IN8_GPIO_Port GPIOA
#define IN9_Pin GPIO_PIN_10
#define IN9_GPIO_Port GPIOA
#define DEV1_Pin GPIO_PIN_11
#define DEV1_GPIO_Port GPIOA
#define DEV2_Pin GPIO_PIN_12
#define DEV2_GPIO_Port GPIOA
#define C_Pin GPIO_PIN_15
#define C_GPIO_Port GPIOA
#define TX_WF_Pin GPIO_PIN_10
#define TX_WF_GPIO_Port GPIOC
#define RX_WF_Pin GPIO_PIN_11
#define RX_WF_GPIO_Port GPIOC
#define INT_ETH_Pin GPIO_PIN_12
#define INT_ETH_GPIO_Port GPIOC
#define CS_ETH_Pin GPIO_PIN_2
#define CS_ETH_GPIO_Port GPIOD
#define CK_IND_Pin GPIO_PIN_3
#define CK_IND_GPIO_Port GPIOB
#define PB4_Pin GPIO_PIN_4
#define PB4_GPIO_Port GPIOB
#define MOSI_IND_Pin GPIO_PIN_5
#define MOSI_IND_GPIO_Port GPIOB
#define CH_PD_WF_Pin GPIO_PIN_6
#define CH_PD_WF_GPIO_Port GPIOB
#define RST_WF_Pin GPIO_PIN_7
#define RST_WF_GPIO_Port GPIOB
#define B_Pin GPIO_PIN_8
#define B_GPIO_Port GPIOB
#define A_Pin GPIO_PIN_9
#define A_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
