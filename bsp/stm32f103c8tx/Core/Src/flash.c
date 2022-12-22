#include "flash.h"

#include "opt_prototypes.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"
#include "stm32f1xx_hal_flash_ex.h"

#include <stdbool.h>
#include <stdint.h>

// 1K page for STM32F013C8T6
#define PAGE_SIZE (1024)

typedef enum
{
  FLASH_DRIVER_STATE_IDLE = 0,
  FLASH_DRIVER_STATE_ERASE,
  FLASH_DRIVER_STATE_WRITE,
} flash_driver_state_t;

static FLASH_EraseInitTypeDef erase_data;
static uint8_t*               data_ptr;
static uint8_t*               data_cache;
static uint32_t               length_cache;
static uint32_t               address_cache;
static opt_callback_t         cb_cache;
static flash_driver_state_t   state;

static void finish(int32_t error_code)
{
  if (NULL != cb_cache)
  {
    cb_cache(error_code, FLASH_SUCCESS == error_code ? length_cache : state);
  }
  state         = FLASH_DRIVER_STATE_IDLE;
  data_cache    = NULL;
  length_cache  = 0;
  address_cache = 0;
  cb_cache      = NULL;
  HAL_FLASH_Lock();
}

static void flash_write_chunk(void)
{
  // Write as much of the cache as we can
  uint32_t stride;
  uint32_t program_type;
  uint8_t  pad_buffer[2];
  uint32_t bytes_written  = data_ptr - data_cache;
  uint32_t bytes_to_write = length_cache - bytes_written;

  if (bytes_to_write > 0)
  {
    if (bytes_to_write >= sizeof(uint64_t))
    {
      program_type = FLASH_TYPEPROGRAM_DOUBLEWORD;
      stride       = sizeof(uint64_t);
    }
    else if (bytes_to_write >= sizeof(uint32_t))
    {
      program_type = FLASH_TYPEPROGRAM_WORD;
      stride       = sizeof(uint32_t);
    }
    else if (bytes_to_write >= sizeof(uint16_t))
    {
      program_type = FLASH_TYPEPROGRAM_HALFWORD;
      stride       = sizeof(uint16_t);
    }
    else
    {
      program_type = FLASH_TYPEPROGRAM_HALFWORD;
      // Pad up to a half word here
      pad_buffer[0] = *data_ptr;
      pad_buffer[1] = 0;
      data_ptr      = (uint8_t*)(&pad_buffer[0]);
      stride        = sizeof(uint8_t);
    }
    HAL_FLASH_Program_IT(program_type, address_cache + bytes_written, *((uint64_t*)data_ptr));
    data_ptr += stride;
  }
}

static void handle_flash_driver_state(void)
{
  switch (state)
  {
    case FLASH_DRIVER_STATE_ERASE:
      finish(FLASH_SUCCESS);
      break;
    case FLASH_DRIVER_STATE_WRITE:
      {
        if ((data_ptr - data_cache) < length_cache)
        {
          flash_write_chunk();
        }
        else
        {
          finish(FLASH_SUCCESS);
        }
      }
    case FLASH_DRIVER_STATE_IDLE:
    default:
      finish(FLASH_ERROR);
      break;
  }
}

void flash_erase(uint32_t address, uint32_t length, opt_callback_t cb)
{
  if (length > 0)
  {
    if (FLASH_DRIVER_STATE_IDLE == state)
    {
      state    = FLASH_DRIVER_STATE_ERASE;
      cb_cache = cb;
      HAL_FLASH_Unlock();
      erase_data.PageAddress = address;
      erase_data.NbPages     = ((length - 1) / PAGE_SIZE) + 1;
      erase_data.TypeErase   = FLASH_TYPEERASE_PAGES;
      HAL_FLASHEx_Erase_IT(&erase_data);
    }
    else
    {
      if (NULL != cb)
      {
        cb(FLASH_BUSY, state);
      }
    }
  }
  else
  {
    length_cache = 0;
    finish(FLASH_SUCCESS);
  }
}

void flash_write(uint32_t address, uint8_t* data, uint32_t length, opt_callback_t cb)
{
  if (length > 0)
  {
    if (FLASH_DRIVER_STATE_IDLE == state)
    {
      state         = FLASH_DRIVER_STATE_WRITE;
      cb_cache      = cb;
      data_cache    = data;
      data_ptr      = data;
      length_cache  = length;
      address_cache = address;
      HAL_FLASH_Unlock();
      flash_write_chunk();
    }
    else
    {
      if (NULL != cb)
      {
        cb(FLASH_BUSY, state);
      }
    }
  }
  else
  {
    finish(FLASH_ERROR);
  }
}

void flash_init(void)
{
  state         = FLASH_DRIVER_STATE_IDLE;
  data_cache    = NULL;
  length_cache  = 0;
  address_cache = 0;
  cb_cache      = NULL;
}

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue)
{
  if (HAL_FLASH_ERROR_NONE == ReturnValue)
  {
    handle_flash_driver_state();
  }
  else
  {
    finish(FLASH_ERROR);
  }
}

void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue)
{
  finish(FLASH_ERROR);
}