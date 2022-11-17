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
#include "stm32f0xx_hal.h"

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
#define CI_TX_Pin GPIO_PIN_0
#define CI_TX_GPIO_Port GPIOA
#define CI_RX_Pin GPIO_PIN_1
#define CI_RX_GPIO_Port GPIOA
#define BI_TX_Pin GPIO_PIN_2
#define BI_TX_GPIO_Port GPIOA
#define BO_TX_Pin GPIO_PIN_4
#define BO_TX_GPIO_Port GPIOA
#define BO_RX_Pin GPIO_PIN_5
#define BO_RX_GPIO_Port GPIOA
#define DETECT_B_Pin GPIO_PIN_6
#define DETECT_B_GPIO_Port GPIOA
#define AI_TX_Pin GPIO_PIN_10
#define AI_TX_GPIO_Port GPIOB
#define AI_RX_Pin GPIO_PIN_11
#define AI_RX_GPIO_Port GPIOB
#define BUS_SDA_OUT_Pin GPIO_PIN_14
#define BUS_SDA_OUT_GPIO_Port GPIOB
#define LED_OUT_Pin GPIO_PIN_15
#define LED_OUT_GPIO_Port GPIOB
#define DETECT_C_Pin GPIO_PIN_8
#define DETECT_C_GPIO_Port GPIOA
#define CO_TX_Pin GPIO_PIN_9
#define CO_TX_GPIO_Port GPIOA
#define CO_RX_Pin GPIO_PIN_10
#define CO_RX_GPIO_Port GPIOA
#define BUS_SCL_OUT_Pin GPIO_PIN_11
#define BUS_SCL_OUT_GPIO_Port GPIOA
#define BI_RX_Pin GPIO_PIN_15
#define BI_RX_GPIO_Port GPIOA
#define AO_TX_Pin GPIO_PIN_3
#define AO_TX_GPIO_Port GPIOB
#define AO_RX_Pin GPIO_PIN_4
#define AO_RX_GPIO_Port GPIOB
#define DETECT_A_Pin GPIO_PIN_5
#define DETECT_A_GPIO_Port GPIOB
#define BUS_SCL_IN_Pin GPIO_PIN_6
#define BUS_SCL_IN_GPIO_Port GPIOB
#define BUS_SDA_IN_Pin GPIO_PIN_7
#define BUS_SDA_IN_GPIO_Port GPIOB
#define EN_3V3_Pin GPIO_PIN_9
#define EN_3V3_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
