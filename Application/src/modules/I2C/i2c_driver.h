// ---------------------------------------------------------------------------------------------------------------------
#ifndef __I2C_DRIVER_H
#define __I2C_DRIVER_H

/** ----------------------------------------------------------------------------
// i2c_driver.h
// -----------------------------------------------------------------------------
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
// $Author: g.salvador $
// $Revision: 16241 $
// $Date: 2018-12-17 23:09:30 +0800 (Mon, 17 Dec 2018) $
// $Id: i2c_driver.h 16241 2018-12-17 15:09:30Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_driver.h $
//

* \addtogroup I2cDriverAPI
* \{
*   \file
*   \brief Local header file for i2c_driver_xx.c
* \}
*/

#include <stdbool.h>

//-----------------------------------------------------------------------------
// parameters
//-----------------------------------------------------------------------------
#define M24LRxx_ADDRESS                 0xA0
#define M24LRxx_ADDR_DATA               (M24LRxx_ADDRESS | 0x06)
#define M24LRxx_ADDR_REGISTER           (M24LRxx_ADDRESS | 0x0E)
#define M24LRxx_DATA_ADDR_WIDTH         2
#define M24LRxx_PAGE_WRITE_LENGTH       4

// Needed time (by the I2c state machine!!!) to let the TAG store the data
#define M24LRxx_PAGE_WRITE_WAIT_TIME_us 2000 //[us]


#define M24LRxx_WRITE_CHECK_MAX_READINGS  3



#define WAIT_FOR_TRANSFER_DONE_TIMEOUT  200
#define MAX_TAG_WRITINGS_DURING_FLUSH   10

#define M24LRxx_MAX_WRITE_LENGTH        9
#define M24LRxx_USER_MEMORY_END         0x07FF
#define M24LRxx_MAX_NACK_COUNTS         250

#define M24LR_I2C_PASSWORD_BYTE_ADDR    0x0900

#define M24LR_RF_PASSWORD_1_CONTROL     0x08
#define M24LR_RF_PASSWORD_2_CONTROL     0x10
#define M24LR_RF_PASSWORD_3_CONTROL     0x18

#define M24LR_SSS_BYTE_AREA_START_ADDR  0x00

// M24_RF_ACCESS_RIGHTS
//
// b2,b1,  Sector   Sector access       Sector access   ACCESS_RIGHT_DEFINE
//         Lock     if PWD NOT          if PWD
//                  presented           presented
//-------- --------------------------------------------------------------------
// xx      0        read, write         read, write     M24LR_RF_ACCESS_RIGHT_0
// 00      1        read, no write      read, write     M24LR_RF_ACCESS_RIGHT_1
// 01      1        read, write         read, write     M24LR_RF_ACCESS_RIGHT_2
// 10      1        no read, no write   read, write     M24LR_RF_ACCESS_RIGHT_3
// 11      1        no read, no write   read, no write  M24LR_RF_ACCESS_RIGHT_4

#define M24LR_RF_ACCESS_RIGHT_0         0x00
#define M24LR_RF_ACCESS_RIGHT_1         0x01
#define M24LR_RF_ACCESS_RIGHT_2         0x02
#define M24LR_RF_ACCESS_RIGHT_3         0x05
#define M24LR_RF_ACCESS_RIGHT_4         0x07

// number of EEPROM sectors in NFC chip
#define M24LR_SECTOR_COUNT              0x10


//-----------------------------------------------------------------------------
// enums
//-----------------------------------------------------------------------------
typedef enum
{
    I2C_ACK_NO,
    I2C_ACK_YES,
    I2C_ACK_MAX
}
i2c_ack_t;

typedef enum
{
    I2C_DRIVER_STATUS_IDLE,
    I2C_DRIVER_STATUS_BUSY,
    I2C_DRIVER_STATUS_ERROR,
    I2C_DRIVER_STATUS_NACK,
    I2C_DRIVER_STATUS_DONE_TX,
    I2C_DRIVER_STATUS_DONE_RX
}
i2c_driver_status_t;


//-----------------------------------------------------------------------------
// structures
//-----------------------------------------------------------------------------
typedef struct
{
    i2c_driver_status_t        status;
    uint16_t                   crc_16;
} i2c_driver_feedback_struct_t;


//-----------------------------------------------------------------------------
// API variables
//-----------------------------------------------------------------------------
extern i2c_driver_feedback_struct_t i2c_driver_feedback_struct;


//-----------------------------------------------------------------------------
// API functions
//-----------------------------------------------------------------------------
void I2cDrvInit(void);
void I2cDrvStop(void);
void I2cDrvUpdateStatus(void);

// Functions to be renamed to fit into I2C Driver's namespace: I2cDrv*()
void start_transfer(void);
bool is_crc_correct(void);
void init_crc_calculation_on_rx(void);
void update_crc_calculation_on_rx(void);
bool is_rx_request_completed(void);


//-----------------------------------------------------------------------------
// Functions to be phased out: Code changes in I2C and in the drivers needed
// Mail GS + WL 2018-11-29
//-----------------------------------------------------------------------------
void I2cDrvMasterInitDma(void);


//-----------------------------------------------------------------------------
// Functions not needed any longer in the public interface:
// Never called from the library nor the tests
// Mail GS + WL 2018-11-29
// - delete from this file as soon as double-checked with deeper NFC + I2C know-how
// - set the functions to be static (if they can't be deleted)
//-----------------------------------------------------------------------------
void I2cDrvWriteData(uint8_t dev_sel_code, uint16_t address, uint8_t *data, uint8_t length);
void I2cDrvReadData(uint8_t dev_sel_code, uint16_t address, uint8_t *data, uint8_t length);
void I2cDrvStartTransfer(uint8_t dev_select_code, uint16_t length);
void I2cDrvStopTransfer(void);
void I2cDrvSetAcknowledge(i2c_ack_t acknowledge);
uint8_t I2cDrvReadByte(void);
bool I2cDrvIsRxBusy(void);
bool I2cDrvIsTxBusy(void);


/** \} */ // I2cDriverAPI
#endif
