// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Power.c
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
// Initial version: Jutta Avril (JA),  13/07/2015
// $Author: j.eisenberg $
// $Revision: 4904 $
// $Date: 2016-07-20 16:50:10 +0800 (Wed, 20 Jul 2016) $
// $Id: MemoryBank_Power.c 4904 2016-07-20 08:50:10Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Power/tags/V2_0/Source/MemoryBank_Power.c $
//
// Implementation of the memory bank "Power Consumption Page", 2D1 2784481-000-02, MPC ID =3. To enable define:
// #define MPC_POWER_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC                                                              // to #include "MemoryBanksDefault.h" in config.h
#include "Config.h"
#include "MpcDefs.h"

#include "MemoryBank_Power.h"
#include "MpcPower_Services.h"

typedef struct
{
    uint8_t lockByte;                                                             ///< \brief The lock byte of the memory bank.
    uint32_t powerConsumption;
} TypeMemoryBank_Power;

TypeMemoryBank_Power memoryBank_Power[DEVICE_CHANNELS];                         /// \brief The data of the memory bank.

uint32_t readingPortBuffer[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];

//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Memory Bank initialisation
/// \param            none
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Power_Init( void )
{
    uint8_t i;
    for( i = 0; i < DEVICE_CHANNELS; i++ )
    {
        memoryBank_Power[i].lockByte = 255;                                         //set the lockByte to the default value
    }
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Memory Bank Reset
/// \param channel    device channel.
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Power_Reset( uint8_t channel )                                  // reset only MPC variables
{
    // no variables of MPC have to be reset
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Reads a value from the memory bank "Power Consumption Package".
///                   Using DaliServices.h to query data from external.
/// \param address    The address where to read from.
/// \param &value      Pointer to a memory address to save the result.
/// \param channel    The device channel.
///
/// \retval            DONE if valid address to read from, NOT_DONE else.
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Power_GetValue_Port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port )
{
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 0:                                                                     // last addressable memory bank location
        *value = MPC_Power_LastADDRESS;
        return MPC_RW_OK;

    case 1:                                                                     // indicator byte not supported
        *value = 0;
        return MPC_RW_OK;

    case 2:                                                                     // lock byte
        *value = memoryBank_Power[channel].lockByte;
        return MPC_RW_OK;

    case 3:                                                                     // number of the multipurpose center
        *value = 3;
        return MPC_RW_OK;

    case 4:                                                                     // version of the multipurpose center implementation
        *value = MPC_Power_VERSION;
        return MPC_RW_OK;

    case 5:
        *value =  MPC_POWER_INFO_BYTE;
        return MPC_RW_OK;

    case 6:
        memoryBank_Power[channel].powerConsumption = MpcPowerServices_GetPowerConsumption( channel );
        readingPortBuffer[channel][port] = memoryBank_Power[channel].powerConsumption;
        *value = (uint8_t)(readingPortBuffer[channel][port] >> 16);
        return MPC_RW_OK;

    case 7:
        *value = (uint8_t)(readingPortBuffer[channel][port] >> 8);
        return MPC_RW_OK;

    case 8:
        *value = (uint8_t)(readingPortBuffer[channel][port] >> 0);
        return MPC_RW_OK;

    default:
        return MPC_RW_OUT_OF_RANGE;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Writes a value into the memory bank "Power Consumption Page".
/// \param address    The address where to write to.
/// \param value      The value to write.
/// \param channel    The device channel.
///
/// \retval            DONE if value was written, NOT_DONE else.
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Power_SetValue_port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port )
{
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 2:                                                                     // store the lock byte
        memoryBank_Power[channel].lockByte = value;
        return MPC_RW_OK;

        /* only optional
            case 9:
            case 10:
              if( memoryBank_Power[channel].lockByte == 0x55 ) {                        // allow write only if enabled
                if (address >= 9)     {                                                 // only correction and lookup tables can be changed
                  // tbd: the optional parts of the power consumption page are not yet implemented
                }
                return true;
              }
              else {
                return false;
              }
        */

    default:
        return MPC_RW_OUT_OF_RANGE;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Reads a value from the memory bank "Power Consumption Package".
///                   Using DaliServices.h to query data from external.
/// \param address    The address where to read from.
/// \param &value      Pointer to a memory address to save the result.
/// \param channel    The device channel.
///
/// \retval            DONE if valid address to read from, NOT_DONE else.
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Power_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{
    // Wrapping the function for DALI Library.
    return MemoryBank_Power_GetValue_Port( address, value, channel, 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief            Writes a value into the memory bank "Power Consumption Page".
/// \param address    The address where to write to.
/// \param value      The value to write.
/// \param channel    The device channel.
///
/// \retval            DONE if value was written, NOT_DONE else.
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Power_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
    return MemoryBank_Power_SetValue_port( address, value, channel, 0 );
}


