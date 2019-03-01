// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Astro.c
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
// $Id: MemoryBank_Astro.c 15312 2018-10-17 15:42:49Z d.fighera $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Outdoor/trunk/Src/MemoryBank_Astro.c $
//
// ---------------------------------------------------------------------------------------------------------------------
///  /brief Implementation of the memory bank "AstroDIM Configuration Page".
///  MPC ID=05, Version 2, 2D1 2786697 04
///  #define MPC_ASTRO_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "MpcDefs.h"
#include "nvm.h"
#include "MemoryBank_Astro.h"

#define MacroLowByte( x )  ( (uint8_t)( x & 0xFF) )                             //Low-Byte eines Integer Wertes
#define MacroHighByte( x ) ( (uint8_t)( (x & 0xFF00) >> 8 ) )                   //High-Byte eines Integer Wertes


typedef struct {
    uint8_t opMode;                 // RW 6
    uint8_t nominal_level;
    uint16_t startUp_fade_time;     // added: 2 bytes for start-up fade time

    uint8_t astro_fade;
    uint8_t switchOff_fade;
    int16_t start_time_dim;

    uint8_t level_1;
    uint16_t duration_1;

    uint8_t level_2;
    uint16_t duration_2;

    uint8_t level_3;
    uint16_t duration_3;

    uint8_t level_4;
    uint8_t toolBytes[5];
    uint16_t ed_ontime;

} TypeMemoryBank_Astro;

uint8_t memoryBankAstro_lockByte[DEVICE_CHANNELS];                              ///< \brief The lock byte of the memory bank.
uint8_t memoryBankAstro_status[DEVICE_CHANNELS];

/// \brief The data of the memory bank.
TypeMemoryBank_Astro * memoryBank_Astro[DEVICE_CHANNELS];
TypeMemoryBank_Astro memoryBank_AstroPhy[DEVICE_CHANNELS];


uint16_t internal_ed_ontime[DEVICE_CHANNELS];

uint16_t readingPortBufferAstro[AVAILABLE_PORTS_NUMBER];
uint16_t writingPortBufferAstro[AVAILABLE_PORTS_NUMBER];


void Astro_factorySettings(uint8_t channel);

//---------------------------------------------------------------------------------------------------------------------------
/// \brief    Initialise the memory bank.
///
/// \retval   none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_Init( void )
{
    uint8_t i, j,tempMode;
    bool resetData;
    for( i = 0; i < DEVICE_CHANNELS; i++ ) {

        memoryBank_Astro[i] = &memoryBank_AstroPhy[i] ;

        resetData = ( nvmDataInit( (void*)memoryBank_Astro[i] , sizeof( TypeMemoryBank_Astro ), (nvm_memclass_t)0 ) != NVM_INIT_OK );

        tempMode = 0;
        if (resetData == false ) {
            tempMode = (memoryBank_Astro[i]->opMode) & ASTRO_RESET_TIME_MASK;
            if ( memoryBank_Astro[i]->opMode & ASTRO_RELOAD_PARAM_MASK ) {
                resetData = true;
            }
        }

        if (resetData) {                                                        // set the default values

            Astro_factorySettings(i);

            memoryBank_Astro[i]->opMode =         configMemoryBank_Astro_opMode;
            memoryBank_Astro[i]->startUp_fade_time = configMemoryBank_Astro_startUp_fade;

            memoryBank_Astro[i]->astro_fade =     configMemoryBank_Astro_astro_fade;
            memoryBank_Astro[i]->switchOff_fade = configMemoryBank_Astro_switchOff_fade;
            memoryBank_Astro[i]->start_time_dim = configMemoryBank_Astro_start_time;

            memoryBank_Astro[i]->nominal_level = configMemoryBank_Astro_nominal_level;
            memoryBank_Astro[i]->level_1 = configMemoryBank_Astro_level1;
            memoryBank_Astro[i]->level_2 = configMemoryBank_Astro_level2;
            memoryBank_Astro[i]->level_3 = configMemoryBank_Astro_level3;
            memoryBank_Astro[i]->level_4 = configMemoryBank_Astro_level4;

            memoryBank_Astro[i]->duration_1 = configMemoryBank_Astro_time1;
            memoryBank_Astro[i]->duration_2 = configMemoryBank_Astro_time2;
            memoryBank_Astro[i]->duration_3 = configMemoryBank_Astro_time3;

            for (j=0; j<(5); j++) {
                memoryBank_Astro[i]->toolBytes[j] = 0;
            }
            memoryBank_Astro[i]->ed_ontime = 0xFFFF;

        } else {                                                                // use persistent memory content

            // thus no code is needed here
        }

        memoryBankAstro_lockByte[i] = 255;
        memoryBankAstro_status[i] = 0x00;

        memoryBank_Astro[i]->opMode |= tempMode;                                // Set the reset history command flag

        internal_ed_ontime[i] = 0xFFFF;


#ifdef AstroTestStarttime
        AstroTestStarttime(&memoryBank_Astro[i]->start_time_dim) ;
#endif
#ifdef AstroTestDuration
        AstroTestDuration(&memoryBank_Astro[i]->duration_1) ;
        AstroTestDuration(&memoryBank_Astro[i]->duration_2) ;
        AstroTestDuration(&memoryBank_Astro[i]->duration_3) ;
#endif

    }
}
//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Bank.
///
/// \param channel    The device channel.
/// \retval            none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_Reset( uint8_t channel )                                     // reset only MPC variables
{
    // no variables of MPC have to be reset
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief  Reset the factory settings of Astro
///
/// \param channel    The device channel.
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_FactoryReset( void )
{
uint8_t i;

  for( i = 0; i < DEVICE_CHANNELS; i++ )
  {
      Astro_factorySettings(i);
  }

}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief Read m.b data
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
mpc_rw_enum MemoryBank_Astro_GetValue_Port( uint8_t address, uint8_t *value, uint8_t channel, uint8_t port )
{
    uint8_t tmp;
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }

    if (address > MemoryBank_Astro_LastAddress) {
        return MPC_RW_OUT_OF_RANGE;
    }

    switch( address ) {
    case 0:                                                                     //last addressable memory bank location
        *value = MemoryBank_Astro_LastAddress;
        return MPC_RW_OK;
    case 1:                                                                     //Checksum is handled by memory_bank.c
        *value = 0;
        return MPC_RW_OK;
    case 2:                                                                     // lock byte
        *value = memoryBankAstro_lockByte[channel];
        return MPC_RW_OK;
    case 3:                                                                     //number of the multipurpose center
        *value = MemoryBank_Astro_ID;
        return MPC_RW_OK;
    case 4:                                                                     //version of the multipurpose center implementation
        *value = MemoryBank_Astro_Version;
        return MPC_RW_OK;
    case 5:
        *value = memoryBankAstro_status[channel];
        return MPC_RW_OK;
    case 6:
        *value = memoryBank_Astro[channel]->opMode;
        return MPC_RW_OK;
    case 7:
        *value = memoryBank_Astro[channel]->nominal_level;
        return MPC_RW_OK;

    case 8:
        readingPortBufferAstro[port] = memoryBank_Astro[channel]->startUp_fade_time;
        *value = readingPortBufferAstro[port] >> 8;                             // MSB
        return MPC_RW_OK;
    case 9:
        *value = readingPortBufferAstro[port];
        return MPC_RW_OK;
    case 10:
        *value = memoryBank_Astro[channel]->astro_fade;
        return MPC_RW_OK;
    case 11:
        *value = memoryBank_Astro[channel]->switchOff_fade;
        return MPC_RW_OK;

    case 12:
        readingPortBufferAstro[port] = memoryBank_Astro[channel]->start_time_dim;
        *value = readingPortBufferAstro[port] >> 8;                             // MSB
        return MPC_RW_OK;
    case 13:
        *value = readingPortBufferAstro[port];
        return MPC_RW_OK;
    case 14:
        *value = memoryBank_Astro[channel]->level_1;
        return MPC_RW_OK;

    case 15:
        readingPortBufferAstro[port] = memoryBank_Astro[channel]->duration_1;
        *value = readingPortBufferAstro[port] >> 8;                             // MSB
        return MPC_RW_OK;
    case 16:
        *value = readingPortBufferAstro[port];
        return MPC_RW_OK;
    case 17:
        *value = memoryBank_Astro[channel]->level_2;
        return MPC_RW_OK;

    case 18:
        readingPortBufferAstro[port] = memoryBank_Astro[channel]->duration_2;
        *value = readingPortBufferAstro[port] >> 8;                             // MSB
        return MPC_RW_OK;
    case 19:
        *value = readingPortBufferAstro[port];
        return MPC_RW_OK;
    case 20:
        *value = memoryBank_Astro[channel]->level_3;
        return MPC_RW_OK;

    case 21:
        readingPortBufferAstro[port] = memoryBank_Astro[channel]->duration_3;
        *value = readingPortBufferAstro[port] >> 8;                             // MSB
        return MPC_RW_OK;
    case 22:
        *value = readingPortBufferAstro[port];
        return MPC_RW_OK;
    case 23:
        *value = memoryBank_Astro[channel]->level_4;
        return MPC_RW_OK;

    case 29:
        *value = MacroHighByte( internal_ed_ontime[channel]);                   // MSB
        return MPC_RW_OK;
    case 30:
        *value = MacroLowByte( internal_ed_ontime[channel]);
        return MPC_RW_OK;

    default:     // 24 - 28
        tmp = (address - 24);
        *value = memoryBank_Astro[channel]->toolBytes[tmp];
        return MPC_RW_OK;
    }
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank
///
/// The bank number in daliDevice->dtr1, address in daliDevice->dtr and channel in daliDevice->channel.
///
/// \param address      The address in the selected memory bank.
/// \param value        The value to write.
/// \param channel      The channel ID of the device .
/// \param port         The port number.
///
/// \retval            'MPC_RW_OK' in case of successful write
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Astro_SetValue_Port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port )
{
    uint8_t tmp;
    if ( (memoryBankAstro_lockByte[channel] != 0x55) && (address != 2)) {
        return MPC_RW_DENY;
    }
    if ( port >= AVAILABLE_PORTS_NUMBER )
    {
        return MPC_RW_OUT_OF_RANGE;
    }
    if (address > MemoryBank_Astro_LastAddress) {
        return MPC_RW_OUT_OF_RANGE;
    }

    //ServiceOutdoorMpcWriteCounter(MemoryBank_Astro_ID);                         // for counting

    switch( address ) {
    case 2:                                                                     // the lock byte
        memoryBankAstro_lockByte[channel] = value;
        return MPC_RW_OK;

    case 6:
        memoryBank_Astro[channel]->opMode = value;
        return MPC_RW_OK;
    case 7:
        memoryBank_Astro[channel]->nominal_level = value;
#ifdef DIM4testDimLevel
        DIM4testDimLevel(&memoryBank_Astro[channel]->nominal_level) ;
#endif
        return MPC_RW_OK;

    case 8:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 9:
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->startUp_fade_time = writingPortBufferAstro[port];
        return MPC_RW_OK;

    case 10:
        memoryBank_Astro[channel]->astro_fade = value;
        return MPC_RW_OK;
    case 11:
        memoryBank_Astro[channel]->switchOff_fade = value;
        return MPC_RW_OK;

    case 12:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 13:
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->start_time_dim = writingPortBufferAstro[port];
#ifdef AstroTestStarttime
        AstroTestStarttime(&memoryBank_Astro[channel]->start_time_dim);
#endif
        return MPC_RW_OK;

    case 14:
        memoryBank_Astro[channel]->level_1 = value;
#ifdef DIM4testDimLevel
        DIM4testDimLevel(&memoryBank_Astro[channel]->level_1) ;
#endif
        return MPC_RW_OK;

    case 15:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 16:
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->duration_1 = writingPortBufferAstro[port];
#ifdef AstroTestDuration
        AstroTestDuration(&memoryBank_Astro[channel]->duration_1) ;
#endif
        return MPC_RW_OK;
    case 17:
        memoryBank_Astro[channel]->level_2 = value;
#ifdef DIM4testDimLevel
        DIM4testDimLevel(&memoryBank_Astro[channel]->level_2) ;
#endif
        return MPC_RW_OK;

    case 18:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 19:
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->duration_2 = writingPortBufferAstro[port];
#ifdef AstroTestDuration
        AstroTestDuration(&memoryBank_Astro[channel]->duration_2) ;
#endif
        return MPC_RW_OK;
    case 20:
        memoryBank_Astro[channel]->level_3 = value;
#ifdef DIM4testDimLevel
        DIM4testDimLevel(&memoryBank_Astro[channel]->level_3);
#endif
        return MPC_RW_OK;
    case 21:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 22:
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->duration_3 = writingPortBufferAstro[port];
#ifdef AstroTestDuration
        AstroTestDuration(&memoryBank_Astro[channel]->duration_3) ;
#endif
        return MPC_RW_OK;
    case 23:
        memoryBank_Astro[channel]->level_4 = value;
#ifdef DIM4testDimLevel
        DIM4testDimLevel(&memoryBank_Astro[channel]->level_4);
#endif
        return MPC_RW_OK;

    case 29:
        writingPortBufferAstro[port] = value << 8;                              // MSB
        return MPC_RW_OK;
    case 30:                                                                    // DALI writes
        writingPortBufferAstro[port] |= value;
        memoryBank_Astro[channel]->ed_ontime = writingPortBufferAstro[port];
        internal_ed_ontime[channel] = memoryBank_Astro[channel]->ed_ontime;
        return MPC_RW_OK;

    default:       // 24 - 28
        if ((address >= 24) && (address <= 28)) {
            tmp = (address - 24);
            memoryBank_Astro[channel]->toolBytes[tmp] = value;
            return MPC_RW_OK;
        }
        return MPC_RW_OUT_OF_RANGE;
    }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank                                   Version without port
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
mpc_rw_enum MemoryBank_Astro_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{
    return MemoryBank_Astro_GetValue_Port( address, value, channel, 0 );
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank                                  Version without port
///
/// \param address      The address in the selected memory bank.
/// \param value        The value to write.
/// \param channel      The channel ID of the device .
///
/// \retval            'MPC_RW_OK' in case of a successful write
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Astro_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
    return MemoryBank_Astro_SetValue_Port( address, value, channel, 0 );
}




//---------------------------------------------------------------------------------------------------------------------------
//    *********************************************     Function to WRITE into MPC
//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to update the mpc status uint8_t
///
/// \param channel        The channel ID of the device.
/// \param value        status
///
/// \retval                none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_UpdateStatus(uint8_t channel, uint8_t value)
{
    memoryBankAstro_status[channel] = value;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief         To update the mpc operating mode
///
/// \param channel      The channel ID of the device.
/// \param value        mode
///
/// \retval             none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_UpdateOpMode(uint8_t channel, uint8_t mode)
{
    memoryBank_Astro[channel]->opMode = mode;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief         To overwrite ED ontime into mpc structure (permanent)
///
/// \param channel      The channel ID of the device.
/// \param value        ED value
///
/// \retval             none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_ClearStartEDontime(uint8_t channel, uint16_t value16)
{
    memoryBank_Astro[channel]->ed_ontime = value16 ;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to set the Estimate Duration time information - runtime
///
/// \param channel      The channel ID of the device.
/// \param value        duration in minutes
///
/// \retval             none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Astro_UpdateEDonTime(uint8_t channel, uint16_t value16)
{
    internal_ed_ontime[channel] = value16;
}


//---------------------------------------------------------------------------------------------------------------------------
//    *********************************************     Function to READ MPC data
//---------------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to inquire the Astro mode
///
/// \param channel        The channel ID of the device.
///
/// \retval               The mode set by the configuation tool into the MPC.
//---------------------------------------------------------------------------------------------------------------------------
uint8_t MemoryBank_Astro_GetMode(uint8_t channel)
{
uint8_t  modeA;

    modeA = memoryBank_Astro[channel]->opMode;
    if ( (modeA & 0x03) == 0 ) {                                                // Astro is disabled
        modeA &= 0xC0;
    }
    return (modeA);
}

// Get Levels %
uint16_t MemoryBank_Astro_GetNominalLevel(uint8_t channel)
{
    return (memoryBank_Astro[channel]->nominal_level);
}
uint16_t MemoryBank_Astro_GetLevel1(uint8_t channel)
{
    return (memoryBank_Astro[channel]->level_1);
}
uint16_t MemoryBank_Astro_GetLevel2(uint8_t channel)
{
    return (memoryBank_Astro[channel]->level_2);
}
uint16_t MemoryBank_Astro_GetLevel3(uint8_t channel)
{
    return (memoryBank_Astro[channel]->level_3);
}
uint16_t MemoryBank_Astro_GetLevel4(uint8_t channel)
{
    return (memoryBank_Astro[channel]->level_4);
}

// Get latched information in minutes (not mpc data)
int16_t MemoryBank_Astro_GetStartTime(uint8_t channel)                          // signed value
{
//    int16_t sstart;
//    sstart = latch_starttime[channel];                                        // latched value
//    return ( sstart );
    return memoryBank_Astro[channel]->start_time_dim;

}
uint16_t MemoryBank_Astro_GetDuration1(uint8_t channel)
{
    return memoryBank_Astro[channel]->duration_1;
}
uint16_t MemoryBank_Astro_GetDuration2(uint8_t channel)
{
    return memoryBank_Astro[channel]->duration_2;
}
uint16_t MemoryBank_Astro_GetDuration3(uint8_t channel)
{
    return memoryBank_Astro[channel]->duration_3;
}

// Get fade times in seconds
uint16_t MemoryBank_Astro_GetStartUpFade(uint8_t channel)
{
    return memoryBank_Astro[channel]->startUp_fade_time;                        // latched value
}
uint16_t MemoryBank_Astro_GetAstroFade(uint8_t channel)
{
    return ( memoryBank_Astro[channel]->astro_fade * SHORTStepTime);
}
uint16_t MemoryBank_Astro_GetSwitchOffFade(uint8_t channel)
{
    if ( memoryBank_Astro[channel]->switchOff_fade == 255) {
        return 65535;
    }
    return ( memoryBank_Astro[channel]->switchOff_fade * LONGStepTime );
}

// Get ED value from mpc structure
uint16_t MemoryBank_Astro_GetStartEDontime(uint8_t channel)
{
    return ( memoryBank_Astro[channel]->ed_ontime ) ;
}




//----------------------------------------------------------------------------------------------------------------------
/// \brief    Set the factory setting
///
/// \param    channel
///
/// \retval   none
//----------------------------------------------------------------------------------------------------------------------

static void Astro_factorySettings(uint8_t chan)
{
uint8_t j;

//      memoryBank_Astro[chan]->opMode =         configMemoryBank_Astro_opMode;
//      memoryBank_Astro[chan]->startUp_fade_time = configMemoryBank_Astro_startUp_fade;
//
//      memoryBank_Astro[chan]->astro_fade =     configMemoryBank_Astro_astro_fade;
//      memoryBank_Astro[chan]->switchOff_fade = configMemoryBank_Astro_switchOff_fade;
//      memoryBank_Astro[chan]->start_time_dim = configMemoryBank_Astro_start_time;
//
//      memoryBank_Astro[chan]->nominal_level = configMemoryBank_Astro_nominal_level;
//      memoryBank_Astro[chan]->level_1 = configMemoryBank_Astro_level1;
//      memoryBank_Astro[chan]->level_2 = configMemoryBank_Astro_level2;
//      memoryBank_Astro[chan]->level_3 = configMemoryBank_Astro_level3;
//      memoryBank_Astro[chan]->level_4 = configMemoryBank_Astro_level4;
//
//      memoryBank_Astro[chan]->duration_1 = configMemoryBank_Astro_time1;
//      memoryBank_Astro[chan]->duration_2 = configMemoryBank_Astro_time2;
//      memoryBank_Astro[chan]->duration_3 = configMemoryBank_Astro_time3;

      for (j=0; j<(5); j++) {
          memoryBank_Astro[chan]->toolBytes[j] = 0;
      }

      memoryBank_Astro[chan]->ed_ontime = 0x0000;  // 0xFFFF;
      memoryBankAstro_lockByte[chan] = 255;
      memoryBankAstro_status[chan] = 0x00;

      internal_ed_ontime[chan] = 0xFFFF;
}



