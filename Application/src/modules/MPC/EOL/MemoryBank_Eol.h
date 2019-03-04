/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : MemoryBank_Eol.h
* Author             : Diego
* Date               :
* Description        : MemoryBank, Id=13, version 0, Eol Configuration Page
*******************************************************************************/

#ifndef __MEMORY_BANK_EOL_H
#define __MEMORY_BANK_EOL_H

#define MPC_EOL_ID                    13
#define MPC_EOL_VERSION                0
#define MPC_EOL_LAST_ADDRESS           7

// Public functions of the MPC

void MemoryBank_Eol_UpdateStatus(uint8_t channel, uint8_t value);
bool MemoryBank_EOL_GetEnable (uint8_t channel);
uint8_t MemoryBank_EOL_GetEOLtime (uint8_t channel);

// Private functions of DaliLibrary: Memory Bank handling

void MemoryBank_Eol_Init( void );
void MemoryBank_Eol_Reset( uint8_t channel );

bool MemoryBank_Eol_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
bool MemoryBank_Eol_SetValue( uint8_t address, uint8_t value, uint8_t channel );

#endif