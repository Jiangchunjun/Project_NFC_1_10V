/** ----------------------------------------------------------------------------
// i2c_module.c
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
// $Author: g.salvador $
// $Revision: 16925 $
// $Date: 2019-02-13 18:45:45 +0800 (Wed, 13 Feb 2019) $
// $Id: i2c_module.c 16925 2019-02-13 10:45:45Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_module.c $
//
// Main I2c module state machine with all related functions

* \addtogroup I2cModule
* \{
*   \file
*   \brief Master file for I2c module
* \}
*/

#define MODULE_I2C

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "Global.h"
// ToDo: check if this include is still needed
#include "Config.h"
// ToDo: check if this include is still needed
#include "Crc16.h"

#include "i2c_driver.h"
#include "i2c_config.h"
#include "i2c_userinterface.h"
#include "i2c_local.h"

#if (! defined(UNIT_TESTING)) & (! defined (ECG_SIMULATOR) )
    #if defined (__SAMD21E17A__) || defined (__SAMD21E16L__)
        #include "i2c_parameters_SAMD21.h"
    #elif XMC1301_T038x0064
        #include "i2c_parameters_XMC1300.h"
    #else
        #error("No I2C driver available for this device.")
    #endif
#endif

#include "ToolboxServices.h"



//-----------------------------------------------------------------------------
// local variables
//-----------------------------------------------------------------------------
i2c_local_state_t        i2c_local_state;

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

i2c_local_state_t *i2c_local_struct;

i2c_feedback_t    i2c_feedback_struct;



//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 * \brief Load the next tx_fifo data into the tx_request structure taking care
 * of the not consecutive addresses, the addresses alignment, and the maximum
 * number of byte that can be written in both register and data writing modes
 */
STATIC void get_next_tx_transfer(void)
{
    uint8_t data_idx;
    uint16_t fifo_idx;
    uint16_t previous_addr;
    uint8_t max_page_write_length;
    bool transfer_complete;

    transfer_complete = false;
    fifo_idx = i2c_local_state.tx_fifo.output_idx;
    data_idx = 0;

    // set TX Request address
    i2c_local_state.tx_request.addr =
       i2c_local_state.tx_fifo.entries[i2c_local_state.tx_fifo.output_idx].addr;

    // set max page write length, due to TAG roll-over when
    // start address is not aligned
    max_page_write_length = M24LRxx_PAGE_WRITE_LENGTH
            - (0x03 & i2c_local_state.tx_request.addr);

    // initialize previous address required for validating consecutive addresses
    // to fulfill condition for the first time
    previous_addr = i2c_local_state.tx_request.addr - 1;

    while ((i2c_local_state.tx_fifo.entry_cnt > 0)
            && (false == transfer_complete))
    {
        // validate consecutive addresses
        if ((uint16_t)(previous_addr + 1)
                != i2c_local_state.tx_fifo.entries[fifo_idx].addr)
        {
            // required to split transfer due to not consecutive addresses
            transfer_complete = true;
        }
        // validate transfer length either less than TAG page write length
        // or TAG register access and less maximal write length
        else if ((data_idx < max_page_write_length)
                || ((i2c_local_state.tx_fifo.entries[fifo_idx].addr & 0x8000)
                        && (data_idx < M24LRxx_MAX_WRITE_LENGTH)))
        {
            transfer_complete = false;

            // copy data from TX FIFO into TX Request Data Buffer
            i2c_local_state.tx_request.data[data_idx] =
                    i2c_local_state.tx_fifo.entries[fifo_idx].data;

            // update previous address
            previous_addr = i2c_local_state.tx_fifo.entries[fifo_idx].addr;

            // increment TX Request Data Buffer Index
            data_idx++;

            // decrement TX FIFO Entries Counter
            i2c_local_state.tx_fifo.entry_cnt--;

            // increment TX FIFO Index
            fifo_idx = (fifo_idx < (I2C_TX_FIFO_LENGTH - 1)) ?
                    (fifo_idx + 1) : 0;
        }
        else
        {
            transfer_complete = true;
        }

    }

    // set TX Request data length
    i2c_local_state.tx_request.length = data_idx;

    // update TX FIFO output index
    i2c_local_state.tx_fifo.output_idx = fifo_idx;
}

////------------------------------------------------------------------------------
///**
// * \brief Set the number of cycles to wait after a succesfull TAG writing and
// * before the next writing attempt.
// * \note There is a specific state into the i2c module state machine called
// * i2c_fsm_wait_timer which porpouse is to wait a configurable number of cycles
// * to let the TAG write the sent data into its internal memory.
// * \param timeout: Usually the TAG takes around 4ms to store the sent data.
// * timeout has to be properly set depending on the frequency of the I2cCyclic()
// * calls.
// */
//STATIC void set_wait_counter_timeout(void)
//{
//    I2cSetCallTimings(run_time);
//}

//------------------------------------------------------------------------------
/**
 * \brief Reset the number of succesfull writings performed on the TAG
  */
STATIC void reset_writing_done_counter(void)
{
  i2c_local_state.writing_done_counter = 0;
}


bool BookWriteVerificationRead(void)
{
  // reset the write_check flag
  i2c_local_state.tx_fifo.write_check[0] = 0;

  i2c_local_state.is_write_verification_activated = true;

  // Instantiate the register read
  return I2cReadRegister(0x920,i2c_local_state.tx_fifo.write_check,1);
}

void DropWriteVerificationRead(void)
{
  i2c_local_state.rx_request.is_filled = false;
  i2c_local_state.is_write_verification_activated = false;
}

bool CheckWriteVerificationFlag(void)
{
  return (i2c_local_state.tx_fifo.write_check[0] & 0x80);
}

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * \brief Set the number of i2c cycles to wait after a succesfull TAG writing
 * and before the next writing attempt. This number is calculated upon the given
 * period of calls (of I2cCyclic) which is expected from the application,
 * and the fixed M24LRxx_PAGE_WRITE_WAIT_TIME_us
 * \note There is a specific state into the i2c module state machine called
 * i2c_fsm_wait_timer which porpouse is to wait a configurable number of cycles
 * to let the TAG write the sent data into its internal memory.
 * \param timeout: Usually the TAG takes around 4ms to store the sent data.
 * timeout has to be properly set depending on the frequency of the I2cCyclic()
 * calls.
 */
void I2cSetCallTimings(uint32_t write_wait_time_ticks)
{
    i2c_local_state.wait_cnt_timeout = write_wait_time_ticks;
}

//-----------------------------------------------------------------------------
/**
 * \brief I2C module initialization function
 * \note Has to be called after the ballast power-up
 */
void I2cInit(void)
{
    // switch on the TAG
    NfcTagPowerOn();

    // initialize feedback structure
    i2c_feedback_struct.state = i2c_state_idle;
    i2c_feedback_struct.is_crc_correct = true;
    i2c_feedback_struct.crc_error_event_cnt = 0;
    i2c_feedback_struct.status_no_response_event_cnt = 0;

    // initialize I2C FSM state
    i2c_local_state.fsm_state = i2c_fsm_idle;

    // initialize wait counter and its timeout
    i2c_local_state.wait_cnt = 0;

    // initialize the wait counter timeout @ the RUNTIME ticks for the wait
    // cycles after a write operation
    I2cSetCallTimings(M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_RUNTIME);

   // set_wait_counter_timeout(I2C_RUNTIME_TIMER_PERIOD_US);
   // set_wait_counter_timeout(M24LRxx_PAGE_WRITE_WAIT_TIME);
   // set_check_counter_timeout(M24LRxx_WRITE_CHECK_WAIT_TIME);

    reset_writing_done_counter();

    // initialize debug and error counter
    i2c_local_state.status_tx_fifo_full_event_cnt = 0;
    i2c_local_state.status_nack_event_cnt = 0;
    i2c_local_state.status_error_event_cnt = 0;
    i2c_local_state.wait_for_transfer_done_cnt = 0;

    // initialize TX FIFO parameter
    i2c_local_state.tx_fifo.entry_cnt = 0;
    i2c_local_state.tx_fifo.input_idx = 0;
    i2c_local_state.tx_fifo.output_idx = 0;

    // initialize RX request parameter
    i2c_local_state.rx_request.is_filled = false;
    // bytes_received_cnt: only for XMC 1300
    i2c_local_state.rx_request.bytes_received_cnt = 0;

    // assign local struct pointers to i2c feedback struct variables for debug
    i2c_feedback_struct.is_rx_request_filled =
            &(i2c_local_state.rx_request.is_filled);
    i2c_feedback_struct.status_tx_fifo_full_event_cnt =
            &i2c_local_state.status_tx_fifo_full_event_cnt;
    i2c_feedback_struct.status_nack_event_cnt =
            &i2c_local_state.status_nack_event_cnt;
    i2c_feedback_struct.status_error_event_cnt =
            &i2c_local_state.status_error_event_cnt;
    i2c_feedback_struct.is_rx_request_filled =
            &(i2c_local_state.rx_request.is_filled);
    i2c_feedback_struct.status_no_response_event_cnt =
            &i2c_local_state.status_no_response_event_cnt;

    i2c_local_struct = &i2c_local_state;

    // initialize I2C HW driver counter
    i2c_local_state.hw_drv_init_nr = 0;

    // initialize the flag for the write verification reading
    i2c_local_state.is_write_verification_activated = false;

    // initialize I2C driver
    I2cDrvInit();

    // configure and enable DMA for I2C
    I2cDrvMasterInitDma();
}

//-----------------------------------------------------------------------------
/**
 * \brief Delete all the pending transfers, both RX and TX
 * \note This is regardless and asyncronous with the i2c state machine: calling
 * this function may end in corrupted data from the TAG point of view
 */
void I2cDiscardAllTransfers(void)
{
    // remove all FIFO entries
    i2c_local_state.tx_fifo.entry_cnt = 0;
    i2c_local_state.tx_fifo.input_idx = 0;
    i2c_local_state.tx_fifo.output_idx = 0;

    // remove I2C Read entry
    i2c_local_state.rx_request.is_filled = false;
    // bytes_received_cnt: only for XMC 1300
    i2c_local_state.rx_request.bytes_received_cnt = 0;
/*
    // during power down we're calling more frequently the I2cCyclic() function
    // than we need to increase the wait time between a succesfull writing and
    // the first attempt for the next
    set_wait_counter_timeout(WRITE_WAIT_TIME_AT_POWERDOWN);
    set_check_counter_timeout(WRITE_CHECK_WAIT_TIME_AT_POWERDOWN);
*/
}

// Deprecated ------------------------------------------------------------------
/**
 * \brief -!!DEPRECATED!!- Flush as fast as possible ALL the entities of the tx
 * fifo queue on the bus.
 * \note -!!DEPRECATED!!- This function is called during the NfcOnPowerDown()
 * procedure in which we call much more frequently the I2cCyclic().
 * Because of this we need to increase the wait time between on writing and the
 * next in order to give the time to the TAG to store the data.
 * At the end of the while loop the normal wait time is restored.
 * This while loop is blocking and the time needed to flush all the data can be
 * longer than the watchdog expiration period. For this reason this function is
 * DEPRECATED and I2cFlush() has to be used instead.
 */
/*
void I2cFlushAll(void)
{
    // during power down we're calling more frequently the I2cCyclic() function
    // then we need to increase the wait time between a succesfull writing and
    // the first attempt for the next
    set_wait_counter_timeout(WRITE_WAIT_TIME_AT_POWERDOWN);
    set_check_counter_timeout(WRITE_CHECK_WAIT_TIME_AT_POWERDOWN);

    // wait until FIFO is empty and FSM is in idle (pending transfers are done)
    while (!((0 == i2c_local_state.tx_fifo.entry_cnt)
            && (i2c_fsm_idle == i2c_local_state.fsm_state)))
    {
        I2cCyclic();
    }

    // set back the regular wait time after the while loop just for safe reasons
    // (e.g.: if you're not powering down really but entering into the sleep
    // state you want back the regular waiting time when you wake-up
    set_wait_counter_timeout(M24LRxx_PAGE_WRITE_WAIT_TIME);
    set_check_counter_timeout(M24LRxx_WRITE_CHECK_WAIT_TIME);
}
*/
//------------------------------------------------------------------------------
/**
 * \brief Flush as fast as possible MAX_TAG_WRITINGS_DURING_FLUSH entities of
 * the tx_fifo queue on the bus.
 * \note This function is called during the NfcOnPowerDown() procedure in which
 * we call much more frequently the I2cCyclic().
 * Because of this we need to increase the wait time between one writing and the
 * next in order to give the time to the TAG to store the data.
 * This is done with the I2cSetCallTimings() which takes as parameter the period
 * of I2cCyclic calls set by the application.
 * Therefore, depending by the application need, before and after the I2cFlush()
 * call, the I2cSetCallTimings() call may be required.
 */
void I2cFlush(void)
{
    // reset the counter for the flush writings
    reset_writing_done_counter();

    // wait until FIFO is empty and FSM is in idle (pending transfers are done).
    // Here we also have to take care of the case where the data cannot be
    // written because of the faulty/missing TAG.
    // This should be the ONLY blocking point, even if the FIFO was previously
    // full because at the beginning of the NfcOnPowerDown() function all the
    // ongoing transactions are discarded.
    // The only chance to have the NfcOnPowerDown blocking BEFORE this point is
    // in the while loop in which we perform the update_from_mcu_to_tag.
    // The TAG copy could be completely misaligned (full of 0xFF) which means
    // we'll fill the fifo during the update process.
    // This will lead to another blocking point!
    while((i2c_local_state.writing_done_counter < MAX_TAG_WRITINGS_DURING_FLUSH)
           &&(!I2cAreAllPendingTransfersDone()))
    {
        I2cCyclic();
        pwrDownSleep();
    }
/*
    // set back the regular wait time after the while loop just for safe reasons
    // (e.g.: if you're not powering down really but entering into the sleep
    // state you want back the regular waiting time when you wake-up
    set_wait_counter_timeout(M24LRxx_PAGE_WRITE_WAIT_TIME);
    set_check_counter_timeout(M24LRxx_WRITE_CHECK_WAIT_TIME);
*/
}


//-----------------------------------------------------------------------------
/**
 * \brief I2c Finite State Machine handler
 * \note This function has to be called at the end of every NfcCyclic() call.
 *       It is also used inside the I2c writing flush loops, such as I2cFlush()
 *       and I2cFlushAll() (now deprecated)
 */
void I2cCyclic(void)
{
    uint16_t fifo_idx;
    uint8_t i;

    switch (i2c_local_state.fsm_state)
    {
        case i2c_fsm_idle:
            // set feedback state to IDLE
            i2c_feedback_struct.state = i2c_state_idle;

            // check for Read request
            if(true == i2c_local_state.rx_request.is_filled)
            {
                // set feedback state to busy_rx
                i2c_feedback_struct.state = i2c_state_busy_rx;

                // set active transaction direction
                i2c_local_state.transfer_direction = i2c_transfer_direction_rx;

                // start transfer
                start_transfer();

                // init CRC calculation
                init_crc_calculation_on_rx();

                // change FSM state to wait_transfer_done
                i2c_local_state.fsm_state = i2c_fsm_wait_transfer_done;
            }
            // check for Write Request
            else if (i2c_local_state.tx_fifo.entry_cnt > 0)
            {
                // fill active_transfer
                get_next_tx_transfer();

                // set feedback state to busy_tx
                i2c_feedback_struct.state = i2c_state_busy_tx;

                // set active transaction direction
                i2c_local_state.transfer_direction = i2c_transfer_direction_tx;

                // change FSM state to wait for Transfer Done
                i2c_local_state.fsm_state = i2c_fsm_wait_transfer_done;

                // start Transfer
                start_transfer();

                // reset the verification reading counter
                i2c_local_state.verification_reading_counter = 0;

                // Here there is no reading to be done into the queue, so we can
                // safely set the verification reading "artificially"
                BookWriteVerificationRead();
            }

        break;

        case i2c_fsm_wait_transfer_done:

            // trigger update of i2c_driver_feedback_struct.status
            // required only by XMC 1300
            I2cDrvUpdateStatus();

            if (I2C_DRIVER_STATUS_ERROR == i2c_driver_feedback_struct.status)
            {
                // reset the no answer counter
                i2c_local_state.wait_for_transfer_done_cnt = 0;

                // Here we have to drop the write verification read if there is
                // the i2c_transfer_direction_tx set, by resetting the
                // rx_request.is_filled flag
                if(i2c_local_state.is_write_verification_activated == true)
                {
                  DropWriteVerificationRead();
                }

                // increment error counter
                if(++i2c_local_state.status_error_event_cnt >= I2C_STATUS_ERROR_EVENTS_MAX)
                {
                    // set feedback state to Error
                    i2c_feedback_struct.state = i2c_state_error;

                    // change FSM state to Error
                    i2c_local_state.fsm_state = i2c_fsm_error;
                }
                else
                {
                    // change FSM state to Idle
                    i2c_local_state.fsm_state = i2c_fsm_idle;

                    // QUESTION: why there is also this start_transfer call
                    // again if we're going back to the idle state?

                    // start transfer
                    start_transfer ();
                }
            }
            else if (I2C_DRIVER_STATUS_NACK
                    == i2c_driver_feedback_struct.status)
            {
                // reset the no answer counter
                i2c_local_state.wait_for_transfer_done_cnt = 0;

                // increment nack counter and check if it has exceeded the limit
                if (++i2c_local_state.status_nack_event_cnt
                        >= M24LRxx_MAX_NACK_COUNTS)
                {
                    // clear the nack counter
                    //i2c_local_state.status_nack_event_cnt = 0;

                    // set feedback state to Error
                    i2c_feedback_struct.state = i2c_state_error;

                    // change FSM state to Error
                    i2c_local_state.fsm_state = i2c_fsm_error;
                }
                else
                {
                    // start transfer
                    start_transfer();
                }
            }
            else if(I2C_DRIVER_STATUS_DONE_RX == i2c_driver_feedback_struct.status)
            {
              // reset the no answer counter
              i2c_local_state.wait_for_transfer_done_cnt = 0;

              // update crc calculation and received byte counters
              update_crc_calculation_on_rx();

              // if all portions of the RX request were received
              if (is_rx_request_completed())
              {
                  // clear NACK error counter
                  i2c_local_state.status_nack_event_cnt = 0;

                    if (i2c_local_state.is_write_verification_activated == true)
                    {
                        // change FSM state to i2c_fsm_wait_timer to check the
                        // T_prog flag
                        i2c_local_state.fsm_state = i2c_fsm_wait_timer;
                    }
                    else
                    {
                        // set RX Request structure to free
                        i2c_local_state.rx_request.is_filled = false;

                        // validate CRC and set CRC status
                        i2c_feedback_struct.is_crc_correct = is_crc_correct();

                        if ((false == i2c_feedback_struct.is_crc_correct)
                                && (i2c_feedback_struct.crc_error_event_cnt
                                        < I2C_CRC_ERRORS_MAX))
                        {
                            i2c_feedback_struct.crc_error_event_cnt++;
                        }

                        // set feedback state to IDLE
                        i2c_feedback_struct.state = i2c_state_idle;

                        // change FSM state to Idle
                        i2c_local_state.fsm_state = i2c_fsm_idle;
                    }

                  // clear bytes_received counter
                  i2c_local_state.rx_request.bytes_received_cnt = 0;
              }
              // if there are portions of the RX request still waiting to be
              // transferred
              else
              {
                // start Transfer
                start_transfer();
              }
            }
            else if (I2C_DRIVER_STATUS_DONE_TX
                    == i2c_driver_feedback_struct.status)
            {
                // reset the no answer counter
                i2c_local_state.wait_for_transfer_done_cnt = 0;

                // clear NACK error counter
                i2c_local_state.status_nack_event_cnt = 0;

                // change FSM state to wait
                i2c_local_state.fsm_state = i2c_fsm_wait_timer;
            }
            // whenever there isn't any of the previous cases we're just waiting
            // for something to happen and we count the number of this waiting
            // cycles
            else if (++i2c_local_state.wait_for_transfer_done_cnt
                    >= WAIT_FOR_TRANSFER_DONE_TIMEOUT)
            {

               // i2c_local_state.wait_for_transfer_done_cnt = 0;

                i2c_local_state.status_no_response_event_cnt++;

                // set feedback state to Error
                i2c_feedback_struct.state = i2c_state_error;

                // change FSM state to Error
                i2c_local_state.fsm_state = i2c_fsm_error;
            }

        break;

        case i2c_fsm_wait_timer:
            if (i2c_local_state.wait_cnt < i2c_local_state.wait_cnt_timeout)
            {
                // increment counter
                i2c_local_state.wait_cnt++;
            }
            // once the timeout for the storage time is expired we have to start
            // the reading (which is already set into the rx_request) in order
            // to verify the T_prog flag into the TAG register
            else
            {
                // Post increment the verification_reading_counter, so only at
                // the first reading we will go through the idle state
                if (i2c_local_state.verification_reading_counter++ == 0)
                {
                    // change FSM state to Idle in order to let the write
                    // verification reading being processed
                    i2c_local_state.fsm_state = i2c_fsm_idle;

                    // no need to reset the waiting counter here, since we're
                    // issuing the verification reading, not another writing!
                    //i2c_local_state.wait_cnt = 0;
                }
                // If we have issued at least one verification reading
                else if (i2c_local_state.verification_reading_counter
                        <= M24LRxx_WRITE_CHECK_MAX_READINGS)
                {
                    if (CheckWriteVerificationFlag() == true)
                    {
                        // reset wait counter
                        i2c_local_state.wait_cnt = 0;

                        // increment the succesfull writing counter
                        i2c_local_state.writing_done_counter++;

                        // change FSM state to Idle
                        i2c_local_state.fsm_state = i2c_fsm_idle;

                        // The check verification is properly done: so we have
                        // to free the rx_request fields in order to let the
                        // other readings being placed
                        DropWriteVerificationRead();
                    }
                    // reset the wait counter and issue again the same reading
                    // coming back to the wait for Transfer Done state.
                    else
                    {
                        // no need to reset the waiting counter here, since
                        // we're issuing the verification reading, not another
                        // writing!
                        //i2c_local_state.wait_cnt = 0;

                        // change FSM state to wait for Transfer Done
                        i2c_local_state.fsm_state = i2c_fsm_wait_transfer_done;

                        // start Transfer
                        start_transfer();
                    }
                }
                // At this point we declare the write verification failed, which
                // means the writing is corrupted and we have to retrigger it
                // again!
                else
                {
                    // reset wait counter
                    i2c_local_state.wait_cnt = 0;

                    // clear the nack counter
                    //i2c_local_state.status_nack_event_cnt = 0;

                    // restore the tx condition
                    i2c_local_state.transfer_direction =
                            i2c_transfer_direction_tx;

                    // set feedback state to Error
                    i2c_feedback_struct.state = i2c_state_error;

                    // change FSM state to Error
                    i2c_local_state.fsm_state = i2c_fsm_error;
                }
            }
        break;

        case i2c_fsm_error:
            // handles
            // - NACKs
            // - I2C HW module error
            // - missing I2C communication
            // if the module doesn't recover from the error event during runtime
            // it will stay in error state until power down: power down triggers
            // a new init to save data from MCU to tag
            if (i2c_local_state.hw_drv_init_nr >= I2C_HW_DRV_INIT_MAX)
            {
                  // I2c driver stop
                  I2cDrvStop();
                  i2c_local_state.fsm_state = i2c_fsm_stop;
            }
            else
            {
                // the data we wanted to transfer didn't succeed, so we have to
                // restore the proper indexes for the fifo outputs
                // (only IF we was issuing a WRITING PROCESS)

                if(i2c_local_state.transfer_direction == i2c_transfer_direction_tx)
                {
                  fifo_idx = i2c_local_state.tx_fifo.output_idx;

                  for(i=0; i< i2c_local_state.tx_request.length; i++)
                  {
                    // increment back TX FIFO entries counter
                    i2c_local_state.tx_fifo.entry_cnt++;

                    // decrement back TX FIFO index
                    fifo_idx = (fifo_idx >0)?
                                (fifo_idx-1):(I2C_TX_FIFO_LENGTH-1);
                  }

                  i2c_local_state.tx_fifo.output_idx = fifo_idx;

                  // Here we have to drop the write verification read if there is
                  // the i2c_transfer_direction_tx set, by resetting the
                  // rx_request.is_filled flag
                  if(i2c_local_state.is_write_verification_activated == true)
                  {
                    DropWriteVerificationRead();
                  }
                }
                else
                {
                  // if we came here during a read procedure we have nothing to do
                  // because the rx_request.is_filled flag is still TRUE:
                  // Switching to the IDLE state will trigger again the same
                  // reading regardless if it was a normal reading or a verification
                  // reading. In this second case the is_write_verification_activated
                  // flag is not cleared, so if the next reading will be succesfull
                  // we will fall correctly into the wait_state in order to finally
                  // check if the previous writing was succesfull or not.
                }

                if((i2c_local_state.status_error_event_cnt >= I2C_STATUS_ERROR_EVENTS_MAX) ||
                   (i2c_local_state.status_nack_event_cnt >=M24LRxx_MAX_NACK_COUNTS) ||
                   (i2c_local_state.wait_for_transfer_done_cnt >= WAIT_FOR_TRANSFER_DONE_TIMEOUT))
                {
                    i2c_local_state.wait_for_transfer_done_cnt = 0;
                    i2c_local_state.status_nack_event_cnt = 0;
                    i2c_local_state.status_error_event_cnt = 0;

                    // increment driver init attempts
                    i2c_local_state.hw_drv_init_nr++;
                }

                // I2c driver stop
                I2cDrvStop();
                // I2c driver reinitialization
                I2cDrvInit();

                // change FSM state to Idle
                i2c_local_state.fsm_state = i2c_fsm_idle;
            }
            break;

        case i2c_fsm_stop:
            // stay here until power down function is called!
            // only set I2C feedback state
            i2c_feedback_struct.state = i2c_state_stop;
            break;


        default:                                        // LCOV_EXCL_LINE: Not reachable, defensive programming only
            // default change FSM state to IDLE
            i2c_local_state.fsm_state = i2c_fsm_idle;   // LCOV_EXCL_LINE: Not reachable, defensive programming only
        break;                                          // LCOV_EXCL_LINE: Not reachable, defensive programming only
    }

}

//------------------------------------------------------------------------------
/**
 * \brief Check if all the transfers still present on the tx_fifo have been flushed out
 * \return true (if FIFO is empty AND fsm is in idle (pending transfers are done))
 * OR (if we're into the Error state for whatever reasons)
 */
bool I2cAreAllPendingTransfersDone(void)
{
  return  (((0 == i2c_local_state.tx_fifo.entry_cnt)
            && (0 == i2c_local_state.rx_request.is_filled)
            && (i2c_fsm_idle == i2c_local_state.fsm_state))
          ||(i2c_feedback_struct.state == i2c_state_error)
          ||(i2c_feedback_struct.state == i2c_state_stop));
}

//-----------------------------------------------------------------------------
/**
 * \brief Add a new I2C read request. Only 1 read request can be handled,
 * thus the return value is false when a pending read access exists.
 * \note length is incremented by I2C_CRC_BYTE_WIDTH bytes (CRC-16)
 * It is always expected that the CRC is appended to the data payload
 * \param addr: start 16 bit TAG address
 * \param data: pointer to tag data
 * \param length: number of bytes for data transfer (without CRC)
 * \return false if RX read request cannot be processed, due to pending previous
 * read access
 */
bool I2cRead(uint16_t addr, uint8_t *data, uint8_t length)
{
    if (true == i2c_local_state.rx_request.is_filled)
    {
        // RX read request cannot be processed
        return false;
    }

    // fill RX request structure
    i2c_local_state.rx_request.addr = addr;
    i2c_local_state.rx_request.data_ptr = data;
    i2c_local_state.rx_request.length = length + I2C_CRC_BYTE_WIDTH;

    // set RX request structure status to filled
    i2c_local_state.rx_request.is_filled = true;

    // clear CRC failure flag
    i2c_feedback_struct.is_crc_correct = true;

    return true;
}

//-----------------------------------------------------------------------------
/**
 * \brief Add new I2C write request.
 * \note This function fills the new write request to a local FiFo. When the
 * FiFo is full, this function return false. This function is called from the
 * NFC module
 * \param addr      start 16 bit TAG address
 * \param data      byte pointer to data
 * \param length    number of bytes for data transfer (without CRC)
 * \return          false if local FiFo is full and no more write requests can
 *                  be handled.
 */
bool I2cWrite(uint16_t addr, uint8_t *data, uint8_t length)
{
    uint32_t i1;
    uint8_t* data_ptr;

    // return false when FIFO is full
    if ((I2C_TX_FIFO_LENGTH - i2c_local_state.tx_fifo.entry_cnt) < length)
    {
        // track TX FIFO full event
        i2c_local_state.status_tx_fifo_full_event_cnt++;
        return false;
    }

    // else add entry ....

    data_ptr = data;

    for (i1 = 0; i1 < length; i1++)
    {
        i2c_local_state.tx_fifo.entries[i2c_local_state.tx_fifo.input_idx].addr =
                addr + i1;
        i2c_local_state.tx_fifo.entries[i2c_local_state.tx_fifo.input_idx].data =
                *(data_ptr++);

        // post increment TX FIFO input index or wrap around (=0)
        i2c_local_state.tx_fifo.input_idx =
                (i2c_local_state.tx_fifo.input_idx < (I2C_TX_FIFO_LENGTH - 1)) ?
                        (i2c_local_state.tx_fifo.input_idx + 1) : 0;
    }

    // increment entries counter by length
    i2c_local_state.tx_fifo.entry_cnt += length;

    // return true when new entries add is done successful
    return true;
}

//-----------------------------------------------------------------------------
/**
 *  \brief I2C write register request.
 *  \note  '1' is added at bit position 14 of the address to signalise a
 * (regular) register write command with max page length = 4 bytes
 * \param addr: start 16 bit TAG address
 * \param data: pointer to data array
 * \param length: number of bytes for data transfer
 * \return false if FIFO is full, true if data was sent
 */
bool I2cWriteRegister(uint16_t addr, uint8_t *data, uint8_t length)
{
    addr |= 0x4000;
    return I2cWrite(addr, data, length);
}

//-----------------------------------------------------------------------------
/**
 * I2C read register.
 * \note  '1' is added at bit position 14 of the address to signalise a
 * (regular) register read command with max page length = 4 bytes
 * \param addr: start 16 bit TAG address
 * \param data: pointer to data array
 * \param length: number of bytes for data transfer
 * \return false if FIFO is full, true if data was sent
 */
bool I2cReadRegister(uint16_t addr, uint8_t *data, uint8_t length)
{
    addr |= 0x4000;
    return I2cRead(addr, data, length);
}

//-----------------------------------------------------------------------------
/**
 * \brief Present I2C password
 * \note  '1' is added at bit position 14 of the address to signalise a
 * (regular) register read command with max page length = 4 bytes
 * \param password:  the new password
 * \return false if FIFO is full, true if data was sent
 */
bool I2cPresentI2cPwd(uint32_t password)
{
    uint16_t addr = 0x8000 | M24LR_I2C_PASSWORD_BYTE_ADDR;
    uint8_t pwd_array[M24LRxx_MAX_WRITE_LENGTH];

    pwd_array[0] = (uint8_t) (password >> 24);
    pwd_array[1] = (uint8_t) (password >> 16);
    pwd_array[2] = (uint8_t) (password >> 8);
    pwd_array[3] = (uint8_t) (password & 0xFF);
    pwd_array[4] = 0x09;
    pwd_array[5] = (uint8_t) (password >> 24);
    pwd_array[6] = (uint8_t) (password >> 16);
    pwd_array[7] = (uint8_t) (password >> 8);
    pwd_array[8] = (uint8_t) (password & 0xFF);

    return I2cWrite(addr, pwd_array, 9);
}

//-----------------------------------------------------------------------------
/**
 * \brief Write new I2C password
 * \note The function is needed for change of write access rights of all the
 * memory sectors. '1' is added at bit position 14 of the address to signalise a
 * (regular) register read command with max page length = 4 bytes
 * \param password: new password
 * \return false if FIFO is full, true if data was sent
 */
bool I2cWriteNewI2cPwd(uint32_t password)
{
    uint16_t addr = 0x8000 | M24LR_I2C_PASSWORD_BYTE_ADDR;
    uint8_t pwd_array[M24LRxx_MAX_WRITE_LENGTH];

    pwd_array[0] = (uint8_t) (password >> 24);
    pwd_array[1] = (uint8_t) (password >> 16);
    pwd_array[2] = (uint8_t) (password >> 8);
    pwd_array[3] = (uint8_t) (password & 0xFF);
    pwd_array[4] = 0x07;
    pwd_array[5] = (uint8_t) (password >> 24);
    pwd_array[6] = (uint8_t) (password >> 16);
    pwd_array[7] = (uint8_t) (password >> 8);
    pwd_array[8] = (uint8_t) (password & 0xFF);

    return I2cWrite(addr, pwd_array, 9);
}

//-----------------------------------------------------------------------------
/**
 * \brief Set protection (SSS bytes) for all EEPROM sectors in NFC tag
 * \note Add '1' at bit position 14 of the TAG address to signalise a register
 *  write command
 * \return false if FIFO is full, true if data was sent
 */
bool I2cInitSectorProtection(void)
{
    uint16_t addr = 0x4000 | M24LR_SSS_BYTE_AREA_START_ADDR;

    return I2cWrite(addr, I2cSectorProtection, M24LR_SECTOR_COUNT);
}

//-----------------------------------------------------------------------------
/**
 * \brief Get the value of the Error event counter
 * \note This counter is incremented every time the interrupt handler is called
 * because of errors occurred on the i2c bus.
 * \retval  I2C driver status error counter value
 */
uint16_t I2cGetDriverStatusErrorEventCnt(void)
{
    return  *i2c_feedback_struct.status_error_event_cnt;
}

//-----------------------------------------------------------------------------
/**
 * \brief Get the value of the Nack event counter
 * \note This counter is incremented every time there is a NACK on the bus
 * \retval I2C NACK event counter value
 */
uint16_t I2cGetNackEventCnt(void)
{
    return *i2c_feedback_struct.status_nack_event_cnt;
}

//-----------------------------------------------------------------------------
/**
 * Get the value of the fifo full event counter
 * \note This counter is incremented when we want to write new data in the bus
 * and the TX FIFO is full
 * \retval I2C FIFO full event counter value
 */
uint16_t I2cGetFifoFullEventCnt(void)
{
    return *i2c_feedback_struct.status_tx_fifo_full_event_cnt;
}

//-----------------------------------------------------------------------------
/**
 * \brief Get the CRC error event counter
 * \note This counter is incremented when there is a CRC error on receviced
 * data on the bus
 * \retval CRC error event counter value
 */
uint16_t I2cGetCrcErrorEventCnt(void)
{
    return i2c_feedback_struct.crc_error_event_cnt;
}

//-----------------------------------------------------------------------------
/**
 * \brief Get the No Response Event counter
 * \note This counter is incremented when we have issued a TX or RX request on
 * the bus but there is no answer whithin 200 I2cCyclic loops
 * \retval  No response event counter value
 */
uint16_t I2cGetNoResponseEventCnt(void)
{
    return *i2c_feedback_struct.status_no_response_event_cnt;
}

//-----------------------------------------------------------------------------
/**
 * \brief Reset all error counters
 */
void I2cResetErrorCounters(void)
{
    *i2c_feedback_struct.status_tx_fifo_full_event_cnt = 0;
    *i2c_feedback_struct.status_error_event_cnt = 0;
    *i2c_feedback_struct.status_nack_event_cnt = 0;
    i2c_feedback_struct.crc_error_event_cnt = 0;
    i2c_feedback_struct.status_no_response_event_cnt = 0;

}
