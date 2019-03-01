/*
***************************************************************************************************
*                           Event Request Unit Implementation
*
* File   : eru.c
* Author : Douglas Xie
* Date   : 2016.06.16
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ----------------------------------------------------------------------------*/
#include "eru.h"

/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/

/* Private Variable ------------------------------------------------------------------------------*/

/* Constant Variable -----------------------------------------------------------------------------*/
/* ERU OVP Event Generator Configuration */
const XMC_ERU_ETL_CONFIG_t OVP_source_config =
{
    .input = ERU_OVP_INPUT,
    .source = XMC_ERU_ETL_SOURCE_A,
    .edge_detection = XMC_ERU_ETL_EDGE_DETECTION_FALLING, //XMC_ERU_ETL_EDGE_DETECTION_BOTH,
    .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
    .enable_output_trigger = true,
    .output_trigger_channel = ERU_OVP_OUTPUT
};

/* ERU OCP Event Generator Configuration */
const XMC_ERU_ETL_CONFIG_t OCP_source_config =
{
    .input = ERU_OCP_INPUT,
    .source = XMC_ERU_ETL_SOURCE_A,
    .edge_detection = XMC_ERU_ETL_EDGE_DETECTION_FALLING, //XMC_ERU_ETL_EDGE_DETECTION_BOTH,
    .status_flag_mode = XMC_ERU_ETL_STATUS_FLAG_MODE_HWCTRL,
    .enable_output_trigger = true,
    .output_trigger_channel = ERU_OCP_OUTPUT
};

/* ERU OVP Event Detection Configuration */
const XMC_ERU_OGU_CONFIG_t OVP_trigger_config =
{
    .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER
};

/* ERU OCP Event Detection Configuration */
const XMC_ERU_OGU_CONFIG_t OCP_trigger_config =
{
    .service_request = XMC_ERU_OGU_SERVICE_REQUEST_ON_TRIGGER
};



/* Function Declaration --------------------------------------------------------------------------*/

/* Function Implement ----------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   ERU Enable OVP Response
* @Param   
* @Note    Config ACMP output signal to trigger ERU interrupt for OVP
* @Return  
*******************************************************************************/
void ERU_EnableOVP(void)
{
    /* Config ERU source input and trigger output */
    XMC_ERU_ETL_Init(ERU_OVP_ELT, &OVP_source_config);
    XMC_ERU_OGU_Init(ERU_OVP_OGU, &OVP_trigger_config);
    
    /* Config interrupt */
    NVIC_SetPriority(ERU_OVP_IRQ, ERU_OVP_PRIORITY);
    NVIC_ClearPendingIRQ(ERU_OVP_IRQ);
    NVIC_EnableIRQ(ERU_OVP_IRQ);
}

/*******************************************************************************
* @Brief   ERU Enable OCP Response
* @Param   
* @Note    Config ACMP output signal to trigger ERU interrupt for OCP
* @Return  
*******************************************************************************/
void ERU_EnableOCP(void)
{
    /* Config ERU source input and trigger output */
    XMC_ERU_ETL_Init(ERU_OCP_ELT, &OCP_source_config);
    XMC_ERU_OGU_Init(ERU_OCP_OGU, &OCP_trigger_config);
    
    /* Config interrupt */
    NVIC_SetPriority(ERU_OCP_IRQ, ERU_OCP_PRIORITY);
    NVIC_EnableIRQ(ERU_OCP_IRQ);
}

/*******************************************************************************
* @Brief   ERU Disable OVP Response
* @Param   
* @Note    Disable ERU interrupt for OVP
* @Return  
*******************************************************************************/
void ERU_DisableOVP(void)
{
    NVIC_DisableIRQ(ERU_OVP_IRQ);
}

/*******************************************************************************
* @Brief   ERU Disable OCP Response
* @Param   
* @Note    Disable ERU interrupt for OCP
* @Return  
*******************************************************************************/
void ERU_DisableOCP(void)
{
    NVIC_DisableIRQ(ERU_OCP_IRQ);
}

/*******************************************************************************
* @Brief   ERU Channel 0 Interrupt Handler
* @Param   
* @Note    for OVP protection
* @Return  
*******************************************************************************/
void ERU0_0_IRQHandler(void)
{ 
#ifdef ENABLE_OVP
    PWM_EnterProtection();
    //DaliBallast_CyclicTask();
#endif
    //USART_PrintInfo("OVP....\n");
}

/*******************************************************************************
* @Brief   ERU Channel 1 Interrupt Handler
* @Param   
* @Note    for OCP protection
* @Return  
*******************************************************************************/
void ERU0_1_IRQHandler(void)
{
#ifdef ENABLE_OVP
    //PWM_EnterProtection();
#endif
    //USART_PrintInfo("OCP....\n");
}

/*******************************************************************************
* @Brief   ERU Channel 2 Interrupt Handler
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void ERU0_2_IRQHandler(void)
{
    //...
}

/*******************************************************************************
* @Brief   ERU Channel 3 Interrupt Handler
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void ERU0_3_IRQHandler(void)
{
    //...
}
