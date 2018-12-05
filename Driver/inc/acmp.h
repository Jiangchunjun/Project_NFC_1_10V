/*
***************************************************************************************************
*                           Analog Compare Function Implementation
*
* File   : acmp.c
* Author : Douglas Xie
* Date   : 2016.06.16
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/


#ifndef _ACMP_H
#define _ACMP_H

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_acmp.h>
#include <xmc_gpio.h>

/* Macro Defines ---------------------------------------------------------------------------------*/
/* ACMP Result State define */
#define ACMP_STATE_NORMAL   (0x00)
#define ACMP_STATE_TRIGGER  (0xAA)

/* ACMP Instance Define (regard as channel) */
#define ACMP_CHANNEL_0      (0)
#define ACMP_CHANNEL_1      (1)
#define ACMP_CHANNEL_2      (2)

/* ACMP Output Pins Define */
#define ACMP_OUT_CH0_PIN    P2_10   /* ACMP0_OUT */
//#define ACMP_OUT_CH0_PIN    P0_10   /* ACMP0_OUT -- input ALT4 */
#define ACMP_OUT_CH1_PIN    P1_0    /* ACMP1_OUT */
#define ACMP_OUT_CH2_PIN    P1_2    /* ACMP2_OUT */
//#define ACMP_OUT_CH2_PIN    P0_5    /* ACMP2_OUT */

/* ACMP Input Pins Define */
#define ACMP_REF_PIN        P2_11   /* ACMP_REF */
#define ACMP_INN_CH0_PIN    P2_8    /* ACMP0_INN */
#define ACMP_INP_CH0_PIN    P2_9    /* ACMP0_INP */
#define ACMP_INN_CH1_PIN    P2_6    /* ACMP1_INN */
#define ACMP_INP_CH1_PIN    P2_7    /* ACMP1_INP */
#define ACMP_INN_CH2_PIN    P2_1    /* ACMP2_INN */
#define ACMP_INP_CH2_PIN    P2_2    /* ACMP2_INP */


/*-------- ACMP Specific Channel Name define --------*/
/* ACMP Specific Name for OVP */
#define ACMP_OVP_CHANNEL        ACMP_CHANNEL_0
#define ACMP_OVP_OUT_PIN        ACMP_OUT_CH0_PIN
#define ACMP_OVP_INN_PIN        ACMP_INN_CH0_PIN
#define ACMP_OVP_INP_PIN        ACMP_INP_CH0_PIN
/* ACMP OVP Register & Mask */
#define ACMP_OVP_REGISTER       COMPARATOR->ANACMP0
#define ACMP_VOP_RESULT_MASK    COMPARATOR_ANACMP0_CMP_OUT_Msk

/* ACMP Specific Name for OCP */
#define ACMP_OCP_CHANNEL        ACMP_CHANNEL_1
#define ACMP_OCP_OUT_PIN        ACMP_OUT_CH1_PIN
#define ACMP_OCP_INN_PIN        ACMP_INN_CH1_PIN
#define ACMP_OCP_INP_PIN        ACMP_INP_CH1_PIN

/* ACMP Specific Name for Power Supply Detect */
#define ACMP_DETECT_CHANNEL     ACMP_CHANNEL_1
#define ACMP_DETECT_OUT_PIN     ACMP_OUT_CH1_PIN
#define ACMP_DETECT_INN_PIN     ACMP_INN_CH1_PIN
#define ACMP_DETECT_INP_PIN     ACMP_INP_CH1_PIN

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable ACMP Module OCP Channel
* @Param   
* @Note    config GPIO, ACMP module OCP channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableOCP(void);

/*******************************************************************************
* @Brief   Enable ACMP Module OVP Channel
* @Param   
* @Note    config GPIO, ACMP module OVP channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableOVP(void);

/*******************************************************************************
* @Brief   Enable ACMP Module Power Supply Detect Channel
* @Param   
* @Note    config GPIO, ACMP module power supply detect channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableDetect(void);

/*******************************************************************************
* @Brief   Disable ACMP Module OCP Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableOCP(void);

/*******************************************************************************
* @Brief   Disable ACMP Module OVP Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableOVP(void);

/*******************************************************************************
* @Brief   Disable ACMP Module Power Supply Detect Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableDetect(void);

/*******************************************************************************
* @Brief   ACMP Enable OVP Level Event
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void ACMP_EnableLevelEvent_OVP(void);

/*******************************************************************************
* @Brief   ACMP Detect OVP Level Event
* @Param   
* @Note    Check ACMP OVP result for level trigger
* @Return  result state ACMP_STATE_NORMAL/ACMP_STATE_TRIGGER
*******************************************************************************/
uint8_t ACMP_CheckLevelEvent_OVP(void);

#endif /* _ACMP_H */ 

