/*
***************************************************************************************************
*                             Watchdog Timer Implementation
*
* File   : wdt.h
* Author : Douglas Xie
* Date   : 2016.06.24
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef WDT_H
#define WDT_H

#ifdef ENABLT_WDT

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_wdt.h>
#include <xmc_scu.h>
#include <xmc_gpio.h>


/* Macro Defines ---------------------------------------------------------------------------------*/
/* Watchdog timer property */
#define WDT_CLOCK_32KHZ     (32000)     /* WDT use 32kHz internal clock from SCU */
#define WDT_UNIT_MS         (32)        /* Count per ms */

/* Watchdog timer bounds define */
#define WDT_LOWER_BOUND     (0 * WDT_UNIT_MS)       /* 0ms, recommend larger than 0ms */
#define WDT_UPPER_BOUND     (300 * WDT_UNIT_MS)     /* 300ms, WDT timeout boundary */

/* Watchdog timer output pulse width define */
#define WDT_PULSE_WIDTH     (255)

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable Watchdog Timer
* @Param   
* @Note    config window watchdog boundary and start timer
*          watchdog timeout counter is set by WDT_UPPER_BOUND
* @Return  
*******************************************************************************/
void WDT_Enable(void);

/*******************************************************************************
* @Brief   Disable Watchdog Timer
* @Param   
* @Note    stop the watchdog timer and disable it
* @Return  
*******************************************************************************/
void WDT_Disable(void);

/*******************************************************************************
* @Brief   Feed Watchdog Timer
* @Param   
* @Note    Service the watchdog to reset counter when the counter is in the window
* @Return  
*******************************************************************************/
void WDT_Feeds(void);

#else

/* define blank function when disabled */
#define WDT_Enable()        
#define WDT_Disable()       
#define WDT_Feeds()     

#endif /* ENABLT_WDT */

#endif /* WDT_H */ 

