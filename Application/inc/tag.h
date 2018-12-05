/*
***************************************************************************************************
*                            NFC Tag M24LRxx EEPROM Operation Functions
*                            M24LRxx series macro define and interface
*
* File   : tag.h
* Author : Douglas Xie
* Date   : 2016.03.18
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef _TAG_H
#define _TAG_H

#ifdef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "ttypes.h"
#include "simu_i2c.h"

/* Macro Defines --------------------------------------------------------------------------------*/
/* M24LR16E-R I2C Default Password */
#define TAG_PASSWORD          0xDEC17828

/* Define M24LR16E-R device hardware address */
/* E2 = 0 */
#define TAG_ADDRESS           0xA6
/* E2 = 1 */
#define TAG_ADDRESS_E2        0xAE

/* Define M24LR16E-R memory size(16k bits, 2048 bytes) */
#define TAG_EEPROM_SIZE       0x0800

/* M24LR16E-R Validate Code(command parameter code for password) */
#define TAG_SUBMIT            0x09
#define TAG_CHANGE            0x07

/* M24LR16E-R I2C Password Address */
#define TAG_PASSWORD_ADDR     0x0900

/* M24LR16E-R I2C RF_WIP/BUSY Config */
#define TAG_WIP_BUSY_ADDR     0x0910

/* M24LR16E-R I2C Energy Harvest Config Address */
#define TAG_EH_ADDR           0x0920

/* M24LR16E-R RF_WIP and RF_BUSY Define */ 
#define TAG_RF_BUSY           0x00            /* TAG is busy in RF command, both write and read command */
#define TAG_RF_WIP            0x01            /* RF is write in process */

/* M24LR16E-R Energy Harvest Mode Define */ 
#define TAG_EH_CFG0           0x00            /* Vout:1.7V, Imax: 6mA */
#define TAG_EH_CFG1           0x01            /* Vout:2.1V, Imax: 3mA */
#define TAG_EH_CFG2           0x02            /* Vout:2.3V, Imax: 1mA */
#define TAG_EH_CFG3           0x03            /* Vout:2.5V, Imax: 300uA */

/* Global Variable ------------------------------------------------------------------------------*/

/* M24LR16E-R Secret Code Union */
#ifndef UNION_PASSWORD
#define UNION_PASSWORD

union password_t
{
    uint32_t  password;
    uint8_t   array[4];
};

#endif


/* Function Declaration -------------------------------------------------------------------------*/

/****************************************************************************
@Brief      TAG Ack Polling to Get I2C Bus Priority
@Param[in]  timeout | Ack polling times limited, 0 is no limited
@Param[out] 
@Note       
@Return     Return TRUE if TAG is available, otherwise return FALSE
****************************************************************************/
uint8_t Tag_AckPolling(uint16_t timeout);

/****************************************************************************
@Brief      Read Data from EEP TAG
@Param[in]  start_addr | Read data from EEP from this address
@Param[in]  length | the number of data that want to read
@Param[out] pdata | Return datas that read from EEP
@Note       if length out of range, it will be cut off
@Return     return OK when read data success or return ERROR
****************************************************************************/
uint8_t Tag_ReadEEP(uint16_t start_addr, uint16_t length, uint8_t *pdata);

/****************************************************************************
@Brief      Write Data to EEP TAG
@Param[in]  start_addr | Write data to EEP from this address
@Param[in]  length | the number of data that want to write
@Param[in]  pdata | Target datas that will write to EEP
@Param[out]  
@Note       Provide solution to solve the TAG EEP limit that one I2C\n
            write command can only write one block( 4 bytes ) data.
<br>        if length out of range, it will be cut off
@Return     return OK when read data success or return ERROR
****************************************************************************/
uint8_t Tag_WriteEEP(uint16_t start_addr, uint16_t length, uint8_t *pdata);

/****************************************************************************
@Brief      Submit I2C Password to TAG
@Param[in]  enable_flag | ENABLE or DISABLE
@Param[out] 
@Note       if enable_flag is TRUE, then submit the right password,\n
            otherwise submit the wrong password to remain TAG locked
<br>        it needs delay time to compare password after submit finish
@Return     Return OK if submit password success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_SubmitPassword(uint8_t enable_flag);

/****************************************************************************
@Brief      Change I2C Password of TAG
@Param[in]  new_password | 32bit unsigned value
@Param[out] 
@Note       should submit the right password before call this function 
<br>        it needs delay time to compare password after write finish
@Return     Return OK if write password success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_ChangePassword(uint32_t new_password );

/****************************************************************************
@Brief      Config EEP to Setup RF WIP/BUSY Pin Interrupt Mode
@Param[in]  mode | TAG_RF_WIP or TAG_RF_BUSY
@Param[out] 
@Note       TAG_RF_WIP: 
<br>        TAG_RF_BUSY:
@Return     Return OK if config success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_ConfigInterrupt(uint8_t mode);

/****************************************************************************
@Brief      Config EEP to Open/Close Energy Harvest
@Param[in]  enable_flag | ENABLE or DISABLE
@Param[in]  byConfig | there are 4 output mode
@Param[out] 
@Note       if enable_flag is ENABLE then open Energy Harvest, otherwise closed
@Return     Return OK if config success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_ConfigEH(uint8_t enable_flag, uint8_t EH_config);


#else 

/* Define empty interface for compiler */
#define Tag_ReadEEP(a,b,c)      (0) /* return error */
#define Tag_WriteEEP(a,b,c)     (0) /* return error */
#endif /* DEBUG_VERSION */

#endif    
