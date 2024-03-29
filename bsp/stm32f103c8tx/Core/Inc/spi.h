/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    spi.h
  * @brief   This file contains all the function prototypes for
  *          the spi.c file
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
#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f1xx_hal.h"

#ifdef HAL_SPI_MODULE_ENABLED
/* Includes ------------------------------------------------------------------*/
#include "opt_prototypes.h"
#include "stm32f1xx.h"
#include <stdint.h>

  /* USER CODE BEGIN Includes */

  /* USER CODE END Includes */

  extern SPI_HandleTypeDef hspi1;

  /* USER CODE BEGIN Private defines */

#define SPI_SUCCESS (0)
#define SPI_ERROR (1)
  /* USER CODE END Private defines */

  void MX_SPI1_Init(void);

  /* USER CODE BEGIN Prototypes */
  void spi_send(uint8_t* buffer, uint32_t length, opt_callback_t cb);
  /* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif
#endif /* __SPI_H__ */

