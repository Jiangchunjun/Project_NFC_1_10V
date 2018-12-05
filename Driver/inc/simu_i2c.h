/*
 ***************************************************************************************************
 *                           Simulate I2C Basic Read and Write Functions
 *
 * File   : simu_i2c.h
 * Author : Douglas Xie
 * Date   : 2016.03.18
 ***************************************************************************************************
 * Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
 ***************************************************************************************************
 */

#ifndef _SIMU_I2C_H
#define _SIMU_I2C_H

#ifdef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "ttypes.h"
#include "xmc_gpio.h"
#include "gpio_xmc1300_tssop38.h"

/* Macro Defines --------------------------------------------------------------------------------*/
/* Define I2C ACK signal */
#define I2C_ACK         0
#define I2C_NO_ACK      1

#define I2C_TIMEOUT     50      /* retry times when I2C bus is busy */

#define I2C_SDA_PIN     P1_2
#define I2C_SCL_PIN     P1_3

#define SCL_OUT_H()     P1_3_set()
#define SCL_OUT_L()     P1_3_reset()
#define SDA_OUT_H()     P1_2_set_mode(OUTPUT_PP_GP); P1_2_set()
#define SDA_OUT_L()     P1_2_set_mode(OUTPUT_PP_GP); P1_2_reset()
#define SET_SDA_IN()    P1_2_set_mode(INPUT)
#define SDA_IN_DATA()   P1_2_read()



/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @brief   Simulate I2C master initial
* @param  
* @note    
* @retval
*******************************************************************************/
void I2C_Initial(void);

/*******************************************************************************
 * @Brief   Make Sure Whether I2C Bus is Busy or not
 * @Param   [in]byDeviceAddr | Device address that need to confirm
 * @Note       
 * @Return  return TRUE when busy or return FALSE
 *******************************************************************************/
uint8_t I2C_IsBusy( uint8_t byDeviceAddr );

/*******************************************************************************
 * @Brief   I2C Read Data from Device from Random Address
 * @Param   [in]byDeviceAddr: Target device address
 *          [in]wStartAddr  : Read data from this address
 *          [in]wLength     : the number of data that want to read
 *          [out]pbyData    : Return datas that read from device
 * @Note       
 * @Return  return OK when read data success or return ERROR
 *******************************************************************************/
uint8_t I2C_ReadData( uint8_t byDeiceAddr, uint16_t wStartAddr, uint16_t wLength, uint8_t *pbyData );

/*******************************************************************************
 * @Brief   I2C Write Data to Device from Random Address
 * @Param   [in]byDeviceAddr: Target device address
 *          [in]wStartAddr  : Write data from this address
 *          [in]wLength     : the number of data that want to write
 *          [in]pbyData     : Target datas that will write to device
 * @Note       
 * @Return  return OK when write data success or return ERROR
 *******************************************************************************/
uint8_t I2C_WriteData( uint8_t byDeiceAddr, uint16_t wStartAddr, uint16_t wLength, uint8_t *pbyData );


#else

/* Define empty interface for compiler */
#define I2C_Initial()       {P1_2_set_mode(INPUT);P1_3_set_mode(INPUT);}

#endif /* DEBUG_VERSION */
#endif 

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
