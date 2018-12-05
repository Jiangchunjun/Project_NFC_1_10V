/*
 ***************************************************************************************************
 *                                  System Task Control Center
 *
 * File   : system.c
 * Author : Douglas Xie
 * Date   : 2016.04.18
 ***************************************************************************************************
 * Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
 ***************************************************************************************************
 */

#ifndef _SYSTEM_H
#define _SYSTEM_H

/* Include Head Files ---------------------------------------------------------------------------*/
#include <xmc_common.h>
#include "adc.h"
#include "acmp.h"
#include "eru.h"
#include "tick.h"
#include "sw_timer.h"
#include "power_task.h"
#include "nfc_task.h"
#include "mcu_config.h"

/* System Task Macro Defines --------------------------------------------------------------------*/
/*------------ TASK ID define --------------------------------------------*/
#define SYS_TASK_MODE_CHANGE        ((uint32_t)1<<0)
#define SYS_TASK_POWER_ON           ((uint32_t)1<<1)
#define SYS_TASK_POWER_OFF          ((uint32_t)1<<2)
#define SYS_TASK_SLEEP              ((uint32_t)1<<3)
#define SYS_TASK_WAKEUP             ((uint32_t)1<<4)
#define SYS_TASK_LOOP               ((uint32_t)1<<5)
#define SYS_TASK_NFC                ((uint32_t)1<<6)

/*------------ TASK State Define -----------------------------------------*/
#define SYS_TASK_ENABLE             (0x0F)
#define SYS_TASK_DISABLE            (0x00)

/*------------ Work Mode Define ------------------------------------------*/
#define SYS_MODE_OFF                (0x00)
#define SYS_MODE_NORMAL             (0x01)
#define SYS_MODE_LEDSET2            (0x02)
#define SYS_MODE_DIM                (0x03)
#define SYS_MODE_RESERVED           (0x04)

/*------------ Power On Task Delay Item ----------------------------------*/
#define SYS_ACMP_EN_DELAY           (12)    /* 5ms, delay for enable ACMP module */
#define SYS_ERU_EN_DELAY            (36)    /* 15ms, delay for enable ERU module */
#define SYS_NFC_INIT_DELYA          (120)//(480)   /* 200ms, delay for NFC task init and prepare OVP OCP reference */
#define SYS_POWER_INIT_DELYA        (130)//(720)   /* 300ms, delay for set target current init pwm duty */
#define SYS_ADC_INIT_DELYA          (140)//(732)   /* 305ms, delay for enable ADC moudle */
#define SYS_CTRL_LOOP_START_DELAY   (260)//(744)   /* 310ms, delay for create power control loop task */ //744

/* Data Type Define -----------------------------------------------------------------------------*/
typedef enum
{ 
    POWER_ON_STATE_ACMP_EN = 0,
    POWER_ON_STATE_ERU_EN,
    POWER_ON_STATE_NFC_INIT,
    POWER_ON_STATE_POWER_INIT,
    POWER_ON_STATE_ADC_INIT,
    POWER_ON_STATE_LOOP_START,
    POWER_ON_STATE_END
} PowerOn_State_t;


/* Global Variable ------------------------------------------------------------------------------*/


/* Function Declaration -------------------------------------------------------------------------*/

/*******************************************************************************
 * @brief  System Task Manager Initial
 * @param  
 * @note   Reset the task state register
 * @retval  
 *******************************************************************************/
void System_TaskInit(void);

/*******************************************************************************
 * @brief  Create System Task
 * @param  
 * @note   Set task register bit to enable the task
 * @retval  
 *******************************************************************************/
void System_CreateTask(uint32_t task_id);

/*******************************************************************************
 * @brief  Close System Task
 * @param  
 * @note   Clear task register bit to disable the task
 * @retval  
 *******************************************************************************/
void System_CloseTask(uint32_t task_id);

/*******************************************************************************
 * @brief  Check Task Enable State
 * @param  
 * @note   Check System Task Enable State and return it 
 * @retval TASK_ENABLE / TASK_DISABLE
 *******************************************************************************/
uint8_t System_CheckTask(uint32_t task_id);

/*******************************************************************************
 * @brief  System Power On Task
 * @param  
 * @note   
 * @retval 
 *******************************************************************************/
void System_PowerOnTask(void);

/*******************************************************************************
 * @brief  System Power Off Task
 * @param  
 * @note   
 * @retval 
 *******************************************************************************/
void System_PowerOffTask(void);

/*******************************************************************************
 * @brief  System Sleep Task
 * @param  
 * @note   
 * @retval 
 *******************************************************************************/
void System_SleepTask(void);

/*******************************************************************************
 * @brief  System Wakeup Task
 * @param  
 * @note   
 * @retval 
 *******************************************************************************/
void System_WakeupTask(void);

/*******************************************************************************
 * @brief  System Work Mode Change Task
 * @param  
 * @note   
 * @retval 
 *******************************************************************************/
void System_WorkModeChangeTask(void);

/*******************************************************************************
 * @brief  System Period Task Management
 * @param  
 * @note   Check timing task which are enabled and run the handler when time up
 * @retval
 *******************************************************************************/
void System_PeriodTaskManagement(void);

#endif /* _SYSTEM_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
