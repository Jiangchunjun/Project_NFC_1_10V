#ifndef __I2C_LOCAL_H__
#define __I2C_LOCAL_H__

/** ----------------------------------------------------------------------------
// i2c_local.h
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
// $Revision: 16420 $
// $Date: 2019-01-10 21:05:27 +0800 (Thu, 10 Jan 2019) $
// $Id: i2c_local.h 16420 2019-01-10 13:05:27Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_local.h $
//
// Contains all the structures required by i2c_module.c

* \addtogroup I2cModule
* \{
*   \file
*   \brief Local header file for i2c_module.c
* \}
*/


#include <stdint.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// parameters
//-----------------------------------------------------------------------------


/** number of TX FIFO entries */
#ifdef UNIT_TESTING
    #define I2C_TX_FIFO_LENGTH              (64)
    #define I2C_RUNTIME_TIMER_PERIOD_US     (500) // [us]
    #define I2C_POWERDOWN_TIMER_PERIOD_US   (4)   // [us]
    #define I2C_SLEEPMODE_TIMER_PERIOD_US   (3)   // [us]
#else
    #define I2C_TX_FIFO_LENGTH              128 //128
#endif

/** Byte width of the CRC */
#define I2C_CRC_BYTE_WIDTH                  2

// parameters relative to I2C error events
#define I2C_CRC_ERRORS_MAX                  0xFFFF
#define I2C_STATUS_ERROR_EVENTS_MAX         0xFFFF

// parameters relative to I2C driver re-initialization after bus error
#define I2C_HW_DRV_INIT_MAX                 0x14


//-----------------------------------------------------------------------------
// macros
//-----------------------------------------------------------------------------

//------------------------------------------------------------------------------
// service functions
//------------------------------------------------------------------------------

extern void pwrDownSleep(void);

//-----------------------------------------------------------------------------
// enums
//-----------------------------------------------------------------------------
/** I2C FSM states */
typedef enum
{
    i2c_fsm_idle,              //!< i2c_fsm_idle
    i2c_fsm_wait_transfer_done,//!< i2c_fsm_wait_transfer_done
    i2c_fsm_wait_timer,        //!< i2c_fsm_wait_timer
    i2c_fsm_error,             //!< I2C module error condition
    i2c_fsm_stop            //!< I2C bus w/o communication
} i2c_fsm_states_t;

//-----------------------------------------------------------------------------
/** I2C transfer direction*/
typedef enum
{
    i2c_transfer_direction_rx,
    i2c_transfer_direction_tx
} i2c_transfer_direction_t;


//-----------------------------------------------------------------------------
// structures
//-----------------------------------------------------------------------------
/** TX Request parameters */
typedef struct
{
    uint16_t                    addr;
    uint8_t                     data[M24LRxx_MAX_WRITE_LENGTH];
    uint8_t                     length;
} tx_request_t;

//-----------------------------------------------------------------------------
/** TX FIFO entry parameters */
typedef struct
{
    uint16_t                    addr;
    uint8_t                     data;
} tx_fifo_entry_t;

//-----------------------------------------------------------------------------
/** TX FIFO parameters */
typedef struct
{
    tx_fifo_entry_t             entries[I2C_TX_FIFO_LENGTH];
    uint16_t                    entry_cnt;
    uint16_t                    output_idx;
    uint16_t                    input_idx;
     uint8_t                    write_check[3];
} tx_fifo_t;

//-----------------------------------------------------------------------------
/** i2c rx request parameters */
typedef struct
{
    bool                        is_filled;
    uint16_t                    addr;
    uint8_t                     *data_ptr;
    uint16_t                    length;
    uint8_t                     length_portion;
    uint16_t                    bytes_received_cnt;
} rx_request_t;

//-----------------------------------------------------------------------------
/** structure of complete local I2C module states and variables */
typedef struct
{
    i2c_fsm_states_t            fsm_state;
    i2c_transfer_direction_t    transfer_direction;
    bool                        is_write_verification_activated;
    rx_request_t                rx_request;
    tx_request_t                tx_request;
    tx_fifo_t                   tx_fifo;
    uint8_t                     writing_done_counter;
    uint8_t                     verification_reading_counter;
    uint16_t                    wait_cnt;
    uint16_t                    wait_cnt_timeout;
    uint16_t                    check_cnt_timeout;
    uint16_t                    wait_for_transfer_done_cnt;
    uint16_t                    status_tx_fifo_full_event_cnt;
    uint16_t                    status_nack_event_cnt;
    uint16_t                    status_error_event_cnt;
    uint16_t                    status_no_response_event_cnt;
    uint16_t                    hw_drv_init_nr;
} i2c_local_state_t;





//-----------------------------------------------------------------------------
// packed structures
//-----------------------------------------------------------------------------
// pack all following structures to at most 1 byte and push current alignment
// on stack to restore later. Required to prevent byte gaps introduced by
// the compiler to optimize performance.
// Pack is valid to the end of the compilation unit.
#pragma pack(push)
#pragma pack(1)

//-----------------------------------------------------------------------------
// restore previous alignment from stack
#pragma pack(pop)
//-----------------------------------------------------------------------------
// end of packed structures
//-----------------------------------------------------------------------------

#endif // __I2C_LOCAL_H__
