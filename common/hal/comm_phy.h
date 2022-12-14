#ifndef COMM_PHY_H
#define COMM_PHY_H
/**
 * @file comm_phy.h
 * @author Austen Danger Bartels (adangerbartels@gmail.com)
 * @brief
 * @version 0.1
 * @date 2022-11-15
 *
 * @copyright Copyright (c) 2022
 *
 * This layer defines the interface for physical communications.
 * Each BSP must implement this interface
 */

#include "message_protocol.h"
#include "opt_prototypes.h"

#include <stdbool.h>

#define COMM_BUFFER_LENGTH (512)
#define COMM_SUCCESS (0)
#define COMM_ERR_UNKNOWN (-1)
#define COMM_PORT_MASK(port) (0x1 << port)
#define COMM_BANK(port) (port / 2)
#define COMM_DIR_OUT (0)
#define COMM_DIR_IN (1)
#define COMM_PORT(bank, dir) ((bank * 2) + (dir ? 1 : 0)

typedef enum
{
  PORT_AO = 0,
  PORT_AI,
  PORT_BO,
  PORT_BI,
  PORT_CO,
  PORT_CI,
  PORT_USB,
  PORT_MAX,
} comm_port_t;

typedef enum
{
  BANK_A,
  BANK_B,
  BANK_C,
  BANK_MAX,
} comm_bank_t;

/* Initialize comm phy structures*/
void comm_phy_init(void);
/* Listen on a port */
void listen(comm_port_t port, message_t* buffer, opt_callback_t cb);
/* Send data to a port */
void send(comm_port_t port, message_t* buffer, opt_callback_t cb);
/* Queries port for activity */
bool port_busy(comm_port_t port);
/**
 * @brief Determines if hardware is connected, only applies to OUTPUT ports
 *
 * @param port
 * @return true if selected port is connected
 * @return false if selected port is not connected
 */
bool is_connected(comm_port_t port);
/**
 * @brief Sets a callback to be fired on any port status change (connect/disconnect)
 *
 * @param cb
 */
void on_connect_disconnect(opt_callback_t cb);

#endif /* COMM_PHY_H */