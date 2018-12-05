/*
***************************************************************************************************
*                             Usart Communition Interface
*       Data Format: StartByte(1) + Command(1) + Param(1) + Data(8) + CheckSum(1) = 12bytes
*
* File   : comm_task.h
* Author : Douglas Xie
* Date   : 2016.10.13
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/



#ifndef  COMM_TASK
#define  COMM_TASK

#ifdef ENABLE_COMM
/* Include Head Files ---------------------------------------------------------------------------*/
#include "mcu_config.h"
#include "system.h"
#include "sw_timer.h"
#include "usart.h"

/* Macro Define ---------------------------------------------------------------------------------*/
/* Comm Receive State define */
#define COMM_RX_READY           0x00    /* Ready for receive data */
#define COMM_RX_RECEVING        0x01    /* Not receive a full data packet */
#define COMM_RX_DONE            0x02    /* Receive a full data packet */

/* Comm Timer Status define */
#define COMM_TIMER_OFF          SWT_STATUS_OFF  /* Timer is trun off */
#define COMM_TIMER_OK           SWT_STATUS_OK   /* Timer is timing and does not up now */
#define COMM_TIMER_UP           SWT_STATUS_UP   /* Timer is up */

/* Comm Timer Timeout define */
#define COMM_RX_TIMEOUT         1000    /* Receive timeout is 1 second */
#define COMM_UNLOCK_TIMEOUT     15000   /* Unlock timeout is 15 second */

/* Comm Result State */
#define COMM_OK                 0x00
#define COMM_ERROR              0xEE

/* Comm Lock State */
#define COMM_LOCK               0x00
#define COMM_UNLOCK             0xA5


/* Data Type Define -----------------------------------------------------------------------------*/
/* Union Variable */
typedef union unUint16Array
{
    uint16_t value;
    uint8_t  array[2];
} Uint16Array_t;

typedef union unUint32Array
{
    uint32_t value;
    uint8_t array[4];
} Uint32Array_t;

typedef union unFloatArray 
{
    float value;
    uint8_t array[4];
} FloatArray_t;


/* Interface Replace ----------------------------------------------------------------------------*/
/* Comm Unlock Timer */
#define Comm_StartUnlockTimer()     SWT_StartTimer(SWT_ID_COMM_UNLOCK, COMM_UNLOCK_TIMEOUT)
#define Comm_CloseUnlockTimer()     SWT_CloseTimer(SWT_ID_COMM_UNLOCK)
#define Comm_CheckUnlockTimer()     SWT_CheckTimer(SWT_ID_COMM_UNLOCK)

/* Comm Receive Timer */
#define Comm_StartRxTimer()         SWT_StartTimer(SWT_ID_COMM_RX, COMM_RX_TIMEOUT)
#define Comm_CloseRxTimer()         SWT_CloseTimer(SWT_ID_COMM_RX)
#define Comm_CheckRxTimer()         SWT_CheckTimer(SWT_ID_COMM_RX)

/* Comm Call Usart Function */
#define Comm_ResetBuffer()          USART_ResetBuffer()  
#define Comm_GetRxCount()           USART_GetRxCount()
#define Comm_GetRxData(ptr)         USART_GetRxData(ptr)  
#define Comm_SendTxData(ptr, num)   USART_SendData(ptr, num)

/* Function Declaration -------------------------------------------------------------------------*/
/*******************************************************************************
* @brief  Comm Generate Password
* @param  
* @note   generate password according to key
* @retval 
*******************************************************************************/
void Comm_GeneratePassword(uint8_t *key, uint8_t *password);

/*******************************************************************************
* @brief  Comm Check Packet
* @param  
* @note   Check usart receive data packet. 
* @retval receive state COMM_RX_READY / COMM_RX_RECEVING / COMM_RX_DONE 
*******************************************************************************/
uint8_t Comm_CheckPacket(void);

/*******************************************************************************
* @Brief   CheckSum Calculate Formula 
* @Param   [in]data  - pointer to the data buffer
*          [in]size - data buffer size
* @Note    calculate data buffer checksum and return
* @Return  checksum
*******************************************************************************/
uint8_t Comm_CalcuCS(uint8_t *data, uint8_t size);

/*******************************************************************************
* @brief  Comm Verify CheckSum
* @param  
* @note   Check if data packet checksum is right and return command
* @retval  
*******************************************************************************/
uint8_t Comm_VerifyCS(void);

/*******************************************************************************
* @brief  Comm Send Feedback Packet
* @param  
* @note   Update StartByte and CheckSum then send out feedback packet
* @retval  
*******************************************************************************/
void Comm_Feedback(uint8_t *data);

/*******************************************************************************
* @brief  Comm Task Implement
* @param  
* @note   Analyse the data packet and excute the command 
* @retval  
*******************************************************************************/
void Comm_Task(void);

#else

#define Comm_GeneratePassword(a,b)
#define Comm_CheckPacket()
#define Comm_CalcuCS(a,b)
#define Comm_VerifyCS()     (1)
#define Comm_Feedback(a)
#define Comm_Task()

#endif /* ENABLE_COMM */

#endif /* COMM_TASK */
