/*
***************************************************************************************************
*                            Simulate I2C Basic Read and Write Functions
*
* File   : simu_i2c.h
* Author : Douglas Xie
* Date   : 2016.03.18
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifdef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "simu_i2c.h"

/* Macro Defines --------------------------------------------------------------------------------*/
/* Delay Function Define */
#define SMALL_DELAY()       //Delay_5us()


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/


/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @brief   Small delay function
* @param  
* @note    delay for 5us
* @retval
*******************************************************************************/
void Delay_5us(void)
{
    /*@Douglas: comment this part for using fast mode */
//    uint8_t i;
//    
//    for (i=5; i>0; i--);
}


/*******************************************************************************
* @brief   Simulate I2C master initial
* @param  
* @note    
* @retval
*******************************************************************************/
void I2C_Initial(void)
{
    XMC_GPIO_CONFIG_t i2c_sda_config;
    XMC_GPIO_CONFIG_t i2c_scl_config;
    
    /* Pins mode, set to output low level when i2c initial */
	i2c_sda_config.mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN;
    i2c_sda_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
	i2c_scl_config.mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN;
    i2c_scl_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    
    /* Configure pins */
	XMC_GPIO_Init(I2C_SDA_PIN, &i2c_sda_config);
    XMC_GPIO_Init(I2C_SCL_PIN, &i2c_scl_config);                   
}

/*******************************************************************************
* @brief   Simulate I2C master start condition
* @param  
* @note    SDA 1->0 while SCL High
* @retval
*******************************************************************************/
void I2C_Start(void)
{
    SDA_OUT_H();                  
    SCL_OUT_H();                   
    Delay_5us();     
    
    SDA_OUT_L();                
    Delay_5us();   
    
    SCL_OUT_L();                    
}


/*******************************************************************************
* @brief   Simulate I2C master stop condition
* @param  
* @note    SDA 0->1 while SCL High
* @retval
*******************************************************************************/
void I2C_Stop(void)
{
    SDA_OUT_L();                   
    SCL_OUT_H();                  
    Delay_5us();   
    
    SDA_OUT_H();                
    Delay_5us();               
}


/*******************************************************************************
* @brief   Simulate I2C master send acknowledge signal 
* @param   ack: I2C_ACK, I2C_NO_ACK 
* @note   
* @retval
*******************************************************************************/
void I2C_SendACK(uint8_t ack)
{
    if(ack == 0)
    {
        SDA_OUT_L(); 
    }  
    else
    {
        SDA_OUT_H(); 
    }
    
    SCL_OUT_H();                 
    Delay_5us();  
    
    SCL_OUT_L();                
    Delay_5us();     
}


/*******************************************************************************
* @brief   Simulate I2C master send one byte data
* @param   sendDAT: data to be send 
* @note   
* @retval  return slave ack state: I2C_ACK, I2C_NO_ACK
*******************************************************************************/
uint8_t I2C_SendByte(uint8_t sendDAT)
{
    uint8_t i,revack;
    
    /* Send one byte data */
    for (i= 0 ; i< 8; i++)         
    {
        SCL_OUT_L();              
        Delay_5us();  
        
        if (sendDAT & 0x80)
        {
            SDA_OUT_H();
        }
        else   
        {
            SDA_OUT_L();
        }
        sendDAT <<=  1;
        
        SCL_OUT_H();              
        Delay_5us();     
    } 
    
    SCL_OUT_L();
    Delay_5us();  
    
    /* Receive ack signal */
    SET_SDA_IN();
    SCL_OUT_H();  
    SMALL_DELAY();
    
    if(SDA_IN_DATA())
    {
        revack = 1;
    }
    else
    {
        revack = 0;
    }
    Delay_5us();
    SCL_OUT_L();
    SDA_OUT_H();
    Delay_5us();  
    
    return revack;
}


/*******************************************************************************
* @brief   Simulate I2C master receive one byte data
* @param  
* @note   
* @retval  return receive data
*******************************************************************************/
uint8_t I2C_RecvByte(void)
{
    uint8_t i;
    uint8_t revDAT = 0;
    
    /* latch the Data port befor reading and set SDA input */
    SDA_OUT_H();
    SET_SDA_IN();
    
    /* Receive one byte data */
    for (i=0; i<8; i++)         
    { 
        revDAT <<= 1;
        
        SCL_OUT_H();
        SMALL_DELAY();
        
        if (SDA_IN_DATA())
        {
            revDAT |= 0x01; 
        }
        else
        {
            revDAT &= 0xFE;
        }
        Delay_5us(); 
        
        SCL_OUT_L();         
        Delay_5us();  
    }
    
    SDA_OUT_H();
    
    return revDAT;
}


/*******************************************************************************
* @Brief   Make Sure Whether I2C Bus is Busy or not
* @Param   [in]byDeviceAddr | Device address that need to confirm
* @Note       
* @Return  return TRUE when busy or return FALSE
*******************************************************************************/
uint8_t I2C_IsBusy( uint8_t byDeviceAddr )
{
    uint16_t wBusy = TRUE;                  /* default I2C bus is busy */
    uint8_t byAck;
    
    I2C_Start();
    byAck = I2C_SendByte( byDeviceAddr );
    
    if( byAck == I2C_ACK )
    {
        wBusy = FALSE;                      /* I2C bus can acknowledge means it is not busy */
    }
    
    I2C_Stop();
    return wBusy;
}


/*******************************************************************************
* @Brief   I2C Read Data from Device from Random Address
* @Param   [in]byDeviceAddr: Target device address
*          [in]wStartAddr  : Read data from this address
*          [in]wLength     : the number of data that want to read
*          [out]pbyData    : Return datas that read from device
* @Note       
* @Return  return OK when read data success or return ERROR
*******************************************************************************/
uint8_t I2C_ReadData( uint8_t byDeiceAddr, uint16_t wStartAddr, uint16_t wLength, uint8_t *pbyData )
{
    uint8_t byAck;
    
    I2C_Start();
    
    byAck = I2C_SendByte( byDeiceAddr );            /* call device by 7 bits hardware address, LSB is write mode */ 
    if( byAck == I2C_NO_ACK )                       /* judge whether slave acknowledge */
    {
        I2C_Stop();
        return ERROR;
    }
    
    byAck = I2C_SendByte( wStartAddr >> 8 );        /* write high byte of memory address */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }
    
    byAck = I2C_SendByte( wStartAddr & 0x00ff );    /* write low byte of memory address */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }
    
    I2C_Start();                                    /* restart to get ready to read */
    byAck = I2C_SendByte(byDeiceAddr + 1);          /* call device by 7 bits hardware address, LSB is read mode */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }
    
    while((--wLength)>0)                /* read (wLength-1) times,each time read one byte data */
    {
        *pbyData = I2C_RecvByte();      /* read one byte data and save to *pbyData and pointer increase */
        pbyData++ ;
        I2C_SendACK( I2C_ACK );
    }
    
    *pbyData = I2C_RecvByte();          /* read last one byte data and save to *pbyData */
    I2C_SendACK( I2C_NO_ACK );
    I2C_Stop();
    
    return OK;
}


/*******************************************************************************
* @Brief   I2C Write Data to Device from Random Address
* @Param   [in]byDeviceAddr: Target device address
*          [in]wStartAddr  : Write data from this address
*          [in]wLength     : the number of data that want to write
*          [in]pbyData     : Target datas that will write to device
* @Note       
* @Return  return OK when write data success or return ERROR
*******************************************************************************/
uint8_t I2C_WriteData( uint8_t byDeiceAddr, uint16_t wStartAddr, uint16_t wLength, uint8_t *pbyData )
{
    uint8_t byAck;
    uint8_t byTimes = I2C_TIMEOUT;
    
    while((I2C_IsBusy(byDeiceAddr) == TRUE) && (byTimes > 0))
    {
        byTimes --;
    }
    
    if( byTimes == 0 )                             /* device is busy and retry timeout */
    {
        return ERROR;
    }
    
	I2C_Start();
	byAck = I2C_SendByte( byDeiceAddr );           /* call the slaver I2C device, in write mode */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }
	
	byAck = I2C_SendByte( wStartAddr >> 8 );       /* write high byte of the memory address */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }
	
	byAck = I2C_SendByte( wStartAddr & 0x00ff );   /* write low byte of the memory address */
    if( byAck == I2C_NO_ACK )
    {
        I2C_Stop();
        return ERROR;
    }	
    
	while(( wLength-- ) != 0 )
	{
		byAck = I2C_SendByte( *pbyData );          /* send data bit by bit */
        pbyData++;
        
        if( byAck == I2C_NO_ACK )
        {
            I2C_Stop();
            return ERROR;
        }
    }
	I2C_Stop();	
    
	return OK;
}

#endif /* DEBUG_VERSION */


/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
