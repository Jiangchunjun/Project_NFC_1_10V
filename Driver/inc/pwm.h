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

#ifndef __PWM_H_
#define __PWM_H_

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_ccu4.h>
#include <xmc_scu.h>
#include <xmc_gpio.h>
#include "ttypes.h"
#include "sw_timer.h"
#include "system.h"

/* Choose config file */
#if defined(OT_NFC_IP67_60W)	
#include "power_config_60w.h"
#elif defined(OT_NFC_IP67_100W)
#include "power_config_100w.h"
#elif defined(OT_NFC_IP67_150W)
#include "power_config_150w.h"
#elif defined(OT_NFC_IP67_200W)
#include "power_config_200w.h"
#else
#error "pwm.h: missing the config file"
#endif 	

/* Macro Defines ---------------------------------------------------------------------------------*/
/* Enable CCU4 PWM channel, comment the define to disable channel */
//#define ENABLE_PWM_CH_3         /* this is the reserved channel */

/* CCU4 PWM channel id define */
#define PWM_ID_CH_0             (0)
#define PWM_ID_CH_1             (1)
#define PWM_ID_CH_2             (2)
#define PWM_ID_CH_3             (3)

/* PWM channel gpio pins define */
#define PWM_GPIO_CH_0           P0_0
#define PWM_GPIO_CH_1           P0_1
#define PWM_GPIO_CH_2           P0_2
#define PWM_GPIO_CH_3           P0_3

/* OVP and OCP accelerate pins define */
#define PWM_ACCELERATE_PIN      P0_3

/* PWM control mode define */
#define PWM_MODE_LIMIT          (0)
#define PWM_MODE_FULL           (1)

/* PWM protection state define */
#define PWM_STATE_NORMAL        (0)
#define PWM_STATE_PROTECT       (1)

/*-------- PWM Specific Channel Name define --------*/
/* PWM Specific Name for OVP */
#define PWM_ID_CH_OVP           PWM_ID_CH_0
#define PWM_GPIO_CH_OVP         PWM_GPIO_CH_0

/* PWM Specific Name for OCP */
#define PWM_ID_CH_OCP           PWM_ID_CH_1
#define PWM_GPIO_CH_OCP         PWM_GPIO_CH_1

/* PWM Specific Name for Power Control */
#define PWM_ID_CH_CTRL          PWM_ID_CH_2
#define PWM_GPIO_CH_CTRL        PWM_GPIO_CH_2

/*-------- PWM Main Property define ----------------*/
/* PWM signal period define     Range: 0~65535           */
/*                              1000  --> 65kHz(0.1%)    */
/*                              10000 --> 6.5kHz(0.01%)  */
/*                              15000 --> 4.3kHz(0.0067%) */
/*                              20000 --> 3.25kHz(0.005%) */
/*                              30000 --> 2.15kHz(0.0033%) */
//#define PWM_PERIOD_CTRL         (15000) //OT NFC 1DIM
#define PWM_PERIOD_CTRL         (25000) //OT NFC 2DIM 150W, 100W
#define PWM_PERIOD_CTRL_H        30000//(30000)//(15000) //OT NFC 2DIM 150W, 100W
#define PWM_PERIOD_CTRL_L         30000//(30000) //OT NFC 2DIM 150W, 100W
#define PWM_UNIT_STEP           (25)
//#define PWM_PERIOD_CTRL         (10000) //OT NFC 2DIM 150W, 100W
//#define PWM_UNIT_STEP           (10)
#define PWM_PERIOD_OVP          (1000)

/* PWM duty cycle default and limitation: unit is 0.1% */
#define PWM_DUTY_FULL           (1000)  /* Full duty is 100.0% */
#define PWM_DUTY_DEFAULT        (600)   /* Default output 60.0% */
#define PWM_DUTY_MAX            (999)   /* Max duty is 99.9% */
#define PWM_DUTY_MIN            (10)    /* Min duty is 1.0% */
#define PWM_DUTY_PROTECT        INIT_CTRL_DUTY_DEFAULT

/* Enable PWM duty output invert */
#define PWM_DUTY_INVERT         (0)     /* 1 - enable, 0 - disable */

/* OCP & OVP Hiccup Period define, unit: ms */



/* OCP & OVP Speed Up Time define, unit: ms */
#if defined(OT_NFC_IP67_200W)
#define OCP_OVP_HICCUP_PERIOD   (2000)  /* 2s */  //200ms 200  Moon
#define OCP_OVP_SPEED_UP_PERIOD (1900)    /* 50ms */
#else
#if defined(OT_NFC_IP67_150W)
#define OCP_OVP_HICCUP_PERIOD   (200)  /* 2s */  //200ms 200  Moon 111
#define OCP_OVP_SPEED_UP_PERIOD (190)    /* 50ms */
#else
#define OCP_OVP_HICCUP_PERIOD   (200)  /* 2s */  //200ms 200  Moon
#define OCP_OVP_SPEED_UP_PERIOD (50)    /* 50ms */
#endif
#endif

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable PWM Module and Start Output
* @Param   
* @Note    Config CCU4 to generate PWM signal and start CCU4 timer
*          Config GPIO CCU40.OUTx to output PWM signal
* @Return  
*******************************************************************************/
void PWM_Enable(void);

/*******************************************************************************
* @Brief   Disable PWM Module and Stop Output
* @Param   
* @Note    Disable CCU4 timer and config GPIO to output low/high level(PWM 0%)
* @Return  
*******************************************************************************/
void PWM_Disable(void);

/*******************************************************************************
* @Brief   Setup PWM Duty Cycle
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]duty: input duty value which unit is 0.1%
*          [in]mode: input PWM_MODE_LIMIT or PWM_MODE_FULL
* @Note    Calculate pulse value according to duty value and update CCU4x timer
*          Protection mode will disable PWM duty limitation
* @Return  
*******************************************************************************/
void PWM_SetDuty(uint8_t channel, uint16_t duty, uint8_t mode);

/*******************************************************************************
* @Brief   Get Current PWM Duty Cycle
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
* @Note    
* @Return  current pwm channel duty cycle
*******************************************************************************/
uint16_t PWM_GetDuty(uint8_t channel);

/*******************************************************************************
* @Brief   PWM Duty Cycle Step Up
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]steps: move steps
* @Note    Set pwm timer pulse increase one step
* @Return  
*******************************************************************************/
void PWM_DutyStepUp(uint8_t channel, uint16_t steps);

/*******************************************************************************
* @Brief   PWM Duty Cycle Step Down
* @Param   [in]channel: input pwm channel id form define PWM_ID_CH_x
*          [in]steps: move steps
* @Note    Set pwm timer pulse decrease one step
* @Return  
*******************************************************************************/
void PWM_DutyStepDown(uint8_t channel, uint16_t steps);

/*******************************************************************************
* @Brief   PWM Enter OVP/OCP Protection
* @Param   
* @Note    Set PWM_CONTROL gpio pin to output high/low level
*          Stop power control channel PWM, set to 0%/100%
* @Return  
*******************************************************************************/
void PWM_EnterProtection(void);

/*******************************************************************************
* @Brief   PWM Exit OVP/OCP Protection
* @Param   
* @Note    Set PWM_CONTROL gpio pin to input HZ state
*          Resume power control channel PWM
* @Return  
*******************************************************************************/
void PWM_ExitProtection(void);

/*******************************************************************************
* @Brief   Get OVP/OCP Protection State
* @Param   
* @Note    Get state that if pwm enter protection state 
* @Return  PWM_STATE_NORMAL / PWM_STATE_PROTECT
*******************************************************************************/
uint8_t PWM_GetProtectState(void);

/*******************************************************************************
* @Brief   Set OVP/OCP Protection State
* @Param   
* @Note    set protection state by set variable of g_protect_state
* @Return  
*******************************************************************************/
void PWM_SetProtectState(void);

/*******************************************************************************
* @Brief   Reset OVP/OCP Protection State
* @Param   
* @Note    reset protection state by clear variable of g_protect_state
* @Return  
*******************************************************************************/
void PWM_ResetProtectState(void);

/*******************************************************************************
* @Brief   PWM Control Pin Accelerate Enable
* @Param   
* @Note    Set accelerate pin to output low level
* @Return  
*******************************************************************************/
void PWM_CtrlAccelerateEnable(void);

/*******************************************************************************
* @Brief   PWM Control Pin Accelerate Disable
* @Param   
* @Note    Set accelerate pin to be input high impedance state
* @Return  
*******************************************************************************/
void PWM_CtrlAccelerateDisable(void);

/*******************************************************************************
* @Brief   PWM_PeriodUpdate
* @Param   
* @Note    Update frequency based on current range
* @Return  period value for pwm
*******************************************************************************/
uint16_t PWM_PeriodUpdate(void);
#endif /* __PWM_H_ */ 
