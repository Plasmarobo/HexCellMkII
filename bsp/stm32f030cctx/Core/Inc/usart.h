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
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/

/* USER CODE BEGIN Includes */
#include "comm_phy.h"

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include <stdint.h>
  /* USER CODE END Includes */

  extern UART_HandleTypeDef huart1;

  extern UART_HandleTypeDef huart2;

  extern UART_HandleTypeDef huart3;

  extern UART_HandleTypeDef huart4;

  extern UART_HandleTypeDef huart5;

  extern UART_HandleTypeDef huart6;

  /* USER CODE BEGIN Private defines */
#define UART_SUCCESS (0)
#define UART_ERR_BUSY (-1)
#define UART_ERR_TIMEOUT (-2)
#define UART_ERR_COMM (-3)
#define UART_ERR_PARAM (-4)
#define UART_ERR_OVERFLOW (-5)
#define UART_ERR_ABORT (-6)

  /* USER CODE END Private defines */

  void MX_USART1_UART_Init(void);
  void MX_USART2_UART_Init(void);
  void MX_USART3_UART_Init(void);
  void MX_USART4_UART_Init(void);
  void MX_USART5_UART_Init(void);
  void MX_USART6_UART_Init(void);

  /* USER CODE BEGIN Prototypes */
  int32_t uart_send(comm_port_t port, uint8_t* buffer, uint16_t length, opt_callback_t cb);
  int32_t uart_receive(comm_port_t port, uint8_t* buffer, uint16_t length, opt_callback_t cb);
  void    uart_abort_tx(comm_port_t port);
  void    uart_abort_rx(comm_port_t port);

  /* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */
