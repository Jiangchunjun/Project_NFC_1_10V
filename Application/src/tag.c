/*
***************************************************************************************************
*                            NFC Tag M24LRxx EEPROM Operation Functions
*                            M24LRxx series macro define and interface
*
* File   : tag.c
* Author : Douglas Xie
* Date   : 2016.03.18
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifdef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "tag.h"

/* Macro Defines --------------------------------------------------------------------------------*/


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/



/* Function Implement ---------------------------------------------------------------------------*/


/****************************************************************************
@Brief      TAG Ack Polling to Get I2C Bus Priority
@Param[in]  timeout | Ack polling times limited, 0 is no limited
@Param[out] 
@Note       
@Return     Return TRUE if TAG is available, otherwise return FALSE
****************************************************************************/
uint8_t Tag_AckPolling(uint16_t timeout)
{
    uint16_t result = FALSE;            /* Device default state is not available */
    uint16_t flag;
    
    if( timeout == 0 )
    {
        while(1)
        {
            flag = I2C_IsBusy( TAG_ADDRESS );
            
            if( flag == FALSE )        /* Device is not busy, so it's available */
            {
                result = TRUE;
                break;
            }
        }
    }
    else
    {
        while( timeout-- > 0 )
        {
            flag = I2C_IsBusy( TAG_ADDRESS );
            
            if( flag == FALSE )        /* Device is not busy, so it's available */
            {
                result = TRUE;
                break;
            }
        }
    }
    
    return result;
}


/****************************************************************************
@Brief      Read Data from EEP TAG
@Param[in]  start_addr | Read data from EEP from this address
@Param[in]  length | the number of data that want to read
@Param[out] pdata | Return datas that read from EEP
@Note       if length out of range, it will be cut off
@Return     return OK when read data success or return ERROR
****************************************************************************/
uint8_t Tag_ReadEEP(uint16_t start_addr, uint16_t length, uint8_t *pdata )
{
    uint16_t result;
    
    /* if length out of range, it will be cut off */
    if(( start_addr + length ) > TAG_EEPROM_SIZE )
    {
        length = TAG_EEPROM_SIZE - start_addr;
    }
    
    result = I2C_ReadData( TAG_ADDRESS, start_addr, length, pdata );
    
    return result;
}


/****************************************************************************
@Brief      Write Data to EEP TAG
@Param[in]  start_addr | Write data to EEP from this address
@Param[in]  length | the number of data that want to write
@Param[in]  pdata | Target datas that will write to EEP
@Param[out]  
@Note       Provide solution to solve the TAG EEP limit that one I2C\n
write command can only write one block( 4 bytes ) data.
<br>    if length out of range, it will be cut off
@Return     return OK when read data success or return ERROR
****************************************************************************/
uint8_t Tag_WriteEEP( uint16_t start_addr, uint16_t length, uint8_t *pdata )
{
    uint16_t result = OK;
    uint16_t block_num_1;
    uint16_t block_num_2;
    uint16_t block_num_3;
    
    /* if length out of range, it will be cut off */   
    if(( start_addr + length ) > TAG_EEPROM_SIZE )
    {
        length = TAG_EEPROM_SIZE - start_addr;
    }
    
    block_num_1 = 4 - start_addr % 4;            /* first block available byte number */
    block_num_2 = ( length - block_num_1 ) / 4;  /* full write block number */
    block_num_3 = ( length - block_num_1 ) % 4;  /* last block left byte number */
    
    if( length <= block_num_1)
    {
        result = I2C_WriteData( TAG_ADDRESS, start_addr, length, pdata );
    }
    else 
    {
        result = I2C_WriteData( TAG_ADDRESS, start_addr, block_num_1, pdata );
        
        if( result == OK )
        {
            pdata += block_num_1;
            start_addr += block_num_1;
            
            for( ; block_num_2 > 0; block_num_2-- )
            {
                result = I2C_WriteData( TAG_ADDRESS, start_addr, 4, pdata );
                
                if( result == OK )
                {
                    pdata += 4;
                    start_addr += 4;
                }
                else
                {
                    result = ERROR;
                    break;
                }
            }
            
            if(( result == OK ) && ( block_num_3 > 0 ))
            {
                result = I2C_WriteData( TAG_ADDRESS, start_addr, block_num_3, pdata );
            }
        }
    }
    
    return result;
}


/****************************************************************************
@Brief      Submit I2C Password to TAG
@Param[in]  enable_flag | ENABLE or DISABLE
@Param[out] 
@Note       if enable_flag is TRUE, then submit the right password,\n
            otherwise submit the wrong password to remain TAG locked
<br>        it needs delay time to compare password after submit finish
@Return     Return OK if submit password success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_SubmitPassword( uint8_t enable_flag )
{
    uint8_t i;
    uint16_t result;
    union password_t send_password;
    uint8_t cmd_buffer[9] = { 0,0,0,0, TAG_SUBMIT, 0,0,0,0 };
    
    /* this password can be save to MCU internal eeprom */
    send_password.password = TAG_PASSWORD;  
    
    if( enable_flag == ENABLE )
    {
        for( i = 0; i < 4; i++ )
        {
            cmd_buffer[i]     = send_password.array[i];
            cmd_buffer[i + 5] = send_password.array[i];            
        }
    }
    else
    {
        for( i = 0; i < 4; i++ )
        {
            cmd_buffer[i]     = 0;//send_password.array[i] + 2;
            cmd_buffer[i + 5] = 0;//send_password.array[i] + 2;            
        }
    }
    
    result = I2C_WriteData( TAG_ADDRESS_E2, TAG_PASSWORD_ADDR, 9, cmd_buffer );
    
    return result;
}


/****************************************************************************
@Brief      Change I2C Password of TAG
@Param[in]  new_password | 32bit unsigned value
@Param[out] 
@Note       should submit the right password before call this function 
<br>        it needs delay time to compare password after write finish
@Return     Return OK if write password success, otherwise return ERROR 
****************************************************************************/
uint8_t TAG_ChangePassword( uint32_t new_password )
{
    uint8_t i;
    uint16_t result;
    union password_t send_password;
    uint8_t cmd_buffer[9] = { 0,0,0,0, TAG_CHANGE, 0,0,0,0 };
    
    /* suggestion: after change new Password, MCU should save to internal eeprom */
    send_password.password = new_password; 
    
    for( i = 0; i < 4; i++ )
    {
        cmd_buffer[i]     = send_password.array[i];
        cmd_buffer[i + 5] = send_password.array[i];   
    }
    
    result = I2C_WriteData( TAG_ADDRESS_E2, TAG_PASSWORD_ADDR, 9, cmd_buffer );
    
    /* Here should add some propriate steps to save the new password */
    //if( result == OK )
    //{
    //    FlashWrite(...)
    //}
    
    return result;
}


/****************************************************************************
@Brief      Config EEP to Setup RF WIP/BUSY Pin Interrupt Mode
@Param[in]  mode | TAG_RF_WIP or TAG_RF_BUSY
@Param[out] 
@Note       TAG_RF_WIP: 
<br>        TAG_RF_BUSY:
@Return     Return OK if config success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_ConfigInterrupt( uint8_t mode )
{
    uint16_t result;
    uint8_t config;
    
    result = I2C_ReadData( TAG_ADDRESS_E2, TAG_WIP_BUSY_ADDR, 1, &config );
    
    if( result == OK )
    {
        if( mode == TAG_RF_WIP )
        {
            config |= 0x08;           /* Set WIP/BUSY bit to select WIP */
        }
        else
        {
            config &= ~0x08;          /* Reset WIP/BUSY bit to select BUSY */
        }
        result = I2C_WriteData( TAG_ADDRESS_E2, TAG_WIP_BUSY_ADDR, 1, &config );
    }
    
    return result;
}


/****************************************************************************
@Brief      Config EEP to Open/Close Energy Harvest
@Param[in]  enable_flag | ENABLE or DISABLE
@Param[in]  config | there are 4 output mode
@Param[out] 
@Note       if enable_flag is ENABLE then open Energy Harvest, otherwise closed
@Return     Return OK if config success, otherwise return ERROR 
****************************************************************************/
uint8_t Tag_ConfigEH( uint8_t enable_flag, uint8_t EH_config )
{
    uint16_t result;
    uint8_t  config1;
    uint8_t  config2;
    
    result = I2C_ReadData( TAG_ADDRESS_E2, TAG_WIP_BUSY_ADDR, 1, &config1 );
    
    if( result == OK )   
    {
        result = I2C_ReadData( TAG_ADDRESS_E2, TAG_EH_ADDR, 1, &config2 );
        
        if( result == OK )
        {
            if( enable_flag == ENABLE )
            {
                /*  */
                config1 &= ~0x07;             /* Bit2 EH_mode = 0, Reset Bit1, Bit0 */
                config1 |= EH_config;         /* Config Bit1, Bit0*/
                config2 |= 0x01;              /* Bit0 EH_enable = 1 */
            }
            else
            {
                config1 &= ~0x03;             /* Reset Bit1, Bit0 */
                config1 |= 0x04 + EH_config;  /* Bit2 EH_mode = 1, Config Bit1, Bit0*/
                config2 &= ~0x01;             /* Bit0 EH_enable = 0 */
            }
            
            result = I2C_WriteData( TAG_ADDRESS_E2, TAG_WIP_BUSY_ADDR, 1, &config1 );
            
            if( result == OK )
            {
                result = I2C_WriteData( TAG_ADDRESS_E2, TAG_EH_ADDR, 1, &config2 );
            }
        }
    }
    
    return result;
}

#endif /* DEBUG_VERSION */
