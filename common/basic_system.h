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
#include "opt_prototypes.h"

#include <stdint.h>

#define FWU_FLAG_FORCED (0x00000001)
#define FWU_FLAG_SKIP_CRC (0x00000002)
#define FWU_FLAG_VERIFY (0x00000004)

#define IMAGE_MAGIC_APP (0xA9911C47)
#define IMAGE_MAGIC_BL (0xB007104D)
#define IMAGE_MAGIC_EMPTY (0xFFFFFFFF)
#define IMAGE_MAGIC_INVALID (0x00000000)

// 0xFFFFFFFF represents an unset/uninitialized HW address
#define HARDWARE_ADDRESS_INVALID (0xFFFFFFFF)
// 0 is the broadcast address, so we want to reserve it
#define HARDWARE_REVISION_INVALID (0x0)

// Structure to write into metadata (persistant) storage
// Most likely flash or eeprom, align to 8 for flash storage
// MUST match storage size allocated in BSP_FLASH.ld
typedef struct __attribute__((packed, aligned(8)))
{
  uint32_t image_magic; // Determines image type
  uint8_t  version_major;
  uint8_t  version_minor;
  uint8_t  version_patch;
  uint8_t  hardware_revision;
  uint32_t reserved;
  uint32_t boot_address;
} image_metadata_t;

typedef struct __attribute__((packed, aligned(8)))
{
  image_metadata_t image_metadata;
  uint32_t         chunk_size;
  uint32_t         image_size;
  uint32_t         crc;
  uint32_t         flags;
} firmware_update_metadata_t;

typedef enum
{
  FWU_ERROR_NONE = 0,
  FWU_ERROR_UNKNOWN_REQUEST,
  FWU_ERROR_NOT_STARTED,
  FWU_ERROR_INVALID_MESSAGE_LENGTH,
  FWU_ERROR_INVALID_IMAGE,
  FWU_ERROR_INVALID_CHUNK_SIZE,
  FWU_ERROR_INVALID_IMAGE_SIZE,
  FWU_ERROR_ERASE_FAILURE,
  FWU_ERROR_PROGRAM_FAILURE,
  FWU_ERROR_CRC_MISMATCH,
  FWU_ERROR_BUSY,
  FWU_ERROR_NO_APP,
  FWU_ERROR_NO_BL,
  FWU_ERROR_MAX,
} firmware_update_error_t;

typedef enum
{
  // Reads metadata from a neighbor
  BASIC_SYSTEM_MESSAGE_READ_IMAGE_METADATA = BASIC_SYSTEM_MESSAGE_TYPE,
  // Requests a reset, contains reason information
  BASIC_SYSTEM_MESSAGE_RESET,
  // Query information about firmware update limits (max chunk, etc)
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_START,
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_ABORT,
  // Sends a chunk of firmware metadata
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_STREAM_DATA,
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_VERIFING,
  // Basic acknowledgement
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_COMPLETE,
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_FAILED,
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_BUSY,
  // Signals booting in the bootloader or other handler, indicates retry after short delay
  BASIC_SYSTEM_MESSAGE_FW_UPDATE_REBOOTING,
  // Misc commands
  BASIC_SYSTEM_MESSAGE_FORWARD_DATA,
  // Misc responses
  BASIC_SYSTEM_MESSAGE_NOT_IMPLEMENTED,
  BASIC_SYSTEM_MESSAGE_MAX,
} basic_system_message_type_t;

// A pointer to the currently active metadata structure
extern const image_metadata_t* current_metadata;

// Image metadata access
const image_metadata_t* get_image_metadata(uint32_t image_type);
void                    set_image_metadata(image_metadata_t data, opt_callback_t cb);

// Firmware state machine
void basic_system_init(uintptr_t image_root);
void basic_system_handle_message(receive_opt_t incoming_message);
void basic_system_update(opt_callback_t cb);

// Misc decl
extern void Error_Handler(void);

#endif // BASIC_SYSTEM_H