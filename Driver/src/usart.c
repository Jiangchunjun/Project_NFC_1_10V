/*
***************************************************************************************************
*                               Usart Communication Function
*
* File   : usart.c
* Author : Douglas Xie
* Date   : 2016.04.26
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifdef ENABLE_USART

/* Include Head Files ----------------------------------------------------------------------------*/
#include "usart.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/
uint8_t g_rx_buffer[USART_RX_BUFFER_SIZE + 2];
uint8_t g_rx_index = 0;

/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Enable USART Module
* @Param   
* @Note    config usart module and start it
* @Return  
*******************************************************************************/
void USART_Enable(void)
{
    /* Pins configuration */
    XMC_GPIO_CONFIG_t usart_tx_config;
    XMC_GPIO_CONFIG_t usart_rx_config; 
    
    /* USART configuration */
    XMC_UART_CH_CONFIG_t usart_channel_config = 
    {	
        .data_bits = 8U,
        .stop_bits = 1U,
        .baudrate = USART_BAUTRATE
    };
    
    /* Configure USART channel */
    XMC_UART_CH_Init(USART_CHANNEL, &usart_channel_config);
    XMC_UART_CH_SetInputSource(USART_CHANNEL, XMC_UART_CH_INPUT_RXD, USART_RXD_SOURCE);
    
    /* Enable Receive Interrupt */
    XMC_UART_CH_SetInterruptNodePointer(USART_CHANNEL, USART_IRQ_NODE);
    XMC_UART_CH_SelectInterruptNodePointer(USART_CHANNEL, XMC_UART_CH_INTERRUPT_NODE_POINTER_RECEIVE, USART_IRQ_NODE);
    XMC_UART_CH_EnableEvent(USART_CHANNEL, XMC_UART_CH_EVENT_STANDARD_RECEIVE);
    NVIC_SetPriority(USART_IRQn, 11);
    NVIC_EnableIRQ(USART_IRQn);
    
	/* Start USART channel */
    XMC_UART_CH_Start(USART_CHANNEL);
    
 	/* Pins mode */
	usart_tx_config.mode = USART_TXD_PIN_MODE;
	usart_rx_config.mode = XMC_GPIO_MODE_INPUT_PULL_UP;
    
    /* Configure pins */
	XMC_GPIO_Init(USART_TX_PIN, &usart_tx_config);
    XMC_GPIO_Init(USART_RX_PIN, &usart_rx_config);
}


/*******************************************************************************
* @Brief   Disable USART Module
* @Param   
* @Note    stop usart channel and set TXD RXD to output low level
* @Return  
*******************************************************************************/
void USART_Disable(void)
{
    /* Pins configuration */
    XMC_GPIO_CONFIG_t usart_tx_config;
    XMC_GPIO_CONFIG_t usart_rx_config;
    
	/* Stop UART channel */
    XMC_UART_CH_Stop(USART_CHANNEL);
    
    /* Pins mode, set to output low level when disable usart */
	usart_tx_config.mode = USART_DISEN_OUT_MODE;
    usart_tx_config.output_level = USART_DISEN_OUT_LEVEL;
	usart_rx_config.mode = USART_DISEN_OUT_MODE;
    usart_rx_config.output_level = USART_DISEN_OUT_LEVEL;
    
    /* Configure pins */
	XMC_GPIO_Init(USART_TX_PIN, &usart_tx_config);
    XMC_GPIO_Init(USART_RX_PIN, &usart_rx_config);
}


/*******************************************************************************
* @Brief   USART Send Data
* @Param   [in]data  - pointer to the data buffer
*          [in]count - data lenghth in buffer
* @Note    send out a data buffer directly
* @Return  
*******************************************************************************/
void USART_SendData(uint8_t *data, uint8_t count)
{
    uint8_t index;
    uint16_t txdata;
        
    /* disable global interrupt to improve active time */
    //__disable_interrupt();
    
    /* send out data buffer one by one */
    for (index = 0; index < count; index++)
    {
        txdata = (uint16_t)(data[index]);
        XMC_UART_CH_Transmit(USART_CHANNEL, txdata);
    }
    
    /* resume global interrupt */
    //__enable_interrupt();
}


/*******************************************************************************
* @Brief   USART Get Receive Data
* @Param   [out]data  - pointer to the data buffer
* @Note    copy recieve buffer data to target
* @Return  
*******************************************************************************/
void USART_GetRxData(uint8_t *data)
{
    uint8_t i;
    
    /* Copy rx buffer to target */
    for (i = 0; i < USART_RX_BUFFER_SIZE; i++)
    {
        /* Copy data */
        *data =  g_rx_buffer[i];
        
        /* Shift to next address */
        data++;
    }
}


/*******************************************************************************
* @Brief   USART Get Receive Data Count
* @Param   
* @Note    output data count that save in receive buffer
* @Return  receive data count
*******************************************************************************/
uint8_t USART_GetRxCount(void)
{    
    uint8_t count = 0;
    
    if(g_rx_index > USART_RX_BUFFER_SIZE)
    {
        count = USART_RX_BUFFER_SIZE;
    }
    else
    {
        count = g_rx_index;
    }
    
    return count;
}


/*******************************************************************************
* @Brief   USART Reset Buffer
* @Param   
* @Note    Clear receiver data buffer and count
* @Return  
*******************************************************************************/
void USART_ResetBuffer(void)
{
    uint8_t i;
    
    /* Read buffer again to clean buffer data */
    XMC_USIC_CH_RXFIFO_Flush(USART_CHANNEL);
    XMC_USIC_CH_TXFIFO_Flush(USART_CHANNEL);
    XMC_UART_CH_GetReceivedData(USART_CHANNEL);
    
    /* Copy rx buffer to target */
    for (i = 0; i < USART_RX_BUFFER_SIZE; i++)
    {
        /* Copy data */
        g_rx_buffer[i] = 0;
    }
    
    g_rx_index = 0;
}


/*******************************************************************************
* @Brief   USART Receive Interrupt Handler
* @Param   
* @Note    get rx byte form usart and save to g_rx_buffer
* @Return  
*******************************************************************************/
void USART_RXD_IRQ_Handler(void)
{
    uint16_t receive = 0;

    /* Get one byte data from USART */
    receive = XMC_UART_CH_GetReceivedData(USART_CHANNEL);
    
    /* Save data if buffer is not full */
    if(g_rx_index < USART_RX_BUFFER_SIZE+2)
    {
        /* Save to receive buffer */
        g_rx_buffer[g_rx_index++] = receive & 0xFF;
    }
}

/*******************************************************************************
* @Brief   USART Print Information
* @Param   [in]pinfo  - pointer to the print string infomation
* @Note    send print string to PC
* @Return  
*******************************************************************************/
void USART_PrintInfo(uint8_t *pinfo)
{    
#ifdef DEBUG_PRINT
    /* send out string until the end */
    while(*pinfo != '\0')
    {
        XMC_UART_CH_Transmit(USART_CHANNEL, *pinfo);
        pinfo++;
    }
#endif
}


#endif /* ENABLE_USART */
