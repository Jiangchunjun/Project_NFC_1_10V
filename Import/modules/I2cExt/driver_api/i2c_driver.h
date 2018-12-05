// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram spa
//     Via Castagnole 65/a
//     31100 Treviso (I)
//
//
// The content of this file is intellectual property of OSRAM spa. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2014-12, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: i2c_driver.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/I2cExt/driver_api/i2c_driver.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __I2C_DRIVER_H
#define __I2C_DRIVER_H

#ifdef MODULE_I2C

/** \addtogroup I2cDriverAPI
 * \{
 *
 * \file
 * \brief Header file for i2c_driver_xx.c
 *
 */

/**********************************************************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/**********************************************************************************************************************/

#define i2c_master_irq              IRQ_Hdlr_9          // User defined function mapped to IRQ_Hdlr_9

/**********************************************************************************************************************/

typedef enum
{
    I2C_MASTER_CMD_READ,
    I2C_MASTER_CMD_WRITE,
    I2C_MASTER_CMD_MAX
}
i2c_master_cmd_t;

typedef enum
{
    I2C_CODE_NONE,
	I2C_CODE_NACK,
	I2C_CODE_FRAME_ERR,
    I2C_CODE_DATA_ERR,
    I2C_CODE_TIMEOUT,
    I2C_CODE_ADDR_ERR,
    I2C_CODE_OK,
    I2C_CODE_MAX
}
i2c_code_t;

typedef enum
{
    I2C_ACK_NO,
    I2C_ACK_YES,
    I2C_ACK_MAX
}
i2c_ack_t;

typedef enum
{
    I2C_TX_STATE_IDLE,
    I2C_TX_STATE_READ,
    I2C_TX_STATE_WRITE,
    I2C_TX_STATE_STOP,
    I2C_TX_STATE_END,
    I2C_TX_STATE_MAX
}
i2c_tx_state_t;

/**********************************************************************************************************************/

void i2c_drv_master_init(void);

void i2c_drv_master_err_irq_callback(i2c_code_t aErrCode);
void i2c_drv_master_tx_irq_callback(void);
void i2c_drv_master_rx_irq_callback(void);
i2c_tx_state_t i2c_drv_master_get_txstate_callback(void);

void i2c_drv_master_start(uint32_t addr, i2c_master_cmd_t rw_cmd, bool rep_start);
void i2c_drv_master_tx_write(uint8_t data);
void i2c_drv_master_tx_read(i2c_ack_t ack);
uint8_t i2c_drv_master_get_rx_data(void);
void i2c_drv_master_stop(void);

/**********************************************************************************************************************/
/** \} */ // I2cDriverAPI

#endif /* MODULE_I2C */

#endif

