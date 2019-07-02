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
  extern uint8_t g_nfc_ini_flag,g_loop_run,g_current_non_0_flag;
  extern Power_State_t g_control_loop_state;
  extern uint16_t g_traget_judge_current,g_uout_real;

    /* MCU Power On Initial */
    MCU_PowerOnInit();   
    
    //AstroInit(); 
    P2_1_enable_digital();
       P2_1_set_mode(OUTPUT_PP_GP);//update       
    /*------------ Application Task Runtime Routine -------------------------*/
    while(1)
    {
        
        if(g_current_non_0_flag==0)
        {
          if(PWM_GetProtectState() != PWM_STATE_PROTECT)
          if(g_control_loop_state==POWER_STATE_INCREASE&&g_loop_run==1)
          {
              P2_1_toggle(); 
              if(g_traget_judge_current>200)
              {
                 PWM_DutyStepUp(PWM_ID_CH_CTRL, 30);
              }
              else
              PWM_DutyStepUp(PWM_ID_CH_CTRL, 5);             
               
          }
        }
        System_PeriodTaskManagement();
        
        System_PowerOnTask();
        
        MCU_OVPLevelTriggerTask();
        
        //Comm_Task();
        
        Power_nfc_handle();
        
        Power_ControlLoopTask();
        //System_SleepTask();
        
        //System_WakeupTask();
        
        if(g_nfc_ini_flag==1)
        {
        DaliBallast_CyclicTask();
        }


        //P2_1_toggle();
        /* Feed watchdog */
        WDT_Feeds();
        
        __WFI();
    }
}

