/*
***************************************************************************************************
*                             Usart Communition Interface
*       Data Format: StartByte(1) + Command(1) + Param(1) + Data(8) + CheckSum(1) = 12bytes
*
* File   : comm_task.c
* Author : Douglas Xie
* Date   : 2016.10.13
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifdef ENABLE_COMM

/* Include Head Files ---------------------------------------------------------------------------*/
#include "comm_task.h"
#include "stdlib.h" /* srand() function */
#include "stdio.h"

#include "adc.h"
#include "pwm.h"
#include "power_task.h"
#include "nfc_task.h"
#include "driver.h"

/* OSRAM Communication Protocol -----------------------------------------------------------------*/
/* Packet data size */
#define COMM_PACKET_SIZE            USART_RX_BUFFER_SIZE /* 12 */

/*--------------------------------------*/
/* StartByte, 1st byte */
#define COMM_HEAD_RX                0xAB
#define COMM_HEAD_TX                0xBA

/*--------------------------------------*/
/* Command, 2nd byte */
/* Command 0xE0 ~ 0xFF always active */
/* Command 0x10 ~ 0xDF active when unlock */
#define COMM_NONE                   0x00

/* Unlock process */
#define COMM_REQUEST_KEY            0xE1
#define COMM_SUBMIT_PASSWORD        0xE2

/* Error information */ 
#define COMM_ERROR_CODE             0xF0    /* Error code start from 0xF0 */
#define COMM_ERROR_PACKET           0xF0
#define COMM_ERROR_COMMAND          0xF1
#define COMM_ERROR_PARAMETER        0xF2
#define COMM_ERROR_DATA             0xF3
#define COMM_ERROR_CS               0xF4
#define COMM_ERROR_LOCK             0xF5
#define COMM_ERROR_PASSWORD         0xF6
#define COMM_ERROR_TIMEOUT          0xF7
#define COMM_ERROR_UNKNOWN          0xFF

/* Echo command, use for keep unlock state */
#define COMM_ECHO                   0xAA

/* Get command */
#define COMM_GET_PROJECT_CODE       0x10    /* 3pm number */
#define COMM_GET_DEVICE_TYPE        0x11
#define COMM_GET_OUTPUT_POWER       0x12
#define COMM_GET_FW_VERSION         0x13
#define COMM_GET_HW_VERSION         0x14
#define COMM_GET_SN                 0x15
#define COMM_GET_SAMPLE_AD          0x16
#define COMM_GET_AVERAGE_AD         0x17
#define COMM_GET_RAW_VALUE          0x18
#define COMM_GET_REAL_VALUE         0x19
#define COMM_GET_LOOP_STATE         0x1A
#define COMM_GET_PWM_DUTY           0x1B
#define COMM_GET_DIMMING_LEVEL      0x1C

/* Set command */
#define COMM_SET_BOOT_MODE          0x20

/* Memory operation */
#define COMM_WRITE_MEMORY           0x30
#define COMM_READ_MEMORY            0x31
#define COMM_ERASE_MEMORY           0x32

/*--------------------------------------*/
/* Parameter, 3rd byte, user define */
/* sample channel define */
#define COMM_CH_IOUT            0x00
#define COMM_CH_UOUT            0x01
#define COMM_CH_DIMM            0x02
#define COMM_CH_ED_TIME         0x03    /* for raw/real value */
#define COMM_CH_RUN_TIME        0x04    /* for raw/real value */

/* pwm channel define */
#define COMM_PWM_CTRL           0x00
#define COMM_PWM_OVP_REF        0x01
#define COMM_PWM_OCP_REF        0x02

/* dimming level channel define */
#define COMM_DIMM_CONSTANT      0x00
#define COMM_DIMM_ASTRO         0x01
#define COMM_DIMM_EOL           0x02
#define COMM_DIMM_ONE2TEN       0x03

/* unlock parameter define */
#define COMM_UNLOCK_ONCE        0x00
#define COMM_UNLOCK_KEEP        0x01


/* Global Variable ------------------------------------------------------------------------------*/
uint8_t g_CommRxState;                  /* Data packet receive state */
uint8_t g_CommBuff[COMM_PACKET_SIZE];

uint8_t g_CommLock = COMM_UNLOCK;
uint8_t g_Key[4];
uint8_t g_Password[4];

/* Function Declaration -------------------------------------------------------------------------*/
/* Comm Task Command Handler */
void Comm_CmdHandler(uint8_t cmd);
void Comm_RequestKey(void);
void Comm_SubmitPassword(void);
void Comm_Echo(void);
void Comm_GetProjectCode(void);
void Comm_GetDeviceType(void);
void Comm_GetOutputPower(void);
void Comm_GetFirmwareVersion(void);
void Comm_GetHardwareVersion(void);
void Comm_GetSerialNumber(void);
void Comm_GetSampleAD(void);
void Comm_GetAverageAD(void);
void Comm_GetRawValue(void);
void Comm_GetRealValue(void);
void Comm_GetLoopState(void);
void Comm_GetPwmDuty(void);
void Comm_GetDimmingLevel(void);
void Comm_SetBootMode(void);
void Comm_WriteMemory(void);
void Comm_ReadMemory(void);
void Comm_EraseMemory(void);


/* Function Implement ---------------------------------------------------------------------------*/

/*******************************************************************************
* @brief  Comm Generate Password
* @param  
* @note   generate password according to key
* @retval 
*******************************************************************************/
void Comm_GeneratePassword(uint8_t *key, uint8_t *password)
{
    const uint8_t vector[4] = {0x1C, 0xAE, 0x95, 0x32};
    uint8_t i;
    
    for(i = 0; i < 4; i++)
    {
        *(password+i) = *(key+i) ^ vector[i];
        *(password+i) = *(password+i) ^ vector[3-i];
    }
}

/*******************************************************************************
* @brief  Comm Check Packet
* @param  
* @note   Check usart receive data packet. 
* @retval receive state COMM_RX_READY / COMM_RX_RECEVING / COMM_RX_DONE 
*******************************************************************************/
uint8_t Comm_CheckPacket(void)
{
    uint8_t rx_data_count = 0;
    uint8_t rx_state = COMM_RX_READY;
    
    /* Get Comm Receive Buffer Data Number */
    rx_data_count = Comm_GetRxCount();
    
    /* If Comm Receive Buffer is not empty */
    if( rx_data_count != 0 )
    { 
        rx_state = COMM_RX_RECEVING;
        
        if( Comm_CheckRxTimer() == COMM_TIMER_OFF )
        {
            /* Setup Comm Receive Timer when starting receive data */
            Comm_StartRxTimer();
        }
        
        /* If Comm Receive Timer is timeout */
        if( Comm_CheckRxTimer() == COMM_TIMER_UP )
        {
            /* Reset Comm Register */
            Comm_ResetBuffer();
            
            /* Close Comm Receive Timer */
            Comm_CloseRxTimer();  
            
            /* Send Error: Receive Timeout */
            g_CommBuff[0] = COMM_HEAD_RX; 
            g_CommBuff[1] = COMM_ERROR_TIMEOUT; 
            
            /* Update Comm Receive State */
            rx_state = COMM_RX_DONE;
        }              
        /* Check if receive data number is enough */
        else if( rx_data_count >= COMM_PACKET_SIZE )
        {
            /* Copy Receive Data to Receive Buffer */
            Comm_GetRxData(g_CommBuff);
            
            /* Close Comm Receive Timer */
            Comm_CloseRxTimer();
            
            /* Reset Comm Register */
            Comm_ResetBuffer();
            
            /* Update Comm Receive State */
            rx_state = COMM_RX_DONE;
        }
    }
    
    return rx_state;
}


/*******************************************************************************
* @Brief   CheckSum Calculate Formula 
* @Param   [in]data  - pointer to the data buffer
*          [in]size - data buffer size
* @Note    calculate data buffer checksum and return
* @Return  checksum
*******************************************************************************/
uint8_t Comm_CalcuCS(uint8_t *data, uint8_t size)
{
    uint8_t i;
    uint8_t checksum = 0;
    
    /* calculate checksum */
    for (i = 0; i < size; i++)
    {
        checksum +=  data[i];
    }
    
    return checksum;
}


/*******************************************************************************
* @brief  Comm Verify CheckSum
* @param  
* @note   Check if data packet checksum is right and return command
* @retval  
*******************************************************************************/
uint8_t Comm_VerifyCS(void)
{
    uint8_t command;
    uint8_t check_sum = 0;    
    
    command = g_CommBuff[1];
    
    /* Check if command is error code */
    if(command < COMM_ERROR_CODE)
    {
        /* Calculate packet checksum */
        check_sum = Comm_CalcuCS(g_CommBuff, COMM_PACKET_SIZE - 1);
        
        /* Verify checksum */
        if( g_CommBuff[COMM_PACKET_SIZE - 1] != check_sum )
        {
            /* Checksum error */
            command = COMM_ERROR_CS;
        }
    }
    
    return command;
}


/*******************************************************************************
* @brief  Comm Send Feedback Packet
* @param  
* @note   Update StartByte and CheckSum then send out feedback packet
* @retval  
*******************************************************************************/
void Comm_Feedback(uint8_t *data)
{
    /* Update StartByte and CheckSum */
    data[0] = COMM_HEAD_TX;
    data[COMM_PACKET_SIZE-1] = Comm_CalcuCS(data, COMM_PACKET_SIZE-1);
    
    /* Send feedback packet to PC */
    Comm_SendTxData(data, COMM_PACKET_SIZE);
}


/*******************************************************************************
* @brief  Comm Task Implement
* @param  
* @note   Analyse the data packet and excute the command 
* @retval  
*******************************************************************************/
void Comm_Task(void)
{
    uint8_t rx_state = COMM_RX_READY;
    uint8_t command = COMM_NONE;
    
    /* Check receive packet */
    rx_state = Comm_CheckPacket();
    
    /* Check if task has receive a full packet */
    if(rx_state == COMM_RX_DONE)
    {        
        /* Verify data packet checksum and return command */
        command = Comm_VerifyCS();
        
        /* Call command handler to excute the command */
        Comm_CmdHandler(command);
        
        /* Reset comm buffer */
        Comm_ResetBuffer();
        
        /* Close Comm Receive Timer */
        Comm_CloseRxTimer();  
    }
    
    /* Check unlock timer */
    if(SWT_CheckTimer(SWT_ID_COMM_UNLOCK) == SWT_STATUS_UP)
    {
        /* Unlock timer time-up, close timer and lock comm */
        SWT_CloseTimer(SWT_ID_COMM_UNLOCK);
        g_CommLock = COMM_UNLOCK;
    }
}


/* Comm Task Command Handle Implement --------------------------------------------------------*/

void Comm_CmdHandler(uint8_t cmd)
{
    /* Command that feedback error */
    if((cmd & 0xF0) == 0xF0)
    {
        switch(cmd)
        {
        case COMM_ERROR_PACKET:
        case COMM_ERROR_COMMAND:
        case COMM_ERROR_PARAMETER:
        case COMM_ERROR_DATA:
        case COMM_ERROR_CS:
        case COMM_ERROR_LOCK: 
        case COMM_ERROR_PASSWORD:
        case COMM_ERROR_TIMEOUT:
            g_CommBuff[1] = cmd; 
            break;
            
        default:
            g_CommBuff[1] = COMM_ERROR_UNKNOWN;
            break;
        }
        
        /* Send feedback packet */  
        Comm_Feedback(g_CommBuff);  
    }
    /* Command that always active */
    else if((cmd & 0xF0) == 0xE0)
    { 
        switch(cmd)
        {
        case COMM_REQUEST_KEY:
            Comm_RequestKey();
            break;
            
        case COMM_SUBMIT_PASSWORD:
            Comm_SubmitPassword();
            break;
            
        default:
            /* Error feedback */
            g_CommBuff[1] = COMM_ERROR_COMMAND; 
            Comm_Feedback(g_CommBuff);  
            break;
        }
    }
    /* Command that active when unlock */
    else
    {
        /* Comm Unlock, Excute command */
        if(g_CommLock == COMM_UNLOCK)
        {
            switch(cmd)
            {
            case COMM_ECHO:
                Comm_Echo();
                break;
            case COMM_GET_PROJECT_CODE:
                Comm_GetProjectCode();
                break;
            case COMM_GET_DEVICE_TYPE:
                Comm_GetDeviceType();
                break;
            case COMM_GET_OUTPUT_POWER:
                Comm_GetOutputPower();
                break;
            case COMM_GET_FW_VERSION:
                Comm_GetFirmwareVersion();
                break;
            case COMM_GET_HW_VERSION:
                Comm_GetHardwareVersion();
                break;
            case COMM_GET_SN:
                Comm_GetSerialNumber();
                break;
            case COMM_GET_SAMPLE_AD:
                Comm_GetSampleAD();
                break;
            case COMM_GET_AVERAGE_AD:
                Comm_GetAverageAD();
                break;
            case COMM_GET_RAW_VALUE:
                Comm_GetRawValue();
                break;
            case COMM_GET_REAL_VALUE:
                Comm_GetRealValue();
                break;
            case COMM_GET_LOOP_STATE:
                Comm_GetLoopState();
                break;
            case COMM_GET_PWM_DUTY:
                Comm_GetPwmDuty();
                break;
            case COMM_GET_DIMMING_LEVEL:
                Comm_GetDimmingLevel();
                break;
            case COMM_SET_BOOT_MODE:
                Comm_SetBootMode();
                break;
            case COMM_WRITE_MEMORY:
                Comm_SetBootMode();
                break;
            case COMM_READ_MEMORY:
                Comm_SetBootMode();
                break;
            case COMM_ERASE_MEMORY:
                Comm_SetBootMode();
                break;
                
            default:
                /* Error feedback */
                g_CommBuff[1] = COMM_ERROR_COMMAND; 
                Comm_Feedback(g_CommBuff);  
                break;  
            }
        }
        /* Comm Lock, Ignore command */
        else
        {
            /* Error feedback */
            g_CommBuff[1] = COMM_ERROR_LOCK; 
            Comm_Feedback(g_CommBuff);
        }
    }
}

//=============================================================================
void Comm_RequestKey(void)
{   
    uint8_t i;
    uint16_t seed;
    
    /* Set seed for srand(), init random function */
    seed = Tick_GetTicks();
    srand(seed); 
    
    /* Generate random key */
    for(i = 0; i < 4; i++)
    {
        g_Key[i] = rand() % 256;
        g_CommBuff[i+3] = g_Key[i];
    }
    
    /* Generate password according to key */
    Comm_GeneratePassword(g_Key, g_Password);
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_SubmitPassword(void)
{   
    uint8_t i;
    uint8_t state = COMM_OK;
    
    /* Verify password from PC */
    for(i = 0; i < 4; i++)
    {
        if(g_Password[i] != g_CommBuff[i+3])
        {
            state = COMM_ERROR;   
            break;
        }
    }
    
    if(state == COMM_OK)
    {
        /* Unlock comm & start unlock timer */
        g_CommLock = COMM_UNLOCK;
        
        if(g_CommBuff[2] == COMM_UNLOCK_ONCE)
        {
            /* Start unlock timer */
            SWT_StartTimer(SWT_ID_COMM_UNLOCK, COMM_UNLOCK_TIMEOUT);
        }
        
        if(g_CommBuff[2] == COMM_UNLOCK_KEEP)
        {
            /* Stop unlock timer */
            SWT_CloseTimer(SWT_ID_COMM_UNLOCK);
        }
        
        /* Feedback the same data when password ok */
    }
    else
    {
        /* Lock comm */
        g_CommLock = COMM_LOCK;
        
        /* Error feedback */
        g_CommBuff[1] = COMM_ERROR_PASSWORD; 
    }
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_Echo(void)
{
    /* Send feedback packet for keep device unlock */
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetProjectCode(void)
{  
    /* Copy code from high to low byte */
    g_CommBuff[3] = (PROJECT_CODE & 0xFF000000) >> 24; 
    g_CommBuff[4] = (PROJECT_CODE & 0x00FF0000) >> 16; 
    g_CommBuff[5] = (PROJECT_CODE & 0x0000FF00) >> 8; 
    g_CommBuff[6] = (PROJECT_CODE & 0x000000FF); 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetDeviceType(void)
{
    /* Copy code from high to low byte */
    g_CommBuff[3] = DEVICE_TYPE >> 8; 
    g_CommBuff[4] = DEVICE_TYPE & 0xFF; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetOutputPower(void)
{
    uint16_t power = OUTPUT_POWER / 1000;
    
    /* Copy code from high to low byte */
    g_CommBuff[3] = power >> 8; 
    g_CommBuff[4] = power & 0xFF; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetFirmwareVersion(void)
{
    /* Copy code from high to low byte */
    g_CommBuff[3] = FW_VERSION_MAJOR; 
    g_CommBuff[4] = FW_VERSION_MINOR; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetHardwareVersion(void)
{
    /* Copy code from high to low byte */
    g_CommBuff[3] = HW_VERSION_MAJOR; 
    g_CommBuff[4] = HW_VERSION_MINOR; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetSerialNumber(void)
{
    /* Copy code from high to low byte */
    g_CommBuff[3] = (SN_CODE & 0xFF000000) >> 24; 
    g_CommBuff[4] = (SN_CODE & 0x00FF0000) >> 16; 
    g_CommBuff[5] = (SN_CODE & 0x0000FF00) >> 8; 
    g_CommBuff[6] = (SN_CODE & 0x000000FF); 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetSampleAD(void)
{
    uint16_t sample_adc = 0;
    
    if(g_CommBuff[2] == COMM_CH_IOUT)
    {
        sample_adc = ADC_GetSample(ADC_CHANNEL_IOUT);
    }
    else if(g_CommBuff[2] == COMM_CH_UOUT)
    {
        sample_adc = ADC_GetSample(ADC_CHANNEL_UOUT);
    }
    else if(g_CommBuff[2] == COMM_CH_DIMM)
    {
        sample_adc = ADC_GetSample(ADC_CHANNEL_DIM);
    }
    else
    {
        g_CommBuff[1] = COMM_ERROR_PARAMETER;
    }
    
    /* Copy value from high to low byte */
    g_CommBuff[3] = 0; 
    g_CommBuff[4] = 0; 
    g_CommBuff[5] = sample_adc >> 8; 
    g_CommBuff[6] = sample_adc & 0xFF; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetAverageAD(void)
{
    uint16_t avg_adc = 0;
    
    if(g_CommBuff[2] == COMM_CH_IOUT)
    {
        avg_adc = Power_GetIoutAvgADC();
    }
    else if(g_CommBuff[2] == COMM_CH_UOUT)
    {
        avg_adc = Power_GetUoutAvgADC();
    }
    else if(g_CommBuff[2] == COMM_CH_DIMM)
    {
        avg_adc = 0;
    }
    else
    {
        g_CommBuff[1] = COMM_ERROR_PARAMETER;
    }
          
    /* Copy value from high to low byte */
    g_CommBuff[3] = 0; 
    g_CommBuff[4] = 0; 
    g_CommBuff[5] = avg_adc >> 8; 
    g_CommBuff[6] = avg_adc & 0xFF; 
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetRawValue(void)
{
    //TODO: NOT IMPLEMENT
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetRealValue(void)
{
    uint32_t real_value = 0;
    
    if(g_CommBuff[2] == COMM_CH_IOUT)
    {
        real_value = Power_GetRealIout() * 100;
    }
    else if(g_CommBuff[2] == COMM_CH_UOUT)
    {
        real_value = Power_GetRealUout() * 100;
    }
    else if(g_CommBuff[2] == COMM_CH_DIMM)
    {
        //TODO: NOT IMPLEMENT 
    }
    else if(g_CommBuff[2] == COMM_CH_ED_TIME)
    {        
         real_value = g_ed_time;
    }
    else if(g_CommBuff[2] == COMM_CH_RUN_TIME)
    {
        real_value = g_nfc_time;
    }
    else
    {
        g_CommBuff[1] = COMM_ERROR_PARAMETER;
    }
          
    /* Copy value from high to low byte */
    g_CommBuff[3] = (real_value & 0xFF000000) >> 24; 
    g_CommBuff[4] = (real_value & 0x00FF0000) >> 16; 
    g_CommBuff[5] = (real_value & 0x0000FF00) >> 8; 
    g_CommBuff[6] = (real_value & 0x000000FF); 
        
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetLoopState(void)
{
    Power_State_t loop_state;
    
    loop_state = Power_GetPowerLoopState();
    
    g_CommBuff[3] = (uint8_t)loop_state;
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetPwmDuty(void)
{
    uint16_t pwm_duty = 0;  /* duty x 100 */
    
    if(g_CommBuff[2] == COMM_PWM_CTRL)
    {
        pwm_duty = PWM_GetDuty(PWM_ID_CH_CTRL) * 10;
    }
    else if(g_CommBuff[2] == COMM_PWM_OCP_REF)
    {
        pwm_duty = PWM_GetDuty(PWM_ID_CH_OCP) * 10;
    }
    else if(g_CommBuff[2] == COMM_PWM_OVP_REF)
    {
        pwm_duty = PWM_GetDuty(PWM_ID_CH_OVP) * 10;
    }
    else
    {
        g_CommBuff[1] = COMM_ERROR_PARAMETER;
    }    
    
    /* Copy value from high to low byte */
    g_CommBuff[3] = pwm_duty >> 8; 
    g_CommBuff[4] = pwm_duty & 0xFF; 
        
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_GetDimmingLevel(void)
{
    uint16_t level = 0;
    
    if(g_CommBuff[2] == COMM_DIMM_CONSTANT)
    {
        level = Power_GetConstantLumenValue();
    }
    else if(g_CommBuff[2] == COMM_DIMM_ASTRO)
    {
        level = Power_GetAstroDimmingLevel();
    }   
    else if(g_CommBuff[2] == COMM_DIMM_EOL)
    {
        level = Power_GetEolDimmingLevel();
    }   
    else if(g_CommBuff[2] == COMM_DIMM_ONE2TEN)
    {
        level = Power_GetOne2TenDimming();
    }
    else
    {
        g_CommBuff[1] = COMM_ERROR_PARAMETER;
    }    
    
    /* Copy value from high to low byte */
    g_CommBuff[3] = level >> 8; 
    g_CommBuff[4] = level & 0xFF; 
        
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_SetBootMode(void)
{
    if(g_CommBuff[2] <= BMI_MAX)
    {
        MCU_ChangeBMI(g_CommBuff[2]);
    }
    else
    {        
        /* Error feedback */
        g_CommBuff[1] = COMM_ERROR_PARAMETER; 
    }
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

//=============================================================================
void Comm_WriteMemory(void)
{
    //TODO: NOT IMPLEMENT
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}


//=============================================================================
void Comm_ReadMemory(void)
{
    //TODO: NOT IMPLEMENT
    
    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}


//=============================================================================
void Comm_EraseMemory(void)
{
    //TODO: NOT IMPLEMENT

    /* Send feedback packet */  
    Comm_Feedback(g_CommBuff);
}

#endif
