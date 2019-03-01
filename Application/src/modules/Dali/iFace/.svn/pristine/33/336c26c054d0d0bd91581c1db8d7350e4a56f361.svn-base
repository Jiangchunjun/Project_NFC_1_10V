/******************** (C) COPYRIGHT 2016 - 2016 OSRAM AG *********************************************************************************
* File Name         : MemoryBanks_Debug.h
* Description       : Interface to device-specific MemoryBank Debug. If not used: Don't care.
*                     If used: Define MPC_Debug_LastADDRESS locally in your MemoryBanks_Debug.c (see template ..)
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 12.01.2016 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**********************************************************************************************************************************/

// address 0: MPC_Debug_LastADDRESS
// address 1 .. MPC_Debug_LastADDRESS: Device specific data (read / write access device-specific)


void MemoryBank_Debug_Init(void);                                               // defined in WeakFunktions.c. May be overwritten in MemoryBank_Debug.c
void MemoryBank_Debug_Reset( uint8_t channel );

bool MemoryBank_Debug_GetValue(uint8_t address, uint8_t *pValue, uint8_t channel );  // defined in WeakFunktions.c. May be overwritten in MemoryBank_Debug.c
bool MemoryBank_Debug_SetValue(uint8_t address, uint8_t value, uint8_t channel );    // defined in WeakFunktions.c. May be overwritten in MemoryBank_Debug.c

