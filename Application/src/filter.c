/*
***************************************************************************************************
*                               Data Filter Algorithm
*
* File   : filter.c
* Author : Douglas Xie
* Date   : 2017.12.19
***************************************************************************************************
* Copyright (C) 2017 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/


/* Include Head Files ---------------------------------------------------------------------------*/
#include "stdint.h"
#include "stdbool.h"
#include "filter.h"

/* Macro Defines --------------------------------------------------------------------------------*/
#define FILTER_RANK     10
#define FILTER_RATIO    {0.2, 0.15, 0.15, 0.1, 0.1, 0.1, 0.05, 0.05, 0.05, 0.05}

/* Private Variable -----------------------------------------------------------------------------*/
const float  filter_ratio[FILTER_RANK] = FILTER_RATIO;

struct
{
    bool init_state;
    uint16_t output;
    uint16_t input[FILTER_RANK];
} filter = 
{
    .init_state = true,
    .output = 0
};


/* Global Variable ------------------------------------------------------------------------------*/

/* Function Declaration -------------------------------------------------------------------------*/

/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @Brief   Filter Algorithm Init
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void Filter_Init(void)
{
    uint16_t i = 0;
    
    for(i=0; i<FILTER_RANK; i++)
    {
        filter.input[i] = 0;
    }

    filter.output = 0;
    filter.init_state = true;
}


/*******************************************************************************
* @Brief   Filter Data Input
* @Param   [in]din: input data
* @Note    
* @Return  filter result
*******************************************************************************/
uint16_t Filter_Input(uint16_t din)
{
    uint16_t i = 0;
    float temp = 0;

    if(filter.init_state == true)
    {
        /* first enter filter after init */
        for(i=0; i<FILTER_RANK; i++)
        {
            filter.input[i] = din;
        }
        
        filter.init_state = false;
        filter.output = din;        
    }
    else
    {
        /* shift data queue */
        for(i=1; i<FILTER_RANK; i++)
        {
            filter.input[i] = filter.input[i-1];
        }
        filter.input[0] = din;
        
        /* filter with rank ratio */
        for(i=0; i<FILTER_RANK; i++)
        {
            temp += filter.input[i] * filter_ratio[i];
        }
        
        filter.output = (uint16_t)temp;
    }
    
    return filter.output;   
}

/*******************************************************************************
* @Brief   Filter Data Output
* @Param   
* @Note    
* @Return  filter result
*******************************************************************************/
uint16_t Filter_GetOnput(void)
{
    return filter.output;
}

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
