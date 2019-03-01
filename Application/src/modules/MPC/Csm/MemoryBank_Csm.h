/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : MemoryBank_Csm.h
* Author             : Fighera Diego
* Date               : 11/2016
* Description        : MemoryBank, Id=12, Version 2, CSM Configuration Page, 2D1 2911382 03
*                     Multiport version
*******************************************************************************/

#ifndef __MEMORY_BANK_CSM_H
#define __MEMORY_BANK_CSM_H

#include <stdint.h>
#include <stdbool.h>

#define MPC_CurrentSetting_ID                12
#define MPC_CurrentSetting_VERSION            2
#define MPC_CurrentSetting_LastADDRESS       12       // updated  

// Public functions of the MPC

/* Set function for the application */

/* ------------------------------------------------------------------------------------
/// \brief     It allows the application to set / reset the Operating Mode to any value.
///        This is useful in production to restore the LEDset Auto-detect Mode, that is the factory default.
/// \param channel    The channel ID of the device.
/// \param value    The new operating mode.
///
/// \retval        none
//----------------------------------------------------------------------------------- */
void MemoryBank_CSM_Set_OpMode(uint8_t channel, uint8_t value);


/* GET functions for the application */

/* ----------------------------------------------------------------------------------
/// \brief    It allows the application to inquire the present Operating Mode
///
/// \param channel    The channel ID of the device.
/// \retval        The present operating mode (bits 0 - 2, 7 of Mode (at address 0x06))
//----------------------------------------------------------------------------------- */
uint8_t MemoryBank_CSM_GetLedsetMode (uint8_t channel);


/* ----------------------------------------------------------------------------------
/// \brief    It allows the application to get the present Nominal Current
///
/// \param channel    The channel ID of the device.
/// \retval        The present nominal current, in mA
//----------------------------------------------------------------------------------- */
uint16_t MemoryBank_CSM_GetNominalCurrent (uint8_t channel);


/* ----------------------------------------------------------------------------------
/// \brief    It allows the application to get the present Protection Current
///
/// \param channel    The channel ID of the device.
/// \retval        The present Protection Current, in mA
//----------------------------------------------------------------------------------- */
uint16_t MemoryBank_CSM_GetProtectionCurrent (uint8_t channel);


/* Functions to update Read only information - periodic call */

/* ----------------------------------------------------------------------------------
/// \brief    It allows the application to update the present LEDset current
///
/// \param channel    The channel ID of the device.
/// \retval        The present LEDset current, in mA
//----------------------------------------------------------------------------------- */
void MemoryBank_CSM_UpdateLedsetCurrent(uint8_t channel, uint16_t value16);


/* ----------------------------------------------------------------------------------
/// \brief    It allows the application to update the present LEDset status
///
/// \param channel    The channel ID of the device.
/// \retval        The present LEDset status
//----------------------------------------------------------------------------------- */
void MemoryBank_CSM_UpdateLedsetStatus(uint8_t channel, uint8_t value);


// Private functions of DaliLibrary: Memory Bank handling

void MemoryBank_Csm_Init( void );
void MemoryBank_Csm_Reset( uint8_t channel );
mpc_rw_enum MemoryBank_Csm_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Csm_SetValue( uint8_t address, uint8_t value, uint8_t channel );

mpc_rw_enum MemoryBank_Csm_GetValue_port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port );
mpc_rw_enum MemoryBank_Csm_SetValue_port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port );

#endif