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

// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2014-12, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: M24LRxx.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/M24LRxx/M24LRxx.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \defgroup M24LRxx M24LRxx module
 * \{
 * \brief Module for M24LRxx chip handling
 *
 *
 * \defgroup M24LRxxAPI M24LRxx API
 * \brief Nfc module user interface
 *
 *
 * \defgroup M24LRxxConfig M24LRxx Configuration
 * \brief Parameters to configure the M24LRxx module
 *
 * \{
 */

#ifdef MODULE_M24LRxx

#include <stddef.h>
#include <stdint.h>
#include "i2c_driver.h"
#include "i2c.h"

//#define MODULE_M24LRxx
#include "Config.h"
#include "sw_timer.h"

#include "M24LRxx.h"

/**********************************************************************************************************************/

typedef struct
{
    uint16_t addr;
    uint8_t e2;
    uint16_t byteNum;
    uint8_t *pRxBuf;
    uint8_t buf[6];
    M24LRxx_state_t state;
    M24LRxx_req_t reqStatus;
    uint32_t err;
}
rx_tx_req_t;

/**********************************************************************************************************************/

static rx_tx_req_t m24lr;

/**********************************************************************************************************************/

static i2c_code_t M24LRxx_PollAck(bool aStop);
static i2c_code_t M24LRxx_Read(uint16_t aAddr, uint8_t e2, uint16_t byteNum, uint8_t *pBuf);
static i2c_code_t M24LRxx_Write(uint16_t aAddr, uint8_t e2, uint16_t byteNum, uint32_t aValue);
static bool M24LRxx_isRegNameValid(reg_addr_t aRegName);
static M24LRxx_req_t M24LRxx_UpdateReqStatus(void);

/**********************************************************************************************************************/

static uint16_t i2c_slave_addr;

/** ********************************************************************************************************************
* \brief Polls tag until reply is given
*
*  Suspensive function call
*
* \param aStop
*        true if stop must be transmitted
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

static i2c_code_t M24LRxx_PollAck(bool aStop)
{
    i2c_code_t ret_code;

    do
    {
        I2C_MasterTransmit(i2c_slave_addr, aStop, NULL, 0);
        while(I2C_IsBusy());
        ret_code = I2C_GetCode();
    }
    while(ret_code == I2C_CODE_NACK);

    return ret_code;
}


/** ********************************************************************************************************************
* \brief Reads buffer at specified M24LRxx address
*
*
* \param aAddr
*        starting address of data to read
*
* \param e2
*        M24LRxx e2 bit (0 = user data, 1 = registers)
*
* \param pBuf
*        pointer where data is copied
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

static i2c_code_t M24LRxx_Read(uint16_t aAddr, uint8_t e2, uint16_t byteNum, uint8_t *pBuf)
{
    loc_type_t type;

    type = ((e2 == 0) ? M24LRxx_TYPE_USER_MEM : M24LRxx_TYPE_REG);

    SWT_StartTimer(SWT_ID_NFC_TIMEOUT, M24LR_TIMEOUT);
    while( M24LRxx_ReadReq(aAddr, type, byteNum, pBuf) != M24LRxx_REQ_DONE)
    {
        M24LRxx_Manager();
          
        /* M24LRxx read/write timeout */
        if(SWT_CheckTimer(SWT_ID_NFC_TIMEOUT) == SWT_STATUS_UP)
        {          
            /* Reset I2C */
            I2C_Init();
            m24lr.state=M24LRxx_STATE_IDLE;
            
            /* NFC tag power off */
            MCU_NfcPowerPinReset();
                    
            /* Feed watchdog */
            WDT_Feeds();
            
            /* Waiting for reset */
            SWT_StartTimer(SWT_ID_NFC_TIMEOUT, M24LR_RESET);
            while(SWT_CheckTimer(SWT_ID_NFC_TIMEOUT) != SWT_STATUS_UP);
            SWT_CloseTimer(SWT_ID_NFC_TIMEOUT);
            WDT_Feeds();   
            
            /* NFC tag power off */
            MCU_NfcPowerPinSet();
            
            break;
        }
    }

    return I2C_CODE_OK;
}

/** ********************************************************************************************************************
* \brief Writes a buffer at specified M24LRxx address.
*
* \param aAddr
*        write address
*
* \param e2
*        M24LRxx e2 bit (0 = user data, 1 = registers)
*
* \param aByteNum
*        number of bytes to write
*
* \param aValue
*        value to write
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

static i2c_code_t M24LRxx_Write(uint16_t aAddr, uint8_t e2, uint16_t byteNum, uint32_t aValue)
{
    loc_type_t type;
    uint8_t *p;

    p = (uint8_t *)&aValue;
    type = ((e2 == 0) ? M24LRxx_TYPE_USER_MEM : M24LRxx_TYPE_REG);

    SWT_StartTimer(SWT_ID_NFC_TIMEOUT, M24LR_TIMEOUT);
    while( M24LRxx_WriteReq(aAddr, type, byteNum, p) != M24LRxx_REQ_DONE)
    {
        M24LRxx_Manager();
        
        /* M24LRxx read/write timeout */
        if(SWT_CheckTimer(SWT_ID_NFC_TIMEOUT) == SWT_STATUS_UP)
        {          
            /* Reset I2C */
            I2C_Init();
            m24lr.state=M24LRxx_STATE_IDLE;
            
            /* NFC tag power off */
            MCU_NfcPowerPinReset();
                    
            /* Feed watchdog */
            WDT_Feeds();
            
            /* Waiting for reset */
            SWT_StartTimer(SWT_ID_NFC_TIMEOUT, M24LR_RESET);
            while(SWT_CheckTimer(SWT_ID_NFC_TIMEOUT) != SWT_STATUS_UP);
            SWT_CloseTimer(SWT_ID_NFC_TIMEOUT);
            WDT_Feeds();   
            
            /* NFC tag power off */
            MCU_NfcPowerPinSet();
            
            break;
        }
    }
    SWT_CloseTimer(SWT_ID_NFC_TIMEOUT);

    return I2C_CODE_OK;
}

/** ********************************************************************************************************************
* \brief Verifies if register name is valid
*
* \param aRegName
*        register name
*
* \retval TRUE if register name valid
*
***********************************************************************************************************************/

static bool M24LRxx_isRegNameValid(reg_addr_t aRegName)
{
    bool regNameOk;

    if(    ((aRegName >= M24LRxx_REG_SSS0)         && (aRegName <= M24LRxx_REG_SSS15))
       ||  ((aRegName >= M24LRxx_REG_LOCK_BITS_0)  && (aRegName <= M24LRxx_REG_LOCK_BITS_1))
       ||  ((aRegName >= M24LRxx_REG_I2C_PASSWORD) && (aRegName <= M24LRxx_REG_CONTROL))   )
    {
        regNameOk = true;
    }
    else
    {
        regNameOk = false;
    }

    return regNameOk;
}


/** ********************************************************************************************************************
* \brief Updates request status according to M24LR Rx/Tx state machine state
*
* \retval M24LRxx_REQ_NOK, M24LRxx_REQ_DONE or M24LRxx_REQ_PENDING
*
***********************************************************************************************************************/

static M24LRxx_req_t M24LRxx_UpdateReqStatus(void)
{
    if(m24lr.reqStatus == M24LRxx_REQ_PENDING)
    {
        if(m24lr.state == M24LRxx_STATE_IDLE)
        {
            m24lr.reqStatus = M24LRxx_REQ_DONE;
        }
        else if(m24lr.state == M24LRxx_STATE_ERR)
        {
            m24lr.reqStatus = M24LRxx_REQ_NOK;
        }
    }

    return m24lr.reqStatus;
}

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief M24LRxx chip initialization
*
*  Initilizes I2C port annd sets M244LRxx address
*
* \warning Blocking function
*
***********************************************************************************************************************/

void M24LRxx_Init(void)
{
    uint32_t  slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E1_E0 << 1));

    I2C_Init();

    M24LRxx_SetAddress(slave_addr);
}

/** ********************************************************************************************************************
* \brief Sets M24LRxx address
*
* \warning Blocking function
*
* \param aAddr
*        address of the chip
*
***********************************************************************************************************************/

void M24LRxx_SetAddress(uint8_t aAddr)
{
    i2c_slave_addr = aAddr;
}

/** ********************************************************************************************************************
* \brief Present a password to enable read/write
*
* \warning Blocking function
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_PresentPassword(uint32_t aPassword)
{
    i2c_code_t ret_code = I2C_CODE_OK;
    uint8_t tx_buffer[11];
    uint32_t  slave_addr = 0;

    /* Choose configuration memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E2 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    tx_buffer[0]  = 0x09;
    tx_buffer[1]  = 0x00;

    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
    tx_buffer[5]  = (uint8_t)(aPassword >> 24);

    tx_buffer[6]  = 0x09;

    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
    tx_buffer[10] = (uint8_t)(aPassword >> 24);

    ret_code = M24LRxx_PollAck(false);

    if(ret_code == I2C_CODE_OK)
    {
        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
        while(I2C_IsBusy());

        if(ret_code == I2C_CODE_OK)
        {
            ret_code = M24LRxx_PollAck(true);
        }
    }
    
    /* Choose eeprom memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E1_E0 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    return ret_code;
}

/** ********************************************************************************************************************
* \brief write a password to enable I2C read/write
*
* \warning Blocking function
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_I2CwritePassword(uint32_t aPassword)
{
    i2c_code_t ret_code = I2C_CODE_OK;
    uint8_t tx_buffer[11];
    uint32_t  slave_addr = 0;

    /* Choose configuration memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E2 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    tx_buffer[0]  = 0x09;
    tx_buffer[1]  = 0x00;

    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
    tx_buffer[5]  = (uint8_t)(aPassword >> 24);

    tx_buffer[6]  = 0x07;

    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
    tx_buffer[10] = (uint8_t)(aPassword >> 24);

    ret_code = M24LRxx_PollAck(false);

    if(ret_code == I2C_CODE_OK)
    {
        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
        while(I2C_IsBusy());

        if(ret_code == I2C_CODE_OK)
        {
            ret_code = M24LRxx_PollAck(true);
        }
    }
    
    /* Choose eeprom memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E1_E0 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    return ret_code;
}
/** ********************************************************************************************************************
* \brief Present a password to enable read/write
*
* \warning Blocking function
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_SectorPassword(uint32_t aPassword)
{
    
    i2c_code_t ret_code = I2C_CODE_OK;
    uint8_t tx_buffer[11];
    uint32_t  slave_addr = 0;

    /* Choose configuration memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E2 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    tx_buffer[0]  = 0x09;
    tx_buffer[1]  = 0x04;

    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
    tx_buffer[5]  = (uint8_t)(aPassword >> 24);

    tx_buffer[6]  = 0x00;//09

    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
    tx_buffer[10] = (uint8_t)(aPassword >> 24);

    ret_code = M24LRxx_PollAck(false);

    if(ret_code == I2C_CODE_OK)
    {
        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
        while(I2C_IsBusy());

        if(ret_code == I2C_CODE_OK)
        {
            ret_code = M24LRxx_PollAck(true);
        }
    }
//    
//    tx_buffer[0]  = 0x09;
//    tx_buffer[1]  = 0x08;
//
//    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[5]  = (uint8_t)(aPassword >> 24);
//
//    tx_buffer[6]  = 0x09;
//
//    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[10] = (uint8_t)(aPassword >> 24);
//
//    ret_code = M24LRxx_PollAck(false);
//
//    if(ret_code == I2C_CODE_OK)
//    {
//        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
//        while(I2C_IsBusy());
//
//        if(ret_code == I2C_CODE_OK)
//        {
//            ret_code = M24LRxx_PollAck(true);
//        }
//    }
//    
//    tx_buffer[0]  = 0x09;
//    tx_buffer[1]  = 0x0C;
//    
//    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[5]  = (uint8_t)(aPassword >> 24);
//
//    tx_buffer[6]  = 0x09;
//
//    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[10] = (uint8_t)(aPassword >> 24);
//
//    ret_code = M24LRxx_PollAck(false);
//
//    if(ret_code == I2C_CODE_OK)
//    {
//        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
//        while(I2C_IsBusy());
//
//        if(ret_code == I2C_CODE_OK)
//        {
//            ret_code = M24LRxx_PollAck(true);
//        }
//     }
    /* Choose eeprom memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E1_E0 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    return ret_code;
}
/** ********************************************************************************************************************/
/** ********************************************************************************************************************
* \brief write a password to enable read/write
*
* \warning Blocking function
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_WritePassword(uint32_t aPassword)
{
    i2c_code_t ret_code = I2C_CODE_OK;
    uint8_t tx_buffer[11];
    uint32_t  slave_addr = 0;

    /* Choose configuration memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E2 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    tx_buffer[0]  = 0x09;
    tx_buffer[1]  = 0x04;

    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
    tx_buffer[5]  = (uint8_t)(aPassword >> 24);

    tx_buffer[6]  = 0x07;

    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
    tx_buffer[10] = (uint8_t)(aPassword >> 24);

    ret_code = M24LRxx_PollAck(false);

    if(ret_code == I2C_CODE_OK)
    {
        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
        while(I2C_IsBusy());

        if(ret_code == I2C_CODE_OK)
        {
            ret_code = M24LRxx_PollAck(true);
        }
    }
    
//    tx_buffer[0]  = 0x09;
//    tx_buffer[1]  = 0x08;
//
//    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[5]  = (uint8_t)(aPassword >> 24);
//
//    tx_buffer[6]  = 0x07;
//
//    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[10] = (uint8_t)(aPassword >> 24);
//
//    ret_code = M24LRxx_PollAck(false);
//
//    if(ret_code == I2C_CODE_OK)
//    {
//        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
//        while(I2C_IsBusy());
//
//        if(ret_code == I2C_CODE_OK)
//        {
//            ret_code = M24LRxx_PollAck(true);
//        }
//    }
//    
//    tx_buffer[0]  = 0x09;
//    tx_buffer[1]  = 0x0C;
//    
//    tx_buffer[2]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[3]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[4]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[5]  = (uint8_t)(aPassword >> 24);
//
//    tx_buffer[6]  = 0x07;
//
//    tx_buffer[7]  = (uint8_t)(aPassword >> 0);
//    tx_buffer[8]  = (uint8_t)(aPassword >> 8);
//    tx_buffer[9]  = (uint8_t)(aPassword >> 16);
//    tx_buffer[10] = (uint8_t)(aPassword >> 24);
//
//    ret_code = M24LRxx_PollAck(false);
//
//    if(ret_code == I2C_CODE_OK)
//    {
//        I2C_MasterTransmit(i2c_slave_addr, true, tx_buffer, 11);
//        while(I2C_IsBusy());
//
//        if(ret_code == I2C_CODE_OK)
//        {
//            ret_code = M24LRxx_PollAck(true);
//        }
//    }
    /* Choose eeprom memory */
    slave_addr = ( M24LRxx_ADDRESS | (M24LRxx_CHIP_EN_ADDR_E1_E0 << 1));
    M24LRxx_SetAddress(slave_addr);
    
    return ret_code;
}

/** ********************************************************************************************************************
* \brief Reads byte at specified M24LRxx address
*
*
* \param aAddr
*        address of the byte to read
*
* \param pBuf
*        pointer where read byte is copied
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_ReadByte(uint16_t aAddr, uint8_t *pBuf)
{
    return M24LRxx_ReadBuffer(aAddr, 1, pBuf);
}

/** ********************************************************************************************************************
* \brief Reads buffer at specified M24LRxx address
*
*
* \param aAddr
*        starting address of data to read
*
* \param pBuf
*        pointer where data is copied
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_ReadBuffer(uint16_t aAddr, uint16_t aByteNum, uint8_t *pBuf)
{
    i2c_code_t ret_code;

    if(((int16_t)aAddr >= M24LRXX_ADDR_START) && ((aAddr + aByteNum) <= (M24LRXX_ADDR_START + M24LRXX_SIZE_BYTE)))
    {
        ret_code = M24LRxx_Read(aAddr, 0, aByteNum, pBuf);
    }
    else
    {
        ret_code = I2C_CODE_ADDR_ERR;
    }

    return ret_code;
}

/** ********************************************************************************************************************
* \brief Writes a byte at specified M24LRxx address
*
* \param aAddr
*        write address
*
* \param aValue
*        value to write
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_WriteByte(uint16_t aAddr, uint8_t aValue)
{
    return M24LRxx_WriteBuffer(aAddr, 1, (uint32_t)aValue);
}

/** ********************************************************************************************************************
* \brief Writes a page (4 bytes) at specified M24LRxx address
*
* \param aAddr
*        write address
*
* \param aValue
*        value to write (uint32_t)
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_WritePage(uint16_t aAddr, uint32_t aValue)
{
    return M24LRxx_WriteBuffer(aAddr, PAGE_SIZE_BYTE, aValue);
}

/** ********************************************************************************************************************
* \brief Writes a buffer at specified M24LRxx address. To be used when buffer lenght < 4
*
* \param aAddr
*        write address
*
* \param aByteNum
*        number of bytes to write
*
* \param aValue
*        value to write
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_WriteBuffer(uint16_t aAddr, uint16_t aByteNum, uint32_t aValue)
{
    i2c_code_t ret_code;

    if(((int16_t)aAddr >= M24LRXX_ADDR_START) && (aByteNum <= PAGE_SIZE_BYTE) && ((aAddr + aByteNum) <= (M24LRXX_ADDR_START + M24LRXX_SIZE_BYTE)))
    {
        ret_code = M24LRxx_Write(aAddr, 0, aByteNum, aValue);
    }
    else
    {
        ret_code = I2C_CODE_ADDR_ERR;
    }

    return ret_code;
}

/** ********************************************************************************************************************
* \brief Reads the specified register
*
* \param aRegName
*        register name
*
* \param pRegValue
*        pointer where register value is copied
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_ReadReg(reg_addr_t aRegName, uint32_t *pRegValue)
{
    i2c_code_t ret_code;

    if(M24LRxx_isRegNameValid(aRegName))
    {
        if(aRegName >= M24LRxx_REG_I2C_PASSWORD)
        {
            ret_code = M24LRxx_Read((uint16_t)aRegName, 0x08, 4, (uint8_t *)pRegValue);          // 4 bytes registers
        }
        else
        {
            ret_code = M24LRxx_Read((uint16_t)aRegName, 0x08, 1, (uint8_t *)pRegValue);
        }
    }
    else
    {
        ret_code = I2C_CODE_ADDR_ERR;
    }

    return ret_code;
}

/** ********************************************************************************************************************
* \brief Writes the specified register
*
* \param aRegName
*        register name
*
* \param aRegValue
*        register value
*
* \retval I2C_CODE_OK when done
*
***********************************************************************************************************************/

i2c_code_t M24LRxx_WriteReg(reg_addr_t aRegName, uint8_t aRegValue)
{
    i2c_code_t ret_code;

    if(M24LRxx_isRegNameValid(aRegName))
    {
        ret_code = M24LRxx_Write((uint16_t)aRegName, 0x08, 1, (uint32_t)aRegValue);
    }
    else
    {
        ret_code = I2C_CODE_ADDR_ERR;
    }

    return ret_code;
}

/** ********************************************************************************************************************
* \brief Read operation request
*
* \param aAddr
*        starting address where to read
*
* \param aLocType
*        location type: M24LRxx_TYPE_REG or M24LRxx_TYPE_USER_MEM
*
* \param aByteNum
*        number of bytes to read
*
* \param pBuf
*        pointer to buffer where data is copied
*
* \retval M24LRxx_REQ_NOK, M24LRxx_REQ_DONE or M24LRxx_REQ_PENDING
*
***********************************************************************************************************************/

M24LRxx_req_t M24LRxx_ReadReq(uint16_t aAddr, loc_type_t aLocType, uint16_t aByteNum, uint8_t *pBuf)
{
    if(m24lr.reqStatus <= M24LRxx_REQ_DONE)
    {
        if((aAddr + aByteNum) <= (M24LRXX_ADDR_START + M24LRXX_SIZE_BYTE))
        {
            m24lr.addr = aAddr;
            m24lr.e2 = ((aLocType == M24LRxx_TYPE_USER_MEM) ? 0x00 : 0x08);
            m24lr.byteNum = aByteNum;
            m24lr.pRxBuf = pBuf;
            m24lr.state = M24LRxx_STATE_RX_START;
            m24lr.reqStatus = M24LRxx_REQ_PENDING;

            m24lr.buf[0] = (uint8_t)(m24lr.addr >> 8);
            m24lr.buf[1] = (uint8_t)(m24lr.addr);
        }
        else
        {
            m24lr.reqStatus = M24LRxx_REQ_NOK;
        }
    }

    return M24LRxx_UpdateReqStatus();
}

/** ********************************************************************************************************************
* \brief Write operation request
*
* \param aAddr
*        starting address where to write
*
* \param aLocType
*        location type: M24LRxx_TYPE_REG or M24LRxx_TYPE_USER_MEM
*
* \param aByteNum
*        number of bytes to write
*
* \param pData
*        pointer to data to write
*
* \retval M24LRxx_REQ_NOK, M24LRxx_REQ_DONE or M24LRxx_REQ_PENDING
*
***********************************************************************************************************************/

M24LRxx_req_t M24LRxx_WriteReq(uint16_t aAddr, loc_type_t aLocType, uint16_t aByteNum, uint8_t *pData)
{
    uint32_t i;

    if(m24lr.reqStatus <= M24LRxx_REQ_DONE)
    {
        if((aByteNum <= PAGE_SIZE_BYTE) && ((aAddr + aByteNum) <= (M24LRXX_ADDR_START + M24LRXX_SIZE_BYTE)))
        {
            m24lr.addr = aAddr;
            m24lr.e2 = ((aLocType == M24LRxx_TYPE_USER_MEM) ? 0x00 : 0x08);
            m24lr.byteNum = aByteNum;
            m24lr.state = M24LRxx_STATE_TX_START;
            m24lr.reqStatus = M24LRxx_REQ_PENDING;

            m24lr.buf[0] = (uint8_t)(m24lr.addr >> 8);
            m24lr.buf[1] = (uint8_t)(m24lr.addr);

            for(i = 0; i < aByteNum; i++)
            {
                m24lr.buf[2 + i] = *pData;
                pData++;
            }
        }
        else
        {
            m24lr.reqStatus = M24LRxx_REQ_NOK;
        }
    }

    return M24LRxx_UpdateReqStatus();
}

/** ********************************************************************************************************************
* \brief Updates request status and gets it.
*
* \retval M24LRxx_REQ_NOK, M24LRxx_REQ_DONE or M24LRxx_REQ_PENDING
*
***********************************************************************************************************************/

M24LRxx_req_t M24LRxx_GetReqStatus(void)
{
    return M24LRxx_UpdateReqStatus();
}

/** ********************************************************************************************************************
* \brief Read and write M24LRxx requests manager
*
* \retval M24LRxx_REQ_NOK, M24LRxx_REQ_DONE or M24LRxx_REQ_PENDING
*
***********************************************************************************************************************/

M24LRxx_state_t M24LRxx_Manager(void)
{
    i2c_code_t ret_code;

    switch(m24lr.state)
    {
        case M24LRxx_STATE_ERR:
        case M24LRxx_STATE_IDLE:
            break;

        //-------------- Read --------------
        case M24LRxx_STATE_RX_START:
            I2C_MasterTransmit(i2c_slave_addr|m24lr.e2, false, m24lr.buf, 2);                         // Addr tx
            m24lr.state = M24LRxx_STATE_RX_DATA;
            break;

        case M24LRxx_STATE_RX_DATA:
            if(I2C_IsBusy() == false)
            {
                ret_code = I2C_GetCode();

                if(ret_code == I2C_CODE_NACK)
                {
                    m24lr.state = M24LRxx_STATE_RX_START;
                }
                else if(ret_code == I2C_CODE_OK)
                {
                    I2C_MasterReceive(i2c_slave_addr|m24lr.e2, true, m24lr.pRxBuf, m24lr.byteNum);      // Data read
                    m24lr.state = M24LRxx_STATE_RX_END;
                }
                else
                {
                    m24lr.err++;
                    m24lr.state = M24LRxx_STATE_ERR;
                }
            }
            break;

        case M24LRxx_STATE_RX_END:
            if(I2C_IsBusy() == false)
            {
                ret_code = I2C_GetCode();

                if(ret_code == I2C_CODE_OK)
                {
                    m24lr.state = M24LRxx_STATE_IDLE;
                }
                else
                {
                    m24lr.err++;
                    m24lr.state = M24LRxx_STATE_ERR;
                }
            }
            break;

        //-------------- Write --------------
        case M24LRxx_STATE_TX_START:
            I2C_MasterTransmit(i2c_slave_addr|m24lr.e2, true, m24lr.buf, (2 + m24lr.byteNum));
            m24lr.state = M24LRxx_STATE_TX_DATA;
            break;

        case M24LRxx_STATE_TX_DATA:
            if(I2C_IsBusy() == false)
            {
                ret_code = I2C_GetCode();

                if(ret_code == I2C_CODE_NACK)
                {
                    m24lr.state = M24LRxx_STATE_TX_START;
                }
                else if(ret_code == I2C_CODE_OK)
                {
                    I2C_MasterTransmit(i2c_slave_addr, true, NULL, 0);
                    m24lr.state = M24LRxx_STATE_TX_END;
                }
                else
                {
                    m24lr.err++;
                    m24lr.state = M24LRxx_STATE_ERR;
                }
            }
            break;

        case M24LRxx_STATE_TX_END:
            if(I2C_IsBusy() == false)
            {
                ret_code = I2C_GetCode();

                if(ret_code == I2C_CODE_OK)
                {
                    m24lr.state = M24LRxx_STATE_IDLE;
                }
                else if(ret_code == I2C_CODE_NACK)
                {
                    I2C_MasterTransmit(i2c_slave_addr, true, NULL, 0);
                }
                else
                {
                    m24lr.err++;
                    m24lr.state = M24LRxx_STATE_ERR;
                }
            }
            break;
    }

    return m24lr.state;
}

/** ********************************************************************************************************************
* \brief M24LRxx timer
*
* \warning Timer period must be < 10ms
*
***********************************************************************************************************************/

void M24LRxx_Timer(void)
{
    I2C_Timer();
}

/**********************************************************************************************************************/

#endif /* MODULE_M24LRxx */
