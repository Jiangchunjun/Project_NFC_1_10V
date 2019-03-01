// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Astro.h
// ---------------------------------------------------------------------------------------------------------------------
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2013-11, d.fighera@osram.it
//
// Change History:
//
// $Author: d.fighera $
// $Revision: 15312 $
// $Date: 2018-10-17 23:42:49 +0800 (Wed, 17 Oct 2018) $
// $Id: MemoryBank_Astro.h 15312 2018-10-17 15:42:49Z d.fighera $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Outdoor/trunk/Src/MemoryBank_Astro.h $
//
// ---------------------------------------------------------------------------------------------------------------------
///  /brief Implementation of the memory bank "Astro Configuration Page".
///  MPC ID=05, Version 2, 2D1 2786697 04
///  #define MPC_ASTRO_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------


#ifndef __MEMORY_BANK_ASTRO_H
#define __MEMORY_BANK_ASTRO_H

/// \brief The ID of the page definition.
#define MemoryBank_Astro_ID                     05

/// \brief The version of the page definition.
#define MemoryBank_Astro_Version                02                              // rev. 2

/// \brief The last address of this memory bank.
#define MemoryBank_Astro_LastAddress            30


#define ASTRO_RESET_TIME_MASK                   0x40
#define ASTRO_RELOAD_PARAM_MASK                 0x80

#define SHORTStepTime                           2                               //  2 s
#define LONGStepTime                            15                              // 15 s


void ServiceOutdoorMpcWriteCounter(uint8_t mpcid);                              // Service function for counting Mpc write


// Public functions of the MPC

void   MemoryBank_Astro_ClearStartEDontime(uint8_t channel, uint16_t value) ;
uint16_t MemoryBank_Astro_GetStartEDontime(uint8_t channel);

uint8_t MemoryBank_Astro_GetMode(uint8_t channel);

uint16_t MemoryBank_Astro_GetNominalLevel(uint8_t channel);
uint16_t MemoryBank_Astro_GetLevel1(uint8_t channel);
uint16_t MemoryBank_Astro_GetLevel2(uint8_t channel);
uint16_t MemoryBank_Astro_GetLevel3(uint8_t channel);
uint16_t MemoryBank_Astro_GetLevel4(uint8_t channel);

int16_t  MemoryBank_Astro_GetStartTime(uint8_t channel);                        // signed
uint16_t MemoryBank_Astro_GetDuration1(uint8_t channel);
uint16_t MemoryBank_Astro_GetDuration2(uint8_t channel);
uint16_t MemoryBank_Astro_GetDuration3(uint8_t channel);

uint16_t MemoryBank_Astro_GetStartUpFade(uint8_t channel);
uint16_t MemoryBank_Astro_GetAstroFade(uint8_t channel);
uint16_t MemoryBank_Astro_GetSwitchOffFade(uint8_t channel);

uint16_t MemoryBank_Astro_GetEDontime(uint8_t channel);

void MemoryBank_Astro_UpdateStatus(uint8_t channel, uint8_t value);
void MemoryBank_Astro_UpdateEDonTime(uint8_t channel, uint16_t value16);

void MemoryBank_Astro_UpdateOpMode(uint8_t channel, uint8_t mode);

// Private functions of DaliLibrary: Memory Bank handling

void MemoryBank_Astro_Init( void);
void MemoryBank_Astro_Reset( uint8_t channel );

void MemoryBank_Astro_FactoryReset( void );

mpc_rw_enum MemoryBank_Astro_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Astro_SetValue( uint8_t address, uint8_t value, uint8_t channel );

mpc_rw_enum MemoryBank_Astro_GetValue_Port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port );
mpc_rw_enum MemoryBank_Astro_SetValue_Port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port );


#endif
