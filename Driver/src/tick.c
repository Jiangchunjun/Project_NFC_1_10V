/*
***************************************************************************************************
*                               System Tick Timer Implementation
*
* File   : tick.c
* Author : Douglas Xie
* Date   : 2016.04.26
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ----------------------------------------------------------------------------*/
#include "tick.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/
volatile uint32_t g_global_ticks = 0;
volatile uint16_t g_tick_frequency = 0;


/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Set System Tick Timer Interrupt Frequency
* @Param   [in]tick_freq: system tick frequency value
* @Note    input value can be TICKS_FREQ_NORMAL or TICKS_FREQ_LOW
* @Return  
*******************************************************************************/
void Tick_SetFrequency(uint16_t tick_freq)
{
    /* Set systick interval */
    SysTick_Config(SystemCoreClock / tick_freq);
    
    g_tick_frequency = tick_freq;
}

/*******************************************************************************
* @Brief   Get System Tick Counter
* @Param   
* @Note    
* @Return  return tick counter value
*******************************************************************************/
uint32_t Tick_GetTicks(void)
{
    return g_global_ticks;
}

/*******************************************************************************
* @Brief   Get System Tick Interrupt Frequency
* @Param   
* @Note    
* @Return  return tick frequency
*******************************************************************************/
uint16_t Tick_GetTickFreq(void)
{
    return g_tick_frequency;
}

/*******************************************************************************
* @Brief   Set System Tick Timer IRQ Handler
* @Param   
* @Note    running tick action 
* @Return  
*******************************************************************************/
void SysTick_Handler(void)
{       
    /* Update global tick counter for software delay */
    g_global_ticks++;
    
    /* Read ADC to buffer */
    ADC_ReadResult();
    
    /* Update software timer ticks */
    SWT_DecreaseTick();
    
    /* Update nfc task software timer */
    NFC_UpdateTimer();
}

