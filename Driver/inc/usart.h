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


#ifndef _USART_H
#define _USART_H

#ifdef ENABLE_USART

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_gpio.h>
#include <xmc_uart.h>

/* Macro Defines ---------------------------------------------------------------------------------*/
/* Pins selection */
#define USART__TX_PIN_2_0__RX_PIN_2_1

#if defined( USART__TX_PIN_0_7__RX_PIN_0_6 )
#define USART_CHANNEL           XMC_UART0_CH1
#define USART_TX_PIN            P0_7
#define USART_TXD_PIN_MODE      XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT7
#define USART_RX_PIN            P0_6
#define USART_RXD_SOURCE        USIC0_C1_DX0_P0_6
#define USART_BAUTRATE          (115200)
#define USART_IRQ_NODE          (2)     /* node can be 0-5, point to USIC0_x_IRQn */
#define USART_IRQn              USIC0_2_IRQn
#define USART_RXD_IRQ_Handler   USIC0_2_IRQHandler

#elif defined( USART__TX_PIN_2_0__RX_PIN_2_1 )
#define USART_CHANNEL           XMC_UART0_CH0
#define USART_TX_PIN            P2_0
#define USART_TXD_PIN_MODE      XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6
#define USART_RX_PIN            P2_1
#define USART_RXD_SOURCE        USIC0_C0_DX0_P2_1
#define USART_BAUTRATE          (115200)
#define USART_IRQ_NODE          (2)     /* node can be 0-5, point to USIC0_x_IRQn */
#define USART_IRQn              USIC0_2_IRQn
#define USART_RXD_IRQ_Handler   USIC0_2_IRQHandler

#elif defined( USART__TX_PIN_0_14__RX_PIN_0_15 )
/* @TODO(Douglas): if enable SWD0, this pin will be debug gpio */
#define USART_CHANNEL           XMC_UART0_CH0
#define USART_TX_PIN            P0_14
#define USART_TXD_PIN_MODE      XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6
#define USART_RX_PIN            P0_15
#define USART_RXD_SOURCE        USIC0_C0_DX0_P0_15
#define USART_BAUTRATE          (115200)
#define USART_IRQ_NODE          (2)     /* node can be 0-5, point to USIC0_x_IRQn */
#define USART_IRQn              USIC0_2_IRQn
#define USART_RXD_IRQ_Handler   USIC0_2_IRQHandler

#else
#error "usart.h: no usart pin config"
#endif

/* Pin mode when usart is disable */
#define USART_DISEN_OUT_MODE    XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN    
#define USART_DISEN_OUT_LEVEL   XMC_GPIO_OUTPUT_LEVEL_LOW    

/* USART receive buffer size define */
/* Data Format: StartByte(1) + Command(1) + Param(1) + Data(8) + CheckSum(1) = 12bytes */
#define USART_RX_BUFFER_SIZE    (12)

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable USART Module
* @Param   
* @Note    config usart module and start it
* @Return  
*******************************************************************************/
void USART_Enable(void);

/*******************************************************************************
* @Brief   Disable USART Module
* @Param   
* @Note    stop usart channel and set TXD RXD to output low level
* @Return  
*******************************************************************************/
void USART_Disable(void);

/*******************************************************************************
* @Brief   USART Send Data
* @Param   [in]data  - pointer to the data buffer
*          [in]count - data lenghth in buffer
* @Note    send out a data buffer directly
* @Return  
*******************************************************************************/
void USART_SendData(uint8_t *data, uint8_t count);

/*******************************************************************************
* @Brief   USART Get Receive Data
* @Param   [out]data  - pointer to the data buffer
* @Note    copy recieve buffer data to target
* @Return  
*******************************************************************************/
void USART_GetRxData(uint8_t *data);

/*******************************************************************************
* @Brief   USART Get Receive Data Count
* @Param   
* @Note    output data count that save in receive buffer
* @Return  receive data count
*******************************************************************************/
uint8_t USART_GetRxCount(void);

/*******************************************************************************
* @Brief   USART Reset Buffer
* @Param   
* @Note    Clear receiver data buffer and count
* @Return  
*******************************************************************************/
void USART_ResetBuffer(void);

/*******************************************************************************
* @Brief   USART Print Information
* @Param   [in]pinfo  - pointer to the print string infomation
* @Note    send print string to PC
* @Return  
*******************************************************************************/
void USART_PrintInfo(uint8_t *pinfo);

#else  /* ENABLE_USART */

/* define the blank interface */
#define USART_Enable()
#define USART_Disable()
#define USART_SendData(a, b)
#define USART_GetRxData(a)
#define USART_CalcuCS(a,b)
#define USART_PrintInfo(a)

#endif /* ENABLE_USART */

#endif /* _USART_H */ 
