/*
***************************************************************************************************
*                               NFC Communication Task
*
* File   : nfc_task.c
* Author : Douglas Xie
* Date   : 2016.08.30
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "nfc_task.h"

/* Macro Defines --------------------------------------------------------------------------------*/

/* Private Variable -----------------------------------------------------------------------------*/
extern uint8_t g_moble_flag;
/* Global Variable ------------------------------------------------------------------------------*/

/* Function Declaration -------------------------------------------------------------------------*/

/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   NFC Task Initial
* @Param   
* @Note    Initial NFC relative module and write default when nfc is empty
* @Return  
*******************************************************************************/
void NFC_TaskInit(void)
{   
    uint8_t temp_data[3]={0,0,0};
    static uint32_t temp_mobile=0;
    static uint8_t  flag_nfc_ini=0;
    
    if(!flag_nfc_ini)
    {
        nfcInit();  
        flag_nfc_ini=1;
    }
   
#ifdef ENABLE_MOBILE
    M24LRxx_ReadBuffer(NFC_MOBILE_REGISTER,3,temp_data);    
    temp_mobile+=temp_data[0]<<16;
    temp_mobile+=(temp_data[1]<<8);
    temp_mobile+=(temp_data[2]<<0);
    
    if(NFC_MOBLIE_DATA!=temp_mobile)
    {   
#endif /* ENABLE_MOBILE */
        
        nfcDali_Init();
        nfc_bank_ini();  
        g_nfc_time_id = nfc_time_id(); 
#ifdef TEST_ASTRO
        //M24LRxx_ReadBuffer(1064,19,g_astro_arr);
        mem_bank_nfc.mem_bank_astro.Astro_status=g_astro_arr[0];
        mem_bank_nfc.mem_bank_astro.Astro_control=g_astro_arr[1];
        mem_bank_nfc.mem_bank_astro.Astro_nom_level=g_astro_arr[2];
        mem_bank_nfc.mem_bank_astro.Astro_startup_fade_h=g_astro_arr[3];
        mem_bank_nfc.mem_bank_astro.Astro_startup_fade_l=g_astro_arr[4];
        mem_bank_nfc.mem_bank_astro.Astro_astro_fade=g_astro_arr[5];
        mem_bank_nfc.mem_bank_astro.Astro_switchoff_fade=g_astro_arr[6];
        mem_bank_nfc.mem_bank_astro.Astro_dim_starttime_h=g_astro_arr[7];
        mem_bank_nfc.mem_bank_astro.Astro_dim_starttime_l=g_astro_arr[8];
        mem_bank_nfc.mem_bank_astro.Astro_dim_level1=g_astro_arr[9];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration1_h=g_astro_arr[10];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration1_l=g_astro_arr[11];
        mem_bank_nfc.mem_bank_astro.Astro_dim_level2=g_astro_arr[12];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration2_h=g_astro_arr[13];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration2_l=g_astro_arr[14];  
        mem_bank_nfc.mem_bank_astro.Astro_dim_level3=g_astro_arr[15];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration3_h=g_astro_arr[16];
        mem_bank_nfc.mem_bank_astro.Astro_dim_duration3_l=g_astro_arr[17]; 
        mem_bank_nfc.mem_bank_astro.Astro_dim_level4=g_astro_arr[18];  // this is only for test using.
#endif
#ifdef ENASTRO
        if(mem_bank_nfc.mem_bank_astro.Astro_control&0X01)//Astro dimming is enbale
        {
            if(!(mem_bank_nfc.mem_bank_astro.Astro_control&0X02))//Astro mode enbale
            {
                nfc_ed_record();
                if(g_ed_time!=0)
                {
                  mem_bank_nfc.mem_bank_astro.Astro_status|=0X02;// Set ED valid flag  
                }
                mem_bank_nfc.mem_bank_astro.Astro_ed_time_l=(g_ed_time&0X00FF);
                mem_bank_nfc.mem_bank_astro.Astro_ed_time_h=(g_ed_time>>8);
                Astro_reset();
            }
            AstroInit();//only need to init this function when Astro is enable
        }
#endif
        
#ifdef ENABLE_MOBILE

        g_moble_flag=0;
    }
    else
    {
        g_moble_flag=1;
    }
    temp_mobile=0;
#endif /* ENABLE_MOBILE */
}


/*******************************************************************************
* @Brief   NFC Communication Task Routine
* @Param   
* @Note    NFC communication and record device lifetime
* @Return  
*******************************************************************************/
void NFC_CommTask(void)
{   
    if(!g_moble_flag) 
    {
        if(g_nfc_ini_flag)
        {     
            nfcManager();  // run nfcManger always
            /* Check if task has beed created */
            
            if(System_CheckTask(SYS_TASK_NFC) == SYS_TASK_DISABLE)
            {
                return;
            }
            
            /*----------- Task Routine -------------------------------------------*/
            /* Update handler */
            nfc_bank_update(); 
            nfc_bank_clm_handle();
            
            /* Update target current when NFC has new data */
            nfc_bank_pwm_control();  
            
            /* Record device lifetime */
            nfc_time_record();
            
            /* Psw update */
            nfc_psw_update();
            /*EOL handle*/
            nfc_eol_handle();
            nfc_nvm_time_write();
            /* write time in flash*/
            nfc_flash_write();
            /*update time in nvm*/
#ifdef ENASTRO
            /* Astro update */	
            if(mem_bank_nfc.mem_bank_astro.Astro_control&0X01)
            {
                
                AstroTimer();
           
            }
#endif
            /*------------ Task Finish -------------------------------------------*/
            /* Restart software timer for next running */
            SWT_StartTimer(SWT_ID_NFC, NFC_TASK_PERIOD);
            
            /* Close Task */
            System_CloseTask( SYS_TASK_NFC );
        }
    }
    else
    {
        nfc_mobile_handle();
    }
}


/*******************************************************************************
* @Brief   NFC Update Software Timer
* @Param   
* @Note    Call in systick timer handler, update nfc timer and record lifetime
* @Return  
*******************************************************************************/
void NFC_UpdateTimer(void)
{
    static uint32_t s_cnt = 0;
    static uint16_t s_time_count = 0;
    static uint8_t  s_time_record_count = 0;
    
    /* Systick timer base 2400 Hz */
    if(++s_cnt == 3)            
    {
        s_cnt = 0;
        
        /* Update every 1250 us */  
        nfcTimer();                   
    }
    
    /* Record driver lifetime every 1s */
    if(s_time_count++ >= Tick_GetTickFreq())  
    {
        /* Reset tick counter */
        s_time_count = 0;
        
        /* Update lifetime counter */
        g_nfc_time++;
        
        /* Write to eeprom every 60s */
        if(++s_time_record_count >= 60)//
        {
            /* Reset record counter */
            s_time_record_count = 0;
            
            /* Set record flag for writing */
            g_nfc_flag_record = 1;
        }
    }
}


/**********************************************************************************************************************
* This function returns the lenght in bytes of the specified memory bank minus 3
*
*/

uint8_t Mpc_GetLength(uint16_t bank)
{
    //uint8_t lenght;
    
    // lenght = get mem bank lenght
    //lenght -= 3;
    
    return 0;
}

#endif

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
