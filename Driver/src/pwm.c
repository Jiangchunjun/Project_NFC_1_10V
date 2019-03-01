/*
***************************************************************************************************
*                           PWM Output Control Function Implementation
*
* File   : pwm.c
* Author : Douglas Xie
* Date   : 2016.06.13
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ----------------------------------------------------------------------------*/
#include "pwm.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/

/* Private Variable ------------------------------------------------------------------------------*/
/* Record the pwm duty for 4 channels */
uint16_t g_pwm_duty[4] = {0, 0, 0, 0};

/* PWM enter protection state variable */
uint8_t g_protect_state = PWM_STATE_NORMAL;

extern uint8_t s_flag_off;
/* Constant Variable -----------------------------------------------------------------------------*/
/* CCU4 timer slice compare config */
const XMC_CCU4_SLICE_COMPARE_CONFIG_t g_pwm_timer_config =
{
    .timer_mode          = XMC_CCU4_SLICE_TIMER_COUNT_MODE_EA,
    .monoshot            = XMC_CCU4_SLICE_TIMER_REPEAT_MODE_REPEAT,
    .shadow_xfer_clear   = 1U,
    .dither_timer_period = 0U,
    .dither_duty_cycle   = 0U,
    .prescaler_mode	     = XMC_CCU4_SLICE_PRESCALER_MODE_NORMAL,
    .mcm_enable		     = 0U,
    .prescaler_initval   = XMC_CCU4_SLICE_PRESCALER_1,
    .float_limit         = 0U,
    .dither_limit		 = 0U,
    .timer_concatenation = 0U,
#if (PWM_DUTY_INVERT == 1)
    .passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_LOW
#else
    .passive_level       = XMC_CCU4_SLICE_OUTPUT_PASSIVE_LEVEL_HIGH
#endif
};

/* CCU4 timer slice event config for Event-1 */
const XMC_CCU4_SLICE_EVENT_CONFIG_t g_slice_event_config = 
{
    .duration     = XMC_CCU4_SLICE_EVENT_FILTER_DISABLED,
    .edge         = XMC_CCU4_SLICE_EVENT_EDGE_SENSITIVITY_RISING_EDGE,
    .level        = XMC_CCU4_SLICE_EVENT_LEVEL_SENSITIVITY_ACTIVE_HIGH, /* Not needed */
    .mapped_input = XMC_CCU4_SLICE_INPUT_I
};

/* Function Declaration --------------------------------------------------------------------------*/
void PWM_CCU4_TimerConfig(void);

/* Function Implement ----------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   PWM CCU4 Timer Config
* @Param   
* @Note    Config CCU4 global timer and slice timer to output PWM signal
* @Return  
*******************************************************************************/
void PWM_CCU4_TimerConfig(void)
{
    /*------------ CCU4 Global Timer Config -----------------------------------*/
    /* Ensure fCCU reaches CCU42 */
    XMC_CCU4_SetModuleClock(CCU40, XMC_CCU4_CLOCK_SCU);
    XMC_CCU4_Init(CCU40, XMC_CCU4_SLICE_MCMS_ACTION_TRANSFER_PR_CR);
    
    /* Start the prescaler and restore clocks to slices */
    XMC_CCU4_StartPrescaler(CCU40);  
    
    /*------------ CCU4 Slice Timer Config for PWM_CH0 ------------------------*/
    /* Get the slice out of idle mode */
    XMC_CCU4_EnableClock(CCU40, PWM_ID_CH_0); 
    
    /* Initialize the Slice */
    XMC_CCU4_SLICE_CompareInit(CCU40_CC40, &g_pwm_timer_config);
    
    /* Configure Event-1 and map it to Input-I */
    XMC_CCU4_SLICE_ConfigureEvent(CCU40_CC40, XMC_CCU4_SLICE_EVENT_1, &g_slice_event_config);
    
    /* Map Event-1 to Start function */
    XMC_CCU4_SLICE_StartConfig(CCU40_CC40, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);
    
    /* Setup PWM period and init duty cycle */
    PWM_SetDuty(PWM_ID_CH_0, PWM_DUTY_DEFAULT, PWM_MODE_LIMIT);
    
    /*------------ CCU4 Slice Timer Config for PWM_CH1 ------------------------*/
    /* Get the slice out of idle mode */
    XMC_CCU4_EnableClock(CCU40, PWM_ID_CH_1); 
    
    /* Initialize the Slice */
    XMC_CCU4_SLICE_CompareInit(CCU40_CC41, &g_pwm_timer_config);
    
    /* Configure Event-1 and map it to Input-I */
    XMC_CCU4_SLICE_ConfigureEvent(CCU40_CC41, XMC_CCU4_SLICE_EVENT_1, &g_slice_event_config);
    
    /* Map Event-1 to Start function */
    XMC_CCU4_SLICE_StartConfig(CCU40_CC41, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);
    
    /* Setup PWM period and init duty cycle */
    PWM_SetDuty(PWM_ID_CH_1, PWM_DUTY_DEFAULT, PWM_MODE_LIMIT);
    
    /*------------ CCU4 Slice Timer Config for PWM_CH2 ------------------------*/
    /* Get the slice out of idle mode */
    XMC_CCU4_EnableClock(CCU40, PWM_ID_CH_2); 
    
    /* Initialize the Slice */
    XMC_CCU4_SLICE_CompareInit(CCU40_CC42, &g_pwm_timer_config);
    
    /* Configure Event-1 and map it to Input-I */
    XMC_CCU4_SLICE_ConfigureEvent(CCU40_CC42, XMC_CCU4_SLICE_EVENT_1, &g_slice_event_config);
    
    /* Map Event-1 to Start function */
    XMC_CCU4_SLICE_StartConfig(CCU40_CC42, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);
    
    /* Setup PWM period and init duty cycle */
    PWM_SetDuty(PWM_ID_CH_2, PWM_DUTY_DEFAULT, PWM_MODE_LIMIT);
    
    /*------------ CCU4 Slice Timer Config for PWM_CH3 ------------------------*/
#ifdef ENABLE_PWM_CH_3
    /* Get the slice out of idle mode */
    XMC_CCU4_EnableClock(CCU40, PWM_ID_CH_3); 
    
    /* Initialize the Slice */
    XMC_CCU4_SLICE_CompareInit(CCU40_CC43, &g_pwm_timer_config);
    
    /* Configure Event-1 and map it to Input-I */
    XMC_CCU4_SLICE_ConfigureEvent(CCU40_CC43, XMC_CCU4_SLICE_EVENT_1, &g_slice_event_config);
    
    /* Map Event-1 to Start function */
    XMC_CCU4_SLICE_StartConfig(CCU40_CC43, XMC_CCU4_SLICE_EVENT_1, XMC_CCU4_SLICE_START_MODE_TIMER_START_CLEAR);
    
    /* Setup PWM period and init duty cycle */
    PWM_SetDuty(PWM_ID_CH_3, PWM_DEFAULT_DUTY, PWM_MODE_LIMIT);
#endif /* ENABLE_PWM_CH_3 */
}

/*******************************************************************************
* @Brief   Enable PWM Module and Start Output
* @Param   
* @Note    Config CCU4 to generate PWM signal and start CCU4 timer
*          Config GPIO CCU40.OUTx to output PWM signal
* @Return  
*******************************************************************************/
void PWM_Enable(void)
{
    XMC_GPIO_CONFIG_t pwm_output_config;
    
    /*------------ CCU4 Configuration -----------------------------------------*/
    /* Config CCU4 timer to generate PWM siganl */
    PWM_CCU4_TimerConfig();
    
    /* Start CCU4 Timer by generating an external start trigger */
    XMC_SCU_SetCcuTriggerHigh(SCU_GENERAL_CCUCON_GSC40_Msk);
    
    /*------------ GPIO Configuration -----------------------------------------*/  
    /* PWM pins mode, set to push-pull output in ALT4 function */
    pwm_output_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT4;
    
    /* PWM channel gpio pins configuration */
    XMC_GPIO_Init(PWM_GPIO_CH_0, &pwm_output_config);
    XMC_GPIO_Init(PWM_GPIO_CH_1, &pwm_output_config);
    XMC_GPIO_Init(PWM_GPIO_CH_2, &pwm_output_config);
    
#ifdef ENABLE_PWM_CH_3
    XMC_GPIO_Init(PWM_GPIO_CH_3, &pwm_output_config);
#endif /* ENABLE_PWM_CH_3 */    
    
    /* Reset channel duty record */
    g_pwm_duty[PWM_ID_CH_0] = 0;
    g_pwm_duty[PWM_ID_CH_1] = 0;
    g_pwm_duty[PWM_ID_CH_2] = 0;
    g_pwm_duty[PWM_ID_CH_3] = 0;    
    
    /* Reset protection state */
    PWM_ResetProtectState();
}


/*******************************************************************************
* @Brief   Disable PWM Module and Stop Output
* @Param   
* @Note   Disable CCU4 timer and config GPIO to output low/high level(PWM 0%)
*          -- if PWM channel is enable, disable PWM Module will output PWM 0%
*             whether is low or high level depends on the define PWM_DUTY_INVERT
*          -- if PWM channel is disable, output will always be low level
* @Return  
*******************************************************************************/
void PWM_Disable(void)
{
    XMC_GPIO_CONFIG_t pwm_output_config;
    
    /*------------ GPIO Configuration -----------------------------------------*/    
    /* PWM pins mode, set to push-pull output in normal function */
    pwm_output_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
#if (PWM_DUTY_INVERT == 1)
    pwm_output_config.output_level = XMC_GPIO_OUTPUT_LEVEL_HIGH;
#else
    pwm_output_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
#endif
    
    /* PWM channel gpio pins configuration */
    XMC_GPIO_Init(PWM_GPIO_CH_0, &pwm_output_config);
    XMC_GPIO_Init(PWM_GPIO_CH_1, &pwm_output_config);
    XMC_GPIO_Init(PWM_GPIO_CH_2, &pwm_output_config);
    
#ifdef ENABLE_PWM_CH_3
    XMC_GPIO_Init(PWM_GPIO_CH_3, &pwm_output_config);
#endif /* ENABLE_PWM_CH_3 */    
    
    /*------------ CCU4 Configuration -----------------------------------------*/
    /* Disable CCU4 timer to stop PWM output */
    XMC_CCU4_DisableModule(CCU40);
    
    /* Reset channel duty record */
    g_pwm_duty[PWM_ID_CH_0] = 0;
    g_pwm_duty[PWM_ID_CH_1] = 0;
    g_pwm_duty[PWM_ID_CH_2] = 0;
    g_pwm_duty[PWM_ID_CH_3] = 0;  
}


/*******************************************************************************
* @Brief   Setup PWM Duty Cycle
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]duty: input duty value which unit is 0.1%
*          [in]mode: input PWM_MODE_LIMIT or PWM_MODE_FULL
* @Note    Calculate pulse value according to duty value and update CCU4x timer
*          Protection mode will disable PWM duty limitation
* @Return  
*******************************************************************************/
void PWM_SetDuty(uint8_t channel, uint16_t duty, uint8_t mode)
{
    uint32_t pulse = 0;
    uint16_t period = 0;
    
#if (PWM_DUTY_INVERT == 1)
    /* Convert pwm duty for power control channel when need invert */
    if(channel == PWM_ID_CH_CTRL)
    {
        duty = PWM_DUTY_FULL - duty;
    }
#endif

     /* Choose period for channel */
    if(channel == PWM_ID_CH_CTRL)
    {
        period = PWM_PeriodUpdate();//PWM_PERIOD_CTRL;
        //duty=370;
    }
    else
    {
        period = PWM_PERIOD_OVP;
    }
    
    /* Normal mode will enable duty limitation */
    if(mode == PWM_MODE_LIMIT)
    {
        /* Upper limit */
        if(duty > PWM_DUTY_MAX)
        {
            duty = PWM_DUTY_MAX;
        }
        
        /* Lower limit */
        if(duty < PWM_DUTY_MIN)
        {
            duty = PWM_DUTY_MIN;
        }
    }
    
    /* Duty convert to pulse */
    pulse = (uint32_t)duty * (uint32_t)period / ((uint32_t)PWM_DUTY_FULL);
    
    /* Set pluse */
    switch(channel)
    {
    case PWM_ID_CH_0:
        if(g_pwm_duty[PWM_ID_CH_0] != pulse)
        {
            g_pwm_duty[PWM_ID_CH_0] = pulse;    // Record puluse counter
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, (uint16_t)pulse);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
        }
        break;     
        
    case PWM_ID_CH_1:
        if(g_pwm_duty[PWM_ID_CH_1] != pulse)
        {
            g_pwm_duty[PWM_ID_CH_1] = pulse;    // Record puluse counter
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC41, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC41, (uint16_t)pulse);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_1);
        }
        break;
        
    case PWM_ID_CH_2:
        
        if(g_pwm_duty[PWM_ID_CH_2] != pulse)
        {
            g_pwm_duty[PWM_ID_CH_2] = pulse;    // Record puluse counter
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC42, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC42, (uint16_t)pulse);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_2);
        }
        break;
        
#ifdef ENABLE_PWM_CH_3
    case PWM_ID_CH_3:
        if(g_pwm_duty[PWM_ID_CH_3] != pulse)
        {
            g_pwm_duty[PWM_ID_CH_3] = pulse;    // Record puluse counter
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC43, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC43, (uint16_t)pulse);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_3);
        }
        break;
#endif /* ENABLE_PWM_CH_3 */
        
    default:
        break;
    }
}

/*******************************************************************************
* @Brief   Get Current PWM Duty Cycle
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
* @Note    
* @Return  current pwm channel duty cycle
*******************************************************************************/
uint16_t PWM_GetDuty(uint8_t channel)
{
    uint16_t duty = 0;
    uint16_t pulse = 0;
    uint16_t period = 0;

     /* Choose period for channel */
    if(channel == PWM_ID_CH_CTRL)
    {
        period = PWM_PeriodUpdate();//PWM_PERIOD_CTRL;
    }
    else
    {
        period = PWM_PERIOD_OVP;
    }
    
    
    /* Get pulse record from global variable */
    switch(channel)
    {
    case PWM_ID_CH_0:
        pulse = g_pwm_duty[PWM_ID_CH_0];
        break;     
        
    case PWM_ID_CH_1:
        pulse = g_pwm_duty[PWM_ID_CH_1];
        break;
        
    case PWM_ID_CH_2:
        pulse = g_pwm_duty[PWM_ID_CH_2];
        break;
        
#ifdef ENABLE_PWM_CH_3
    case PWM_ID_CH_3:
        pulse = g_pwm_duty[PWM_ID_CH_3];
        break;
#endif /* ENABLE_PWM_CH_3 */
        
    default:
        break;
    }

    /* Transform to duty */
    duty = (uint16_t)((uint32_t)pulse * (uint32_t)PWM_DUTY_FULL / (uint32_t)period);

#if (PWM_DUTY_INVERT == 1)
    /* Convert pwm duty for power control channel when need invert */
    if(channel == PWM_ID_CH_CTRL)
    {
        duty = PWM_DUTY_FULL - duty;
    }
#endif
    
    return duty;
}


/*******************************************************************************
* @Brief   PWM Duty Cycle Step Up
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]steps: move steps
* @Note    Set pwm timer pulse increase one step
* @Return  
*******************************************************************************/
void PWM_DutyStepUp(uint8_t channel, uint16_t steps)
{
    uint16_t max_pulse = 0;
    uint16_t period = 0;
    uint16_t new_pulse = 0;
    
     /* Choose period for channel */
    if(channel == PWM_ID_CH_CTRL)
    {
        period = PWM_PeriodUpdate();//PWM_PERIOD_CTRL;
    }
    else
    {
        period = PWM_PERIOD_OVP;
    }
    
    /* Calculate max pulse counte */
    max_pulse = (uint16_t)((uint32_t)PWM_DUTY_MAX * (uint32_t)period / ((uint32_t)PWM_DUTY_FULL));

    /* Increase pwm channel pulse width one step */
    switch(channel)
    {
    case PWM_ID_CH_0:
        if(max_pulse > (g_pwm_duty[PWM_ID_CH_0] + steps))
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_0] + steps;
        }
        else
        {
            new_pulse = max_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_0] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_0] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, g_pwm_duty[PWM_ID_CH_0]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
        }
        break;     
        
    case PWM_ID_CH_1:
        if(max_pulse > (g_pwm_duty[PWM_ID_CH_1] + steps))
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_1] + steps;
        }
        else
        {
            new_pulse = max_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_1] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_1] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC41, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC41, g_pwm_duty[PWM_ID_CH_1]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_1);
        }
        break;
        
    case PWM_ID_CH_2:
        if(max_pulse > (g_pwm_duty[PWM_ID_CH_2] + steps))
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_2] + steps;
        }
        else
        {
            new_pulse = max_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_2] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_2] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC42, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC42, g_pwm_duty[PWM_ID_CH_2]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_2);
        }
        break;
        
#ifdef ENABLE_PWM_CH_3
    case PWM_ID_CH_3:
        if(max_pulse > (g_pwm_duty[PWM_ID_CH_3] + steps))
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_3] + steps;
        }
        else
        {
            new_pulse = max_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_3] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_3] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC43, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC43, g_pwm_duty[PWM_ID_CH_3]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_3);
        }
        break;
#endif /* ENABLE_PWM_CH_3 */
        
    default:
        break;
    }
}


/*******************************************************************************
* @Brief   PWM Duty Cycle Step Down
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]steps: move steps
* @Note    Set pwm timer pulse decrease one step
* @Return  
*******************************************************************************/
void PWM_DutyStepDown(uint8_t channel, uint16_t steps)
{
    uint16_t min_pulse = 0;
    uint16_t period = 0;
    uint16_t new_pulse = 0;
    
     /* Choose period for channel */
    if(channel == PWM_ID_CH_CTRL)
    {
        period = PWM_PeriodUpdate();//PWM_PERIOD_CTRL;
    }
    else
    {
        period = PWM_PERIOD_OVP;
    }
    
    /* Calculate max pulse counte */
    min_pulse = (uint16_t)((uint32_t)PWM_DUTY_MIN * (uint32_t)period / ((uint32_t)PWM_DUTY_FULL));

    /* Increase pwm channel pulse width one step */
    switch(channel)
    {
    case PWM_ID_CH_0:
        if((min_pulse + steps) < g_pwm_duty[PWM_ID_CH_0])
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_0] - steps;
        }
        else
        {
            new_pulse = min_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_0] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_0] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC40, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC40, g_pwm_duty[PWM_ID_CH_0]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_0);
        }
        break;     
        
    case PWM_ID_CH_1:
        if((min_pulse + steps) < g_pwm_duty[PWM_ID_CH_1])
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_1] - steps;
        }
        else
        {
            new_pulse = min_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_1] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_1] = new_pulse;
        XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC41, period);
        XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC41, g_pwm_duty[PWM_ID_CH_1]);
        XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_1);
        }
        break;
        
    case PWM_ID_CH_2:
        if((min_pulse + steps) < g_pwm_duty[PWM_ID_CH_2])
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_2] - steps;
        }
        else
        {
            new_pulse = min_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_2] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_2] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC42, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC42, g_pwm_duty[PWM_ID_CH_2]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_2);
        }
        break;
        
#ifdef ENABLE_PWM_CH_3
    case PWM_ID_CH_3:
        if((min_pulse + steps) < g_pwm_duty[PWM_ID_CH_3])
        {
            new_pulse = g_pwm_duty[PWM_ID_CH_3] - steps;
        }
        else
        {
            new_pulse = min_pulse;
        }
        if(g_pwm_duty[PWM_ID_CH_3] != new_pulse)
        {
            g_pwm_duty[PWM_ID_CH_3] = new_pulse;
            XMC_CCU4_SLICE_SetTimerPeriodMatch(CCU40_CC43, period);
            XMC_CCU4_SLICE_SetTimerCompareMatch(CCU40_CC43, g_pwm_duty[PWM_ID_CH_3]);
            XMC_CCU4_EnableShadowTransfer(CCU40, XMC_CCU4_SHADOW_TRANSFER_SLICE_3);
        }
        break;
#endif /* ENABLE_PWM_CH_3 */
        
    default:
        break;
    }
}


/*******************************************************************************
* @Brief   PWM Enter OVP/OCP Protection
* @Param   
* @Note    Set PWM_CONTROL gpio pin to output high/low level
*          Stop power control channel PWM, set to 0%/100%
* @Return  
*******************************************************************************/
void PWM_EnterProtection(void)
{
    static uint16_t time=0;
    
    if(PWM_GetProtectState() == PWM_STATE_PROTECT)  
    {
        return;
    }
    
    /* disable global interrupt to improve active time */
    __disable_interrupt();
    
    /* Enable control pwm pull down accerlerate */
    PWM_CtrlAccelerateEnable();

    /*------------ PWM Channel Configuration ----------------------------------*/
#if (PWM_DUTY_INVERT == 1)
    /* PWM_CONTROL channel output 100% */
    PWM_SetDuty(PWM_ID_CH_CTRL, PWM_DUTY_FULL, PWM_MODE_FULL);
#else
    /* PWM_CONTROL channel output 3% */
    PWM_SetDuty(PWM_ID_CH_CTRL, PWM_DUTY_PROTECT, PWM_MODE_FULL);
#endif
    
    /*------------ Application Action -----------------------------------------*/
    /* Update protection state */
    PWM_SetProtectState();
    
    /* Start software timer for hiccup */
    if(s_flag_off==1)      
    {
#if  defined(OT_NFC_IP67_200W)
      SWT_StartTimer(SWT_ID_HICCUP, 2000);
      /* Start software timer to release accelerate pin */
      SWT_StartTimer(SWT_ID_SPEED_UP, 1800);
#else
      SWT_StartTimer(SWT_ID_HICCUP, 1000);
      /* Start software timer to release accelerate pin */
      SWT_StartTimer(SWT_ID_SPEED_UP, 800);
#endif
    }
    else
    {
      SWT_StartTimer(SWT_ID_HICCUP, OCP_OVP_HICCUP_PERIOD);
      /* Start software timer to release accelerate pin */
      SWT_StartTimer(SWT_ID_SPEED_UP, OCP_OVP_SPEED_UP_PERIOD);
    }       
    /* Close software timer for control loop task */
    SWT_CloseTimer(SWT_ID_LOOP);
    
    /* Close control loop task */
    System_CloseTask(SYS_TASK_LOOP);
    


    /* resume global interrupt */
    __enable_interrupt();
}


/*******************************************************************************
* @Brief   PWM Exit OVP/OCP Protection
* @Param   
* @Note    Set PWM_CONTROL gpio pin to input HZ state
*          Resume power control channel PWM
* @Return  
*******************************************************************************/
void PWM_ExitProtection(void)
{
    uint16_t i_set = 0;
    
    /* disable global interrupt to improve active time */
    __disable_interrupt();
    
    /*------------ Application Action -----------------------------------------*/
    /* Update protection state */
    PWM_ResetProtectState();
    
    /* Close software timer for hiccup */
    SWT_CloseTimer(SWT_ID_HICCUP);
    
    /* Create control loop task */
    System_CreateTask(SYS_TASK_LOOP);
    
    /* Set target current again for hiccup */
    i_set = Power_GetCurrent();
    Power_SetCurrent(i_set, SET_MODE_POWER_ON);
    
    s_flag_off=0;
    
    /* resume global interrupt */
    __enable_interrupt();
}


/*******************************************************************************
* @Brief   Get OVP/OCP Protection State
* @Param   
* @Note    Get state that if pwm enter protection state 
* @Return  PWM_STATE_NORMAL / PWM_STATE_PROTECT
*******************************************************************************/
uint8_t PWM_GetProtectState(void)
{
    return g_protect_state;
}

/*******************************************************************************
* @Brief   Set OVP/OCP Protection State
* @Param   
* @Note    set protection state by set variable of g_protect_state
* @Return  
*******************************************************************************/
void PWM_SetProtectState(void)
{
    g_protect_state = PWM_STATE_PROTECT;
}

/*******************************************************************************
* @Brief   Reset OVP/OCP Protection State
* @Param   
* @Note    reset protection state by clear variable of g_protect_state
* @Return  
*******************************************************************************/
void PWM_ResetProtectState(void)
{
    g_protect_state = PWM_STATE_NORMAL;
}

/*******************************************************************************
* @Brief   PWM Control Pin Accelerate Enable
* @Param   
* @Note    Set accelerate pin to output low level
* @Return  
*******************************************************************************/
void PWM_CtrlAccelerateEnable(void)
{    
    XMC_GPIO_CONFIG_t accelerate_config;
    
    /* For OVP and OCP accelerate */
    accelerate_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL;
    accelerate_config.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    XMC_GPIO_Init(PWM_ACCELERATE_PIN, &accelerate_config);
}

/*******************************************************************************
* @Brief   PWM Control Pin Accelerate Disable
* @Param   
* @Note    Set accelerate pin to be input high impedance state
* @Return  
*******************************************************************************/
void PWM_CtrlAccelerateDisable(void)
{
    XMC_GPIO_CONFIG_t accelerate_config;
    
    /* For OVP and OCP accelerate */
    accelerate_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(PWM_ACCELERATE_PIN, &accelerate_config);
}
/*******************************************************************************
* @Brief   PWM_PeriodUpdate
* @Param   
* @Note    Update frequency based on current range
* @Return  
*******************************************************************************/
uint16_t PWM_PeriodUpdate(void)
{
    static uint8_t current_flag=0;
    extern uint16_t g_current_a;
    extern uint32_t g_s_period, g_a_period;
    uint32_t temp,temp1;
    static uint32_t pre_period=0;
    
//    if(g_current_a>300&&current_flag==0)
//    {
//        current_flag=1;
//    }
//    if(g_current_a<250&&current_flag==1)
//    {
//        current_flag=0;
//    }
//    if(current_flag==1)
//    {
//        return PWM_PERIOD_CTRL_L;
//    }
//    else
//    {
//        return PWM_PERIOD_CTRL_H;
//    }
     temp=g_current_a;
     if(temp>400)temp=400;
     if(temp<70)temp=70;
     temp1=34242-(temp*15515>>8);
     if (temp1>30000) temp1=30000;
     g_s_period=temp1;
     if(abs(g_s_period-pre_period)<200)
     {
         g_s_period=pre_period;
     }
     
     pre_period=g_s_period;
#if defined(OT_NFC_IP67_200W)
     return 30000; //10000
#else
     return 30000;
    
#endif     
}