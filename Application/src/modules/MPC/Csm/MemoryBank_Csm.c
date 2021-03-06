/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : MemoryBank_Csm.c
* Author             : Fighera Diego, modified by Jutta Avril
* Date               : 11/2016
* Description        : MemoryBank, Id=12, Version 2, CSM current setting module, 2D1 2911382 03
*
*                     Multiport version
*******************************************************************************/

// $Author: d.fighera $
// $Revision: 8883 $
// $Date: 2017-06-29 23:24:02 +0800 (Thu, 29 Jun 2017) $
// $Id: MemoryBank_Csm.c 8883 2017-06-29 15:24:02Z d.fighera $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Csm/tags/Revision3.0/Source/MemoryBank_Csm.c $

///
/// Implementation of the memory bank "CSM Configuration Page". To enable define:
/// #define MPC_CSM_NUMBER      [bank number]
/// #define AVAILABLE_PORTS_NUMBER

#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "MpcDefs.h"
#include "nvm.h"

#include "MemoryBank_Csm.h"




#ifdef MPC_CSM_LEDset2
#define MPC_CSM_LEDset2_FLAG        0x80
#else
#define MPC_CSM_LEDset2_FLAG        0x00
#endif

#ifndef MPC_CSM_PROTECT_CURRENT
  #define MPC_CSM_PROTECT_CURRENT   0xFFFF
#endif


#ifndef AVAILABLE_PORTS_NUMBER
  #define AVAILABLE_PORTS_NUMBER    1
  #warning  "Undefined number of ports for MPC access !"
#endif

#define MPC_CSM_MODE_MASK           0x7F                                        // bit no


#define MacroLowByte( x )  ( (uint8_t)( x & 0xFF) )                                 //Low-Byte eines Integer Wertes
#define MacroHighByte( x ) ( (uint8_t)( (x & 0xFF00) >> 8 ) )                       //High-Byte eines Integer Wertes


typedef struct
{
    uint8_t lockByte;                                                           ///< \brief The lock uint8_t of the memory bank.
    uint8_t opMode;                    // RW 6
    uint8_t nom_cur_hi;                // RW
    uint8_t nom_cur_lo;                // RW
    uint8_t protect_cur_hi;            // RW
    uint8_t protect_cur_lo;            // RW

} TypeMemoryBank_CSM;

uint8_t  ledset_status[DEVICE_CHANNELS];
uint16_t ledset_current[DEVICE_CHANNELS];
// uint16_t actual_current[DEVICE_CHANNELS];


/// \brief The data of the memory bank.
TypeMemoryBank_CSM memoryBank_CSMPhy[DEVICE_CHANNELS];
TypeMemoryBank_CSM * memoryBank_CSM[DEVICE_CHANNELS];


uint16_t latch_nominal_current[DEVICE_CHANNELS];
uint16_t latch_protect_current[DEVICE_CHANNELS];

uint16_t readingPortBufferLedsetCurrent[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];

uint16_t readingPortBufferCsmCurrent[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];
uint16_t writingPortBufferCsmCurrent[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];

uint16_t readingPortBufferCsmProtection[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];
uint16_t writingPortBufferCsmProtection[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];


uint16_t joinBytes(uint8_t *ptr);


//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory bank.
///
///
/// \retval   Return true, the persistent data are changed, else the persistent data are unchanged.
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Csm_Init(  )
{
    uint8_t i;
    bool resetData;

    for( i = 0; i < DEVICE_CHANNELS; i++ )
    {

        memoryBank_CSM[i] = &memoryBank_CSMPhy[i] ;

        resetData = ( nvmDataInit( (void*)memoryBank_CSM[i] , sizeof( TypeMemoryBank_CSM ), (nvm_memclass_t)0 ) != NVM_INIT_OK );

        if (resetData)                                                          // set the default values
        {

            memoryBank_CSM[i]->opMode = MPC_CSM_MODE;
            memoryBank_CSM[i]->nom_cur_hi = MacroHighByte(MPC_CSM_DEFAULT_CURRENT);
            memoryBank_CSM[i]->nom_cur_lo = MacroLowByte(MPC_CSM_DEFAULT_CURRENT);
            memoryBank_CSM[i]->protect_cur_hi = MacroHighByte(MPC_CSM_PROTECT_CURRENT);
            memoryBank_CSM[i]->protect_cur_lo = MacroLowByte(MPC_CSM_PROTECT_CURRENT);

            ledset_status[i] = MPC_CSM_LEDset2_FLAG;

        }
        else                                                                    // use persistent memory content
        {
            // copy into buffer is done during calling HalPersistentMemoryGetBlock
            // thus no code is needed here
        }

        latch_nominal_current[i] = joinBytes( &memoryBank_CSM[i]->nom_cur_hi) ;
        latch_protect_current[i] = joinBytes( &memoryBank_CSM[i]->protect_cur_hi);

#ifdef CSMtestNominalCurrent
        CSMtestNominalCurrent(&latch_nominal_current[i]) ;
        memoryBank_CSM[i]->nom_cur_hi = latch_nominal_current[i] >> 8 ;
        memoryBank_CSM[i]->nom_cur_lo = latch_nominal_current[i] ;
#endif
#ifdef CSMtestProtectionCurrent
        CSMtestProtectionCurrent(&latch_protect_current[i]) ;
        memoryBank_CSM[i]->protect_cur_hi = latch_protect_current[i] >> 8 ;
        memoryBank_CSM[i]->protect_cur_lo = latch_protect_current[i] ;
#endif

        //thermal_index[i] = 0xFF;
        ledset_current[i] = 0xFFFF;

        memoryBank_CSM[i]->lockByte = 255;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Bank.
///
/// \param channel    The device channel.
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Csm_Reset( uint8_t channel )                                    // reset only MPC variables
{
    // no variables of MPC have to be reset
    memoryBank_CSM[channel]->lockByte = 255;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "CSM".
///
/// The data will be collected from the real device.
/// The checksum will be handled in MemoryBank.c
///
/// \param address      The address in the selected memory bank.
/// \param value        Pointer to a memory address to save the result.
/// \param channel      The device channel.
/// \param port         The port number.
///
/// \retval            'MPC_RW_OK' in case of successful read
//---------------------------------------------------------------------------------------------------------------------------

mpc_rw_enum MemoryBank_Csm_GetValue_port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port )
{
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 0:                                                                     //last addressable memory bank location
        *value = MPC_CurrentSetting_LastADDRESS;
        return MPC_RW_OK;

    case 1:                                                                     // indicator byte is always 0
        *value = 0;
        return MPC_RW_OK;

    case 2:                                                                     // lock byte
        *value = memoryBank_CSM[channel]->lockByte;
        return MPC_RW_OK;

    case 3:                                                                     //number of the multipurpose center
        *value = MPC_CurrentSetting_ID ;
        return MPC_RW_OK;

    case 4:                                                                     //version of the multipurpose center implementation
        *value = MPC_CurrentSetting_VERSION;
        return MPC_RW_OK;

    case 5:                                                                     // enable
        *value = ledset_status[channel] | MPC_CSM_LEDset2_FLAG;
        return MPC_RW_OK;

    case 6:
        *value = memoryBank_CSM[channel]->opMode;
        return MPC_RW_OK;

    case 7:
        readingPortBufferCsmCurrent[channel][port] = ((uint16_t)memoryBank_CSM[channel]->nom_cur_hi) << 8 | (uint16_t)memoryBank_CSM[channel]->nom_cur_lo;
        *value = (uint8_t)(readingPortBufferCsmCurrent[channel][port] >> 8);
        return MPC_RW_OK;

    case 8:
        *value = (uint8_t)(readingPortBufferCsmCurrent[channel][port]);
        return MPC_RW_OK;


    case 9:
        readingPortBufferCsmProtection[channel][port] = ((uint16_t)memoryBank_CSM[channel]->protect_cur_hi) << 8 | (uint16_t)memoryBank_CSM[channel]->protect_cur_lo;
        *value = (uint8_t)(readingPortBufferCsmProtection[channel][port] >> 8);
        return MPC_RW_OK;

    case 10:
        *value = (uint8_t)(readingPortBufferCsmProtection[channel][port]);
        return MPC_RW_OK;


    case 11:
        readingPortBufferLedsetCurrent[channel][port] = ledset_current[channel];
        *value = MacroHighByte( readingPortBufferLedsetCurrent[channel][port] );
        return MPC_RW_OK;

    case 12:
        *value = MacroLowByte( readingPortBufferLedsetCurrent[channel][port] );
        return MPC_RW_OK;

    default:
        return MPC_RW_OUT_OF_RANGE;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in the memory bank "CSM".
///
/// The bank number in daliDevice->dtr1, address in daliDevice->dtr and channel in daliDevice->channel.
///
/// \param address      The address in the selected memory bank.
/// \param value        The value to write.
/// \param channel      The channel ID of the device .
/// \param port         The port number.
///
/// \retval            'MPC_RW_OK' in case of a successful write
//---------------------------------------------------------------------------------------------------------------------------

mpc_rw_enum MemoryBank_Csm_SetValue_port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port )
{
    if ( (memoryBank_CSM[channel]->lockByte != 0x55) && (address != 2))
    {
        return MPC_RW_DENY;
    }

    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 2:                                                                     // the lock byte
        memoryBank_CSM[channel]->lockByte = value;
        return MPC_RW_OK;

    case 6:                                                                     // 0x80 reserved bit
        //value &= (memoryBank_CSM[channel]->opMode | 0x7F);                      // it can not be changed from 0 to 1
        value &= MPC_CSM_MODE_MASK;                                             // any writes reset reserved bit-7 
        memoryBank_CSM[channel]->opMode = value;
        return MPC_RW_OK;

    case 7:
        memoryBank_CSM[channel]->nom_cur_hi = value;

        writingPortBufferCsmCurrent[channel][port] = ( (uint16_t)value ) << 8;
        return MPC_RW_OK;

    case 8:
        memoryBank_CSM[channel]->nom_cur_lo = value;

        writingPortBufferCsmCurrent[channel][port] |= (uint16_t)value;

        latch_nominal_current[channel] = writingPortBufferCsmCurrent[channel][port] ;

#ifdef CSMtestNominalCurrent
        CSMtestNominalCurrent(&latch_nominal_current[channel]) ;
        memoryBank_CSM[channel]->nom_cur_hi = latch_nominal_current[channel] >> 8 ;
        memoryBank_CSM[channel]->nom_cur_lo = latch_nominal_current[channel] ;
#endif
        return MPC_RW_OK;


    case 9:
#ifdef MPC_CSM_PROTECT_CURRENT
        memoryBank_CSM[channel]->protect_cur_hi = value;

        writingPortBufferCsmProtection[channel][port] = ((uint16_t)value) << 8;
#endif
        return MPC_RW_OK;

    case 10:
#ifdef MPC_CSM_PROTECT_CURRENT
        memoryBank_CSM[channel]->protect_cur_lo = value;

        writingPortBufferCsmProtection[channel][port] |= (uint16_t)value;
#else
        writingPortBufferCsmProtection[channel][port] = ((uint16_t)memoryBank_CSM[channel]->protect_cur_hi) << 8 | (uint16_t)memoryBank_CSM[channel]->protect_cur_lo;
#endif

        latch_protect_current[channel] = writingPortBufferCsmProtection[channel][port];

#ifdef CSMtestProtectionCurrent
        CSMtestProtectionCurrent(&latch_protect_current[channel]) ;
        memoryBank_CSM[channel]->protect_cur_hi = latch_protect_current[channel] >> 8 ;
        memoryBank_CSM[channel]->protect_cur_lo = latch_protect_current[channel] ;
#endif
        return MPC_RW_OK;

    default:
        return MPC_RW_OUT_OF_RANGE;                              // other values are not writable
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "CSM".                            Version without port
///
/// The data will be collected from the real device.
/// The checksum will be handled in MemoryBank.c
///
/// \param address      The address in the selected memory bank.
/// \param value        Pointer to a memory address to save the result.
/// \param channel      The device channel.
///
/// \retval            'MPC_RW_OK' in case of successful read
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Csm_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{
    return MemoryBank_Csm_GetValue_port( address, value, channel, 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank "CSM".                           Version without port
///
/// The bank number in daliDevice->dtr1, address in daliDevice->dtr and channel in daliDevice->channel.
///
/// \param address      The address in the selected memory bank.
/// \param value        The value to write.
/// \param channel      The channel ID of the device .
///
/// \retval            'MPC_RW_OK' in case of a successful write
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Csm_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
    return MemoryBank_Csm_SetValue_port( address, value, channel, 0 );
}


//--------------------------------------------------------------------------------------------------------------------------
//
//                        Application functions
//
//--------------------------------------------------------------------------------------------------------------------------


// **************************************************** Function to GET MPC data

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to inquire the current mode
///
/// \param channel     The channel ID of the device.
///
/// \retval            The mode set by the configuation tool into the MPC.
//---------------------------------------------------------------------------------------------------------------------------
uint8_t MemoryBank_CSM_GetLedsetMode (uint8_t channel)
{
    return ((memoryBank_CSM[channel]->opMode) & 0x87);
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to get the nominal current set the configuation tool into the MPC.
///
/// \param channel     The channel ID of the device.
///
/// \retval            nominal current
//---------------------------------------------------------------------------------------------------------------------------
uint16_t MemoryBank_CSM_GetNominalCurrent(uint8_t channel)
{
    return latch_nominal_current[channel] ;
}


#ifdef MPC_CSM_PROTECT_CURRENT
//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to get the protection current set the configuation tool into the MPC.
///
/// \param channel     The channel ID of the device.
///
/// \retval            protection current
//---------------------------------------------------------------------------------------------------------------------------
uint16_t MemoryBank_CSM_GetProtectionCurrent(uint8_t channel)
{
    return latch_protect_current[channel] ;
}
#endif


// ************************************************** Function to WRITE into MPC


//---------------------------------------------------------------------------------------------------------------------------
/// \brief   allow the application to set / reset the operating mode to any value
///
/// \param channel     The channel ID of the device.
/// \param value       The new operating mode.
///
/// \retval            none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_CSM_Set_OpMode(uint8_t channel, uint8_t value)                  /***** OSRAM RESERVED */
{
    memoryBank_CSM[channel]->opMode = value;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to set the status flags
///
/// \param channel     The channel ID of the device.
/// \param value       The status information.
///
/// \retval            none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_CSM_UpdateLedsetStatus(uint8_t channel, uint8_t value)
{
    ledset_status[channel] = value;
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to set the LEDset current
///
/// \param channel    The channel ID of the device.
/// \param value      The present current
///
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_CSM_UpdateLedsetCurrent(uint8_t channel, uint16_t value16)
{
    ledset_current[channel] = value16 ;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    Put together two consecutive byte to have 16 bit value
///
/// \param    byte pointer, MSB first
///
/// \retval   16 bit result value
//----------------------------------------------------------------------------------------------------------------------

uint16_t joinBytes(uint8_t *ptr)
{
    return ( (uint16_t)(ptr[0] << 8) + ptr[1] );
}

