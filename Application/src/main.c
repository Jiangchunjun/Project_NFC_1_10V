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
  extern uint8_t g_nfc_ini_flag;
        static uint8_t save=0;
    /* MCU Power On Initial */
    MCU_PowerOnInit();       
             
    /*------------ Application Task Runtime Routine -------------------------*/
    while(1)
    {
        
        
        System_PeriodTaskManagement();
        
        System_PowerOnTask();
        
        MCU_OVPLevelTriggerTask();
        
        //Comm_Task();
        
      
        Power_ControlLoopTask();
        System_SleepTask();
        
        System_WakeupTask();
        
        if(g_nfc_ini_flag==1)
        {
        DaliBallast_CyclicTask();
        }


        //P2_1_toggle();
        /* Feed watchdog */
        WDT_Feeds();
        
        //__WFI();
    }
}

