/*
***************************************************************************************************
*                               Output Power Control Loop Task
*
* File   : power_task.c
* Author : Douglas Xie
* Date   : 2016.04.26
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ---------------------------------------------------------------------------*/
#include "power_task.h"
#include "stdlib.h"
/* Macro Defines --------------------------------------------------------------------------------*/

/* Private Variable -----------------------------------------------------------------------------*/
/* OVP & OCP counter for active protection function */
uint8_t  g_OVP_counter = 0;
uint8_t  g_OCP_counter = 0;

/* Target Current Get from NFC Tag */
uint16_t g_target_current = 0; 
uint16_t g_max_voltage = 0; 
uint16_t g_min_current = IOUT_MIN; 
uint16_t g_astro_dimming_level = POWER_MAX_DIMMING;     /* 0.01% */
uint16_t g_constant_lumen = POWER_MAX_DIMMING;          /* 0.01% */
uint16_t g_eol_dimming_level = POWER_MAX_DIMMING;       /* 0.01% */
uint16_t g_one2ten_dimming_level = POWER_MAX_DIMMING;   /* 0.01% */
uint32_t g_s_period=0, g_a_period=30000;                /*period*/
/* Power Control Loop State */
Power_State_t g_control_loop_state = POWER_STATE_KEEP;

/* Current Adjustment Speed Choose Threshold */
uint16_t g_iout_threshold_high = 0;
uint16_t g_iout_threshold_mid = 0;
uint16_t g_iout_threshold_low = 0;

/* PWM Stable Range */
uint16_t g_pwm_stable_uout = PWM_STABLE_UOUT_0;
uint16_t g_pwm_stable_iout = PWM_STABLE_IOUT_0;

/* Output Current Stable Flag */
uint8_t g_current_stable_flag = OUTPUT_UNSTABLE;

/* Max PWM Duty Table */
uint16_t g_max_pwm_duty = PWM_DUTY_FULL; /* default 100.0% */
uint8_t g_max_duty_enable = 0;

/* Uout and Iout Real Value */
uint16_t g_uout_real = 0;       /* real voltage output value */
uint16_t g_iout_real = 0;       /* real current output value */

/* Uout and Iout Real Value */
uint16_t g_uout_avg_adc = 0;    /* voltage average adc */
uint16_t g_iout_avg_adc = 0;    /* current average adc */
uint16_t g_one2ten_avg_adc = 0; /* 1-10V input average adc */

/* Global Variable ------------------------------------------------------------------------------*/
uint16_t g_current_a=0;
uint8_t s_flag_off=0;
uint8_t s_one_ten_update=0;
uint8_t g_current_non_0_flag=0;
uint8_t g_loop_run=0;
uint16_t g_traget_judge_current=0;
/* Function Declaration -------------------------------------------------------------------------*/

/* Output Power Control Function Implement ------------------------------------------------------*/

/*******************************************************************************
* @Brief   Output Power Control Loop Initial
* @Param   
* @Note    Config ADC moudle
* @Return  
*******************************************************************************/
void Power_TaskInit(void)
{           
    /* Global vairable initial */
    g_OVP_counter = 0;
    g_OCP_counter = 0;
    g_control_loop_state = POWER_STATE_KEEP;
     
    /* Set to global variable */
    Power_SetCurrent(g_target_current, SET_MODE_NORMAL);//g_target_current
    
    /* Start timer for set real load voltage */
    SWT_StartTimer(SWT_ID_SET_LOAD, SET_LOAD_VOL_TIME);
    
    Filter_Init();
}


/*******************************************************************************
* @Brief   Get Output Power Level
* @Param   [in]target_current: user current setting value, unit is mA
* @Note    Get output current level for control loop choosing threshold
* @Return  Power_Level_t type level
*******************************************************************************/
Power_Level_t Power_GetPowerLevel(uint16_t target_current)
{
    Power_Level_t level = POWER_LEVEL_0;
    
    if(target_current < CURRENT_LEVEL_0)
    {
        level = POWER_LEVEL_0;
    }    
    else if(target_current < CURRENT_LEVEL_1)
    {
        level = POWER_LEVEL_1;
    }
    else if(target_current < CURRENT_LEVEL_2)
    {
        level = POWER_LEVEL_2;
    }
    else if(target_current < CURRENT_LEVEL_3)
    {
        level = POWER_LEVEL_3;
    }
    else if(target_current < CURRENT_LEVEL_4)
    {
        level = POWER_LEVEL_4;
    }
    else if(target_current < CURRENT_LEVEL_5)
    {
        level = POWER_LEVEL_5;
    }
    else if(target_current < CURRENT_LEVEL_6)
    {
        level = POWER_LEVEL_6;
    }
    else if(target_current < CURRENT_LEVEL_7)
    {
        level = POWER_LEVEL_7;
    }
    else if(target_current < CURRENT_LEVEL_8)
    {
        level = POWER_LEVEL_8;
    }
    else if(target_current < CURRENT_LEVEL_9)
    {
        level = POWER_LEVEL_9;
    }
    else if(target_current < CURRENT_LEVEL_10)
    {
        level = POWER_LEVEL_10;
    }
    else if(target_current < CURRENT_LEVEL_11)
    {
        level = POWER_LEVEL_11;
    }
    else
    {
        level = POWER_LEVEL_12;
    }
    
    return level;
}


/*******************************************************************************
* @Brief   Get Control Initial PWM Duty
* @Param   [in]level: target current level
* @Note    Get control initial pwm duty according to level
* @Return  
*******************************************************************************/
uint16_t Power_GetControlInitDuty(Power_Level_t level)
{
    uint16_t pwm_duty = 0;
    

    /* Get control initial pwm duty according to level */
    switch(level)
    {
    case POWER_LEVEL_0:
        pwm_duty = INIT_CTRL_DUTY_0;
        break;

    case POWER_LEVEL_1:
        pwm_duty = INIT_CTRL_DUTY_1;
        break;
        
    case POWER_LEVEL_2:
        pwm_duty = INIT_CTRL_DUTY_2;
        break;   
        
    case POWER_LEVEL_3:
        pwm_duty = INIT_CTRL_DUTY_3;
        break;  
        
    case POWER_LEVEL_4:
        pwm_duty = INIT_CTRL_DUTY_4;
        break;  
        
    case POWER_LEVEL_5:
        pwm_duty = INIT_CTRL_DUTY_5;
        break;  
        
    case POWER_LEVEL_6:
        pwm_duty = INIT_CTRL_DUTY_6;
        break;  
        
    case POWER_LEVEL_7:
        pwm_duty = INIT_CTRL_DUTY_7;
        break;  
        
    case POWER_LEVEL_8:
        pwm_duty = INIT_CTRL_DUTY_8;
        break; 
        
    case POWER_LEVEL_9:
        pwm_duty = INIT_CTRL_DUTY_9;
        break; 
                
    case POWER_LEVEL_10:
        pwm_duty = INIT_CTRL_DUTY_10;
        break; 

    case POWER_LEVEL_11:
        pwm_duty = INIT_CTRL_DUTY_11;
        break; 

    case POWER_LEVEL_12:
        pwm_duty = INIT_CTRL_DUTY_12;
        break; 

    default:
        pwm_duty = INIT_CTRL_DUTY_1;
        break; 
    }
    
    return pwm_duty;
}


/*******************************************************************************
* @Brief   Get OCP Trigger Point PWM Duty
* @Param   
* @Note    Get OCP trigger point pwm duty according to target current
* @Return  
*******************************************************************************/
uint16_t Power_GetOCPTriggerDuty(void)
{
    uint16_t pwm_duty = 0;
    uint16_t ocp_current = 0;
    
    /* Get OCP current */
#if 0
    ocp_current = g_target_current + OCP_TRIGGER_EXCEED;
#else
    ocp_current = OCP_CURRENT_CONST;
#endif
    
    /* Calculate OCP trigger point pwm duty */
    pwm_duty = (uint16_t)((OCP_TO_DUTY_D2 * ocp_current + OCP_TO_DUTY_D1) * ocp_current + OCP_TO_DUTY_D0);
    
    return pwm_duty;
}


/*******************************************************************************
* @Brief   Get OVP Trigger Point PWM Duty
* @Param   
* @Note    Get OVP trigger point pwm duty according to target max voltage
* @Return  
*******************************************************************************/
uint16_t Power_GetOVPTriggerDuty(void)
{
    uint16_t pwm_duty = 0;
    uint16_t ovp_voltage = 0;
    
    /* Get OVP voltage */
    ovp_voltage = g_max_voltage + OVP_TRIGGER_EXCEED;
    
    /* Calculate OCP trigger point pwm duty */
    pwm_duty = (uint16_t)((OVP_TO_DUTY_E2 * ovp_voltage + OVP_TO_DUTY_E1) * ovp_voltage + OVP_TO_DUTY_E0);
    
    return pwm_duty;
}


/*******************************************************************************
* @Brief   Prepare OVP and OCP Reference
* @Param   
* @Note    Read NFC tag and get OVP OCP reference before loop task begin
* @Return  
*******************************************************************************/
void Power_PrepareReference(void)
{
#ifdef DEBUG_VERSION
    uint8_t  state = ERROR;
    uint8_t  eep_data[4] = {0, 0, 0, 0};
    uint16_t input_current = 0;
    
    /* Read current setting from NFC tag */
    state = Tag_ReadEEP(0x0000, 4, eep_data);

    /* Select initial PWM duty according to target current */
    if(state == OK)
    {
        /* Get current setting value */
        input_current = eep_data[0] * 1000 + 
                        eep_data[1] * 100 + 
                        eep_data[2] * 10 + 
                        eep_data[3];    
        
        /* Set to global variable */
        Power_SetCurrent(input_current, SET_MODE_NORMAL);
        
        USART_PrintInfo("\nread tag success\n\n");
    }
    else
    {
        /* Set default output current */
        Power_SetCurrent(POWER_DEFALUT_CURRENT, SET_MODE_NORMAL);
    
        USART_PrintInfo("\nread tag error\n\n");
    }
    
    /* Write firmware version to NFC tag */
    eep_data[0] = FW_VERSION_MAJOR;
    eep_data[1] = FW_VERSION_MINOR;
    eep_data[2] = HW_VERSION_MAJOR;
    eep_data[3] = HW_VERSION_MINOR;
    state = Tag_WriteEEP(0x0008, 4, eep_data);  /* write in block 2, SW + HW */
#endif /* DEBUG_VERSION */
}

/*******************************************************************************
* @Brief   Set Target Output Power Current
* @Param   [in]target_current: user current setting value, unit is mA
*          [in]mode: SET_MODE_NORMAL / SET_MODE_POWER_ON
* @Note    set target current value to variable and calculate max_voltage
*          provide a init pwm duty according to the target current
*          update OCP & OVP trigger voltage
* @Return  
*******************************************************************************/
void Power_SetCurrent(uint16_t target_current, uint8_t mode)
{
    uint16_t ctrl_duty = 0;
    uint16_t ocp_duty = 0;
    uint16_t ovp_duty = 0;
    Power_Level_t power_level = POWER_LEVEL_1;
   //target_current=70;// add testing value Moon
    /* Validate input current setting */
    if(target_current < g_min_current)
    {
        /* Limit to minimum value */
        target_current = g_min_current;
    }
    else if(target_current > IOUT_MAX)
    {
        /* Limit to maximum value */
        target_current = IOUT_MAX;
    }
    
    /* Update control loop current adjustment speed threshold */
    if(target_current < IOUT_DIVIDER_1_0)
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_0;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_0;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_0;  
        g_pwm_stable_uout = PWM_STABLE_UOUT_0;
        g_pwm_stable_iout = PWM_STABLE_IOUT_0;
    }
    else if(target_current < IOUT_DIVIDER_2_1)
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_1;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_1;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_1;   
        g_pwm_stable_uout = PWM_STABLE_UOUT_1;
        g_pwm_stable_iout = PWM_STABLE_IOUT_1;
    }
    else
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_2;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_2;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_2;   
        g_pwm_stable_uout = PWM_STABLE_UOUT_2;
        g_pwm_stable_iout = PWM_STABLE_IOUT_2;
    }

    /* Update when target current change */
    if((target_current != g_target_current) || (mode == SET_MODE_POWER_ON))
    {
        /* Calculate average ADC */
        ADC_CalculateAverage();
        
        /* Get raw data(average ADC) */
        g_iout_avg_adc = ADC_GetAverage(ADC_CHANNEL_IOUT);
        g_uout_avg_adc = ADC_GetAverage(ADC_CHANNEL_UOUT);
        
        /* Update 1-10V Dimming Level */
        g_one2ten_avg_adc = ADC_GetAverage(ADC_CHANNEL_DIM);
        if(g_one2ten_avg_adc<ONE_TEN_LOW_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;
        if(g_one2ten_avg_adc>ONE_TEN_HIGH_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_HIGH_VOLTAGE;     
          //g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;
        Power_UpdateOne2TenDimming(g_one2ten_avg_adc);
#ifndef ENABLE_ONE2TEN
        g_one2ten_dimming_level = POWER_MAX_DIMMING;
#endif
        //g_one2ten_dimming_level=10000;
        
        /*-------- Update Target Current -------------------------------------*/
        /* Reset current to not stable */
        g_current_stable_flag = OUTPUT_UNSTABLE; 
        g_max_duty_enable = 0;
        g_max_pwm_duty = PWM_DUTY_FULL;
    
        /* Set to global variable */
        g_target_current = target_current;
        g_max_voltage = (uint16_t)((uint32_t)OUTPUT_POWER * POWER_UOUT_TOLERANCE / g_target_current);
        
        /* Global max voltage limit*/
        if(g_max_voltage > UOUT_MAX)
        {
            g_max_voltage = (uint16_t)(UOUT_MAX * POWER_UOUT_TOLERANCE);
        }
     
        /*-------- Get New Target PWM Duty -----------------------------------*/
        /* Start current with astro dimming control */
        target_current = (uint16_t)((uint32_t)g_target_current * g_astro_dimming_level / POWER_MAX_DIMMING);
        /* Start current with constant lumen control */
        target_current = (uint16_t)((uint32_t)target_current * g_constant_lumen / POWER_MAX_DIMMING);
        /* Start current with eol dimming level */
        target_current = (uint16_t)((uint32_t)target_current * g_eol_dimming_level / POWER_MAX_DIMMING);
#ifdef ENABLE_ONE2TEN
        /* Target current with 1-10V dimming level */
        target_current = (uint16_t)((uint32_t)target_current * g_one2ten_dimming_level / POWER_MAX_DIMMING);
#endif
        
        if(target_current < g_min_current)
        {
            target_current = g_min_current;
        }
        
        /* Update power level */
        target_current=(target_current*g_one2ten_dimming_level/10000);

        power_level = Power_GetPowerLevel(target_current);        
        
        /* Get initial pwm duty accroding to power level */
        
        ctrl_duty = Power_GetControlInitDuty(power_level); 
        
        /* Get ocp trigger point pwm duty accroding to target current */
        ocp_duty = Power_GetOCPTriggerDuty(); 
        
        /* Get ovp trigger point pwm duty accroding to target max voltage */
        ovp_duty = Power_GetOVPTriggerDuty(); 
        
        /*-------- Update PWM Duty -------------------------------------------*/
        if(mode == SET_MODE_POWER_ON)
        {
            if(PWM_GetProtectState() != PWM_STATE_PROTECT)
            {
                /* Provide init pwm duty for fast adjustment */
                PWM_SetDuty(PWM_ID_CH_CTRL, ctrl_duty, PWM_MODE_LIMIT);
            }
        }
        
        /* Update OCP trigger voltage for short protection */
        PWM_SetDuty(PWM_ID_CH_OCP, ocp_duty, PWM_MODE_LIMIT);
        
        /* Update OVP trigger voltage for open protection */
        PWM_SetDuty(PWM_ID_CH_OVP, ovp_duty, PWM_MODE_LIMIT);
    }
}


/*******************************************************************************
* @Brief   Get Target Output Current
* @Param  
* @Return  target current that save in g_target_current
*******************************************************************************/
uint16_t Power_GetCurrent(void)
{
    return g_target_current;
}


/*******************************************************************************
* @Brief   Get Minmum Dimming Level
* @Param   
* @Note    calculate minmum dimming level according to the minmum iout and iset
* @Return  minmum dimming level(percent with unit 0.01%)
*******************************************************************************/
uint16_t Power_GetMinLevel(void)
{
    uint16_t minmum_level = 0;
    
    /* Calculate minmum dimming level */
    if(g_target_current != 0)
    {
        minmum_level = (uint16_t)((uint32_t)g_min_current * 10000 / g_target_current);
    }
    else
    {
        minmum_level = (uint16_t)((uint32_t)g_min_current * 10000 / IOUT_MAX);
    }
    
    return (minmum_level);
}


/*******************************************************************************
* @Brief   Set Astro Dimming Level
* @Param   [in]percent: dimming percent value, unit is 0.01%
* @Note    set dimming value of dimming mode which are set from T4T
* @Return  
*******************************************************************************/
void Power_SetAstroDimmingLevel(uint16_t percent)
{
    /* Check input value */
    if(percent > POWER_MAX_DIMMING)
    {
        percent = POWER_MAX_DIMMING;
    }
    
    if(g_astro_dimming_level != percent)
    {    
        /* Set to global variable */
        g_astro_dimming_level = percent;
        
        /* Reset current to not stable */
        g_current_stable_flag = OUTPUT_UNSTABLE; 
        g_max_duty_enable = 0;
        g_max_pwm_duty = PWM_DUTY_FULL;
    } 
}


/*******************************************************************************
* @Brief   Set Constant Lumen Value
* @Param   [in]percent: constant lumen percent value, unit is 0.01%
* @Note    set constant lumen dimming percent value which are set from T4T
* @Return  
*******************************************************************************/
void Power_SetConstantLumenValue(uint16_t percent)
{
    /* Check input value */
    if(percent > POWER_MAX_DIMMING)
    {
        percent = POWER_MAX_DIMMING;
    }
    
    if(g_constant_lumen != percent)
    {
        /* Set to global variable */
        g_constant_lumen = percent;
        
        /* Reset current to not stable */
        
        g_current_stable_flag = OUTPUT_UNSTABLE; 
        g_max_duty_enable = 0;
        g_max_pwm_duty = PWM_DUTY_FULL;
    }
}

/*******************************************************************************
* @Brief   Set EOL Dimming Level
* @Param   [in]percent: eol dimming percentage value, unit is 0.01%
* @Note    
* @Return  
*******************************************************************************/
void Power_SetEolDimmingLevel(uint16_t percent)
{
    /* Check input value */
    if(percent > POWER_MAX_DIMMING)
    {
        percent = POWER_MAX_DIMMING;
    }
    
    if(g_eol_dimming_level != percent)
    {
        /* Set to global variable */
        g_eol_dimming_level = percent;
        
        /* Reset current to not stable */
        g_current_stable_flag = OUTPUT_UNSTABLE; 
        g_max_duty_enable = 0;
        g_max_pwm_duty = PWM_DUTY_FULL;
    }
}


/*******************************************************************************
* @Brief   Update 1_10V Dimming Level
* @Param   [in]adc: 1-10V input voltage adc
* @Note    
* @Return  
*******************************************************************************/
void Power_UpdateOne2TenDimming(uint16_t adc)
{
    float level = 0;
    uint16_t difference = 0,next_level=0;
    uint32_t temp_data=0;
    static int32_t temp1=0;
    static int32_t temp2=0;
    static uint32_t dim_level_1_10=0;
    static uint32_t s_min_level=0;
    /* Calculate 1-10V dimming level */
    
    if(O2T_GetEnableConfig()==0) //TDL
    {
      g_one2ten_dimming_level= POWER_MAX_DIMMING;     
    }
    else                                          //TDL
    {
      //s_min_level=(mem_bank_nfc.mem_bank_1_10.Level_h_1_10<<8)+mem_bank_nfc.mem_bank_1_10.Level_l_1_10; //TDL
      s_min_level=O2T_GetMinDimLevel();
      s_min_level*=10000;
      s_min_level>>=15;
      
      temp_data=(POWER_MAX_DIMMING-s_min_level)*(adc-ONE_TEN_LOW_VOLTAGE);
      level=temp_data;
      level/=(ONE_TEN_HIGH_VOLTAGE-ONE_TEN_LOW_VOLTAGE);
      level+=s_min_level;
      //level = (float)ONE2TEN_PARAM_C1 * adc + (float)ONE2TEN_PARAM_C0;
      /********Get level filter value******/
      temp1 =(uint32_t)level;//
      
      if(temp1>ONE2TEN_UPPER_LIMIT)
        
        temp1=ONE2TEN_UPPER_LIMIT;
      
      if(temp1<ONE2TEN_LOWER_LIMIT)
        
        temp1=ONE2TEN_LOWER_LIMIT;
      
      temp2 += (((temp1<<10)- temp2)>>4);
      
      dim_level_1_10=temp2>>10;
      
      if(abs(dim_level_1_10-(uint16_t)(level))<50)
      {
        level=(float)dim_level_1_10;
      }
      /* Dimming level change difference */
      if(level > g_one2ten_dimming_level)
      {
        difference = (uint16_t)level - g_one2ten_dimming_level;
      }
      else
      {
        difference = g_one2ten_dimming_level - (uint16_t)level;
      }
      
      /* Validate dimming result */
      if(level > ONE2TEN_UPPER_LIMIT)
      {
        difference = ONE2TEN_UPPER_LIMIT - g_one2ten_dimming_level;        
        next_level = ONE2TEN_UPPER_LIMIT;
      }
      else if(level < ONE2TEN_LOWER_LIMIT)
      {
        difference = g_one2ten_dimming_level - ONE2TEN_LOWER_LIMIT;        
        next_level = ONE2TEN_LOWER_LIMIT;
      }
      else
      {
        next_level = (uint16_t)level;
      }
      
      /* Update dimming level with rank filter */
      //g_one2ten_dimming_level = Filter_Input(next_level);//moon update
      g_one2ten_dimming_level=(uint16_t)level;
      //g_one2ten_dimming_level=next_level;
      //g_one2ten_dimming_level=1000; //10% testing using
      /* Stable flag update for speed up */
      //g_one2ten_dimming_level=5000;
      if(difference >= POWER_DIM_UNSTABLE_DUTY)
      {
        s_one_ten_update=1;
      }
      {
        g_current_stable_flag = OUTPUT_UNSTABLE; 
        g_max_duty_enable = 0;
        g_max_pwm_duty = PWM_DUTY_FULL;
      }
    }
}

/*******************************************************************************
* @Brief   Get Power Loop State
* @Param   
* @Note    
* @Return  power loop state
*******************************************************************************/
Power_State_t Power_GetPowerLoopState(void)
{
    return g_control_loop_state;
}

/*******************************************************************************
* @Brief   Get Real Output Current Value
* @Param   
* @Note    
* @Return  g_iout_real
*******************************************************************************/
uint16_t Power_GetRealIout(void)
{
    return g_iout_real;
}

/*******************************************************************************
* @Brief   Get Real Output Voltage Value
* @Param   
* @Note    
* @Return  g_uout_real
*******************************************************************************/
uint16_t Power_GetRealUout(void)
{
    return g_uout_real;
}

/*******************************************************************************
* @Brief   Get Output Current Average ADC
* @Param   
* @Note    
* @Return  g_iout_real
*******************************************************************************/
uint16_t Power_GetIoutAvgADC(void)
{
    return g_iout_avg_adc;
}

/*******************************************************************************
* @Brief   Get Output Voltage Average ADC
* @Param   
* @Note    
* @Return  g_uout_real
*******************************************************************************/
uint16_t Power_GetUoutAvgADC(void)
{
    return g_uout_avg_adc;
}

/*******************************************************************************
* @Brief   Get Constant Lumen Dimming Level Value
* @Param   
* @Note    
* @Return  g_constant_lumen
*******************************************************************************/
uint16_t Power_GetConstantLumenValue(void)
{
    return g_constant_lumen;
}

/*******************************************************************************
* @Brief   Get Astro Dimming Level
* @Param   
* @Note    
* @Return  g_astro_dimming_level
*******************************************************************************/
uint16_t Power_GetAstroDimmingLevel(void)
{
    return g_astro_dimming_level;
}

/*******************************************************************************
* @Brief   Get EOL Dimming Level
* @Param   
* @Note    
* @Return  g_eol_dimming_level
*******************************************************************************/
uint16_t Power_GetEolDimmingLevel(void)
{
    return g_eol_dimming_level;
}

/*******************************************************************************
* @Brief   Get 1_10V Dimming Level
* @Param   
* @Note    
* @Return  g_one2ten_dimming_level
*******************************************************************************/
uint16_t Power_GetOne2TenDimming(void)
{
    return g_one2ten_dimming_level;
}

/*******************************************************************************
 * @Brief   Output Power Control Loop Task
 * @Param   
 * @Note    Power max 200W
 *                  Measure range    |   Actural range
 *          Iout    0.07V - 0.28V    |   0.35A - 1.4A 
 *          Uout    2.84V - 1.22V    |   333V  - 143V
 * @Return  
 *******************************************************************************/
void Power_ControlLoopTask(void)
{
    uint8_t i = 0;
    static uint32_t target_current = 0;    /* target current with dimming */
    uint16_t difference = 0;        /* difference between real and target value */
    uint16_t pwm_duty = 0;
    uint16_t adjust_speed = 0;
    static uint8_t s_hiccup_counter = 0;
    static int32_t temp1=0;
    static int32_t temp2=0;
    static uint32_t i_out_roll=0;
    static int32_t temp3=0;
    static int32_t temp4=0;
    static uint32_t u_out_roll=0;
    static uint16_t g_set_current=400,g_test_current=0;
    static uint8_t delay=0;
    static uint32_t g_power_current=0,g_power_current_pre=0;
    
    static uint16_t start_time=0;
    static uint8_t start_flag=0;
    static uint8_t current_minus_flag=0;
    int32_t test1;

    uint8_t  tx_buff[20]; 
    uint16_t duty = 0;
    uint16_t avg_adc_iout = 0;
    uint16_t avg_adc_uout = 0;
    
#ifdef DEBUG_PRINT
#endif
         //DaliBallast_CyclicTask();    
    /* Check if task has beed created */
    if(System_CheckTask(SYS_TASK_LOOP) == SYS_TASK_DISABLE)
    {
        return;
    }
    
    
    /* Don't run control loop while in protect state */
    if(PWM_GetProtectState() == PWM_STATE_PROTECT)
    {
        return;
    }
    g_loop_run=1;
    if(start_flag==0)
    {
      if(start_time++>4500/POWER_TASK_PERIOD)
      {
        start_flag=1;
      }
    }
    P2_1_toggle();
    /*-------- Get Output Average Result from ADC Module -----------------------*/    
    /* Calculate average ADC */
    ADC_CalculateAverage();
    
    /* Get raw data(average ADC) */
    g_iout_avg_adc = ADC_GetAverage(ADC_CHANNEL_IOUT);
    g_uout_avg_adc = ADC_GetAverage(ADC_CHANNEL_UOUT);
    
    
//        uint8_t *pinfo; //add test
//    sprintf((char*)tx_buff, "%d \n", g_iout_avg_adc);
//    pinfo=tx_buff;
//        while(*pinfo != '\0')
//    {
//        XMC_UART_CH_Transmit(USART_CHANNEL, *pinfo);
//        pinfo++;
//    }
    /* Update 1-10V Dimming Level */
    if(SWT_CheckTimer(SWT_ID_ONE2TEN) != SWT_STATUS_OK)
    {
        SWT_StartTimer(SWT_ID_ONE2TEN, ONE2TEN_UPDATE_TIME);
        g_one2ten_avg_adc = ADC_GetAverage(ADC_CHANNEL_DIM);
        if(g_one2ten_avg_adc<ONE_TEN_LOW_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;
        if(g_one2ten_avg_adc>ONE_TEN_HIGH_VOLTAGE)
            g_one2ten_avg_adc=ONE_TEN_HIGH_VOLTAGE; 
          //g_one2ten_avg_adc=ONE_TEN_LOW_VOLTAGE;//Add Moon
        Power_UpdateOne2TenDimming(g_one2ten_avg_adc);
    }
#ifndef ENABLE_ONE2TEN
    g_one2ten_dimming_level = POWER_MAX_DIMMING;
#endif
    
    /* Convert Iout ADC to Real Current in mA */
    g_iout_real = (uint16_t)((((IOUT_PARAM_A2 * g_iout_avg_adc) + IOUT_PARAM_A1) * g_iout_avg_adc) + IOUT_PARAM_A0);
    
    /* Convert Uout ADC to Real Voltage in V */
    g_uout_real = (uint16_t)((((UOUT_PARAM_B2 * g_uout_avg_adc) + UOUT_PARAM_B1) * g_uout_avg_adc) + UOUT_PARAM_B0);
    g_uout_real*=UOUT_COMPENSATION;
    //g_uout_real=200;
    g_test_current=g_iout_real;
    /* Iout compensation when disable DEBUG PRINT */
    g_iout_real = (uint32_t)(g_iout_real * IOUT_COMPENSATION + IOUT_OFFSET);
        /* Get stable vlaue for I_out Add by moon 2018.3.9*/   
    
    if(g_iout_real<=0)
      g_iout_real=1;
    temp1=g_iout_real;
    temp2 += (((temp1<<10)- temp2)>>7);//2
    i_out_roll=temp2>>10;
    if(abs(i_out_roll-g_iout_real)<5&&g_iout_real>60)//10mA
    {
        g_iout_real=i_out_roll;
    }
//    if(g_iout_real<250&&current_minus_flag==0)
//    {
//      current_minus_flag=1;
//    }
//    else
//    {
//      if(g_iout_real>270&&g_iout_real==1)
//      {
//        current_minus_flag=0;
//      }
//    }
//    if(current_minus_flag==1)
//      g_iout_real+=0;
    /* Target current with dimming control */
    target_current = (uint16_t)((uint32_t)g_target_current * g_astro_dimming_level / POWER_MAX_DIMMING);
    /* Target current with constant lumen control */
    target_current = (uint16_t)((uint32_t)target_current * g_constant_lumen / POWER_MAX_DIMMING);
    /* Target current with eol dimming level */
    target_current = (uint16_t)((uint32_t)target_current * g_eol_dimming_level / POWER_MAX_DIMMING);
#ifdef ENABLE_ONE2TEN
    /* Target current with 1-10V dimming level */
    target_current = (uint16_t)((uint32_t)target_current * g_one2ten_dimming_level / POWER_MAX_DIMMING);
    if(MemoryBank_Tfm_GetEnable(0)==1)
    {
      target_current*=MemoryBank_Tfm_GetTuningFactor(0);
      target_current/=100;
    }
    //target_current=300;
    g_traget_judge_current=target_current;
    //target_current=g_set_current; //add test code moon
        /* Update control loop current adjustment speed threshold */
#endif    
    temp3=g_uout_real;
    temp4 += (((temp3<<10)- temp4)>>3);
    u_out_roll=temp4>>10;
    if(abs(u_out_roll-g_uout_real)<10)
    {
      g_uout_real=u_out_roll;
    }
    if(g_uout_real!=0&&g_iout_real>50)
    {
#ifdef OT_NFC_IP67_200W
      g_power_current=(uint32_t)(OUTPUT_POWER*1.015/g_uout_real);// constant power control
#else
      g_power_current=(uint32_t)(OUTPUT_POWER*1.002/g_uout_real);// constant power control
#endif 
      
      if(abs(g_power_current-g_power_current_pre)<10)
        g_power_current=g_power_current_pre;
    }
    else
    {
      g_power_current=target_current;
    }
    g_power_current_pre=g_power_current;
    
    if(target_current>g_power_current)
    {
      target_current=g_power_current;
    }
    
    if(target_current < IOUT_DIVIDER_1_0)
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_0;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_0;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_0;  
        g_pwm_stable_uout = PWM_STABLE_UOUT_0;
        g_pwm_stable_iout = PWM_STABLE_IOUT_0;
    }
    else if(target_current < IOUT_DIVIDER_2_1)
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_1;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_1;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_1;   
        g_pwm_stable_uout = PWM_STABLE_UOUT_1;
        g_pwm_stable_iout = PWM_STABLE_IOUT_1;
    }
    else
    {
        g_iout_threshold_high = IOUT_THRESHOLD_HIGH_2;
        g_iout_threshold_mid = IOUT_THRESHOLD_MID_2;
        g_iout_threshold_low = IOUT_THRESHOLD_LOW_2;   
        g_pwm_stable_uout = PWM_STABLE_UOUT_2;
        g_pwm_stable_iout = PWM_STABLE_IOUT_2;
    }
    /*Add by moon ************************************/
//    if(abs(g_iout_real-target_current)<20)
//    {
//    if(g_s_period>g_a_period)
//        g_a_period+=2;
//    else
//        if(g_s_period<g_a_period)
//            g_a_period-=2;
//    }
     /*Add by moon ************************************/
    /* Power limit after startup */
    if(SWT_CheckTimer(SWT_ID_SET_LOAD) == SWT_STATUS_UP)
    {       
        /* Update minimum output current 10% POWER */
      if(g_uout_real<40)
        g_uout_real=40;
        g_min_current = (uint16_t)((uint32_t)OUTPUT_POWER * POWER_MIN_OUTPUT / g_uout_real / 100);
        
        /* Minimum current limit */
        if(g_min_current > IOUT_MAX)
        {
            g_min_current = IOUT_MAX;
        }
        
        if(g_min_current <= IOUT_MIN)
        {
            g_min_current = IOUT_MIN;
        }
    }

    if(target_current < g_min_current)
    {
        target_current = g_min_current;
    }
    if(target_current<70)
      target_current=70;
    g_current_a=target_current;
    //target_current=700; //test add Moon

    /*-------- Verify Current & Voltage Range ----------------------------------*/    
    /* Output current does not reach the target */
    if(g_iout_real < target_current)
    {
        /* Output voltage not match the max output voltage */
        if(g_uout_real < g_max_voltage+OVP_TRIGGER_EXCEED*3)//update FW OVP check point
        {
            if(g_uout_real < (g_max_voltage -  (uint32_t)g_max_voltage * g_pwm_stable_uout / 1000+OVP_TRIGGER_EXCEED*3))
            {
                /* PWM duty will increase */
                g_control_loop_state = POWER_STATE_INCREASE;
            }
            else
            {            
                /* Keep PWM duty */
                //g_control_loop_state = POWER_STATE_KEEP;
            }
        }
        /* Output voltage exceed the max output voltage */
        else if(g_uout_real > g_max_voltage)
        {
        #ifdef ENABLE_LOOP_OVP_OCP        
            if(g_uout_real >= (g_max_voltage + OVP_AVERAGE_EXCEED))
            {
                /* Output voltage exceed voltage limit, need OVP protection */
                g_control_loop_state = POWER_STATE_OVP;
                
            }
            else if(g_uout_real >= (g_max_voltage + g_max_voltage * g_pwm_stable_uout / 1000))
        #else
            if(g_uout_real >= (g_max_voltage + g_max_voltage * g_pwm_stable_uout / 1000))
        #endif
            {
                /* Output voltage exceed the upper limit,  PWM duty will decrease */
                //g_control_loop_state = POWER_STATE_DECREASE;
            }
            else
            {
                /* Keep PWM duty */
                //g_control_loop_state = POWER_STATE_KEEP;
            }
        }
        else
        {
            /* Output voltage reach the upper limit, keep the PWM duty */
            //g_control_loop_state = POWER_STATE_KEEP;
        }
    }
    /* Output current exceed the target */
    else if(g_iout_real > target_current)
    {
    #ifdef ENABLE_LOOP_OVP_OCP
        /* Output current exceed the target, PWM duty will decrease */
        if(g_iout_real > (target_current + OCP_AVERAGE_EXCEED))
        {
            /* Output current exceed limit and need OCP protection */
            g_control_loop_state = POWER_STATE_OCP;
        }
        else if(g_iout_real > (target_current + (uint32_t)target_current * g_pwm_stable_iout / 1000))
    #else
        if(g_iout_real > (target_current +  (uint32_t)target_current * g_pwm_stable_iout / 1000))
    #endif
        {
            /* Output current exceed target, PWM duty will decrease */
            g_control_loop_state = POWER_STATE_DECREASE;
        }
        else     
        {
            /* Keep PWM duty */
            //g_control_loop_state = POWER_STATE_KEEP;
        }
    }
    else
    {
        /* Output current reach the target, keep the PWM duty */
        g_control_loop_state = POWER_STATE_KEEP;
        s_one_ten_update=0;

    }
    if(g_iout_real>target_current)
    {
      g_control_loop_state=POWER_STATE_DECREASE;
    }
    else
      if(g_iout_real<target_current)
      {
        g_control_loop_state=POWER_STATE_INCREASE;
      }
    
    /*Current tolerance 10mA****/ //Add by Moon 2018.3,9
    if(g_iout_real>target_current)
        test1=g_iout_real-target_current;
    else
        test1=target_current-g_iout_real;
    //test1=abs((int16_t)(g_iout_real-target_current));
    if (test1<4)//current tolerance when current
    {
        g_iout_real=target_current;
        g_control_loop_state = POWER_STATE_KEEP;
        s_one_ten_update=0;
    }
    else
    {
        sprintf((char*)tx_buff, "%3d.%1d \n", g_iout_real);
        USART_PrintInfo(tx_buff);
    }
    
    /***** new loop control *************************************************/
    
    switch(g_control_loop_state)
    {
      case POWER_STATE_INCREASE:
        
        
        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL);
        
        if(g_current_non_0_flag==0)
        {
//           P2_1_toggle(); 
//          pwm_duty+=8;
//          if(pwm_duty > g_max_pwm_duty)
//          {
//            pwm_duty = g_max_pwm_duty; 
//            USART_PrintInfo("-MAX- ");
//          }
//          PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
        }
        else
        {
        
        if((target_current-g_iout_real)>500)
        {
          pwm_duty+=20;//20
          if(pwm_duty > g_max_pwm_duty)
          {
            pwm_duty = g_max_pwm_duty; 
            USART_PrintInfo("-MAX- ");
          }
          PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
        }
        else
        {
          if((target_current-g_iout_real)>50)//200
          {         
             pwm_duty+=3;//5
            
            if(pwm_duty > g_max_pwm_duty)
            {
              pwm_duty = g_max_pwm_duty; 
              USART_PrintInfo("-MAX- ");
            }
            //PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
            PWM_DutyStepUp(PWM_ID_CH_CTRL, 50);
          }
          else
          {
            if((target_current-g_iout_real)>20)
            {
              pwm_duty+=1;//3
              
              if(pwm_duty > g_max_pwm_duty)
              {
                pwm_duty = g_max_pwm_duty; 
                USART_PrintInfo("-MAX- ");
              }
              if(0)//start_flag==0)
                PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
              else
              PWM_DutyStepUp(PWM_ID_CH_CTRL, 30);
            }
            else
              if((target_current-g_iout_real)>5)
              {
                pwm_duty+=1;
                
                if(pwm_duty > g_max_pwm_duty)
                {
                  pwm_duty = g_max_pwm_duty; 
                  USART_PrintInfo("-MAX- ");
                }
                if(0)//start_flag==0)
                  PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
                else
                PWM_DutyStepUp(PWM_ID_CH_CTRL, 3);//
              }
              else 
                PWM_DutyStepUp(PWM_ID_CH_CTRL, 1);//Moon change 1
          }
        }
        }
        
        
        
        break;
        
      case POWER_STATE_DECREASE:  
        
        
        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL);
        
        if((g_iout_real-target_current)>500)
        {
          if(pwm_duty>20)
            pwm_duty-=20;
          else
            pwm_duty=0;
          
          PWM_DutyStepDown(PWM_ID_CH_CTRL, 700);//PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
        }
        else
        {
          if((g_iout_real-target_current)>150)
          {
            if(pwm_duty>3)
              pwm_duty-=2;
            else
              pwm_duty=0;
            
           PWM_DutyStepDown(PWM_ID_CH_CTRL, 300);//PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
          }
          else
          {
            if((g_iout_real-target_current)>40)
            {
              if(pwm_duty>1)
                pwm_duty-=1;
              else
                pwm_duty=0;
              
             PWM_DutyStepDown(PWM_ID_CH_CTRL, 50); //PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
            }
            else
            {
              if((g_iout_real-target_current)>30)
              {
                if(pwm_duty>2)
                  pwm_duty-=2;
                else
                  pwm_duty=0;
                
                PWM_DutyStepDown(PWM_ID_CH_CTRL, 6); //PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
              }
              else
              {
                if((g_iout_real-target_current)>10)
                {
                  if(pwm_duty>1)
                    pwm_duty-=1;
                  else
                    pwm_duty=0;
                  
                  PWM_DutyStepDown(PWM_ID_CH_CTRL, 2); //PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
                }
                else
                  PWM_DutyStepDown(PWM_ID_CH_CTRL, 1); //Moon change to 0
              }
            }
          }
        }
        
        
        break;
    
    }
    
    
    
    
    
    
    
 /*power off judge**/   
#ifndef OT_NFC_IP67_100W 
//    if(s_flag_off==0)
//    {
//      if(delay++>50)
//      {
//        delay=150;
//        
//        if(((g_iout_real+50)<target_current)&&(s_one_ten_update==0)&&(target_current<400))
//        {
//          s_flag_off=1;
//          //PWM_EnterProtection();
//          delay=0;
//        }
//      }  
//    }
#endif   
//    uint8_t *pinfo; //add test
//    sprintf((char*)tx_buff, "%d \n", PWM_GetDuty(PWM_ID_CH_CTRL));
//    pinfo=tx_buff;
//        while(*pinfo != '\0')
//    {
//        XMC_UART_CH_Transmit(USART_CHANNEL, *pinfo);
//        pinfo++;
//    }
    /*-------- Adjust PWM for Control Loop -------------------------------------*/
//    switch(g_control_loop_state)
//    {
//    case POWER_STATE_INCREASE:
//        /* Clear OVP & OCP counter */
//        g_OVP_counter = 0;
//        g_OCP_counter = 0;
//        
//        /* Calculate the difference between real value and target value */
//        difference = target_current - g_iout_real;
//        
//        /* Select Adjust Speed */
//        /* Current unstable has full range speed */
//        if(g_current_stable_flag != OUTPUT_STABLE)
//        {
//            if(difference < g_iout_threshold_low)
//            {
//                adjust_speed = PWM_SPEED_STEP;
//                USART_PrintInfo("UP ");
//            }
//            else if(difference < g_iout_threshold_mid)
//            {
//                adjust_speed = PWM_SPEED_LOW;
//                USART_PrintInfo("INC_L ");
//            }
//            else if(difference <g_iout_threshold_high)
//            {
//                adjust_speed = PWM_SPEED_MID;
//                USART_PrintInfo("INC_M ");
//            }
//            else
//            {
//                adjust_speed = PWM_SPEED_HIGH;
//                USART_PrintInfo("INC_H ");
//            }
//        }
//        /* Current enter stable condition, only have two speed */
//        else
//        {
//            if(difference < g_iout_threshold_low)
//            {
//                adjust_speed = PWM_SPEED_STEP;
//                USART_PrintInfo("UP ");
//            }
//            else
//            {
//                adjust_speed = PWM_SPEED_LOW;
//                USART_PrintInfo("INC_L ");
//            }
//        }
//        
//        /* Choose Adjust Mode and Update PWM Duty */
//        if(adjust_speed == PWM_SPEED_STEP)
//        {
//            if(PWM_GetDuty(PWM_ID_CH_CTRL) <= g_max_pwm_duty)
//            {
//                /* Step adjust mode, use step up function for fine turning */
//                PWM_DutyStepUp(PWM_ID_CH_CTRL, 1);//Moon change 1
//            }
//            else
//            {
//                USART_PrintInfo("-MAX- ");
//            }
//        }
//        else
//        {
//            if(target_current < IOUT_DIVIDER_1_0) 
//            {
//                if(adjust_speed == PWM_SPEED_LOW)
//                {
//                    if(PWM_GetDuty(PWM_ID_CH_CTRL) <= g_max_pwm_duty)
//                    {
//                        PWM_DutyStepUp(PWM_ID_CH_CTRL, PWM_SPEED_L1);
//                    }
//                    else
//                    {
//                        USART_PrintInfo("-MAX- ");
//                    }
//                }
//                else 
//                {
//                    if(adjust_speed == PWM_SPEED_MID)
//                    {
//                      if(start_flag==0)
//                        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) + PWM_SPEED_L2;
//                      else
//                        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) + 1;
//                    }
//                    else
//                    {
//                      if(start_flag==0)
//                        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) + PWM_SPEED_L3;
//                      else
//                        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) + 1;
//                    }
//                    
//                    if(pwm_duty > g_max_pwm_duty)
//                    {
//                        pwm_duty = g_max_pwm_duty; 
//                        USART_PrintInfo("-MAX- ");
//                    }
//                    PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
//                }
//            }
//            else
//            {
//                /* Get current pwm duty cycle */
//                pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL);
//                
//                /* Calculate Next PWM duty */
//                if(adjust_speed<9)
//                {
//                  if(g_iout_real<200&&start_flag==0)
//                  adjust_speed=3;
//                  else
//                    adjust_speed=1;
//                }
//                pwm_duty += adjust_speed;
//                if(pwm_duty > g_max_pwm_duty)  //if(pwm_duty > PWM_DUTY_FULL)
//                {
//                    /* Upper limit */
//                    pwm_duty = g_max_pwm_duty; //pwm_duty = PWM_DUTY_FULL;
//                    USART_PrintInfo("-MAX- ");
//                }
//                
//                /* Set new PWM duty */
//                PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
//            }
//        }
//        break;
//        
//    case POWER_STATE_DECREASE:
//        /* Clear OVP & OCP counter */
//        g_OVP_counter = 0;
//        g_OCP_counter = 0;
//
//        /* Over Current State */
//        if(g_iout_real > target_current)
//        {        
//            /* Calculate the difference between real value and target value */
//            difference = g_iout_real - target_current;
//            
//            /* Select Adjust Speed for Over-Current State */
//            /* Current unstable has full range speed */
//            if(g_current_stable_flag != OUTPUT_STABLE)
//            {
//                if(difference < g_iout_threshold_low)
//                {
//                    adjust_speed = PWM_SPEED_STEP;
//                    USART_PrintInfo("DOWN ");
//                }
//                else if(difference < g_iout_threshold_mid)
//                {
//                    adjust_speed = PWM_SPEED_LOW;
//                    USART_PrintInfo("DEC_L ");
//                }
//                else if(difference < g_iout_threshold_high)
//                {
//                    adjust_speed = PWM_SPEED_MID;
//                    USART_PrintInfo("DEC_M ");
//                }
//                else
//                {
//                    adjust_speed = PWM_SPEED_HIGH;
//                    USART_PrintInfo("DEC_H ");
//                }
//            }
//            /* Current enter stable condition, only have two speed */
//            else
//            {
//                if(difference < g_iout_threshold_low)
//                {
//                    adjust_speed = PWM_SPEED_STEP;
//                    USART_PrintInfo("DOWN ");
//                }
//                else
//                {
//                    adjust_speed = PWM_SPEED_LOW;
//                    USART_PrintInfo("DEC_L ");
//                }
//            }
//        }
//        /* Over Voltage State */
//        else
//        {            
//            /* Calculate the difference between real value and target value */
//            difference = g_uout_real - g_max_voltage;
//            
//            /* Select Adjust Speed for Over-Voltage State */
//            /* Over voltage always has full adjust speed */
//            if(difference < UOUT_THRESHOLD_LOW)
//            {
//                adjust_speed = PWM_SPEED_STEP;
//                USART_PrintInfo("DOWN ");
//            }
//            else if(difference < UOUT_THRESHOLD_MID)
//            {
//                adjust_speed = PWM_SPEED_LOW;
//                USART_PrintInfo("DEC_L ");
//            }
//            else if(difference < UOUT_THRESHOLD_HIGH)
//            {
//                adjust_speed = PWM_SPEED_MID;
//                USART_PrintInfo("DEC_M ");
//            }
//            else
//            {
//                adjust_speed = PWM_SPEED_HIGH;
//                USART_PrintInfo("DEC_H ");
//            }
//        }
//        
//        /* Choose Adjust Mode and Update PWM Duty */
//        if(adjust_speed == PWM_SPEED_STEP)
//        {
//            /* Step adjust mode, use step down function for fine turning */
//            PWM_DutyStepDown(PWM_ID_CH_CTRL, 1); //Moon change to 0
//        }
//        else
//        {
//            if(target_current < IOUT_DIVIDER_1_0) 
//            {
//                if(adjust_speed == PWM_SPEED_LOW)
//                {
//                    PWM_DutyStepDown(PWM_ID_CH_CTRL, PWM_SPEED_L1);
//                }
//                else 
//                {
//                    if(adjust_speed == PWM_SPEED_MID)
//                    {
//                        adjust_speed = PWM_SPEED_L2;
//                    }
//                    else
//                    {
//                      if(0)//start_flag==0)
//                        adjust_speed = PWM_SPEED_L3;
//                      else
//                        adjust_speed = 1;
//                    }
//                    pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL);
//                    if(pwm_duty > adjust_speed)
//                    {
//                        pwm_duty -= adjust_speed;
//                    }
//                    else
//                    {
//                        /* Lower limit */
//                        pwm_duty = 0;
//                    }
//                    PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
//                }
//            }
//            else
//            {
//                /* Get current pwm duty cycle */
//                pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL);
//                
//                if(adjust_speed<9||g_iout_real<200)
//                  adjust_speed=1;
//                /* Calculate Next PWM duty */
//                if(pwm_duty > adjust_speed)
//                {
//                    pwm_duty -= adjust_speed;
//                }
//                else
//                {
//                    /* Lower limit */
//                    pwm_duty = 0;
//                }
//                
//                /* Set new PWM duty */
//                PWM_SetDuty(PWM_ID_CH_CTRL, pwm_duty, PWM_MODE_LIMIT);
//            }
//        }
//        break;
//
//#ifdef ENABLE_LOOP_OVP_OCP        
//    case POWER_STATE_OCP:
//        /* Update OCP counter */
//        g_OCP_counter++;
//        
//        /* Trigger protection function if match the trigger condition */
//        if(g_OCP_counter >= OCP_OVP_TRIGGER_COUNTER)
//        {
//            PWM_EnterProtection();
//            USART_PrintInfo("OCP_Now ");
//        }
//        break;
//        
//    case POWER_STATE_OVP:
//        /* Update OVP counter */
//        g_OVP_counter++;
//        
//        /* Trigger protection function if match the trigger condition */
//        if(g_OVP_counter >= OCP_OVP_TRIGGER_COUNTER)
//        {
//            //PWM_EnterProtection();
//            USART_PrintInfo("OVP_Now ");
//        }      
//        break;
//#endif /* ENABLE_LOOP_OVP_OCP*/
//
//    case POWER_STATE_KEEP:
//    default:
//        /* Clear OVP & OCP counter */
//        g_OVP_counter = 0;
//        g_OCP_counter = 0;
//        
//        /* Keep PWM duty */
//        USART_PrintInfo("KEEP ");
//        
//        /* Output current enter stable condition */
//        g_current_stable_flag = OUTPUT_STABLE;
//        
//#ifdef MAX_DUTY_TOLERANCE
//        if(g_max_duty_enable == 0)
//        {
//            g_max_duty_enable = 1;
//            stable_keep = 1;
//            
//            /* Update max pwm duty */
//            g_max_pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) + MAX_DUTY_TOLERANCE; /* max duty + tolerance */
//        }
//#else
//        g_max_pwm_duty = 1000;
//#endif
//        break;
//    }
    
#ifdef NOENABLE_OVP //no software ovp function
    /*-------- OVP Hiccup Process, Add in 2016.08.25 ---------------------------*/
    /* Iout very low and Uout reach max voltage means it was open/overload state */
    if((g_uout_real >= (g_max_voltage -  (uint32_t)g_max_voltage * g_pwm_stable_uout / 1000)) && /* Uout reach max voltage */
       (g_iout_real <= OVP_HICCUP_IOUT) )     /* Iout very low */
    {
        /* Increase counter */
        s_hiccup_counter++;
        
        /* Active OVP protection for hiccup */
        if(s_hiccup_counter >= OVP_HICCUP_COUNTER)
        {
            //PWM_EnterProtection();
        }
    }
    else
    {
        /* Reset counter */
        s_hiccup_counter = 0;
    }
#endif
    
#ifdef DEBUG_PRINT
    /* Test print pwm duty and current adc */
    duty = PWM_GetDuty(PWM_ID_CH_CTRL);
    avg_adc_iout = ADC_GetAverage(ADC_CHANNEL_IOUT);
    avg_adc_uout = ADC_GetAverage(ADC_CHANNEL_UOUT);
//    sprintf((char*)tx_buff, "%4d %4d %4d %4d \n",  avg_adc_iout, avg_adc_uout, g_iout_real, g_uout_real);
//    sprintf((char*)tx_buff, "%3d.%1d %4d %4d \n",  duty/10, duty%10, avg_adc_iout, avg_adc_uout);
//    sprintf((char*)tx_buff, "%3d.%1d %4d %4d \n",  duty/10, duty%10, g_iout_real, g_uout_real);
    sprintf((char*)tx_buff, "%3d.%1d \n",  duty/10, duty%10);
    USART_PrintInfo(tx_buff);
#endif /* DEBUG_PRINT */
//    if(delay++>100)
//    {
//      delay=150;
//      SWT_StartTimer(SWT_ID_LOOP, 200);
//    }
//    else
    {
      if(target_current < IOUT_DIVIDER_1_0)
      {
        SWT_StartTimer(SWT_ID_LOOP, POWER_TASK_PERIOD_LOW);
      }
      else
      {
        SWT_StartTimer(SWT_ID_LOOP, POWER_TASK_PERIOD);
      }
    }
    
    /* Close Task */
    System_CloseTask( SYS_TASK_LOOP );
}

/*************************************************************************************************/
void Power_nfc_handle(void)
{
  extern uint8_t g_nfc_tag_read, g_nfc_flag_save;
  
  extern uint32_t g_nfc_time;
  
  static uint8_t count_time=0;
  
  static uint16_t nfc_save_time=0;
  
  if(System_CheckTask(SYS_TASK_NFC_HANDLE) == SYS_TASK_DISABLE)
  {
    return;
  }
  
  nfc_time_hanlde();
  
  AstroTimer();
  
  if(g_nfc_tag_read==4)
  {
    SWT_StartTimer(SWT_ID_NFC_HANDLE, NFC_HANDLE_TIME); 
    
    if(count_time++>=1)// run every 1s
    {
      g_nfc_time+=1;
      count_time=0;
      //P2_1_toggle();
      if(nfc_save_time++>120)// nfc save time 67minutes
      {
        g_nfc_flag_save=1;
        
        nfc_save_time=0;
      }
    }

  }
  else
  {
    SWT_StartTimer(SWT_ID_NFC_HANDLE, 1);  
  }
  System_CloseTask( SYS_TASK_NFC_HANDLE );
  
}
/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
