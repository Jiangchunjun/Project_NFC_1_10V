#ifndef __I2C_USERINTERFACE_H__
#define __I2C_USERINTERFACE_H__

/** ----------------------------------------------------------------------------
// i2c_userinterface.h
// -----------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: Sammy el Baradie,  27/07/2016
//
// $Author: g.salvador $
// $Revision: 16923 $
// $Date: 2019-02-13 18:11:01 +0800 (Wed, 13 Feb 2019) $
// $Id: i2c_userinterface.h 16923 2019-02-13 10:11:01Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_userinterface.h $
//
// I2c module include file required by the user to get the i2c state machine working

* \addtogroup I2cModule
* \{
*
* \file
* \brief User header file for the whole I2c Module
*\}
*/


#include <stdint.h>
#include <stdbool.h>
#include "i2c_driver.h"
#include "Config.h"



//------------------------------------------------------------------------------
// enums
//------------------------------------------------------------------------------
typedef enum
{
    i2c_state_idle,
    i2c_state_busy_rx,
    i2c_state_busy_tx,
    i2c_state_error,
    i2c_state_stop
} i2c_states_t;

//------------------------------------------------------------------------------
// structs
//------------------------------------------------------------------------------
typedef struct
{
    i2c_states_t        state;
    bool                is_crc_correct;
    bool                *is_rx_request_filled;
    uint16_t            crc_error_event_cnt;
    uint16_t            *status_no_response_event_cnt;
    uint16_t            *status_tx_fifo_full_event_cnt;
    uint16_t            *status_nack_event_cnt;
    uint16_t            *status_error_event_cnt;
} i2c_feedback_t;


//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------
extern i2c_feedback_t i2c_feedback_struct;

//------------------------------------------------------------------------------
// global definitions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#if !defined(I2C_RUNTIME_TIMER_PERIOD_US)
  #warning "ATTENTION! A standard default value of I2C Cyclic calling period\
 I2C_RUNTIME_TIMER_PERIOD_US is going to be used! A custom value, specific\
 for each application is recommended and has to be placed inside ConfigI2c.h"
  #define I2C_RUNTIME_TIMER_PERIOD_US 500
#endif
#define M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_RUNTIME \
    ((uint32_t)(( 1.0 * M24LRxx_PAGE_WRITE_WAIT_TIME_us / I2C_RUNTIME_TIMER_PERIOD_US) + 0.99 ))

//------------------------------------------------------------------------------
#if !defined(I2C_SLEEPMODE_TIMER_PERIOD_US)
  #warning "ATTENTION! A standard default value of I2C Cyclic calling period\
 I2C_SLEEPMODE_TIMER_PERIOD_US is going to be used! A custom value, specific\
 for each application is recommended and has to be placed inside ConfigI2c.h"
  #define I2C_SLEEPMODE_TIMER_PERIOD_US 500
#endif
#define M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_SLEEPMODE \
    ((uint32_t)(( 1.0 * M24LRxx_PAGE_WRITE_WAIT_TIME_us / I2C_SLEEPMODE_TIMER_PERIOD_US) + 0.99 ))

//------------------------------------------------------------------------------
#if !defined(I2C_POWERDOWN_TIMER_PERIOD_US)
  #warning "ATTENTION! A standard default value of I2C Cyclic calling period\
 I2C_POWERDOWN_TIMER_PERIOD_US is going to be used! A custom value, specific\
 for each application is recommended and has to be placed inside ConfigI2c.h"
  #define I2C_POWERDOWN_TIMER_PERIOD_US 500
#endif
#define M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_POWERDOWN \
    ((uint32_t)(( 1.0 * M24LRxx_PAGE_WRITE_WAIT_TIME_us / I2C_POWERDOWN_TIMER_PERIOD_US) + 0.99 ))

//------------------------------------------------------------------------------
// global functions
//------------------------------------------------------------------------------
void I2cInit(void);
void I2cDiscardAllTransfers(void);
void I2cFlushAll(void);
void I2cFlush(void);
void I2cCyclic(void);
void I2cWriteDma(uint16_t address, uint8_t *data, uint8_t length);
void I2cReadDma(uint16_t address, uint8_t *data, uint8_t length);
bool I2cWrite(uint16_t addr, uint8_t *data, uint8_t length);
bool I2cRead(uint16_t addr, uint8_t *data, uint8_t length);
uint16_t I2cGetDriverStatusErrorEventCnt(void);
uint16_t I2cGetNackEventCnt(void);
uint16_t I2cGetFifoFullEventCnt(void);
uint16_t I2cGetCrcErrorEventCnt(void);
uint16_t I2cGetNoResponseEventCnt(void);
void I2cResetErrorCounters(void);
bool I2cPresentI2cPwd(uint32_t);
bool I2cWriteNewI2cPwd(uint32_t);
bool I2cWriteRegister(uint16_t, uint8_t *, uint8_t);
bool I2cReadRegister(uint16_t, uint8_t *, uint8_t);
bool I2cInitSectorProtection(void);
bool I2cAreAllPendingTransfersDone(void);
void I2cSetCallTimings(uint32_t write_wait_time_ticks);
#endif // __I2C_USERINTERFACE_H__
