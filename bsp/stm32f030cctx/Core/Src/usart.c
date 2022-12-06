/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    usart.c
 * @brief   This file provides code for the configuration
 *          of the USART instances.
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
/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
#include "bsp.h"

#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"

#include <string.h>

typedef struct
{
  UART_HandleTypeDef* huart;
  opt_callback_t      rx_cb;
  opt_callback_t      tx_cb;
} uart_port_state_t;

static uart_port_state_t uart_state_data[PORT_MAX];
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;
UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart6;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */
  uart_state_data[PORT_CO].huart  = &huart1;
  uart_state_data[PORT_CO].rx_cb  = NULL;
  uart_state_data[PORT_CO].tx_cb  = NULL;
  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance                         = USART1;
  huart1.Init.BaudRate                    = 220400;
  huart1.Init.WordLength                  = UART_WORDLENGTH_8B;
  huart1.Init.StopBits                    = UART_STOPBITS_1;
  huart1.Init.Parity                      = UART_PARITY_NONE;
  huart1.Init.Mode                        = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl                   = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling                = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling              = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit      = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart1.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */
}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */
  uart_state_data[PORT_BI].huart  = &huart2;
  uart_state_data[PORT_BI].rx_cb  = NULL;
  uart_state_data[PORT_BI].tx_cb  = NULL;
  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance                         = USART2;
  huart2.Init.BaudRate                    = 220400;
  huart2.Init.WordLength                  = UART_WORDLENGTH_8B;
  huart2.Init.StopBits                    = UART_STOPBITS_1;
  huart2.Init.Parity                      = UART_PARITY_NONE;
  huart2.Init.Mode                        = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl                   = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling                = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling              = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit      = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart2.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */
}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */
  uart_state_data[PORT_AI].huart  = &huart3;
  uart_state_data[PORT_AI].rx_cb  = NULL;
  uart_state_data[PORT_AI].tx_cb  = NULL;
  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance                         = USART3;
  huart3.Init.BaudRate                    = 220400;
  huart3.Init.WordLength                  = UART_WORDLENGTH_8B;
  huart3.Init.StopBits                    = UART_STOPBITS_1;
  huart3.Init.Parity                      = UART_PARITY_NONE;
  huart3.Init.Mode                        = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl                   = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling                = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling              = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit      = UART_ADVFEATURE_DMADISABLEONERROR_INIT;
  huart3.AdvancedInit.DMADisableonRxError = UART_ADVFEATURE_DMA_DISABLEONRXERROR;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */
}
/* USART4 init function */

void MX_USART4_UART_Init(void)
{

  /* USER CODE BEGIN USART4_Init 0 */
  uart_state_data[PORT_CI].huart  = &huart4;
  uart_state_data[PORT_CI].rx_cb  = NULL;
  uart_state_data[PORT_CI].tx_cb  = NULL;
  /* USER CODE END USART4_Init 0 */

  /* USER CODE BEGIN USART4_Init 1 */

  /* USER CODE END USART4_Init 1 */
  huart4.Instance                    = USART4;
  huart4.Init.BaudRate               = 220400;
  huart4.Init.WordLength             = UART_WORDLENGTH_8B;
  huart4.Init.StopBits               = UART_STOPBITS_1;
  huart4.Init.Parity                 = UART_PARITY_NONE;
  huart4.Init.Mode                   = UART_MODE_TX_RX;
  huart4.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  huart4.Init.OverSampling           = UART_OVERSAMPLING_16;
  huart4.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  huart4.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart4) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART4_Init 2 */

  /* USER CODE END USART4_Init 2 */
}
/* USART5 init function */

void MX_USART5_UART_Init(void)
{

  /* USER CODE BEGIN USART5_Init 0 */
  uart_state_data[PORT_AO].huart  = &huart5;
  uart_state_data[PORT_AO].rx_cb  = NULL;
  uart_state_data[PORT_AO].tx_cb  = NULL;
  /* USER CODE END USART5_Init 0 */

  /* USER CODE BEGIN USART5_Init 1 */

  /* USER CODE END USART5_Init 1 */
  huart5.Instance                    = USART5;
  huart5.Init.BaudRate               = 220400;
  huart5.Init.WordLength             = UART_WORDLENGTH_8B;
  huart5.Init.StopBits               = UART_STOPBITS_1;
  huart5.Init.Parity                 = UART_PARITY_NONE;
  huart5.Init.Mode                   = UART_MODE_TX_RX;
  huart5.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  huart5.Init.OverSampling           = UART_OVERSAMPLING_16;
  huart5.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  huart5.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart5) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART5_Init 2 */

  /* USER CODE END USART5_Init 2 */
}
/* USART6 init function */

void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */
  uart_state_data[PORT_BO].huart  = &huart6;
  uart_state_data[PORT_BO].rx_cb  = NULL;
  uart_state_data[PORT_BO].tx_cb  = NULL;
  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance                    = USART6;
  huart6.Init.BaudRate               = 220400;
  huart6.Init.WordLength             = UART_WORDLENGTH_8B;
  huart6.Init.StopBits               = UART_STOPBITS_1;
  huart6.Init.Parity                 = UART_PARITY_NONE;
  huart6.Init.Mode                   = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl              = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling           = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling         = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */
}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = { 0 };
  if (uartHandle->Instance == USART1)
  {
    /* USER CODE BEGIN USART1_MspInit 0 */

    /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin       = CO_TX_Pin | CO_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART1 interrupt Init */
    HAL_NVIC_SetPriority(USART1_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspInit 1 */

    /* USER CODE END USART1_MspInit 1 */
  }
  else if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspInit 0 */

    /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA15     ------> USART2_RX
    */
    GPIO_InitStruct.Pin       = BI_TX_Pin | BI_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspInit 1 */

    /* USER CODE END USART2_MspInit 1 */
  }
  else if (uartHandle->Instance == USART3)
  {
    /* USER CODE BEGIN USART3_MspInit 0 */

    /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin       = AI_TX_Pin | AI_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART3;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_6_IRQn);
    /* USER CODE BEGIN USART3_MspInit 1 */

    /* USER CODE END USART3_MspInit 1 */
  }
  else if (uartHandle->Instance == USART4)
  {
    /* USER CODE BEGIN USART4_MspInit 0 */

    /* USER CODE END USART4_MspInit 0 */
    /* USART4 clock enable */
    __HAL_RCC_USART4_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART4 GPIO Configuration
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX
    */
    GPIO_InitStruct.Pin       = CI_TX_Pin | CI_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART4;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART4 interrupt Init */
    HAL_NVIC_SetPriority(USART3_6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_6_IRQn);
    /* USER CODE BEGIN USART4_MspInit 1 */

    /* USER CODE END USART4_MspInit 1 */
  }
  else if (uartHandle->Instance == USART5)
  {
    /* USER CODE BEGIN USART5_MspInit 0 */

    /* USER CODE END USART5_MspInit 0 */
    /* USART5 clock enable */
    __HAL_RCC_USART5_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART5 GPIO Configuration
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX
    */
    GPIO_InitStruct.Pin       = AO_TX_Pin | AO_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART5;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART5 interrupt Init */
    HAL_NVIC_SetPriority(USART3_6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_6_IRQn);
    /* USER CODE BEGIN USART5_MspInit 1 */

    /* USER CODE END USART5_MspInit 1 */
  }
  else if (uartHandle->Instance == USART6)
  {
    /* USER CODE BEGIN USART6_MspInit 0 */

    /* USER CODE END USART6_MspInit 0 */
    /* USART6 clock enable */
    __HAL_RCC_USART6_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART6 GPIO Configuration
    PA4     ------> USART6_TX
    PA5     ------> USART6_RX
    */
    GPIO_InitStruct.Pin       = BO_TX_Pin | BO_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_USART6;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART6 interrupt Init */
    HAL_NVIC_SetPriority(USART3_6_IRQn, 3, 0);
    HAL_NVIC_EnableIRQ(USART3_6_IRQn);
    /* USER CODE BEGIN USART6_MspInit 1 */

    /* USER CODE END USART6_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if (uartHandle->Instance == USART1)
  {
    /* USER CODE BEGIN USART1_MspDeInit 0 */

    /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, CO_TX_Pin | CO_RX_Pin);

    /* USART1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART1_IRQn);
    /* USER CODE BEGIN USART1_MspDeInit 1 */

    /* USER CODE END USART1_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART2)
  {
    /* USER CODE BEGIN USART2_MspDeInit 0 */

    /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA15     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, BI_TX_Pin | BI_RX_Pin);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
    /* USER CODE BEGIN USART2_MspDeInit 1 */

    /* USER CODE END USART2_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART3)
  {
    /* USER CODE BEGIN USART3_MspDeInit 0 */

    /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, AI_TX_Pin | AI_RX_Pin);

    /* USART3 interrupt Deinit */
    /* USER CODE BEGIN USART3:USART3_6_IRQn disable */
    /**
     * Uncomment the line below to disable the "USART3_6_IRQn" interrupt
     * Be aware, disabling shared interrupt may affect other IPs
     */
    /* HAL_NVIC_DisableIRQ(USART3_6_IRQn); */
    /* USER CODE END USART3:USART3_6_IRQn disable */

    /* USER CODE BEGIN USART3_MspDeInit 1 */

    /* USER CODE END USART3_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART4)
  {
    /* USER CODE BEGIN USART4_MspDeInit 0 */

    /* USER CODE END USART4_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART4_CLK_DISABLE();

    /**USART4 GPIO Configuration
    PA0     ------> USART4_TX
    PA1     ------> USART4_RX
    */
    HAL_GPIO_DeInit(GPIOA, CI_TX_Pin | CI_RX_Pin);

    /* USART4 interrupt Deinit */
    /* USER CODE BEGIN USART4:USART3_6_IRQn disable */
    /**
     * Uncomment the line below to disable the "USART3_6_IRQn" interrupt
     * Be aware, disabling shared interrupt may affect other IPs
     */
    /* HAL_NVIC_DisableIRQ(USART3_6_IRQn); */
    /* USER CODE END USART4:USART3_6_IRQn disable */

    /* USER CODE BEGIN USART4_MspDeInit 1 */

    /* USER CODE END USART4_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART5)
  {
    /* USER CODE BEGIN USART5_MspDeInit 0 */

    /* USER CODE END USART5_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART5_CLK_DISABLE();

    /**USART5 GPIO Configuration
    PB3     ------> USART5_TX
    PB4     ------> USART5_RX
    */
    HAL_GPIO_DeInit(GPIOB, AO_TX_Pin | AO_RX_Pin);

    /* USART5 interrupt Deinit */
    /* USER CODE BEGIN USART5:USART3_6_IRQn disable */
    /**
     * Uncomment the line below to disable the "USART3_6_IRQn" interrupt
     * Be aware, disabling shared interrupt may affect other IPs
     */
    /* HAL_NVIC_DisableIRQ(USART3_6_IRQn); */
    /* USER CODE END USART5:USART3_6_IRQn disable */

    /* USER CODE BEGIN USART5_MspDeInit 1 */

    /* USER CODE END USART5_MspDeInit 1 */
  }
  else if (uartHandle->Instance == USART6)
  {
    /* USER CODE BEGIN USART6_MspDeInit 0 */

    /* USER CODE END USART6_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART6_CLK_DISABLE();

    /**USART6 GPIO Configuration
    PA4     ------> USART6_TX
    PA5     ------> USART6_RX
    */
    HAL_GPIO_DeInit(GPIOA, BO_TX_Pin | BO_RX_Pin);

    /* USART6 interrupt Deinit */
    /* USER CODE BEGIN USART6:USART3_6_IRQn disable */
    /**
     * Uncomment the line below to disable the "USART3_6_IRQn" interrupt
     * Be aware, disabling shared interrupt may affect other IPs
     */
    /* HAL_NVIC_DisableIRQ(USART3_6_IRQn); */
    /* USER CODE END USART6:USART3_6_IRQn disable */

    /* USER CODE BEGIN USART6_MspDeInit 1 */

    /* USER CODE END USART6_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
static void handle_abort(int32_t user1, uint32_t user2)
{
  // Noop
}

int32_t uart_send(comm_port_t port, uint8_t* buffer, uint16_t length, opt_callback_t cb)
{
  int32_t response = UART_SUCCESS;
  if ((port >= PORT_MAX) || (NULL == buffer) || (0 == length) || (NULL == cb))
  {
    response = UART_ERR_PARAM;
  }
  else if (NULL != uart_state_data[port].tx_cb)
  {
    response = UART_ERR_BUSY;
  }
  else
  {
    uart_state_data[port].tx_cb = cb;
    if (HAL_OK != HAL_UART_Transmit_IT(uart_state_data[port].huart, buffer, length))
    {
      response = UART_ERR_BUSY;
    }
  }
  return response;
}

int32_t uart_receive(comm_port_t port, uint8_t* buffer, uint16_t length, opt_callback_t cb)
{
  int32_t response = UART_SUCCESS;
  if ((port >= PORT_MAX) || (NULL == buffer) || (0 == length) || (NULL == cb))
  {
    response = UART_ERR_PARAM;
  }
  else if (NULL != uart_state_data[port].tx_cb)
  {
    response = UART_ERR_BUSY;
  }
  else
  {
    uart_state_data[port].rx_cb = cb;
    if (HAL_OK != HAL_UART_Receive_IT(uart_state_data[port].huart, buffer, length))
    {
      response = UART_ERR_BUSY;
    }
  }
  return response;
}

void uart_abort_tx(comm_port_t port)
{
  if (port < PORT_MAX)
  {
    // Set the CB to prevent other ops
    uart_state_data[port].tx_cb = handle_abort;
    HAL_UART_AbortTransmit_IT(uart_state_data[port].huart);
  }
}

void uart_abort_rx(comm_port_t port)
{
  if (port < PORT_MAX)
  {
    // Set the CB to prevent other ops
    uart_state_data[port].rx_cb = handle_abort;
    HAL_UART_AbortReceive_IT(uart_state_data[port].huart);
  }
}

void HAL_UART_TxHalfCpltCallback(UART_HandleTypeDef* huart)
{
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef* huart)
{
  for (uint8_t i = PORT_CO; i < PORT_MAX; ++i)
  {
    if ((huart == uart_state_data[i].huart) && (NULL != uart_state_data[i].tx_cb))
    {
      uart_state_data[i].tx_cb(UART_SUCCESS, i);
      uart_state_data[i].tx_cb = NULL;
      return;
    }
  }
}

void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef* huart)
{
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart)
{
  for (uint8_t i = PORT_CO; i < PORT_MAX; ++i)
  {
    if ((huart == uart_state_data[i].huart) && (NULL != uart_state_data[i].rx_cb))
    {
      uart_state_data[i].rx_cb(UART_SUCCESS, i);
      uart_state_data[i].rx_cb = NULL;
      return;
    }
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef* huart)
{
  for (uint8_t i = PORT_CO; i < PORT_MAX; ++i)
  {
    if ((huart == uart_state_data[i].huart) && (NULL != uart_state_data[i].rx_cb))
    {
      uart_state_data[i].rx_cb(UART_ERR_COMM, 0);
      uart_state_data[i].rx_cb = NULL;
      return;
    }
    if ((huart == uart_state_data[i].huart) && (NULL != uart_state_data[i].tx_cb))
    {
      uart_state_data[i].tx_cb(UART_ERR_COMM, 0);
      uart_state_data[i].tx_cb = NULL;
      return;
    }
  }
}

void HAL_UART_AbortTransmitCpltCallback(UART_HandleTypeDef* huart)
{
  for (uint8_t i = PORT_CO; i < PORT_MAX; ++i)
  {
    if (huart == uart_state_data[i].huart)
    {
      uart_state_data[i].tx_cb = NULL;
      return;
    }
  }
}

void HAL_UART_AbortReceiveCpltCallback(UART_HandleTypeDef* huart)
{
  for (uint8_t i = PORT_CO; i < PORT_MAX; ++i)
  {
    if (huart == uart_state_data[i].huart)
    {
      uart_state_data[i].rx_cb = NULL;
      return;
    }
  }
}

/* USER CODE END 1 */
