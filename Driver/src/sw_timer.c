/*
***************************************************************************************************
*                               Software Timer Implementation
*          Software timer is base on the system tick timer which provide the ticks base
*
* File   : sw_timer.c
* Author : Douglas Xie
* Date   : 2016.04.15
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/


/* Include Head Files ---------------------------------------------------------------------------*/
#include "sw_timer.h"

/* Macro Defines --------------------------------------------------------------------------------*/


/* Private Variable -----------------------------------------------------------------------------*/
__IO uint32_t  sw_timer_register = 0x00000000;      /* Software timer enable register */
__IO uint32_t  sw_timer_tick[SWT_ID_TOTAL_NUM];     /* Software timer tick counter array */


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/


/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @brief  Software Timer Default Initial
* @param  
* @note   Close all software timer
* @retval 
*******************************************************************************/
void SWT_Init(void)
{    
    uint8_t i;
    
    /* timer_id all disable */
    sw_timer_register = 0x00000000;
    
    /* timer_id tick counter all reset to zero */
    for( i = 0; i < SWT_ID_TOTAL_NUM; i++ )
    {
        sw_timer_tick[i] = 0x00000000;
    }
}


/*******************************************************************************
* @brief  Software Timer Setup and Start
* @param  [in]timer_id: software timer id that define in header file
*         [in]period: uint 1ms, software timer delay timing period value
* @note   Enable timer_id and load it's tick value  
* @retval 
*******************************************************************************/
void SWT_StartTimer(uint8_t timer_id, uint16_t period)
{
    uint32_t tick_value = 0;
    
    /* Check timer_id */
    if( timer_id >= SWT_ID_TOTAL_NUM )
    {
        /* timer_id out of range, get into endless loop for debug */
        while(1);
    }
    
    /* Register and enable timer_id */
    sw_timer_register |= ( 1 << timer_id );
    
    /* Calculate tick value, rounded to one decimal place */
    //tick_value =  ((uint32_t)period * TICKS_PER_SECOND * 10 + 5) / 10000;
    tick_value =  ((uint32_t)period * (Tick_GetTickFreq()) * 10 + 5) / 10000;
    
    /* Load timer_id tick value */
    sw_timer_tick[timer_id] = (uint16_t)tick_value;
}

/*******************************************************************************
* @brief  Software Timer Close
* @param  
* @note   Disable timer_id
* @retval 
*******************************************************************************/
void SWT_CloseTimer( uint8_t timer_id )
{
    /* Check timer_id */
    if( timer_id >= SWT_ID_TOTAL_NUM )
    {
        /* timer_id out of range, get into endless loop for debug */
        while(1);
    }
    
    /* Disable timer_id */
    sw_timer_register &= ~( 1 << timer_id );
    
    /* Clear timer_id ick value */
    sw_timer_tick[timer_id] = 0x00000000;
}


/*******************************************************************************
* @brief  Software Timer Check
* @param  
* @note   Check timer_id tick value and return it's status
* @retval timer_id tick status
*           -- SWT_STATUS_OFF: timer_id is disable
*           -- SWT_STATUS_OK:  timer_id is counting and not up yet
*           -- SWT_STATUS_UP:  timer_id is up, timing finished
*******************************************************************************/
uint8_t SWT_CheckTimer(uint8_t timer_id)
{
    uint8_t  status;
    uint32_t timer_id_Index;
    
    /* Check timer_id */
    if( timer_id >= SWT_ID_TOTAL_NUM )
    {
        /* timer_id out of range, get into endless loop for debug */
        while(1);
    }
    
    /* Check timer_id state */
    timer_id_Index = ( 1 << timer_id );
    if(( sw_timer_register & timer_id_Index ) == 0 )
    {
        /* timer_id is closed */
        status = SWT_STATUS_OFF;
    }
    else
    {
        /* timer_id is registered and enable */ 
        if( sw_timer_tick[timer_id] == 0 )
        {
            /* timer_id is Up */
            status = SWT_STATUS_UP;
        }
        else
        {
            /* timer_id is Counting */
            status = SWT_STATUS_OK;
        }               
    }
    
    return status;
}


/*******************************************************************************
* @brief  Software Timer Decrease Tick
* @param  
* @note   Called by Basic timer interrupt IRQ handler, decrease the counters
* @retval 
*******************************************************************************/
void SWT_DecreaseTick(void)
{
    uint8_t i = 0;
    
    /* Every software timer tick counter decrement until zero */
    for( i = 0; i < SWT_ID_TOTAL_NUM; i++ )
    {
        if( sw_timer_tick[i] != 0 )
        {
            sw_timer_tick[i]--;
        }
    }
}

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
