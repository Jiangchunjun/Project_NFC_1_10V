//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   MemoryBanks_Dali.h
///   \since  2017-03-16
///   \brief  Declaration of the interface to access MemoryBanks "DaliParams", MemoryBank0, MemoryBank1.
///           Interface to MemoryBank_Debug declared in a separate header.
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is confidential and not intended for public release.
///   All rights reserved.  Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

//---  M e m o r y B a n k   a c c e s s   -------------------------------------
#define MPC_Dali_VERSION                  2                                     // MemoryBanks 0 and 1 are defined in DALI: Use number of Dali-edition as version

#define MPC_MEMBANK0_OFFSET               2
#define MEMORY_BANK0_LastADDRESS          0x1A                                  // factory burn-in in the range of 0x1A .. 0xFE (0x1B - 0x7F: reserved)
#define MPC_MEMBANK1_OFFSET               3
#define MEMORY_BANK1_LastADDRESS          DALI_CONFIG_MEMORY_BANK1_LastADDRESS  // factory burn-in in the range of 0x10 .. 0xFE

// Interface to Dali MemoryBank 0, no reset as data is constant for most ports
void MemoryBank_Dali0_Init(void);                                               // all channels access identical data
mpc_rw_enum MemoryBank_Dali0_GetValue( uint8_t address, uint8_t *pValue, uint8_t channel );
mpc_rw_enum MemoryBank_Dali0_SetValue( uint8_t address, uint8_t value, uint8_t channel );

// Interface to Dali MemoryBank 1
void MemoryBank_Dali1_Init(void);                                               // all channels access identical data
void MemoryBank_Dali1_Reset( void );                                            //  "
mpc_rw_enum MemoryBank_Dali1_GetValue( uint8_t address, uint8_t *pValue, uint8_t channel );
mpc_rw_enum MemoryBank_Dali1_SetValue( uint8_t address, uint8_t value, uint8_t channel );

//---  M o d u l e   i n t e r f a c e   ---------------------------------------
uint16_t Ballast_GetHW_Version( void );                                         // direct access to module data: returns the HardwareVersion in format [8 bit major.8 bit minor]