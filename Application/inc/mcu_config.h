/*
 ***************************************************************************************************
 *                                     MCU Configuration
 *
 * File   : mcu_config.h
 * Author : Douglas Xie
 * Date   : 2016.03.18
 ***************************************************************************************************
 * Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
 ***************************************************************************************************
 */

#ifndef _MCU_CONFIG_H
#define _MCU_CONFIG_H

/* Include Head Files ---------------------------------------------------------------------------*/
#include <xmc_common.h>
#include <xmc_vadc.h>
#include <xmc_scu.h>
#include <xmc_gpio.h>
#include "gpio_xmc1300_tssop38.h"
#include "adc.h"
#include "tick.h"
#include "pwm.h"
#include "acmp.h"
#include "eru.h"
#include "wdt.h"
#include "system.h"
#include "power_task.h"
#include "simu_i2c.h"
#include "tag.h"
#include "usart.h"
#include "nfc_task.h"
#include "xmc_flash.h"

/* Macro Defines for Version Information---------------------------------------------------------*/
/* Project Information define, max length 20 bytes */
#define SW_PROJECT_NAME             "OT_NFC_1-10V"
#define SW_CREATE_DATE              "2016.08.02"
#define SW_DEVELOPER_1              "Douglas Xie"
#define SW_DEVELOPER_2              "Moon Jiang"
#define SW_DEVELOPER_3              ""
#define SW_DEVELOPER_4              ""

/* Project Code: 4bytes, 3pm Number*/
#define PROJECT_CODE                (12345678)      //"1001XXXX"

/* Device Type: 2bytes */
#define DEVICE_TYPE                 (0xB001)        //Axxx-indoor, Bxxx-outdoor

/* Serial Number: 4bytes */
#define SN_CODE                     (88888888)

/* Firmware Version: V02.00 */
#define FW_VERSION_MAJOR            (2)
#define FW_VERSION_MINOR            (0)

/* Hardware Version: V00.02 */
#define HW_VERSION_MAJOR            (0)
#define HW_VERSION_MINOR            (2)


/* Macro Defines for BMI Config -----------------------------------------------------------------*/
/* BMI mode */
typedef enum _BMI_MODE
{
    BMI_PRODUCTIVE = 1,    // User Mode Productive
    BMI_SWD_0_USER = 2,    // SWD_0 User Mode
    BMI_SWD_1_USER = 3,    // SWD_1 User Mode
    BMI_SPD_0_USER = 4,    // SPD_0 User Mode
    BMI_SPD_1_USER = 5,    // SPD_1 User Mode
    BMI_SWD_0_HAR = 6,     // SWD_0 HAR Mode
    BMI_SWD_1_HAR = 7,     // SWD_1 HAR Mode
    BMI_SPD_0_HAR = 8,     // SPD_0 HAR Mode
    BMI_SPD_1_HAR = 9,     // SPD_1 HAR Mode
    BMI_ASC_BSL_TO = 10,   // ASC_BSL TO 5sec FD Mode
    BMI_ASC_BSL = 11,      // ASC_BSL Mode 
    BMI_MAX
} BMI_MODE_t;


//BMI value to be install, if OK will trigger reset and does not return to calling routine.
#define MCU_BmiInstallationReq      (*((unsigned long (**) (unsigned short)) _BmiInstallationReq)) 

/* Interface Replace ----------------------------------------------------------------------------*/
/* System Reset Interface re-define */
#define MCU_SystemReset()           NVIC_SystemReset()


/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   MCU Clock Initial
* @Param   
* @Note    Config clock for MCU and peripherals
*          dco_dclk = 64MHz, idiv should > 0
*          MCLK = dco_dclk / (2*(idiv + fdiv/256))) = 32MHz
*          PCLK = dco_dclk / (idiv + fdiv/256)) = 64MHz
* @Return  
*******************************************************************************/
void MCU_ClockInit(void);

/*******************************************************************************
* @Brief   MCU Clock Low Speed Config
* @Param   
* @Note    Config clock for MCU and peripherals
*          dco_dclk = 64MHz, idiv should > 0
*          MCLK = dco_dclk / (2*(idiv + fdiv/256))) = 4MHz
*          PCLK = dco_dclk / (idiv + fdiv/256)) = 8MHz
* @Return  
*******************************************************************************/
void MCU_ClockConfigLowSpeed(void);

/*******************************************************************************
* @Brief   MCU GPIO Initialization
* @Param   
* @Note    Set unused pin to output low level
* @Return  
*******************************************************************************/
void MCU_GpioInit(void);

/*******************************************************************************
* @Brief   MCU Power On Initialization
* @Param   
* @Note    Config core and peripherals, init the software task
* @Return  
*******************************************************************************/
void MCU_PowerOnInit(void);

/*******************************************************************************
* @Brief   MCU System Tick Timer Disable
* @Param   
* @Note    Disable system tick timer and interrupt request
* @Return  
*******************************************************************************/
void MCU_SysTickDisable(void);

/*******************************************************************************
* @Brief   MCU Power Dip Event
* @Param   
* @Note    Close peripherals and set clock to low speed for saving energy
* @Return  
*******************************************************************************/
void MCU_PowerDipEvent(void);

/*******************************************************************************
* @Brief   MCU Change BMI Mode
* @Param   [in]BMI - BMI value to be install
* @Note    If OK will trigger reset and does not return to calling routine.
* @Return  
*******************************************************************************/
void MCU_ChangeBMI(BMI_MODE_t bmi_mode);

/*******************************************************************************
* @Brief   MCU OVP Level Triggger Detect Task
* @Param   
* @Note    Detect OVP level trigger and enter protect mode if triggered
*          If application has been protected mode, this will not active again
* @Return  
*******************************************************************************/
void MCU_OVPLevelTriggerTask(void);


/* Define blank function interface */
#if defined(OT_NFC_IP67_200W) || defined(OT_NFC_IP67_150W) || defined(OT_NFC_IP67_60W)
    #define MCU_NfcPowerPinInit()           {P0_4_set_mode(OUTPUT_PP_GP); P0_4_set();}
    #define MCU_NfcPowerPinToggle()         {P0_4_toggle();}
    #define MCU_NfcPowerPinSet()            {P0_4_set();} 
    #define MCU_NfcPowerPinReset()          {P0_4_reset();}

#elif defined(OT_NFC_IP67_100W)
    #define MCU_NfcPowerPinInit()           {P0_4_set_mode(OUTPUT_PP_GP); P0_4_set();}
    #define MCU_NfcPowerPinToggle()         {P0_4_toggle();}
    #define MCU_NfcPowerPinSet()            {P0_4_set();} 
    #define MCU_NfcPowerPinReset()          {P0_4_reset();}

#else
#error "mcu_config.h: missing the config file"
#endif 	

#endif /* _MCU_CONFIG_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
