/*
***************************************************************************************************
*                               Software Timer Implementation
*
* File   : sw_timer.h
* Author : Douglas Xie
* Date   : 2016.04.15
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef _SW_TIMER_H
#define _SW_TIMER_H

/* Include Head Files ---------------------------------------------------------------------------*/
#include "tick.h"

/* Macro Defines --------------------------------------------------------------------------------*/
/* Software timer number */
#define  SWT_ID_TOTAL_NUM               9    

/* Software timer timer_id define */
#define  SWT_ID_LOOP                    0       /* Delay is define by SWT_PERIOD_0 */
#define  SWT_ID_HICCUP                  1       /* Delay is define by SWT_PERIOD_1 */
#define  SWT_ID_NFC                     2       /* Delay is define by SWT_PERIOD_2 */
#define  SWT_ID_SPEED_UP                3       /* Delay is define by SWT_PERIOD_3 */
#define  SWT_ID_COMM_RX                 4       /* Delay is define by SWT_PERIOD_4 */
#define  SWT_ID_COMM_UNLOCK             5       /* Delay is define by SWT_PERIOD_5 */
#define  SWT_ID_SET_LOAD                6       /* Delay is define by SWT_PERIOD_6 */
#define  SWT_ID_NFC_TIMEOUT             7       /* Delay is define by SWT_PERIOD_7 */
#define  SWT_ID_ONE2TEN                 8       /* Delay is define by SWT_PERIOD_8 */

/* Software timer status define */
#define  SWT_STATUS_OFF                 0xFF    /* timer_id is disable */
#define  SWT_STATUS_OK                  0x01    /* timer_id is counting and not up yet */
#define  SWT_STATUS_UP                  0x00    /* timer_id is up, timing finished */

/* Data Structure Define ------------------------------------------------------------------------*/


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @brief  Software Timer Default Initial
* @param  
* @note   Close all software timer
* @retval 
*******************************************************************************/
void SWT_Init(void);

/*******************************************************************************
* @brief  Software Timer Setup and Start
* @param  
* @note   Enable timer_id and load it's tick value  
* @retval 
*******************************************************************************/
void SWT_StartTimer(uint8_t timer_id, uint16_t period);

/*******************************************************************************
* @brief  Software Timer Close
* @param  
* @note   Disable timer_id
* @retval 
*******************************************************************************/
void SWT_CloseTimer(uint8_t timer_id);

/*******************************************************************************
* @brief  Software Timer Check
* @param  
* @note   Check timer_id tick value and return it's status
* @retval timer_id tick status
*           --  SWT_STATUS_OFF: timer_id is disable
*           --  SWT_STATUS_OK:  timer_id is counting and not up yet
*           --  SWT_STATUS_UP:  timer_id is up, timing finished
*******************************************************************************/
uint8_t SWT_CheckTimer(uint8_t timer_id);

/*******************************************************************************
* @brief  Software Timer Decrease Tick
* @param  
* @note   Called by Basic timer interrupt IRQ handler, decrease the counters
* @retval 
*******************************************************************************/
void SWT_DecreaseTick(void);


#endif /* _SW_TIMER_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
