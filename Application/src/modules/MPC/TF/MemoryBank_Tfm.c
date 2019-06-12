// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Tfm.c
// ---------------------------------------------------------------------------------------------------------------------
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version:
// $Author: d.fighera $
// $Revision: 12696 $
// $Date: 2018-04-04 21:51:24 +0800 (Wed, 04 Apr 2018) $
// $Id: MemoryBank_Tfm.c 12696 2018-04-04 13:51:24Z d.fighera $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/TuningFactor/trunk/Source/MemoryBank_Tfm.c $
//
// Implementation of the memory bank "Tuning factor", MPC ID=22, Version 0, TFM , 2D1 3431755 00
// #define MPC_TFM_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "MpcDefs.h"
#include "nvm.h"
#include "power_task.h"
//#include "Acl.h"

#include "MemoryBank_Tfm.h"


#ifndef AVAILABLE_PORTS_NUMBER
  #define AVAILABLE_PORTS_NUMBER    1
  #warning  "Undefined number of ports for MPC access !"
#endif


typedef struct
{
    uint8_t enableByte;                   // RW 5
    uint8_t tuningFactor;
    uint8_t tuningFactorMin;
    uint8_t tuningFactorMax;

    uint32_t referenceLumenOutput;

} TypeMemoryBank_Tfm;

uint8_t memoryBankTfm_lockByte[DEVICE_CHANNELS];

/// \brief The data of the memory bank.
TypeMemoryBank_Tfm memoryBank_TfmPhy[DEVICE_CHANNELS];
TypeMemoryBank_Tfm * memoryBank_Tfm[DEVICE_CHANNELS];

uint32_t readingPortBufferReferenceLumenOutput[AVAILABLE_PORTS_NUMBER];
uint32_t writingPortBufferReferenceLumenOutput[AVAILABLE_PORTS_NUMBER];

static uint32_t tuningFactQ15[DEVICE_CHANNELS];

static void calcTuningFactQ15(uint8_t channel){
  // the addition of 255 is an optimum to minimize errors due to integer truncation;
  // optimum factor is 2^24/100 to get needed resolution = 167772; this is quite precise
  tuningFactQ15[channel] = ((uint32_t)memoryBank_Tfm[channel]->tuningFactor * 167772 + 255) >> 9;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory bank.
///
///
/// \retval   none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Tfm_Init()
{
    uint8_t i;
    bool resetData;

    for( i = 0; i < DEVICE_CHANNELS; i++ )
    {
        memoryBank_Tfm[i] = &memoryBank_TfmPhy[i] ;
        resetData = ( nvmDataInit( (void*)memoryBank_Tfm[i] , sizeof( TypeMemoryBank_Tfm ), (nvm_memclass_t)0 ) != NVM_INIT_OK );

        if (resetData)                                                          // set the default values
        {
            memoryBank_Tfm[i]->enableByte = MPC_TFM_DEFAULT_ENABLE;
            memoryBank_Tfm[i]->tuningFactor = MPC_TFM_DEFAULT_TUNING_FACTOR;
            memoryBank_Tfm[i]->tuningFactorMin = MPC_TFM_DEFAULT_TUNING_FACTOR_MIN;
            memoryBank_Tfm[i]->tuningFactorMax = MPC_TFM_DEFAULT_TUNING_FACTOR_MAX;
            memoryBank_Tfm[i]->referenceLumenOutput = MPC_TFM_DEFAULT_REFERENCE_LUMEN_OUTPUT;

        }
        memoryBankTfm_lockByte[i] = 255;

        #ifdef TFMtestTuningFactor
        TFMtestTuningFactor(&memoryBank_Tfm[i]->tuningFactor);
        #endif

        #ifdef TFMtestMinimumTuningFactor
        TFMtestMinimumTuningFactor(&memoryBank_Tfm[i]->tuningFactorMin);
        #endif
        #ifdef TFMtestMaximumTuningFactor
        TFMtestMaximumTuningFactor(&memoryBank_Tfm[i]->tuningFactorMax);
        #endif

        // tuning factor relative tests: only during MPC init, to avoid interlock conditions during configuration

        if (memoryBank_Tfm[i]->tuningFactorMin > memoryBank_Tfm[i]->tuningFactorMax) {      // Min - Max
          memoryBank_Tfm[i]->tuningFactorMin = memoryBank_Tfm[i]->tuningFactorMax;
        }
        if (memoryBank_Tfm[i]->tuningFactor < memoryBank_Tfm[i]->tuningFactorMin) {         // TF - Min
          memoryBank_Tfm[i]->tuningFactor = memoryBank_Tfm[i]->tuningFactorMin;
        }
        if (memoryBank_Tfm[i]->tuningFactor > memoryBank_Tfm[i]->tuningFactorMax) {         // TF - Max
          memoryBank_Tfm[i]->tuningFactor = memoryBank_Tfm[i]->tuningFactorMax;
        }

        // set the Q15 values
        // calcTuningFactQ15(i);
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Bank.
///
/// \param channel    The device channel.
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Tfm_Reset( uint8_t channel )                                    // reset only MPC variables
{
    // no variables of MPC have to be reset
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank
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

mpc_rw_enum MemoryBank_Tfm_GetValue_Port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port )
{
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
      // Test Coverage Exception: Unreachable, due to testing with VirtECG, which allows DALI only [29.05.17]
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 0:                                                                     //last addressable memory bank location
        *value = MPC_TuningFactor_LastADDRESS;
        return MPC_RW_OK;

    case 1:                                                                     // indicator byte is always 0
        *value = 0;
        return MPC_RW_OK;

    case 2:                                                                     // lock byte
        *value = memoryBankTfm_lockByte[channel];
        return MPC_RW_OK;

    case 3:                                                                     //number of the multipurpose center
        *value = MPC_TuningFactor_ID ;
        return MPC_RW_OK;

    case 4:                                                                     //version of the multipurpose center implementation
        *value = MPC_TuningFactor_VERSION;
        return MPC_RW_OK;

    case 5:                                                                     // enable
        *value = memoryBank_Tfm[channel]->enableByte ;
        return MPC_RW_OK;

    case 6:
        *value = memoryBank_Tfm[channel]->tuningFactor;
        return MPC_RW_OK;

    case 7:
        *value = memoryBank_Tfm[channel]->tuningFactorMin;
        return MPC_RW_OK;

    case 8:
        *value = memoryBank_Tfm[channel]->tuningFactorMax;
        return MPC_RW_OK;

    case 9:                                                                     // MSB latch reading  TODO
        readingPortBufferReferenceLumenOutput[port] = memoryBank_Tfm[channel]->referenceLumenOutput;
        *value = readingPortBufferReferenceLumenOutput[port] >> 16;
        return MPC_RW_OK;

    case 10:
        *value = readingPortBufferReferenceLumenOutput[port] >> 8;
        return MPC_RW_OK;

    case 11:
        *value = readingPortBufferReferenceLumenOutput[port];
        return MPC_RW_OK;

    default:
        return MPC_RW_OUT_OF_RANGE;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in the memory bank
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

mpc_rw_enum MemoryBank_Tfm_SetValue_Port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port )
{
    if ( (memoryBankTfm_lockByte[channel] != 0x55) && (address != 2))
    {
      // Test Coverage Exception: Unreachable, due to dealing with lockbyte in MpcWriteDali
        return MPC_RW_DENY;
    }
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
      // Test Coverage Exception: Unreachable, due to testing with VirtECG, which allows DALI only [29.05.17]
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address )
    {
    case 2:                                                                     // the lock byte
        memoryBankTfm_lockByte[channel] = value;
        return MPC_RW_OK;

    case 5:
        memoryBank_Tfm[channel]->enableByte = value & 0x01;        // only allow valid values
        return MPC_RW_OK;

    case 6:
        memoryBank_Tfm[channel]->tuningFactor = value;
        // calcTuningFactQ15(channel);
#ifdef TFMtestTuningFactor
        TFMtestTuningFactor(&memoryBank_Tfm[channel]->tuningFactor);
#endif
        return MPC_RW_OK;
    case 7:
        memoryBank_Tfm[channel]->tuningFactorMin = value;
#ifdef TFMtestMinimumTuningFactor
        TFMtestMinimumTuningFactor(&memoryBank_Tfm[channel]->tuningFactorMin);
#endif
        return MPC_RW_OK;
     case 8:
        memoryBank_Tfm[channel]->tuningFactorMax = value;
#ifdef TFMtestMaximumTuningFactor
        TFMtestMaximumTuningFactor(&memoryBank_Tfm[channel]->tuningFactorMax);
#endif
        return MPC_RW_OK;
    case 9:
        writingPortBufferReferenceLumenOutput[port] = value << 16;
        return MPC_RW_OK;
    case 10:
        writingPortBufferReferenceLumenOutput[port] |= value << 8;
        return MPC_RW_OK;
    case 11:
        writingPortBufferReferenceLumenOutput[port] |= value;
        memoryBank_Tfm[channel]->referenceLumenOutput = writingPortBufferReferenceLumenOutput[port];
        return MPC_RW_OK;

    default:
        return MPC_RW_OUT_OF_RANGE;                                             // other values are not writable
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "                            Version without port
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
// Test Coverage Exception: Not Tested, because unused in VirtECG
mpc_rw_enum MemoryBank_Tfm_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{
    return MemoryBank_Tfm_GetValue_Port( address, value, channel, 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank                            Version without port
///
/// The bank number in daliDevice->dtr1, address in daliDevice->dtr and channel in daliDevice->channel.
///
/// \param address      The address in the selected memory bank.
/// \param value        The value to write.
/// \param channel      The channel ID of the device .
///
/// \retval            'MPC_RW_OK' in case of a successful write
//---------------------------------------------------------------------------------------------------------------------------
// Test Coverage Exception: Not Tested, because unused in VirtECG
mpc_rw_enum MemoryBank_Tfm_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
    return MemoryBank_Tfm_SetValue_Port( address, value, channel, 0 );
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
uint8_t MemoryBank_Tfm_GetEnable (uint8_t channel)
{
    return (memoryBank_Tfm[channel]->enableByte & 0x01);
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to inquire the tuning factor
///
/// \param channel     The channel ID of the device.
///
/// \retval            Set tuning factor - this value is already limited inside the Min/Max values
///                    and checked against MPC_TFM_ABS_UPPER_LIMIT/MPC_TFM_ABS_LOWER_LIMIT.
///                    No additional check is neccessary if those are set well.
//---------------------------------------------------------------------------------------------------------------------------
uint8_t MemoryBank_Tfm_GetTuningFactor(uint8_t channel)
{
    return memoryBank_Tfm[channel]->tuningFactor;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to get the tuning factor limits
///
/// \param channel     The channel ID of the device.
///
/// \retval
//---------------------------------------------------------------------------------------------------------------------------
uint8_t MemoryBank_Tfm_GetMinimumTuningFactor(uint8_t channel)
{
    return memoryBank_Tfm[channel]->tuningFactorMin;
}

uint8_t MemoryBank_Tfm_GetMaximumTuningFactor(uint8_t channel)
{
    return memoryBank_Tfm[channel]->tuningFactorMax;
}


// ---------------------------------------------------------------------------------------------------------------------
/*! \brief Return the tuning factor as a q15 number.
 *
 * This returns the TF as standard q15 number - 100% equal 1.0000, which is represented as 0x8000
 * The range of q15 is 0 to 1.999 such 0% to 199 can be represented in a 16Bit number.
 *
 * \note As the function uses uint32_t as return value, 255% are possible, but not reasonable.
 *
 * \tparam channel The channel ID of the device.
 *
 * \retval TF in q15 representation
 *
 */
uint32_t MemoryBank_Tfm_GetTuningFactorQ15(uint8_t channel)
{
    calcTuningFactQ15(channel);	
    return tuningFactQ15[channel];
}


