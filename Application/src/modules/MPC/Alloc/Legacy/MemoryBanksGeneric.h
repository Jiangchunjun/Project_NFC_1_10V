/******************** (C) COPYRIGHT 2014 - 2016 OSRAM AG *********************************************************************************
* File Name         : MemoryBanksGeneric.h
* Description       :
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 14.03.2014 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
*                   :
**********************************************************************************************************************************/

/// \file
/// \brief Implementation of MemoryBanks with MPC-ID >= 2 to provide OSRAM specific MultiPurposeCenters.

void MemoryBankGeneric_Init( void );
void MemoryBankGeneric_Reset( uint8_t bank, uint8_t channel );
mpc_rw_enum MemoryBankGeneric_GetValue( uint8_t bank, uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBankGeneric_SetValue( uint8_t bank, uint8_t address, uint8_t value, uint8_t channel );


//void MemoryBankGeneric_FastTimer(void);
void MemoryBankGeneric_SlowTimer( uint8_t channels );
