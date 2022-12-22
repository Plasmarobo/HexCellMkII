/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    crc.h
  * @brief   This file contains all the function prototypes for
  *          the crc.c file
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
#ifndef __CRC_H__
#define __CRC_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"
#include <stdint.h>
  /* USER CODE BEGIN Includes */

  /* USER CODE END Includes */

  extern CRC_HandleTypeDef hcrc;

  /* USER CODE BEGIN Private defines */

  /* USER CODE END Private defines */

  void MX_CRC_Init(void);

  /* USER CODE BEGIN Prototypes */
  void     start_crc(uint8_t* buffer, uint32_t length);
  void     continue_crc(uint8_t* buffer, uint32_t length);
  uint32_t finish_crc(void);
  /* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CRC_H__ */

