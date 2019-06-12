/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : MemoryBank_Tfm.h
* Author             : Fighera Diego
* Date               : 12/2016
* Description        : MemoryBank, ID=22, Version 0, TFM , 2D1 3431755 00
*
*******************************************************************************/

#ifndef __MEMORY_BANK_TFM_H
#define __MEMORY_BANK_TFM_H

#include <stdint.h>
#include <stdbool.h>
#include "MpcDefs.h"

#define MPC_TuningFactor_ID                22
#define MPC_TuningFactor_VERSION            0
#define MPC_TuningFactor_LastADDRESS       11

// Public functions of the MPC




/* GET functions for the application */

uint8_t MemoryBank_Tfm_GetEnable (uint8_t channel);

uint8_t MemoryBank_Tfm_GetTuningFactor (uint8_t channel);

uint8_t MemoryBank_Tfm_GetMinimumTuningFactor (uint8_t channel);

uint8_t MemoryBank_Tfm_GetMaximumTuningFactor (uint8_t channel);

uint32_t MemoryBank_Tfm_GetTuningFactorQ15(uint8_t channel);

// Private functions of DaliLibrary: Memory Bank handling

void MemoryBank_Tfm_Init( void );
void MemoryBank_Tfm_Reset( uint8_t channel );

mpc_rw_enum MemoryBank_Tfm_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Tfm_SetValue( uint8_t address, uint8_t value, uint8_t channel );

mpc_rw_enum MemoryBank_Tfm_GetValue_Port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port);
mpc_rw_enum MemoryBank_Tfm_SetValue_Port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port);

#endif
