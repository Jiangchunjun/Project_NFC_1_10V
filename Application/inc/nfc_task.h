/*
***************************************************************************************************
*                               NFC Communication Task
*
* File   : nfc_task.h
* Author : Douglas Xie
* Date   : 2016.08.30
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/


#ifndef NFC_TASK_H
#define NFC_TASK_H

#ifndef DEBUG_VERSION

/* Include Head Files ---------------------------------------------------------------------------*/
#include "i2c_driver.h"
#include "M24LRxx.h"
#include "nfcDali.h"
#include "nfc.h"
#include "driver.h"
#include "system.h"

/* Macro Defines --------------------------------------------------------------------------------*/
/* NFC Task Period define, unit: ms */
#define NFC_TASK_PERIOD             (5)    /* 5ms */

/* Data Type Define -----------------------------------------------------------------------------*/

/* Global Variable ------------------------------------------------------------------------------*/

/* Function Declaration -------------------------------------------------------------------------*/
/*******************************************************************************
* @Brief   NFC Task Initial
* @Param   
* @Note    Initial NFC relative module and write default when nfc is empty
* @Return  
*******************************************************************************/
void NFC_TaskInit(void);

/*******************************************************************************
* @Brief   NFC Communication Task Routine
* @Param   
* @Note    NFC communication and record device lifetime
* @Return  
*******************************************************************************/
void NFC_CommTask(void);

/*******************************************************************************
* @Brief   NFC Update Software Timer
* @Param   
* @Note    Call in systick timer handler, update nfc timer and record lifetime
* @Return  
*******************************************************************************/
void NFC_UpdateTimer(void);

/* unknown function, but nfc task need it */
uint8_t Mpc_GetLength(uint16_t bank);

#else
/* Define empty interface for compiler */
#define NFC_TaskInit()
#define NFC_CommTask()
#define NFC_UpdateTimer()
#define Mpc_GetLength()         (0)

#endif /* DEBUG_VERSION */
#endif /* NFC_TASK_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
