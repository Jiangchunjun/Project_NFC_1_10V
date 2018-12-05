/*
***************************************************************************************************
*                           Event Request Unit Implementation
*
* File   : eru.h
* Author : Douglas Xie
* Date   : 2016.06.16
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef _ERU_H
#define _ERU_H

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_gpio.h>
#include <xmc_eru.h>
#include "pwm.h"
#include "usart.h"

/* Macro Defines ---------------------------------------------------------------------------------*/
/* ERU Input Channel Select Define */
#define ERU_CH0_INPUT       ERU0_ETL0_INPUTA_ACMP0_OUT
#define ERU_CH1_INPUT       ERU0_ETL1_INPUTA_ACMP1_OUT
#define ERU_CH2_INPUT       ERU0_ETL2_INPUTA_ACMP2_OUT
#define ERU_CH3_INPUT       ERU0_ETL3_INPUTA_VADC0_G0BFLOUT3

/* ERU Output Channel Select Define */
#define ERU_CH0_OUTPUT      XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL0
#define ERU_CH1_OUTPUT      XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL1
#define ERU_CH2_OUTPUT      XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL2
#define ERU_CH3_OUTPUT      XMC_ERU_ETL_OUTPUT_TRIGGER_CHANNEL3

/*-------- ERU Specific Channel Name define --------*/
/* ERU Specific Name for OVP */
#define ERU_OVP_ELT         ERU0_ETL0
#define ERU_OVP_OGU         ERU0_OGU0
#define ERU_OVP_IRQ         ERU0_0_IRQn
#define ERU_OVP_PRIORITY    (1U)
#define ERU_OVP_INPUT       ERU_CH0_INPUT
#define ERU_OVP_OUTPUT      ERU_CH0_OUTPUT

/* ERU Specific Name for OCP */
#define ERU_OCP_ELT         ERU0_ETL1
#define ERU_OCP_OGU         ERU0_OGU1
#define ERU_OCP_IRQ         ERU0_1_IRQn
#define ERU_OCP_PRIORITY    (1U)
#define ERU_OCP_INPUT       ERU_CH1_INPUT
#define ERU_OCP_OUTPUT      ERU_CH1_OUTPUT

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   ERU Enable OVP Response
* @Param   
* @Note    Config ACMP output signal to trigger ERU interrupt for OVP
* @Return  
*******************************************************************************/
void ERU_EnableOVP(void);

/*******************************************************************************
* @Brief   ERU Enable OCP Response
* @Param   
* @Note    Config ACMP output signal to trigger ERU interrupt for OCP
* @Return  
*******************************************************************************/
void ERU_EnableOCP(void);

/*******************************************************************************
* @Brief   ERU Disable OVP Response
* @Param   
* @Note    Disable ERU interrupt for OVP
* @Return  
*******************************************************************************/
void ERU_DisableOVP(void);

/*******************************************************************************
* @Brief   ERU Disable OCP Response
* @Param   
* @Note    Disable ERU interrupt for OCP
* @Return  
*******************************************************************************/
void ERU_DisableOCP(void);


#endif /* _ERU_H */ 

