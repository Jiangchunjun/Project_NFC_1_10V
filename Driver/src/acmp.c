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

/* Include Head Files ----------------------------------------------------------------------------*/
#include "acmp.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/
uint8_t g_enable_trigger_ovp = 0;

/* Private Variable ------------------------------------------------------------------------------*/

/* Constant Variable -----------------------------------------------------------------------------*/
/* ACMP Slice configuration */
const XMC_ACMP_CONFIG_t mg_acmp_config =
{
    .filter_disable = 1U,
    .output_invert = 0U,
    .hysteresis = XMC_ACMP_HYSTERESIS_OFF
};


/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Enable ACMP Module OCP Channel
* @Param   
* @Note    config GPIO, ACMP module OCP channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableOCP(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_OCP_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_OCP_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    acmp_gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
//    XMC_GPIO_Init(ACMP_OCP_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* ACMP instance config */
    XMC_ACMP_Init(XMC_ACMP0, ACMP_OCP_CHANNEL, &mg_acmp_config);
    
    /* Enable instance comparator */
    XMC_ACMP_EnableComparator(XMC_ACMP0, ACMP_OCP_CHANNEL);
}


/*******************************************************************************
* @Brief   Enable ACMP Module OVP Channel
* @Param   
* @Note    config GPIO, ACMP module OVP channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableOVP(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_OVP_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_OVP_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    acmp_gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
//    XMC_GPIO_Init(ACMP_OVP_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* ACMP instance config */
    XMC_ACMP_Init(XMC_ACMP0, ACMP_OVP_CHANNEL, &mg_acmp_config);
    
    /* Enable instance comparator */
    XMC_ACMP_EnableComparator(XMC_ACMP0, ACMP_OVP_CHANNEL);
    
    g_enable_trigger_ovp = 0;
}


/*******************************************************************************
* @Brief   Enable ACMP Module Power Supply Detect Channel
* @Param   
* @Note    config GPIO, ACMP module power supply detect channel and start it
* @Return  
*******************************************************************************/
void ACMP_EnableDetect(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_DETECT_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_DETECT_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    acmp_gpio_config.mode = XMC_GPIO_MODE_OUTPUT_PUSH_PULL_ALT6;
//    XMC_GPIO_Init(ACMP_DETECT_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* ACMP instance config */
    XMC_ACMP_Init(XMC_ACMP0, ACMP_DETECT_CHANNEL, &mg_acmp_config);
    
    /* Enable instance comparator */
    XMC_ACMP_EnableComparator(XMC_ACMP0, ACMP_DETECT_CHANNEL);
}


/*******************************************************************************
* @Brief   Disable ACMP Module OCP Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableOCP(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_OCP_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_OCP_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    XMC_GPIO_Init(ACMP_OCP_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* Disable instance comparator */
    XMC_ACMP_DisableComparator(XMC_ACMP0, ACMP_OCP_CHANNEL);
}


/*******************************************************************************
* @Brief   Disable ACMP Module OVP Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableOVP(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_OVP_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_OVP_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    XMC_GPIO_Init(ACMP_OVP_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* Disable instance comparator */
    XMC_ACMP_DisableComparator(XMC_ACMP0, ACMP_OVP_CHANNEL);
    
     g_enable_trigger_ovp = 0;
}


/*******************************************************************************
* @Brief   Disable ACMP Module Power Supply Detect Channel
* @Param   
* @Note    config GPIO to input mode and stop ACMP module
* @Return  
*******************************************************************************/
void ACMP_DisableDetect(void)
{
    XMC_GPIO_CONFIG_t acmp_gpio_config;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Input config */
    acmp_gpio_config.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    XMC_GPIO_Init(ACMP_DETECT_INN_PIN, &acmp_gpio_config);
    XMC_GPIO_Init(ACMP_DETECT_INP_PIN, &acmp_gpio_config);
    
//    /* Output config */
//    XMC_GPIO_Init(ACMP_DETECT_OUT_PIN, &acmp_gpio_config);
    
    /*--------- ACMP Configuration --------------------------------------------*/
    /* Disable instance comparator */
    XMC_ACMP_DisableComparator(XMC_ACMP0, ACMP_DETECT_CHANNEL);
}

/*******************************************************************************
* @Brief   ACMP Enable OVP Level Event
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void ACMP_EnableLevelEvent_OVP(void)
{
    g_enable_trigger_ovp = 1;
}

/*******************************************************************************
* @Brief   ACMP Detect OVP Level Event
* @Param   
* @Note    Check ACMP OVP result for level trigger
* @Return  result state ACMP_STATE_NORMAL/ACMP_STATE_TRIGGER
*******************************************************************************/
uint8_t ACMP_CheckLevelEvent_OVP(void)
{
    uint8_t state = ACMP_STATE_NORMAL;
    
    /* Run if trigger event has enabled */
    if(g_enable_trigger_ovp == 0)
    {
        state = ACMP_STATE_NORMAL;
    }
    else
    {
        if((ACMP_OVP_REGISTER & ACMP_VOP_RESULT_MASK) == 0)
        {
            /* Signal > Reference, Trigger OVP */
            state = ACMP_STATE_TRIGGER;
        }
        else
        {
            /* Signal < Reference, Normal Operation */
            state = ACMP_STATE_NORMAL;
        }
    }
    
    return state;
}
