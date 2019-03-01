/*
***************************************************************************************************
*                               System Tick Timer Implementation
*
* File   : tick.h
* Author : Douglas Xie
* Date   : 2016.04.26
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef _TICK_H
#define _TICK_H

/* Include Head Files ----------------------------------------------------------------------------*/
#include "xmc_scu.h"
#include "adc.h"
#include "sw_timer.h"
#include "power_task.h"

/* Macro Defines ---------------------------------------------------------------------------------*/
/* System tick frequency define */
#define TICKS_FREQ_NORMAL   (2400)  /* 2.4kHz: it can be 2n*Freq of 50Hz and 60Hz */
#define TICKS_FREQ_LOW      (100)   /* 100Hz: 10ms wakeup again */


/* Global Variable -------------------------------------------------------------------------------*/


/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Set System Tick Timer Interrupt Frequency
* @Param   [in]tick_freq: system tick frequency value
* @Note    input value can be TICKS_FREQ_NORMAL or TICKS_FREQ_LOW
* @Return  
*******************************************************************************/
void Tick_SetFrequency(uint16_t tick_freq);

/*******************************************************************************
* @Brief   Get System Tick Counter
* @Param   
* @Note    tick counter unit is ms
* @Return  return tick counter value
*******************************************************************************/
uint32_t Tick_GetTicks(void);

/*******************************************************************************
* @Brief   Get System Tick Interrupt Frequency
* @Param   
* @Note    
* @Return  return tick frequency
*******************************************************************************/
uint16_t Tick_GetTickFreq(void);

#endif /* _TICK_H */ 
