#include "keystore.h"

#include "memory_layout.h"

#include "stm32f1xx_hal.h"
#include "stm32f1xx_hal_flash.h"

#include <string.h>

static keystore_t keystore_cache;

const keystore_t* get_keystore(void)
{
  return (const keystore_t*)ORIGIN_KEYSTORE;
}

// Write to keystore cache
void set_hardware_id(uint32_t hwid)
{
  keystore_cache.hardware_id = hwid;
}

void set_key0(uint8_t* key)
{
  memcpy(keystore_cache.key0, key, KEY_LENGTH);
}

void set_key1(uint8_t* key)
{
  memcpy(keystore_cache.key1, key, KEY_LENGTH);
}

static void write_keystore_cache(void)
{
  // HAL_FLASH_Program()
}

static void erase_keystore(void)
{
  // HAL_FLASH_Unlock();
  // HAL_FLASHEx_Erase();
}

// Write keystore cache to flash
void commit_keystore()
{
  // Erase flash, then write flash
}