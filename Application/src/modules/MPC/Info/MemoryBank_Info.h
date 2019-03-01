// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Info.h
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: Jörg Ottensmeyer, Jutta Avril, 2012
// $Author: j.eisenberg $
// $Revision: 9997 $
// $Date: 2017-09-28 20:47:43 +0800 (Thu, 28 Sep 2017) $
// $Id: MemoryBank_Info.h 9997 2017-09-28 12:47:43Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Info/tags/V3_3/Source/MemoryBank_Info.h $
//
//
// Implementation of the memory bank "General Info Page" (2784480 000 01). To enable define:
// #define MPC_INFO_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __MEMORY_BANK_INFO_H
#define __MEMORY_BANK_INFO_H

#define MPC_Info_LastADDRESS         16
#define MPC_Info_ID                  2
#define MPC_Info_VERSION             0

void MemoryBank_Info_Init( void );
void MemoryBank_Info_Reset( uint8_t channel );
mpc_rw_enum MemoryBank_Info_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Info_SetValue( uint8_t address, uint8_t value, uint8_t channel );
mpc_rw_enum MemoryBank_Info_GetValue_port( uint8_t address, uint8_t *pValue, uint8_t channel, uint8_t port );
mpc_rw_enum MemoryBank_Info_SetValue_port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port );

void MemoryBank_Info_SlowTimer( void );


uint32_t InfoMPC_GetLampOnMinutes(uint8_t channel);
uint32_t InfoMPC_GetLampOnTicks(uint8_t channel) ;

#endif