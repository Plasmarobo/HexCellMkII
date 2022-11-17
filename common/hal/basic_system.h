#ifndef BASIC_SYSTEM_H
#define BASIC_SYSTEM_H
/**
 * @file basic_system.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * Defines basic system constructs for querying image information
 */
#include "message_protocol.h"

#include <stdint.h>

#define FWU_FLAG_FORCED (0x00000001)
#define FWU_FLAG_SKIP_CRC (0x00000002)
#define FWU_FLAG_VERIFY (0x00000004)

#define IMAGE_MAGIC_APP (0xA9911C47)
#define IMAGE_MAGIC_BL (0xB007104D)
#define IMAGE_MAGIC_META (0xFE7AD474)
#define IMAGE_MAGIC_EMPTY (0xFFFFFFFF)
#define IMAGE_MAGIC_INVALID (0x00000000)

// Structure to write into metadata (persistant) storage
// Most likely flash or eeprom, align to 8 for flash storage
typedef struct __attribute__((packed, aligned(8)))
{
  uint32_t image_magic; // Determines image type
  uint8_t  version_major;
  uint8_t  version_minor;
  uint8_t  version_patch;
  uint8_t  hardware_revision;
  uint32_t boot_address;
} image_metadata_t;

typedef struct __attribute__((packed, aligned(8)))
{
  image_metadata_t image_metadata;
  uint32_t         chunk_size;
  uint32_t         chunk_count;
  uint32_t         crc;
  uint32_t         flags;
} firmware_update_metadata_t;

typedef enum
{
  FWU_ERROR_NONE = 0,
  FWU_ERROR_INVALID_QUERY,
  FWU_ERROR_INVALID_MESSAGE_LENGTH,
  FWU_ERROR_INVALID_IMAGE,
  FWU_ERROR_CHUNK_TOO_LONG,
  FWU_ERROR_IMAGE_TOO_LONG,
} firmware_update_error_t;

typedef enum
{
  // Reads metadata from a neighbor
  BASIC_SYSTEM_MESSAGE_READ_IMAGE_METADATA = BASIC_SYSTEM_MESSAGE_TYPE,
  // Writes metadata to a neighbor
  BASIC_SYSTEM_MESSAGE_WRITE_IMAGE_METADATA,
  // Requests a reset, contains reason information
  BASIC_SYSTEM_MESSAGE_RESET,
  // Query information about firmware update limits (max chunk, etc)
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_QUERY_CLIENT,
  // Sends information required to start a firmware update, including CRC, chunk-size, and number of chunks
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_METADATA,
  // Signals firmware update ready
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_CLIENT_READY,
  // Sends a chunk of firmware metadata
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA,
  // Basic acknowledgement
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_ACK,
  // Sends a chunk of
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_COMPLETE,
  BASIC_SYSTEM_MESSAGE_MAX,
} basic_system_message_type_t;

// Type contained in data field of a message
typedef struct
{
  union
  {

  },
} basic_system_message_t;

// Image metadata init
void image_metadata_init(uintptr_t root);

// Image metadata access
image_metadata_t get_image_metadata(void);
void             set_image_metadata(image_metadata_t data);

// Firmware state machine
void firmware_update_init(void);
void firmware_update_start(firmware_update_metadata_t data);
void firmware_update_update(void);

#endif // BASIC_SYSTEM_H