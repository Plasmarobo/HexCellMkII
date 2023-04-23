/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#define INT_B_Pin GPIO_PIN_0
#define INT_B_GPIO_Port GPIOA
#define DETECT_B_Pin GPIO_PIN_1
#define DETECT_B_GPIO_Port GPIOA
#define PORT_B_TX_Pin GPIO_PIN_2
#define PORT_B_TX_GPIO_Port GPIOA
#define PORT_B_RX_Pin GPIO_PIN_3
#define PORT_B_RX_GPIO_Port GPIOA
#define SIGNAL_B_Pin GPIO_PIN_4
#define SIGNAL_B_GPIO_Port GPIOA
#define SEL_B_Pin GPIO_PIN_5
#define SEL_B_GPIO_Port GPIOA
#define SEL_BB_Pin GPIO_PIN_6
#define SEL_BB_GPIO_Port GPIOA
#define SEL_C_Pin GPIO_PIN_7
#define SEL_C_GPIO_Port GPIOA
#define SIGNAL_C_Pin GPIO_PIN_0
#define SIGNAL_C_GPIO_Port GPIOB
#define INT_C_Pin GPIO_PIN_1
#define INT_C_GPIO_Port GPIOB
#define DETECT_C_Pin GPIO_PIN_2
#define DETECT_C_GPIO_Port GPIOB
#define PORT_C_TX_Pin GPIO_PIN_10
#define PORT_C_TX_GPIO_Port GPIOB
#define PORT_C_RX_Pin GPIO_PIN_11
#define PORT_C_RX_GPIO_Port GPIOB
#define SEL_AA_Pin GPIO_PIN_12
#define SEL_AA_GPIO_Port GPIOB
#define SEL_A_Pin GPIO_PIN_13
#define SEL_A_GPIO_Port GPIOB
#define SIGNAL_A_Pin GPIO_PIN_14
#define SIGNAL_A_GPIO_Port GPIOB
#define INT_A_Pin GPIO_PIN_15
#define INT_A_GPIO_Port GPIOB
#define DETECT_A_Pin GPIO_PIN_8
#define DETECT_A_GPIO_Port GPIOA
#define PORT_A_TX_Pin GPIO_PIN_9
#define PORT_A_TX_GPIO_Port GPIOA
#define PORT_A_RX_Pin GPIO_PIN_10
#define PORT_A_RX_GPIO_Port GPIOA
#define LED_CLK_Pin GPIO_PIN_3
#define LED_CLK_GPIO_Port GPIOB
#define SEL_CC_Pin GPIO_PIN_4
#define SEL_CC_GPIO_Port GPIOB
#define LED_DATA_Pin GPIO_PIN_5
#define LED_DATA_GPIO_Port GPIOB
#define BOOT_REQ_Pin GPIO_PIN_6
#define BOOT_REQ_GPIO_Port GPIOB
#define BOOT_SENSE_Pin GPIO_PIN_7
#define BOOT_SENSE_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
