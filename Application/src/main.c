/*
***************************************************************************************************
*                               Application Main Entrance
*
* File   : main.c
* Author : Douglas Xie
* Date   : 2016.06.16
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ----------------------------------------------------------------------------*/
#include "mcu_config.h"
#include "system.h"
#include "power_task.h"
#include "nfc_task.h"
#include "comm_task.h"
#include "wdt.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   MCU Main Application
* @Param   
* @Note    Main application entry point, running all application function
* @Return  
*******************************************************************************/
/* Application entry point */

int main(void)
{       
    /* MCU Power On Initial */
    MCU_PowerOnInit();
                
    /*------------ Application Task Runtime Routine -------------------------*/
    while(1)
    {
        System_PeriodTaskManagement();
        
        System_PowerOnTask();
        
        MCU_OVPLevelTriggerTask();
                Comm_Task();
        
        NFC_CommTask();
        
        Power_ControlLoopTask();
        
        System_SleepTask();
        
        System_WakeupTask();

        
        /* Feed watchdog */
        WDT_Feeds();
        
        __WFI();
    }
}

