/*
***************************************************************************************************
*                               Output Power Control Loop Task
*
* File   : power_task.h
* Author : Douglas Xie
* Date   : 2016.04.26
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef _POWER_TASK_H
#define _POWER_TASK_H

/* Include Head Files ---------------------------------------------------------------------------*/
#include "adc.h"
#include "pwm.h"
#include "tick.h"
#include "wdt.h"
#include "sw_timer.h"
#include "system.h"
#include "tag.h"
#include "usart.h"
#include "mcu_config.h"
#include "filter.h"

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
#error "power_task.h: missing the config file"
#endif 	    

/* Macro Defines --------------------------------------------------------------------------------*/
/* Power Control Loop Task Period define, unit: ms */
//#define POWER_TASK_PERIOD       (12)       /* 12ms */
//#define POWER_TASK_PERIOD_LOW   (5)        /* 5ms */

/* 1-10V signal update dimming level period */
#define ONE2TEN_UPDATE_TIME     (30)       /* 100ms */ //update time from 100 to 30ms

/* Set real load voltage delay time */
#define SET_LOAD_VOL_TIME       (200)       /* 200ms + 300ms(power init) = 0.5s */

/*---------------- Output Power Limitation define ------------------*/
/* Output Range define */
#define IOUT_MAX                (1400)      /* Max Current = 1.4A = 1400mA */
#define IOUT_MIN                (70)        /* Min Current = 0.050A = 50mA */
#define IOUT_MIN_RATED          (700)       /* Min Rated Current = 0.7A = 700mA */

/* Default output current setting */
#define POWER_DEFALUT_CURRENT   (1400)  /* Unit: mA, default output 400mA */

/* Maximum output dimming percent 100% */
#define POWER_MAX_DIMMING       (10000)     /* Unit: 0.01%, default output 100% */

/* Minimum output power percent 10% */
#define POWER_MIN_OUTPUT        (10)       /* 10%*/

/*---------------- 1-10V Dimming Relative define ------------------*/
/* 1-10V Dimming Range: 5% - 100% */
#define ONE2TEN_LOWER_LIMIT     (1000)//500
#define ONE2TEN_UPPER_LIMIT     (10000)

#define NFC_HANDLE_TIME         500//1s 500ms

/* 1-10V dimming change */
#define POWER_DIM_CHANGE_DUTY   (100)
#define POWER_DIM_UNSTABLE_DUTY (100)//300

/*--------------- NFC Data Backup Flash Address define -------------*/
/* No OTA Bootloader */
#define  FLASH_START_ADDRESS    (0x10010000)  /* address for Flash save area, locate in 60k */
#define  FLASH_TIME_ADDRESS     (0X10010700)  /* address for run time */


/*------------------ Current Setting Mode define -------------------*/
/* Current setting mode */
#define SET_MODE_NORMAL         (0)         /* Normal mode. Set current & OVP OCP reference pwm */
#define SET_MODE_POWER_ON       (1)         /* Power on mode. Set current and give init output ctrl & OVP OCP reference pwm */

/*---------------- Hiccup Process Parameter define -----------------*/
/* Hiccup process parameter */
#define OVP_HICCUP_IOUT         (50)        /* Unit: mA */
#define OVP_HICCUP_COUNTER      (3)         /* Continous such time will active OVP protection */

/*------------------ Current Stable Flag define --------------------*/
/* Output current stable flag define */
#define OUTPUT_STABLE           (1)         /* Output current enter stable condition */
#define OUTPUT_UNSTABLE         (0)

/* Data Type Define -----------------------------------------------------------------------------*/
typedef enum
{
    POWER_STATE_OFF = 0,
    POWER_STATE_KEEP,
    POWER_STATE_INCREASE,
    POWER_STATE_DECREASE,
    POWER_STATE_OCP,
    POWER_STATE_OVP
} Power_State_t;

typedef enum
{
    POWER_LEVEL_0 = 0,
    POWER_LEVEL_1,
    POWER_LEVEL_2,
    POWER_LEVEL_3,
    POWER_LEVEL_4,
    POWER_LEVEL_5,
    POWER_LEVEL_6,
    POWER_LEVEL_7,
    POWER_LEVEL_8,
    POWER_LEVEL_9,
    POWER_LEVEL_10,
    POWER_LEVEL_11,
    POWER_LEVEL_12,
    POWER_LEVEL_MAX
} Power_Level_t;

/* Global Variable ------------------------------------------------------------------------------*/

/* Output Power Control Function Declaration ----------------------------------------------------*/

/*******************************************************************************
* @Brief   Output Power Control Initial
* @Param   
* @Note    Config ADC moudle
* @Return  
*******************************************************************************/
void Power_TaskInit(void);

/*******************************************************************************
* @Brief   Get Output Power Level
* @Param   [in]target_current: user current setting value, unit is mA
* @Note    get output current level for control loop choosing threshold
* @Return  Power_Level_t type level
*******************************************************************************/
Power_Level_t Power_GetPowerLevel(uint16_t target_current);

/*******************************************************************************
* @Brief   Get Control Initial PWM Duty
* @Param   [in]level: target current level
* @Note    Get control initial pwm duty according to level
* @Return  
*******************************************************************************/
uint16_t Power_GetControlInitDuty(Power_Level_t level);

/*******************************************************************************
* @Brief   Get OCP Trigger Point PWM Duty
* @Param   
* @Note    Get OCP trigger point pwm duty according to target current
* @Return  
*******************************************************************************/
uint16_t Power_GetOCPTriggerDuty(void);

/*******************************************************************************
* @Brief   Get OVP Trigger Point PWM Duty
* @Param   
* @Note    Get OVP trigger point pwm duty according to target max voltage
* @Return  
*******************************************************************************/
uint16_t Power_GetOVPTriggerDuty(void);

/*******************************************************************************
* @Brief   Prepare OVP and OCP Reference
* @Param   
* @Note    Read NFC tag and get OVP OCP reference before loop task begin
* @Return  
*******************************************************************************/
void Power_PrepareReference(void);

/*******************************************************************************
* @Brief   Set Target Output Power Current
* @Param   [in]target_current: user current setting value, unit is mA
*          [in]mode: SET_MODE_NORMAL / SET_MODE_POWER_ON
* @Note    set target current value to variable and calculate max_voltage
*          provide a init pwm duty according to the target current
*          update OCP & OVP trigger voltage
* @Return  
*******************************************************************************/
void Power_SetCurrent(uint16_t target_current, uint8_t mode);

/*******************************************************************************
* @Brief   Get Target Output Current
* @Param  
* @Return  target current that save in g_target_current
*******************************************************************************/
uint16_t Power_GetCurrent(void);

/*******************************************************************************
* @Brief   Get Minmum Dimming Level
* @Param   
* @Note    calculate minmum dimming level according to the minmum iout and iset
* @Return  minmum dimming level(percent with unit 0.01%)
*******************************************************************************/
uint16_t Power_GetMinLevel(void);

/*******************************************************************************
* @Brief   Set Astro Dimming Level
* @Param   [in]percent: dimming percent value, unit is 0.01%
* @Note    set dimming value of dimming mode which are set from T4T
* @Return  
*******************************************************************************/
void Power_SetAstroDimmingLevel(uint16_t percent);

/*******************************************************************************
* @Brief   Set Constant Lumen Value
* @Param   [in]percent: constant lumen percent value, unit is 0.01%
* @Note    set constant lumen dimming percent value which are set from T4T
* @Return  
*******************************************************************************/
void Power_SetConstantLumenValue(uint16_t percent);

/*******************************************************************************
* @Brief   Set EOL Dimming Level
* @Param   [in]percent: eol dimming percentage value, unit is 0.01%
* @Note    
* @Return  
*******************************************************************************/
void Power_SetEolDimmingLevel(uint16_t percent);

/*******************************************************************************
* @Brief   Update 1_10V Dimming Level
* @Param   [in]adc: 1-10V input voltage adc
* @Note    
* @Return  
*******************************************************************************/
void Power_UpdateOne2TenDimming(uint16_t adc);

/*******************************************************************************
* @Brief   Get Power Loop State
* @Param   
* @Note    
* @Return  power loop state
*******************************************************************************/
Power_State_t Power_GetPowerLoopState(void);

/*******************************************************************************
* @Brief   Get Real Output Current Value
* @Param   
* @Note    
* @Return  g_iout_real
*******************************************************************************/
uint16_t Power_GetRealIout(void);

/*******************************************************************************
* @Brief   Get Real Output Voltage Value
* @Param   
* @Note    
* @Return  g_uout_real
*******************************************************************************/
uint16_t Power_GetRealUout(void);

/*******************************************************************************
* @Brief   Get Output Current Average ADC
* @Param   
* @Note    
* @Return  g_iout_real
*******************************************************************************/
uint16_t Power_GetIoutAvgADC(void);

/*******************************************************************************
* @Brief   Get Output Voltage Average ADC
* @Param   
* @Note    
* @Return  g_uout_real
*******************************************************************************/
uint16_t Power_GetUoutAvgADC(void);

/*******************************************************************************
* @Brief   Get Constant Lumen Dimming Level Value
* @Param   
* @Note    
* @Return  g_constant_lumen
*******************************************************************************/
uint16_t Power_GetConstantLumenValue(void);

/*******************************************************************************
* @Brief   Get Astro Dimming Level
* @Param   
* @Note    
* @Return  g_astro_dimming_level
*******************************************************************************/
uint16_t Power_GetAstroDimmingLevel(void);

/*******************************************************************************
* @Brief   Get EOL Dimming Level
* @Param   
* @Note    
* @Return  g_eol_dimming_level
*******************************************************************************/
uint16_t Power_GetEolDimmingLevel(void);

/*******************************************************************************
* @Brief   Get 1_10V Dimming Level
* @Param   
* @Note    
* @Return  g_one2ten_dimming_level
*******************************************************************************/
uint16_t Power_GetOne2TenDimming(void);

/*******************************************************************************
* @Brief   Output Power Control Loop Task
* @Param   
* @Note    Power max 200W
*                  Measure range    |   Actural range
*          Iout    0.07V - 0.28V    |   0.35A - 1.4A 
*          Uout    2.84V - 1.22V    |   333V  - 143V
* @Return  
*******************************************************************************/
void Power_ControlLoopTask(void);

/*******************************************************************************/

void Power_nfc_handle(void);
#endif /* _POWER_TASK_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
