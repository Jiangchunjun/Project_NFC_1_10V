/*
***************************************************************************************************
*                             Watchdog Timer Implementation
*
* File   : wdt.c
* Author : Douglas Xie
* Date   : 2016.06.24
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifdef ENABLT_WDT

/* Include Head Files ----------------------------------------------------------------------------*/
#include "wdt.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/

/* Private Variable ------------------------------------------------------------------------------*/

/* Constant Variable -----------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable Watchdog Timer
* @Param   
* @Note    config window watchdog boundary and start timer
*          watchdog timeout counter is set by WDT_UPPER_BOUND
* @Return  
*******************************************************************************/
void WDT_Enable(void)
{    
    XMC_WDT_CONFIG_t wdt_config =
    {
        .window_lower_bound = WDT_LOWER_BOUND,
        .window_upper_bound = WDT_UPPER_BOUND,
        .service_pulse_width = WDT_PULSE_WIDTH
    };
    
    /* Clear system reset status */
    XMC_SCU_RESET_ClearDeviceResetReason();
    
    /* Config watchdog boundary and start timer */
    XMC_WDT_Init(&wdt_config);
    XMC_WDT_SetMode(XMC_WDT_MODE_TIMEOUT);
    XMC_WDT_Start();
}


/*******************************************************************************
* @Brief   Disable Watchdog Timer
* @Param   
* @Note    stop the watchdog timer and disable it
* @Return  
*******************************************************************************/
void WDT_Disable(void)
{
    /* Stop watchdog timer and disable it */
    XMC_WDT_Stop();
    XMC_WDT_Disable();
}


/*******************************************************************************
* @Brief   Feed Watchdog Timer
* @Param   
* @Note    Service the watchdog to reset counter when the counter is in the window
* @Return  
*******************************************************************************/
void WDT_Feeds(void)
{
    /* Feeds the watchdog by writing the Magic word */
    XMC_WDT_Service();
    
//    /* Feeds action should be active if counter is between lower bound and upper bound */
//    uint32_t count = XMC_WDT_GetCounter();
//    
//    if(count > WDT_LOWER_BOUND)
//    {
//        /* Feeds the watchdog by writing the Magic word */
//        XMC_WDT_Service();
//    } 
}

#endif /* ENABLT_WDT */
