/*
***************************************************************************************************
*                                ADC Interface Implementation
*
* File   : adc.c
* Author : Douglas Xie
* Date   : 2016.06.02
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

/* Include Head Files ----------------------------------------------------------------------------*/
#include "adc.h"
#include "power_config_100w.h"
#include "power_config_150w.h"
#include "power_config_200w.h"
#include "power_config_60w.h"
/* Macro Defines ---------------------------------------------------------------------------------*/

/* Global Variable -------------------------------------------------------------------------------*/

/* Private Variable ------------------------------------------------------------------------------*/
/* Identifier of the hardware group */
static XMC_VADC_GROUP_t *g_group_identifier = ADC_GROUP_PTR;

/* ADC Buffer */
static uint16_t g_buffer_iout[ADC_BUFFER_SIZE_IOUT];
static uint16_t g_buffer_uout[ADC_BUFFER_SIZE_UOUT];
#ifdef ENABLE_ADC_CH_DIM
static uint16_t g_buffer_dim[ADC_BUFFER_SIZE_DIM];
#endif /* ENABLE_ADC_CH_DIM */

/* ADC Buffer Store Index */
static uint16_t g_buffer_index_iout = 0;
static uint16_t g_buffer_index_uout = 0;
#ifdef ENABLE_ADC_CH_DIM
static uint16_t g_buffer_index_dim  = 0;
#endif /* ENABLE_ADC_CH_DIM */

/* ADC Buffer Data Counter */
static uint16_t g_buffer_count_iout = 0;
static uint16_t g_buffer_count_uout = 0;
#ifdef ENABLE_ADC_CH_DIM
static uint16_t g_buffer_count_dim  = 0;
#endif /* ENABLE_ADC_CH_DIM */

/* ADC Buffer Data Average */
static uint16_t g_buffer_average_iout = 0;
static uint16_t g_buffer_average_uout = 0;
#ifdef ENABLE_ADC_CH_DIM
static uint16_t g_buffer_average_dim  = 0;
#endif /* ENABLE_ADC_CH_DIM */

/* ADC Sample Value */
static uint16_t g_sample_iout = 0;
static uint16_t g_sample_uout = 0;
#ifdef ENABLE_ADC_CH_DIM
static uint16_t g_sample_dim  = 0;
#endif /* ENABLE_ADC_CH_DIM */

/* ADC Enable Flag */
static uint8_t g_adc_enable_flag = ADC_SAMPLE_STOP;

/* Constant Variable -----------------------------------------------------------------------------*/
/* Initialization data of VADC global resources */
const XMC_VADC_GLOBAL_CONFIG_t g_global_handle =
{
    .disable_sleep_mode_control = 0,
    .class0 = 
    {
        .conversion_mode_standard     =  XMC_VADC_CONVMODE_12BIT,
        .sample_time_std_conv         = 3U,
        .conversion_mode_emux         =  XMC_VADC_CONVMODE_12BIT,
        .sampling_phase_emux_channel  = 0,
    },
    .class1 = 
    {
        .conversion_mode_standard     =  XMC_VADC_CONVMODE_12BIT,
        .sample_time_std_conv         = 3U,
        .conversion_mode_emux         =  XMC_VADC_CONVMODE_12BIT,
        .sampling_phase_emux_channel  = 3U
    },
    .data_reduction_control = 0,
    .wait_for_read_mode = false,
    .event_gen_enable = false,
    .boundary0 = 0,
    .boundary1 = 0
};

/* Initialization data of a VADC group */
const XMC_VADC_GROUP_CONFIG_t g_group_handle =
{
    .class0 = 
    {
        .conversion_mode_standard     =  XMC_VADC_CONVMODE_12BIT,
        .sample_time_std_conv         = 3U,
        .conversion_mode_emux         =  XMC_VADC_CONVMODE_12BIT,
        .sampling_phase_emux_channel  = 3U,
    },
    .class1 = 
    {
        .conversion_mode_standard     =  XMC_VADC_CONVMODE_12BIT,
        .sample_time_std_conv         = 3U,
        .conversion_mode_emux         =  XMC_VADC_CONVMODE_12BIT,
        .sampling_phase_emux_channel  = 3U,
    },
    .arbitration_round_length = 0x0,
    .arbiter_mode =  XMC_VADC_GROUP_ARBMODE_ALWAYS,
    .boundary0 = 0, /* Boundary-0 */
    .boundary1 = 0, /* Boundary-1 */
    .emux_config = 
    {
        .emux_mode = XMC_VADC_GROUP_EMUXMODE_SWCTRL,
        .stce_usage = 0,
        .emux_coding = XMC_VADC_GROUP_EMUXCODE_BINARY,
        .starting_external_channel = 0,
        .connected_channel = 0
    }
};

/* Channel configuration data for output voltage */
const XMC_VADC_CHANNEL_CONFIG_t  g_channel_handle_uout =
{
    .channel_priority           = 1U,    /* true or false */
    .input_class                = XMC_VADC_CHANNEL_CONV_GROUP_CLASS1,
    .alias_channel              = XMC_VADC_CHANNEL_ALIAS_DISABLED,
    .bfl                        = 0,
    .event_gen_criteria         = XMC_VADC_CHANNEL_EVGEN_ALWAYS,
    .alternate_reference        = XMC_VADC_CHANNEL_REF_INTREF,
    .result_reg_number          = (uint8_t)ADC_RESULT_REG_UOUT,
    .sync_conversion            = false, /* Sync Feature disabled*/
    .result_alignment           = XMC_VADC_RESULT_ALIGN_RIGHT,
    .use_global_result          = false,
    .broken_wire_detect_channel = false,
    .broken_wire_detect         = false
};

/* Channel configuration data for output current */
const XMC_VADC_CHANNEL_CONFIG_t  g_channel_handle_iout =
{
    .channel_priority           = 1U,    /* true or false */
    .input_class                = XMC_VADC_CHANNEL_CONV_GROUP_CLASS1,
    .alias_channel              = XMC_VADC_CHANNEL_ALIAS_DISABLED,
    .bfl                        = 0,
    .event_gen_criteria         = XMC_VADC_CHANNEL_EVGEN_ALWAYS,
    .alternate_reference        = XMC_VADC_CHANNEL_REF_INTREF,
    .result_reg_number          = (uint8_t)ADC_RESULT_REG_IOUT,
    .sync_conversion            = false, /* Sync Feature disabled*/
    .result_alignment           = XMC_VADC_RESULT_ALIGN_RIGHT,
    .use_global_result          = false,
    .broken_wire_detect_channel = false,
    .broken_wire_detect         = false
};

/* Channel configuration data for 1-10V dimming */
#ifdef ENABLE_ADC_CH_DIM
const XMC_VADC_CHANNEL_CONFIG_t  g_channel_handle_dim =
{
    .channel_priority           = 1U,    /* true or false */
    .input_class                = XMC_VADC_CHANNEL_CONV_GROUP_CLASS1,
    .alias_channel              = XMC_VADC_CHANNEL_ALIAS_DISABLED,
    .bfl                        = 0,
    .event_gen_criteria         = XMC_VADC_CHANNEL_EVGEN_ALWAYS,
    .alternate_reference        = XMC_VADC_CHANNEL_REF_INTREF,
    .result_reg_number          = (uint8_t)ADC_RESULT_REG_DIM,
    .sync_conversion            = false, /* Sync Feature disabled*/
    .result_alignment           = XMC_VADC_RESULT_ALIGN_RIGHT,
    .use_global_result          = false,
    .broken_wire_detect_channel = false,
    .broken_wire_detect         = false
};
#endif /* ENABLE_ADC_CH_DIM */

/* Result configuration data for all channel */
const XMC_VADC_RESULT_CONFIG_t g_result_handle = 
{
    .post_processing_mode   = XMC_VADC_DMM_REDUCTION_MODE,
    .data_reduction_control = 0,
    .part_of_fifo           = false,    /* No FIFO */
    .wait_for_read_mode     = true,     /* WFS */
    .event_gen_enable       = false     /* No result event */
};

/* Queue hardware configuration data */
const XMC_VADC_QUEUE_CONFIG_t g_queue_handle =
{
    .req_src_priority = (uint8_t)3, /* Highest Priority = 3, Lowest = 0 */
    .conv_start_mode  = XMC_VADC_STARTMODE_WFS,
    .external_trigger = (bool) false, /* No external trigger */
    .trigger_signal   = XMC_VADC_REQ_TR_A, /* Irrelevant */
    .trigger_edge     = XMC_VADC_TRIGGER_EDGE_NONE,
    .gate_signal      = XMC_VADC_REQ_GT_A, /* Irrelevant */
    .timer_mode       = (bool) false, /* No timer mode */
};

/* Queue entry of output voltage */
const XMC_VADC_QUEUE_ENTRY_t g_queue_entry_uout =
{
    .channel_num        = ADC_CHANNEL_UOUT,
    .refill_needed      = true,     /* Refill is needed */
    .generate_interrupt = false,    /* Interrupt generation is needed */
    .external_trigger   = false     /* External trigger is required */
};

/* Queue entry of output current */
const XMC_VADC_QUEUE_ENTRY_t g_queue_entry_iout =
{
    .channel_num        = ADC_CHANNEL_IOUT,
    .refill_needed      = true,     /* Refill is needed */
    .generate_interrupt = false,    /* Interrupt generation is needed */
    .external_trigger   = false     /* External trigger is required */
};

/* Queue entry of 1-10V dimming */
#ifdef ENABLE_ADC_CH_DIM
const XMC_VADC_QUEUE_ENTRY_t g_queue_entry_dim =
{
    .channel_num        = ADC_CHANNEL_DIM,
    .refill_needed      = true,     /* Refill is needed */
    .generate_interrupt = false,    /* Interrupt generation is needed */
    .external_trigger   = false     /* External trigger is required */
};
#endif /* ENABLE_ADC_CH_DIM */


/* Function Declaration -------------------------------------------------------------------------*/
void ADC_SHS_Calibration(void);

/* Function Implement ---------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   Enable ADC Module and Start
* @Param   
* @Note    config VADC module and start
* @Return  
*******************************************************************************/
void ADC_Enable(void)
{  
    XMC_GPIO_CONFIG_t str_gpio_input;
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Set gpio mode to be input state */
    str_gpio_input.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
    
    /* Set GPIO to input */
    XMC_GPIO_Init(ADC_GPIO_PORT_UOUT, ADC_GPIO_PIN_UOUT, &str_gpio_input);
    XMC_GPIO_Init(ADC_GPIO_PORT_IOUT, ADC_GPIO_PIN_IOUT, &str_gpio_input);
#ifdef ENABLE_ADC_CH_DIM
    XMC_GPIO_Init(ADC_GPIO_PORT_DIM, ADC_GPIO_PIN_DIM, &str_gpio_input);
#else
    str_gpio_input.mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN;
    str_gpio_input.output_level = XMC_GPIO_OUTPUT_LEVEL_LOW;
    XMC_GPIO_Init(ADC_GPIO_PORT_DIM, ADC_GPIO_PIN_DIM, &str_gpio_input);
#endif /* ENABLE_ADC_CH_DIM */
    
    /*--------- Global Configuration ------------------------------------------*/
    /* Initialize the VADC global registers */
    XMC_VADC_GLOBAL_Init(VADC, &g_global_handle);
    
    /* Configure a conversion kernel */
    XMC_VADC_GROUP_Init(g_group_identifier, &g_group_handle);
    
    /* Configure the queue request source of the aforesaid conversion kernel */
    XMC_VADC_GROUP_QueueInit(g_group_identifier, &g_queue_handle);

    /*--------- ADC Calibration -----------------------------------------------*/            
    /* Enable the analog converters */
    XMC_VADC_GROUP_SetPowerMode(g_group_identifier, XMC_VADC_GROUP_POWERMODE_NORMAL);
    
    /* Perform calibration of the converter */
    XMC_VADC_GLOBAL_StartupCalibration(VADC);
    
    ADC_SHS_Calibration();
    
    /*--------- Channel Configuration -----------------------------------------*/
    /* Configure a channel belonging to the aforesaid conversion kernel */
    XMC_VADC_GROUP_ChannelInit(g_group_identifier, ADC_CHANNEL_UOUT, &g_channel_handle_uout);
    XMC_VADC_GROUP_ChannelInit(g_group_identifier, ADC_CHANNEL_IOUT, &g_channel_handle_iout);
#ifdef ENABLE_ADC_CH_DIM
    XMC_VADC_GROUP_ChannelInit(g_group_identifier, ADC_CHANNEL_DIM, &g_channel_handle_dim);
#endif /* ENABLE_ADC_CH_DIM */
    
    /* Configure a result resource belonging to the aforesaid conversion kernel */
    XMC_VADC_GROUP_ResultInit(g_group_identifier, ADC_RESULT_REG_UOUT, &g_result_handle);
    XMC_VADC_GROUP_ResultInit(g_group_identifier, ADC_RESULT_REG_IOUT, &g_result_handle);
#ifdef ENABLE_ADC_CH_DIM
    XMC_VADC_GROUP_ResultInit(g_group_identifier, ADC_RESULT_REG_DIM, &g_result_handle);
#endif /* ENABLE_ADC_CH_DIM */
    
    /* Add the channel to the queue */
    XMC_VADC_GROUP_QueueInsertChannel(g_group_identifier, g_queue_entry_uout);
    XMC_VADC_GROUP_QueueInsertChannel(g_group_identifier, g_queue_entry_iout);
#ifdef ENABLE_ADC_CH_DIM
    XMC_VADC_GROUP_QueueInsertChannel(g_group_identifier, g_queue_entry_dim);
#endif /* ENABLE_ADC_CH_DIM */
    
    /* Set channel gain factor */
    XMC_VADC_GLOBAL_SHS_Init(SHS0, NULL);
    XMC_VADC_GLOBAL_SHS_EnableGainAndOffsetCalibrations(SHS0, ADC_GROUP_INDEX);
    XMC_VADC_GLOBAL_SHS_SetGainFactor(SHS0, ADC_GAIN_UOUT, ADC_GROUP_INDEX, ADC_CHANNEL_UOUT);
    XMC_VADC_GLOBAL_SHS_SetGainFactor(SHS0, ADC_GAIN_IOUT, ADC_GROUP_INDEX, ADC_CHANNEL_IOUT);
#ifdef ENABLE_ADC_CH_DIM
    XMC_VADC_GLOBAL_SHS_SetGainFactor(SHS0, ADC_GAIN_DIIM, ADC_GROUP_INDEX, ADC_CHANNEL_DIM);
#endif /* ENABLE_ADC_CH_DIM */
    
    /* Enable the analog converters */
    XMC_VADC_GROUP_SetPowerMode(g_group_identifier, XMC_VADC_GROUP_POWERMODE_NORMAL); 
    
    /*--------- Data Buffer Reset ---------------------------------------------*/
    ADC_BufferReset();
    g_adc_enable_flag = ADC_SAMPLE_START;  /* adc is enable, ready for sample */
}

/*******************************************************************************
* @Brief   Disable ADC Module
* @Param   
* @Note    Disable VADC module and set gpio to normal input state
* @Return  
*******************************************************************************/
void ADC_Disable(void)
{     
    XMC_GPIO_CONFIG_t str_gpio_input;
    
    /*--------- ADC Module Configuration --------------------------------------*/
    XMC_VADC_GLOBAL_DisableModule();
    
    /*--------- GPIO Configuration --------------------------------------------*/
    /* Set gpio to normal input state */
    str_gpio_input.mode = XMC_GPIO_MODE_INPUT_TRISTATE;
   
    /* Set GPIO to input */
    XMC_GPIO_Init(ADC_GPIO_PORT_UOUT, ADC_GPIO_PIN_UOUT, &str_gpio_input);
    XMC_GPIO_Init(ADC_GPIO_PORT_IOUT, ADC_GPIO_PIN_IOUT, &str_gpio_input);
    XMC_GPIO_Init(ADC_GPIO_PORT_DIM,  ADC_GPIO_PIN_DIM,  &str_gpio_input);
    
    /*--------- Data Buffer Reset ---------------------------------------------*/
    ADC_BufferReset();
    g_adc_enable_flag = ADC_SAMPLE_STOP;  /* adc has disabled */
}


/*******************************************************************************
* @Brief   Reset ADC Buffer
* @Param   
* @Note    Clear buffer and relative value
* @Return  
*******************************************************************************/
void ADC_BufferReset(void)
{
    /* Reset output voltage buffer */
    g_buffer_index_uout = 0;
    g_buffer_count_uout = 0;
    g_buffer_average_uout = 0;
    memset(g_buffer_uout, 0, sizeof(g_buffer_uout[0])*ADC_BUFFER_SIZE_UOUT);
    
    /* Reset output current buffer */
    g_buffer_index_iout = 0;
    g_buffer_count_iout = 0;
    g_buffer_average_iout = 0;
    memset(g_buffer_iout, 0, sizeof(g_buffer_iout[0])*ADC_BUFFER_SIZE_IOUT);
    
    /* Reset 1-10V dimming buffer */
#ifdef ENABLE_ADC_CH_DIM
    g_buffer_index_dim = 0;
    g_buffer_count_dim = 0;
    g_buffer_average_dim = 0;
    memset(g_buffer_dim, 0, sizeof(g_buffer_dim[0])*ADC_BUFFER_SIZE_DIM);
#endif /* ENABLE_ADC_CH_DIM */
}


/*******************************************************************************
* @Brief   Read ADC Convert Result
* @Param   
* @Note    read out result to data buffer, calculation will be process in main.c
* @Return  
*******************************************************************************/
void ADC_ReadResult(void)
{
    uint16_t        step_differ  = 0;
    static uint8_t  s_step_count_i = 0;
    static uint8_t  s_step_count_u = 0;
    extern uint8_t g_current_non_0_flag;
    static uint8_t  s_current_count=0;
    /* Check adc enable flag */
    if(g_adc_enable_flag == ADC_SAMPLE_STOP)
    {
        /* return if adc is disabled */
        return;
    }
        
    /*----------- Output Current Iout ----------------------------------------*/
    /* Read result from register */
    g_sample_iout = XMC_VADC_GROUP_GetResult(g_group_identifier, ADC_RESULT_REG_IOUT);
    g_buffer_iout[g_buffer_index_iout] = g_sample_iout;
    
    if(g_current_non_0_flag==0)
    {
      if(g_sample_iout>5)
      {
        s_current_count++;
        if(s_current_count>3)
          g_current_non_0_flag=1;
      }
    }
    /* Update output current buffer index */    
    g_buffer_index_iout++;
    if(g_buffer_index_iout >= ADC_BUFFER_SIZE_IOUT)
    {
        g_buffer_index_iout = 0;
    }
    
    /* Update output current buffer counter */
    if(g_buffer_count_iout < ADC_BUFFER_SIZE_IOUT)
    {
        g_buffer_count_iout++;
    }
    
    /* Get different between current adc and last average */
    if(g_buffer_iout[g_buffer_index_iout-1] > g_buffer_average_iout)
    {
        step_differ = g_buffer_iout[g_buffer_index_iout-1] - g_buffer_average_iout;
    }
    else
    {
        step_differ = g_buffer_average_iout - g_buffer_iout[g_buffer_index_iout-1];
    }
    
    /* Step respond process */
    if(step_differ >= ADC_STEP_DIFFER_I)
    {
        /* Respond active */
        if(s_step_count_i >= ADC_STEP_RESPOND_COUNT)
        {
            /* save lastest adc to step_differ */
            step_differ = g_buffer_iout[g_buffer_index_iout-1];
            
            /* Clear current buffer and use average value as the first data */
            memset(g_buffer_iout, 0, sizeof(g_buffer_iout[0])*ADC_BUFFER_SIZE_IOUT);
            g_buffer_iout[0] = step_differ;
            g_buffer_index_iout = 1;
            g_buffer_count_iout = 1;
            g_buffer_average_iout = g_buffer_iout[0];
        }
        else
        {
            /* Update step respond counter */
            s_step_count_i++;                
        }
    }
    else
    {
        /* Reset step respond counter */
        s_step_count_i = 0; 
    }
    
    /*----------- Output Voltage Uout ----------------------------------------*/
    /* Read result from register */
    g_sample_uout = XMC_VADC_GROUP_GetResult(g_group_identifier, ADC_RESULT_REG_UOUT);
    g_buffer_uout[g_buffer_index_uout] = g_sample_uout;
        
    /* Update output voltage buffer index */    
//    g_buffer_index_uout++;
//    if(g_buffer_index_uout >= ADC_BUFFER_SIZE_UOUT)
//    { 
//        g_buffer_index_uout = 0;
//    }
    
    /* Update output voltage buffer counter */    
    if(g_buffer_count_uout < ADC_BUFFER_SIZE_UOUT)
    {
        g_buffer_count_uout++;
    }
     
    /* Get different between current adc and last average */
    if(g_buffer_uout[g_buffer_index_uout] > g_buffer_average_uout)
    {
        step_differ = g_buffer_uout[g_buffer_index_uout]- g_buffer_average_uout;
    }
    else
    {
        step_differ = g_buffer_average_uout - g_buffer_uout[g_buffer_index_uout];
    }
    
    /* Step respond process */
    if(step_differ >= ADC_STEP_DIFFER_U)
    {
        /* Respond active */
        if(s_step_count_u >= ADC_STEP_RESPOND_COUNT)
        {
            /* save lastest adc to step_differ */
            step_differ = g_buffer_uout[g_buffer_index_uout];
            
            /* Clear current buffer and use average value as the first data */
            memset(g_buffer_uout, 0, sizeof(g_buffer_uout[0])*ADC_BUFFER_SIZE_UOUT);
            g_buffer_uout[0] = step_differ;
            g_buffer_index_uout = 1;
            g_buffer_count_uout = 1;
            g_buffer_average_uout = g_buffer_uout[0];
        }
        else
        {
            /* Update step respond counter */
            s_step_count_u++;                
        }
    }
    else
    {
        /* Reset step respond counter */
        s_step_count_u = 0; 
    }
    g_buffer_index_uout++;
    if(g_buffer_index_uout >= ADC_BUFFER_SIZE_UOUT)
    { 
      g_buffer_index_uout = 0;
    }
    
    
#ifdef ENABLE_ADC_CH_DIM
    /*----------- 1-10V Dimming Input ----------------------------------------*/
    /* Read result from register */
    g_sample_dim = XMC_VADC_GROUP_GetResult(g_group_identifier, ADC_RESULT_REG_DIM); 
    g_buffer_dim[g_buffer_index_dim] = g_sample_dim;

    /* Update 1-10V dimming buffer index */
    g_buffer_index_dim++;
    if(g_buffer_index_dim >= ADC_BUFFER_SIZE_DIM)
    {
        g_buffer_index_dim = 0;
    }
    
    /* Update 1-10V dimming buffer counter */
    if(g_buffer_count_dim < ADC_BUFFER_SIZE_DIM)
    {
        g_buffer_count_dim++;
    }
#endif /* ENABLE_ADC_CH_DIM */
}


/*******************************************************************************
* @Brief   Calculate Average ADC
* @Param   
* @Note    read out result to data buffer, calculation will be process in main.c
* @Return  
*******************************************************************************/
void ADC_CalculateAverage(void)
{
    uint16_t  i = 0;
    uint32_t  sum = 0;    
    
    /*----------- Output Current Iout ----------------------------------------*/
    /* Iout Average */
    if(g_buffer_count_iout > 0)
    {    
        sum = 0;
        for(i = 0; i < g_buffer_count_iout; i++)
        {
            sum += g_buffer_iout[i];
        }
        g_buffer_average_iout = sum / g_buffer_count_iout;
    }
    else
    {
        g_buffer_average_iout = 0;
    }
    
    /*----------- Output Voltage Uout ----------------------------------------*/
    /* Uout Average */
    if(g_buffer_count_uout > 0)
    {
        sum = 0;
        for(i = 0; i < g_buffer_count_uout; i++)
        {
            sum += g_buffer_uout[i];
        }
        g_buffer_average_uout = sum / g_buffer_count_uout;
    }
    else
    {
        g_buffer_average_uout = 0;
    }   
    
#ifdef ENABLE_ADC_CH_DIM
    /*----------- 1-10V Dimming Input ----------------------------------------*/
    /* Dimming Average */
    if(g_buffer_count_dim > 0)
    {
        sum = 0;
        for(i = 0; i < g_buffer_count_dim; i++)
        {
            sum += g_buffer_dim[i];
        }
        g_buffer_average_dim = sum / g_buffer_count_dim;
    }
    else
    {
        g_buffer_average_dim = 0;
    }  
#endif /* ENABLE_ADC_CH_DIM */
}

/*******************************************************************************
* @Brief   Get Average ADC
* @Param   [in]channel: input ADC channel such as ADC_CHANNEL_IOUT
* @Note     
* @Return  channel average ADC result
*******************************************************************************/
uint16_t ADC_GetAverage(uint8_t channel)
{
    uint16_t avg_result = 0;
    
    /* Set channel average result to variable */
    switch(channel)
    {
    case ADC_CHANNEL_UOUT:
        avg_result = g_buffer_average_uout;
        break;
        
    case ADC_CHANNEL_IOUT:
        avg_result = g_buffer_average_iout;
        break;
        
#ifdef ENABLE_ADC_CH_DIM
    case ADC_CHANNEL_DIM:
        avg_result = g_buffer_average_dim;
        break;
#endif /* ENABLE_ADC_CH_DIM */
        
    default:
        avg_result = 0;
        break;
    }
    
    return avg_result;
}

/*******************************************************************************
* @Brief   Get Sample ADC
* @Param   [in]channel: input ADC channel such as ADC_CHANNEL_IOUT
* @Note     
* @Return  channel sample ADC result
*******************************************************************************/
uint16_t ADC_GetSample(uint8_t channel)
{
    uint16_t avg_result = 0;
    
    /* Set channel average result to variable */
    switch(channel)
    {
    case ADC_CHANNEL_UOUT:
        avg_result = g_sample_uout;
        break;
        
    case ADC_CHANNEL_IOUT:
        avg_result = g_sample_iout;
        break;
        
#ifdef ENABLE_ADC_CH_DIM
    case ADC_CHANNEL_DIM:
        avg_result = g_sample_dim;
        break;
#endif /* ENABLE_ADC_CH_DIM */
        
    default:
        avg_result = 0;
        break;
    }
    
    return avg_result;
}

/*******************************************************************************
* @Brief   Get ADC Start Flag
* @Param   
* @Note     
* @Return  start flag of g_adc_enable_flag
*******************************************************************************/
uint8_t ADC_GetStartFlag(void)
{
    return g_adc_enable_flag;
}

/*******************************************************************************
* @Brief   Calibration VADC
* @Param   
* @Note    get offset calibration value and set to register
*          but not sure if it is right
* @Return  
*******************************************************************************/
void ADC_SHS_Calibration(void)
{
    uint8_t byOffset;
    
    /* Read calibration offset value from chip */
    byOffset = XMC_VADC_GLOBAL_SHS_GetOffsetCalibrationValue(SHS0, XMC_VADC_GROUP_INDEX_1, XMC_VADC_SHS_GAIN_LEVEL_1);
    
    /* Set calibration offset value to register */
    XMC_VADC_GLOBAL_SHS_SetOffsetCalibrationValue(SHS0, XMC_VADC_GROUP_INDEX_1, XMC_VADC_SHS_GAIN_LEVEL_1, byOffset);
}


#ifdef DEBUG_VERSION
void ADC_PrintBuffer(void)
{
    uint16_t i = 0;
    uint8_t buffer[8];
    
    /********************************************/
    sprintf((char*)buffer, "Iout\n\n");
    USART_PrintInfo(buffer);
    for(i = 0; i < ADC_BUFFER_SIZE_IOUT; i++)
    {
        sprintf((char*)buffer, "%4d\n", g_buffer_iout[i]);
        USART_PrintInfo(buffer);        
    }
    
    /********************************************/
    for(i = 0; i < 10; i++);
    sprintf((char*)buffer, "\n\n\n\n");
    USART_PrintInfo(buffer);
    sprintf((char*)buffer, "Uout\n\n");
    USART_PrintInfo(buffer);
    for(i = 0; i < ADC_BUFFER_SIZE_UOUT; i++)
    {
        sprintf((char*)buffer, "%4d\n", g_buffer_uout[i]);
        USART_PrintInfo(buffer);        
    }
}
#endif /* DEBUG_VERSION */

