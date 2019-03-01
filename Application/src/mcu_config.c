/*
 ***************************************************************************************************
 *                                     MCU Configuration
 *
 * File   : mcu_config.c
 * Author : Douglas Xie
 * Date   : 2016.03.18
 ***************************************************************************************************
 * Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
 ***************************************************************************************************
 */


/* Include Head Files ---------------------------------------------------------------------------*/
#include "mcu_config.h"

/* Macro Defines --------------------------------------------------------------------------------*/

/* Private Variable -----------------------------------------------------------------------------*/

/* Global Variable ------------------------------------------------------------------------------*/

/* Function Declaration -------------------------------------------------------------------------*/

/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   MCU Clock Initial
* @Param   
* @Note    Config clock for MCU and peripherals
*          dco_dclk = 64MHz, idiv should > 0
*          MCLK = dco_dclk / (2*(idiv + fdiv/256))) = 32MHz
*          PCLK = dco_dclk / (idiv + fdiv/256)) = 64MHz
* @Return  
*******************************************************************************/
void MCU_ClockInit(void)
{
    /* Initialization data of the SCU driver */
    XMC_SCU_CLOCK_CONFIG_t clock_object =
    {
        0U, /* fdiv = 0 */
        1U, /* idiv = 1, idiv should > 0 */
        XMC_SCU_CLOCK_PCLKSRC_DOUBLE_MCLK, 
        XMC_SCU_CLOCK_RTCCLKSRC_DCO2,
    };
    
    /* Initialize clock */
    XMC_SCU_CLOCK_Init(&clock_object);
}


/*******************************************************************************
* @Brief   MCU Clock Low Speed Config
* @Param   
* @Note    Config clock for MCU and peripherals
*          dco_dclk = 64MHz, idiv should > 0
*          MCLK = dco_dclk / (2*(idiv + fdiv/256))) = 4MHz
*          PCLK = dco_dclk / (idiv + fdiv/256)) = 8MHz
* @Return  
*******************************************************************************/
void MCU_ClockConfigLowSpeed(void)
{
    /* Initialization data of the SCU driver */
    XMC_SCU_CLOCK_CONFIG_t clock_object =
    {
        0U, /* fdiv = 0 */
        8U, /* idiv = 1, idiv should > 0 */
        XMC_SCU_CLOCK_PCLKSRC_MCLK, //XMC_SCU_CLOCK_PCLKSRC_DOUBLE_MCLK,
        XMC_SCU_CLOCK_RTCCLKSRC_DCO2,
    };
    
    /* Initialize clock */
    XMC_SCU_CLOCK_Init(&clock_object);
}


/*******************************************************************************
* @Brief   MCU GPIO Initialization
* @Param   
* @Note    Set unused pin to output low level
* @Return  
*******************************************************************************/
void MCU_GpioInit(void)
{     
    /* PORT0 unused pins */
    P0_5_set_mode(OUTPUT_OD_GP);
    P0_5_reset();
    P0_6_set_mode(OUTPUT_OD_GP);
    P0_6_reset();
    P0_7_set_mode(OUTPUT_OD_GP);
    P0_7_reset();
    P0_8_set_mode(OUTPUT_OD_GP);
    P0_8_reset();
    P0_9_set_mode(OUTPUT_OD_GP);
    P0_9_reset();
    P0_10_set_mode(OUTPUT_OD_GP);
    P0_10_reset();
    P0_11_set_mode(OUTPUT_OD_GP);
    P0_11_reset();
    P0_12_set_mode(OUTPUT_OD_GP);
    P0_12_reset();
    P0_13_set_mode(OUTPUT_OD_GP);
    P0_13_reset();

    /* Debug pins */
    P0_14_set_mode(INPUT_PU);
    P0_15_set_mode(INPUT_PU);
    
    /* PORT1 unused pins */
    P1_0_set_mode(OUTPUT_OD_GP);
    P1_0_reset();
    P1_4_set_mode(OUTPUT_OD_GP);
    P1_4_reset();
    P1_5_set_mode(OUTPUT_OD_GP);
    P1_5_reset();

    /* PORT2 unused pins */
    P2_0_set_mode(OUTPUT_OD_GP);
    P2_0_reset();
    P2_1_set_mode(OUTPUT_PP_GP);//update
    P2_1_reset();
    P2_2_set_mode(OUTPUT_PP_GP);//update
    P2_2_reset();
    P2_3_set_mode(OUTPUT_OD_GP);
    P2_3_reset();
    P2_4_set_mode(OUTPUT_OD_GP);
    P2_4_reset();
    P2_5_set_mode(OUTPUT_OD_GP);
    P2_5_reset();
}


/*******************************************************************************
* @Brief   MCU Power On Initialization
* @Param   
* @Note    Config core and peripherals, init the software task
* @Return  
*******************************************************************************/
void MCU_PowerOnInit(void)
{    
    /*------------ Hardware Module Initial -----------------------------------*/
    /* Enable control pwm accelerate pin to avoid shoot current */
    //PWM_CtrlAccelerateEnable();   
   
    /* MCU clock config initial */
    MCU_ClockInit();   
    
    /* MCU unused pins initial */
    MCU_GpioInit(); 
    
    /* GPIO Initial */
    MCU_NfcPowerPinInit();

    /* Enable PWM module and output init pwm duty */
    PWM_Enable();
    PWM_SetDuty(PWM_ID_CH_CTRL, INIT_CTRL_DUTY_DEFAULT, PWM_MODE_FULL);
    PWM_SetDuty(PWM_ID_CH_OCP, OCP_TRIGGER_DUTY_DEFAULT, PWM_MODE_FULL);
    PWM_SetDuty(PWM_ID_CH_OVP, OVP_TRIGGER_DUTY_DEFAULT, PWM_MODE_FULL);
                
    /* enable acmp for detect OVP & OCP */
    ACMP_EnableOVP();
//    ACMP_EnableOCP();
                
    /* Disable ADC to set ADC GPIO to input state */
    ADC_Disable();
    
    /* SysTick config, tick timer base */
    Tick_SetFrequency(TICKS_FREQ_NORMAL); 
    
    /* Initial usart for print information when debug */
    USART_Enable(); 
    
    /* Enable window watchdog for release version */
    WDT_Enable();
    
    /*------------ Software Initial -----------------------------------------*/
    /* System task controler init */
    System_TaskInit();
    
    /* Software timer init */
    SWT_Init();
    
    /* Simulate I2C init */
    I2C_Initial();

    /* Create power on task first */
    System_CreateTask(SYS_TASK_POWER_ON);
}


/*******************************************************************************
* @Brief   MCU System Tick Timer Disable
* @Param   
* @Note    Disable system tick timer and interrupt request
* @Return  
*******************************************************************************/
void MCU_SysTickDisable(void)
{
  SysTick->CTRL  &= ~(SysTick_CTRL_CLKSOURCE_Msk |
                      SysTick_CTRL_TICKINT_Msk   |
                      SysTick_CTRL_ENABLE_Msk);         /* Disable SysTick IRQ and SysTick Timer */
}


/*******************************************************************************
* @Brief   MCU Power Dip Event
* @Param   
* @Note    Close peripherals and set clock to low speed for saving energy
* @Return  
*******************************************************************************/
void MCU_PowerDipEvent(void)
{
    /* MCU clock low speed config*/
    MCU_ClockConfigLowSpeed();   
    
    /* Disable system tick timer */
    MCU_SysTickDisable();
    
    /* Disable PWM module */
    PWM_Disable();

    /* Disable ACMP and ERU for detect OVP & OCP */
    ACMP_DisableOVP();
    ACMP_DisableOCP();
    ERU_DisableOVP();
    ERU_DisableOCP();
    
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
}


/*******************************************************************************
* @Brief   MCU Change BMI Mode
* @Param   [in]BMI - BMI value to be install
* @Note    If OK will trigger reset and does not return to calling routine.
* @Return  
*******************************************************************************/
void MCU_ChangeBMI(BMI_MODE_t bmi_mode)
{
    uint16_t start_address = 0;
    
    switch (bmi_mode)
    {
    case BMI_PRODUCTIVE:
        start_address = 0xF8C1; //User Mode Productive
        break;
    case BMI_SWD_0_USER:
        start_address = 0xF8C3; //SWD_0 User Mode
        break;
    case BMI_SWD_1_USER:
        start_address = 0xFAC3; //SWD_1 User Mode
        break;
    case BMI_SPD_0_USER:
        start_address = 0xF9C3; //SPD_0 User Mode
        break;
    case BMI_SPD_1_USER:
        start_address = 0xFBC3; //SPD_1 User Mode
        break;
    case BMI_SWD_0_HAR:
        start_address = 0xF8C7; //SWD_0 HAR Mode
        break;
    case BMI_SWD_1_HAR:
        start_address = 0xFAC7; //SWD_1 HAR Mode
        break;
    case BMI_SPD_0_HAR:
        start_address = 0xF9C7; //SPD_0 HAR Mode
        break;
    case BMI_SPD_1_HAR:
        start_address = 0xFBC7; //SPD_1 HAR Mode
        break;
    case BMI_ASC_BSL_TO:
        start_address = 0xFFD0; //ASC_BSL TO:5sec Mode
        break;
    case BMI_ASC_BSL:
    default:
        start_address = 0xFFC0;  //ASC_BSL
        break;
    }
    
    /*the change BMI routine */
	MCU_BmiInstallationReq(start_address);;
}


/*******************************************************************************
* @Brief   MCU OVP Level Triggger Detect Task
* @Param   
* @Note    Detect OVP level trigger and enter protect mode if triggered
*          If application has been protected mode, this will not active again
* @Return  
*******************************************************************************/
void MCU_OVPLevelTriggerTask(void)
{
#ifdef ENABLE_OVP
    uint8_t ovp_level_state;
    
    /* Run task if application is not in protect mode */
    
         //DaliBallast_CyclicTask();
    if(PWM_GetProtectState() == PWM_STATE_PROTECT)
    {
        return;
    }
    
    /* Get OVP level trigger state */
    ovp_level_state = ACMP_CheckLevelEvent_OVP();
    
    if(ovp_level_state == ACMP_STATE_TRIGGER)
    {
        PWM_EnterProtection();
    }    
#endif
}



/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
