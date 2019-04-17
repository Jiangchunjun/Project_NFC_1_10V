/*
***************************************************************************************************
*                                ADC Interface Implementation
*
* File   : adc.h
* Author : Douglas Xie
* Date   : 2016.06.02
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef __ADC_H_
#define __ADC_H_

/* Include Head Files ----------------------------------------------------------------------------*/
#include <xmc_vadc.h>
#include <xmc_gpio.h>

#ifdef DEBUG_VERSION
#include "usart.h"
#include "stdio.h"
#endif /* DEBUG_VERSION */

/* Macro Defines ---------------------------------------------------------------------------------*/
/* Function Enable define, comment the macro define to disable the function */
#define ENABLE_ADC_CH_DIM       /* Enable ADC channel for 1-10V dimming */

/* VADC Precision */
#define ADC_MAX_COUNTER         (4095)      /* 12bit ADC, max = 2^12 - 1 */
#define ADC_REF_VOLTAGE         (5000)      /* Unit: mV,  Reference Voltage: Vref = 5V = 5000mV */

/* VADC SHS Gain Factor define */
#define ADC_GAIN_FACTOR_1       (0)
#define ADC_GAIN_FACTOR_3       (1)
#define ADC_GAIN_FACTOR_6       (2)
#define ADC_GAIN_FACTOR_12      (3)

/* VADC Global Parameter define */
#define ADC_GROUP_PTR           (VADC_G1)   /* Group 1 */
#define ADC_GROUP_ID            (1)
#define ADC_GROUP_INDEX         (XMC_VADC_GROUP_INDEX_1)
#define ADC_IRQ_PRIORITY        (10U)

/* VADC Channel Parameter define for Output Voltage */
#define ADC_CHANNEL_UOUT        (2U)        /* VADC0_G1CH2 - P2.10 */
#define ADC_RESULT_REG_UOUT     (0)         
#define ADC_GPIO_PORT_UOUT      (XMC_GPIO_PORT2)
#define ADC_GPIO_PIN_UOUT       (10)
#define ADC_GAIN_UOUT           (ADC_GAIN_FACTOR_1)

/* VADC Channel Parameter define for Output Current */
#define ADC_CHANNEL_IOUT        (3U)        /* VADC0_G1CH3 - P2.11 */
#define ADC_RESULT_REG_IOUT     (4)
#define ADC_GPIO_PORT_IOUT      (XMC_GPIO_PORT2)
#define ADC_GPIO_PIN_IOUT       (11)
#define ADC_GAIN_IOUT           (ADC_GAIN_FACTOR_6)

/* VADC Channel Parameter define for 1-10V Dimming */
#define ADC_CHANNEL_DIM         (7U)        /* VADC0_G1CH7 - P2.5 */
#define ADC_RESULT_REG_DIM      (12)
#define ADC_GPIO_PORT_DIM       (XMC_GPIO_PORT2)
#define ADC_GPIO_PIN_DIM        (5)
#define ADC_GAIN_DIIM           (ADC_GAIN_FACTOR_1)

/* ADC Buffer Size Define */
//#define ADC_BUFFER_SIZE_IOUT    (240)
#define ADC_BUFFER_SIZE_UOUT    (120)
#define ADC_BUFFER_SIZE_DIM     (120)//120 add by moon

/* ADC Start Flag define */
#define ADC_SAMPLE_START        (1)
#define ADC_SAMPLE_STOP         (0)

/* ADC Step Respond define */
//#define ADC_STEP_DIFFER_I       (50)//50
#define ADC_STEP_DIFFER_U       (300)    /* about 3.4V */
#define ADC_STEP_RESPOND_COUNT  (5)
//#define ADC_STEP_DIFFER_I       (15)
//#define ADC_STEP_DIFFER_U       (25)    /* about 3.4V */
//#define ADC_STEP_RESPOND_COUNT  (5)

/* Global Variable -------------------------------------------------------------------------------*/

/* Function Declaration --------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable ADC Module and Start
* @Param   
* @Note    config VADC module and start
*          define ENABLE_ADC_IRQ can enable IRQ function
* @Return  
*******************************************************************************/
void ADC_Enable(void);

/*******************************************************************************
* @Brief   Disable ADC Module
* @Param   
* @Note    Disable VADC module and set gpio to normal input state
* @Return  
*******************************************************************************/
void ADC_Disable(void);

/*******************************************************************************
* @Brief   Reset ADC Buffer
* @Param   
* @Note    Clear buffer and relative value
* @Return  
*******************************************************************************/
void ADC_BufferReset(void);

/*******************************************************************************
* @Brief   Read ADC Convert Result
* @Param   
* @Note    read out result to data buffer, calculation will be process in main.c
* @Return  
*******************************************************************************/
void ADC_ReadResult(void);

/*******************************************************************************
* @Brief   Calculate Average ADC
* @Param   
* @Note    read out result to data buffer, calculation will be process in main.c
* @Return  
*******************************************************************************/
void ADC_CalculateAverage(void);

/*******************************************************************************
* @Brief   Get Average ADC
* @Param   [in]channel: input ADC channel such as ADC_CHANNEL_IOUT
* @Note     
* @Return  channel average ADC result
*******************************************************************************/
uint16_t ADC_GetAverage(uint8_t channel);

/*******************************************************************************
* @Brief   Get Sample ADC
* @Param   [in]channel: input ADC channel such as ADC_CHANNEL_IOUT
* @Note     
* @Return  channel sample ADC result
*******************************************************************************/
uint16_t ADC_GetSample(uint8_t channel);

/*******************************************************************************
* @Brief   Get ADC Start Flag
* @Param   
* @Note     
* @Return  start flag of g_adc_enable_flag
*******************************************************************************/
uint8_t ADC_GetStartFlag(void);

#ifdef DEBUG_VERSION
void ADC_PrintBuffer(void);
#endif /* DEBUG_VERSION */

#endif /* __ADC_H_ */ 
