/*
***************************************************************************************************
*                                  System Task Control Center
*
* File   : system.c
* Author : Douglas Xie
* Date   : 2016.04.18
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/


/* Include Head Files ---------------------------------------------------------------------------*/
#include "system.h"

/* Macro Defines --------------------------------------------------------------------------------*/


/* Private Variable -----------------------------------------------------------------------------*/
volatile uint32_t  g_system_task_state = 0x00000000;  /* System Task state: max 32 tasks */
volatile uint8_t   g_work_mode;        


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/


/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @brief  System Task Manager Initial
* @param  
* @note   Reset the task state register
* @retval  
*******************************************************************************/
void System_TaskInit(void)
{
    g_system_task_state = 0x00000000;
}


/*******************************************************************************
* @brief  Create System Task
* @param  
* @note   Set task register bit to enable the task
* @retval  
*******************************************************************************/
void System_CreateTask(uint32_t task_id)
{
    g_system_task_state |= task_id;
}


/*******************************************************************************
* @brief  Close System Task
* @param  
* @note   Clear task register bit to disable the task
* @retval  
*******************************************************************************/
void System_CloseTask(uint32_t task_id)
{
    g_system_task_state &= ~task_id;
}


/*******************************************************************************
* @brief  Check Task Enable State
* @param  
* @note   Check System Task Enable State and return it 
* @retval SYS_TASK_ENABLE / SYS_TASK_DISABLE
*******************************************************************************/
uint8_t System_CheckTask(uint32_t task_id)
{
    uint8_t byState = SYS_TASK_DISABLE;
    
    if( g_system_task_state & task_id )
    {
        byState = SYS_TASK_ENABLE;
    }
    
    return byState;
}


/*******************************************************************************
* @brief  System Power On Task
* @param  
* @note   
* @retval 
*******************************************************************************/
void System_PowerOnTask(void)
{
    static uint8_t state = 0;       /* task state */
    static uint32_t systick = 0;
    extern uint16_t g_one2ten_avg_adc;
    /* Check if task has been created */
    if( System_CheckTask(SYS_TASK_POWER_ON) == SYS_TASK_DISABLE )
    {
        return;
    }
    
    /* Get current system tick counter */
    systick = Tick_GetTicks();
    
    /* Power task state machine */
    switch(state)
    {
        /* state of delay for initial ACMP */
    case POWER_ON_STATE_ACMP_EN: 
        /* Check delay for enable ACMP */
        if(systick >= SYS_ACMP_EN_DELAY)
        {
            /* enable acmp for detect OVP & OCP */
            ACMP_EnableOVP();
            //            ACMP_EnableOCP();
            
            /* Disable control pwm accelerate pin to avoid shoot current */
            PWM_CtrlAccelerateDisable();
            
            /* swtich to next state */
            state++;
        }
        break;   
        
        /* state of delay for enable ERU interrupt */
    case POWER_ON_STATE_ERU_EN:
        /* Check delay for enable ERU */
        if(systick >= SYS_ERU_EN_DELAY)
        {       
#ifdef ENABLE_OVP
            /* enable eru for trigger OVP protection */
            ERU_EnableOVP();
            ACMP_EnableLevelEvent_OVP();
#endif
            
            /* enable eru for trigger OCP protection */
            //ERU_EnableOCP();
            
            /* swtich to next state */
            state++;
        }
        break;
        
        /* state of delay for NFC task init and prepare OVP OCP reference */
    case POWER_ON_STATE_NFC_INIT:
        /* Check delay for NFC task init */
              g_one2ten_avg_adc = ADC_GetAverage(ADC_CHANNEL_DIM);
        if(g_one2ten_avg_adc<ONE_TEN_LOW_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;
        if(g_one2ten_avg_adc>ONE_TEN_HIGH_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_HIGH_VOLTAGE; 
          //g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;//Add Moon
        //Power_UpdateOne2TenDimming(g_one2ten_avg_adc);
        if(systick >= SYS_NFC_INIT_DELYA)
        {            
            /* read target current and prepare reference */
            Power_PrepareReference();
            
            /* Power on NFC tag */
            MCU_NfcPowerPinSet();
            
            /* nfc task init */
            NFC_TaskInit();
            
            /* Create nfc communication task */
            System_CreateTask(SYS_TASK_NFC);  
            
            /* swtich to next state */
            state++;            
        }
        break;
        
        /* state of delay for set current init pwm duty */
    case POWER_ON_STATE_POWER_INIT:
        /* Check delay for read NFC config */
              g_one2ten_avg_adc = ADC_GetAverage(ADC_CHANNEL_DIM);
        if(g_one2ten_avg_adc<ONE_TEN_LOW_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;
        if(g_one2ten_avg_adc>ONE_TEN_HIGH_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_HIGH_VOLTAGE; 
          //g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;//Add Moon
      //Power_UpdateOne2TenDimming(g_one2ten_avg_adc);
        if(systick >= SYS_POWER_INIT_DELYA)
        {
            /* Power control loop task init */
            Power_TaskInit();
            
            /* swtich to next state */
            state++;            
        }
        break;
        
        /* state of delay for initial ADC and calibrate ADC */
    case POWER_ON_STATE_ADC_INIT:
        /* Check delay for enable ADC */
        if(systick >= SYS_ADC_INIT_DELYA)
        {           
            /* enable ADC again for output current and voltage monitor */
            ADC_Enable();
            
            /* swtich to next state */
            state++;            
        }
        break;
        
        /* state of delay for start control loop */
    case POWER_ON_STATE_LOOP_START:
        /* Check delay for create control loop task */
        if(systick >= SYS_CTRL_LOOP_START_DELAY)
        {           
            /* Create power control loop task */
            System_CreateTask(SYS_TASK_LOOP);
            
            /* swtich to next state */
            state++;
        }
        break;
        
        /* state of delay for finish power on task */
    case POWER_ON_STATE_END:
        /* Close power on task */
        System_CloseTask(SYS_TASK_POWER_ON);
        //Power_SetCurrent(700, SET_MODE_NORMAL);
        break;
        
    default: /* error state */
        /* enter infinite loop, wait for watchdog reset MCU */
        while(1);
        break;
    }
    
    return;
}


/*******************************************************************************
* @brief  System Power Off Task
* @param  
* @note   
* @retval 
*******************************************************************************/
void System_PowerOffTask(void)
{
    /* Check if task has been created */
    if( System_CheckTask(SYS_TASK_POWER_OFF) == SYS_TASK_DISABLE )
    {
        return;
    }
    
    /* TASK IMPLEMENTATION FROM HERE */
    
    System_CloseTask(SYS_TASK_POWER_OFF);
}


/*******************************************************************************
* @brief  System Sleep Task
* @param  
* @note   
* @retval 
*******************************************************************************/
void System_SleepTask(void)
{
    /* Check if task has been created */
    if( System_CheckTask(SYS_TASK_SLEEP) == SYS_TASK_DISABLE )
    {
        return;
    }
    
    /* MCU clock low speed config*/
    MCU_ClockConfigLowSpeed();   
    
    /* Disable system tick timer */
    MCU_SysTickDisable();
    
    /* Disable PWM module */
    PWM_Disable();
    
    /* Disable ACMP and ERU for detect OVP & OCP */
    ACMP_DisableOVP();
    ERU_DisableOVP();
    
    /* Disable ADC */
    ADC_Disable();
    
    /* Disable USART */
    USART_Disable(); 
    
    /* System task controler init */
    System_TaskInit();
    
    /* Software timer init */
    SWT_Init();
    
    /* Simulate I2C init */
    I2C_Initial();
    
    /* Feed watchdog for refresh timer */
    WDT_Feeds();
    
    System_CloseTask(SYS_TASK_SLEEP);
}


/*******************************************************************************
* @brief  System Wakeup Task
* @param  
* @note   
* @retval 
*******************************************************************************/
void System_WakeupTask(void)
{
    /* Check if task has been created */
    if( System_CheckTask(SYS_TASK_WAKEUP) == SYS_TASK_DISABLE )
    {
        return;
    }
    
    /* TASK IMPLEMENTATION FROM HERE */
    
    System_CloseTask(SYS_TASK_WAKEUP);
}


/*******************************************************************************
* @brief  System Work Mode Change Task
* @param  
* @note   
* @retval 
*******************************************************************************/
void System_WorkModeChangeTask(void)
{
    /* Check if task has been created */
    if( System_CheckTask(SYS_TASK_MODE_CHANGE) == SYS_TASK_DISABLE )
    {
        return;
    }
    
    /* TASK IMPLEMENTATION FROM HERE */
    
    System_CloseTask(SYS_TASK_MODE_CHANGE);
}


/*******************************************************************************
* @brief  System Period Task Management
* @param  
* @note   Check timing task which are enabled and run the handler when time up
* @retval
*******************************************************************************/
void System_PeriodTaskManagement(void)
{
    /* Power control loop task period */
    if(SWT_CheckTimer(SWT_ID_LOOP) == SWT_STATUS_UP)
    {        
        /* Create power control loop task */
        System_CreateTask(SYS_TASK_LOOP); 
    }
    
    /* NFC communication task period */
    if(SWT_CheckTimer(SWT_ID_NFC) == SWT_STATUS_UP)
    {        
        /* Create nfc communication task */
        System_CreateTask(SYS_TASK_NFC);  
    }
    
    /* Hiccup period timer for OVP and OCP */
    if(SWT_CheckTimer(SWT_ID_HICCUP) == SWT_STATUS_UP)
    {        
        /* Exit Protection */
        PWM_ExitProtection();
    }  
    
    /* OVP accelerate pin release */
    if(SWT_CheckTimer(SWT_ID_SPEED_UP) == SWT_STATUS_UP)
    {        
        /* Release control accerlerate pin to avoid lock primary IC */
        PWM_CtrlAccelerateDisable();
        
        SWT_CloseTimer(SWT_ID_SPEED_UP);
    }
}

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
