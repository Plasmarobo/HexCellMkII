#ifndef KEYSTORE_H
#define KEYSTORE_H

#include <stdint.h>

#define KEY_LENGTH (256)

typedef struct
{
  uint32_t hardware_id;
  uint8_t  key0[KEY_LENGTH];
  uint8_t  key1[KEY_LENGTH];
} keystore_t;

// Read keystore from FLASH
const keystore_t* get_keystore();
// Write to keystore cache
void set_hardware_id(uint32_t hwid);
void set_key0(uint8_t* key);
void set_key1(uint8_t* key);
// Write keystore cache to flash
void commit_keystore();

#endif