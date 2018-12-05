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
// $Id: i2c.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/I2cExt/api/i2c.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \defgroup I2c
 * \{
 * \brief Hardware abstraction for i2c
 *
 * \defgroup I2cAPI i2c API
 * \brief I2c user interface
 *
 * \file
 * \brief Central functions for i2c messaging.
 *
 * \defgroup I2cConfig I2C Configuration
 * \brief Parameters to configure the I2C module
 *
 * \defgroup I2cDriverAPI I2c Driver API
 * \brief I2c module hardware driver interface
 *
 * \{
 */
     
#ifdef MODULE_I2C

#include "Config.h"
#include "Global.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "i2c_driver.h"
#include "i2c.h"

/*************************************************** Defines **********************************************************/

#define I2C_TIMEOUT_MS                  (10)                                                                            // ms

#if ( I2C_TIMER_PERIOD_US > 10000 )
	#error "I2C timer period must be smaller than 10 ms"
#endif

#define I2C_TIMEOUT                     ( ( (1000*I2C_TIMEOUT_MS) + (I2C_TIMER_PERIOD_US/2) ) / I2C_TIMER_PERIOD_US)    // Timer ticks

/*************************************************** Structs **********************************************************/

typedef struct
{
    uint16_t slaveAddr;
    i2c_master_cmd_t cmd;
    bool stop;
    bool prevStop;
    uint8_t *buf;
    uint32_t bufSize;
    uint32_t txIdx;
    uint32_t rxIdx;
    i2c_tx_state_t txState;
    uint32_t err;
}
i2c_master_sm_t;

/************************************************ Local functions *****************************************************/

static void i2cMasterDataInit(void);
static i2c_code_t* i2cMasterCmd(uint16_t aSlaveAddress, i2c_master_cmd_t aReadWrite, bool aStopFlag, uint8_t *pBuf, uint32_t aBufSize);

/**************************************************** Local data ******************************************************/

static i2c_code_t      i2c_code;
static i2c_master_sm_t i2cMSM;
static uint32_t        i2c_timeout = 0;


/** ********************************************************************************************************************
* \brief Rx Irq
*
* \Warning declared in i2c_driver.h
*
***********************************************************************************************************************/

void i2c_drv_master_rx_irq_callback(void)
{
    i2cMSM.buf[i2cMSM.rxIdx] = i2c_drv_master_get_rx_data();
    i2cMSM.rxIdx++;
}

/** ********************************************************************************************************************
* \brief Tx Irq
*
* \Warning declared in i2c_driver.h
*
***********************************************************************************************************************/

void i2c_drv_master_tx_irq_callback(void)
{
    uint32_t data;

    switch(i2cMSM.txState)
    {
        case I2C_TX_STATE_IDLE:
            break;

        case I2C_TX_STATE_READ:
            if(++i2cMSM.txIdx < i2cMSM.bufSize)
            {
                i2c_drv_master_tx_read(I2C_ACK_YES);
            }
            else
            {
                i2cMSM.txState = (i2cMSM.stop ? I2C_TX_STATE_STOP : I2C_TX_STATE_END);
                i2c_drv_master_tx_read(I2C_ACK_NO);
            }
            break;

        case I2C_TX_STATE_WRITE:
            data = ((uint32_t)i2cMSM.buf[i2cMSM.txIdx]);

            if(++i2cMSM.txIdx == i2cMSM.bufSize)
            {
                i2cMSM.txState = (i2cMSM.stop ? I2C_TX_STATE_STOP : I2C_TX_STATE_END);
            }

            i2c_drv_master_tx_write(data);
            break;

        case I2C_TX_STATE_STOP:
            if(i2cMSM.stop)
            {
                i2c_drv_master_stop();
            }

            i2cMSM.txState = I2C_TX_STATE_END;
            break;

        case I2C_TX_STATE_END:
            if(i2cMSM.cmd == I2C_MASTER_CMD_WRITE)
            {
                i2c_code = I2C_CODE_OK;
            }
            else if(i2cMSM.cmd == I2C_MASTER_CMD_READ)
            {
                if(i2cMSM.rxIdx != i2cMSM.bufSize)
                {
                    i2c_code = I2C_CODE_DATA_ERR;
                }
                else
                {
                    i2c_code = I2C_CODE_OK;
                }
            }

            i2cMSM.txState = I2C_TX_STATE_IDLE;
            break;
    }
}

/** ********************************************************************************************************************
* \brief Err Irq
*
* \Warning declared in i2c_driver.h
*
* \param aErrCode
*        source error code
*
***********************************************************************************************************************/

void i2c_drv_master_err_irq_callback(i2c_code_t aErrCode)
{
    i2c_code = aErrCode;

    if(i2cMSM.txState != I2C_TX_STATE_IDLE)
    {
        i2cMSM.txState = I2C_TX_STATE_IDLE;
    }
}

/** ********************************************************************************************************************
* \brief I2C Get SM State
*
* Returns the current state in which the transmission SM is.
* 
***********************************************************************************************************************/

i2c_tx_state_t i2c_drv_master_get_txstate_callback( void )
{
    return i2cMSM.txState;
}

/** ********************************************************************************************************************
* \brief I2C master module init
*
***********************************************************************************************************************/

static void i2cMasterDataInit(void)
{
	i2c_code = I2C_CODE_OK;

	i2cMSM.slaveAddr = 0;
    i2cMSM.cmd		 = I2C_MASTER_CMD_READ;
    i2cMSM.stop	     = true;
    i2cMSM.prevStop  = true;
	i2cMSM.buf		 = NULL;
    i2cMSM.bufSize   = 0;
    i2cMSM.txIdx     = 0;
    i2cMSM.rxIdx     = 0;
    i2cMSM.err       = 0;
    i2cMSM.txState   = I2C_TX_STATE_IDLE;
}

/** ********************************************************************************************************************
* \brief Cmd manager to send a I2C read/write frames
*
* \param aSlaveAddr
*        slave address
*
* \param aReadWrite
*        read/write command
*
* \param aStop
*        frame with stop
*
* \param pBuf
*        pointer to buffer
*
* \param aBufSize
*        size of the buffer
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

static i2c_code_t* i2cMasterCmd(uint16_t aSlaveAddress, i2c_master_cmd_t aReadWrite, bool aStopFlag, uint8_t *pBuf, uint32_t aBufSize)
{
	bool repStart    = false;

    i2cMSM.slaveAddr = aSlaveAddress;
    i2cMSM.cmd       = aReadWrite;
    i2cMSM.stop      = aStopFlag;
    i2cMSM.buf       = pBuf;
    i2cMSM.bufSize   = aBufSize;

    if(i2cMSM.txState == I2C_TX_STATE_IDLE)
    {
        if(i2c_code != I2C_CODE_OK)               // Last code was not OK
        {
            i2cMSM.prevStop = true;
            i2cMSM.stop     = true;
        }

        if(i2cMSM.prevStop == false)
        {
            repStart = true;
        }

        i2c_timeout = I2C_TIMEOUT;
        i2cMSM.txIdx = 0;
        i2cMSM.rxIdx = 0;
        i2cMSM.prevStop = i2cMSM.stop;

		if(i2cMSM.bufSize)
        {
            i2cMSM.txState = ((i2cMSM.cmd == I2C_MASTER_CMD_WRITE) ? I2C_TX_STATE_WRITE : I2C_TX_STATE_READ);
        }
        else
        {
            i2cMSM.txState = (i2cMSM.stop ? I2C_TX_STATE_STOP : I2C_TX_STATE_END);
        }

        i2c_drv_master_start(i2cMSM.slaveAddr, i2cMSM.cmd, repStart);
    }

    return &i2c_code;
}

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief I2C module init
*
***********************************************************************************************************************/

void I2C_Init(void)
{
    i2c_drv_master_init();

	i2cMasterDataInit();
}

/** ********************************************************************************************************************
* \brief Transmit a I2C write-frame
*
* \param aSlaveAddr
*        slave address
*
* \param aStop
*        frame with stop
*
* \param pTxBuf
*        pointer to data buffer to write
*
* \param aTxBufSize
*        size of the Tx buffer
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t* I2C_MasterTransmit(uint16_t aSlaveAddr, bool aStop, uint8_t *pTxBuf, uint32_t aTxBufSize)
{
	return i2cMasterCmd(aSlaveAddr, I2C_MASTER_CMD_WRITE, aStop, pTxBuf, aTxBufSize);
}

/** ********************************************************************************************************************
* \brief Transmit a I2C read-frame
*
* \param aSlaveAddr
*        slave address
*
* \param aStop
*        frame with stop
*
* \param pRxBuf
*        pointer to buffer where data is copied
*
* \param aRxBufSize
*        size of the Tx buffer
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t* I2C_MasterReceive(uint16_t aSlaveAddr, bool aStop, uint8_t *pRxBuf, uint32_t aRxBufSize)
{
	return i2cMasterCmd(aSlaveAddr, I2C_MASTER_CMD_READ, aStop, pRxBuf, aRxBufSize);
}

/** ********************************************************************************************************************
* \brief Checks whether bus is busy
*
* \retval TRUE if bus is busy
*
***********************************************************************************************************************/

bool I2C_IsBusy(void)
{
	bool isBusy;

	if(i2cMSM.txState == I2C_TX_STATE_IDLE)
	{
		isBusy = false;
	}
    else
    {
        isBusy = true;
    }

    return isBusy;
}

/** ********************************************************************************************************************
* \brief Gets i2c code
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t I2C_GetCode(void)
{
    return i2c_code;
}

/** ********************************************************************************************************************
* \brief Gets number of bytes read/written
*
* \retval number of bytes
*
***********************************************************************************************************************/

uint32_t I2C_GetDataCount(void)
{
    uint32_t dataCnt = 0;

    if(i2cMSM.cmd == I2C_MASTER_CMD_READ)
    {
        dataCnt = i2cMSM.rxIdx;
    }
    else if(i2cMSM.cmd == I2C_MASTER_CMD_WRITE)
    {
        dataCnt = i2cMSM.txIdx;
    }

    return dataCnt;
}

/** ********************************************************************************************************************
* \brief I2C timer
*
* \warning Timer period must be < 10ms. To be configured (see ConfgI2C.h)
*
***********************************************************************************************************************/

void I2C_Timer(void)
{
    if(i2c_timeout)
    {
        i2c_timeout--;
    }
    else
    {
        if(i2cMSM.txState != I2C_TX_STATE_IDLE)
        {
            i2cMSM.err++;
            i2c_drv_master_init();                          // In case of time out driver re-initialized
            i2c_code = I2C_CODE_TIMEOUT;
            i2cMSM.txState = I2C_TX_STATE_IDLE;
        }
    }
}

/**********************************************************************************************************************/
#endif /* MODULE_I2C */

/** \} */ // I2cAPI
/** \} */ // I2c