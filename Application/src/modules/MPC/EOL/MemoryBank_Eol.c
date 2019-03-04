// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Eol.c
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
// $Id: MemoryBank_Eol.c 15312 2018-10-17 15:42:49Z d.fighera $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Outdoor/tags/rev1.3/Src/MemoryBank_Eol.c $
//
// ---------------------------------------------------------------------------------------------------------------------
///  /brief Implementation of the memory bank "End Of Life Configuration Page".
///  MPC ID=13, Version 0, 2D1 2980505
///  #define MPC_EOL_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------


#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "nvm.h"

#include "MemoryBank_Eol.h"


typedef struct {
    uint8_t enable;            // RW 6
    uint8_t eol_time;
} TypeMemoryBank_Eol;

uint8_t memoryBankEol_lockByte[DEVICE_CHANNELS];                                ///< \brief The lock byte of the memory bank.
uint8_t memoryBankEol_status[DEVICE_CHANNELS];

/// \brief The data of the memory bank.
TypeMemoryBank_Eol memoryBank_EolPhy[DEVICE_CHANNELS];
TypeMemoryBank_Eol * memoryBank_Eol[DEVICE_CHANNELS];



//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory bank.
///
///
/// \retval            Return true, the persistent data are changed, else the persistent data are unchanged.
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Eol_Init(void )
{
    uint8_t i;
    bool resetData;

    for( i = 0; i < DEVICE_CHANNELS; i++ ) {

        memoryBank_Eol[i] = &memoryBank_EolPhy[i] ;
        resetData = ( nvmDataInit( (void*)memoryBank_Eol[i] , sizeof( TypeMemoryBank_Eol ), (nvm_memclass_t)0 ) != NVM_INIT_OK );

        if (resetData) {                                                        // set the default values

            memoryBank_Eol[i]->enable = configMemoryBank_Eol_enable;
            memoryBank_Eol[i]->eol_time = configMemoryBank_Eol_eol_time;

        } else {                                                                // use persistent memory content
            //
            // thus no code is needed here
        }

        memoryBankEol_lockByte[i] = 255;                                        ///< \brief The lock byte of the memory bank.
        memoryBankEol_status[i] = 0;

    }

}
//---------------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Bank.
///
/// \param channel    The device channel.
/// \retval           none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Eol_Reset( uint8_t channel )                                       // reset only MPC variables
{
    // no variables of MPC have to be reset
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
///
/// \retval            True if successful and value valid.
//---------------------------------------------------------------------------------------------------------------------------
bool MemoryBank_Eol_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{

    switch( address ) {
    case 0:                                                                     //last addressable memory bank location
        *value = MPC_EOL_LAST_ADDRESS;
        return true;
    case 1:                                                                     //Checksum is handled by memory_bank.c
        *value = 0;
        return true;                                                            //
    case 2:                                                                     // lock byte
        *value = memoryBankEol_lockByte[channel];
        return true;
    case 3:                                                                     //number of the multipurpose center
        *value = MPC_EOL_ID;
        return true;
    case 4:                                                                     //version of the multipurpose center implementation
        *value = MPC_EOL_VERSION;
        return true;
    case 5:
        *value = memoryBankEol_status[channel];
        return true;
    case 6:
        *value = memoryBank_Eol[channel]->enable;
        return true;
    case 7:
        *value = memoryBank_Eol[channel]->eol_time;
        return true;

    default:                                                                    //access the sensor table
        return false;
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
///
/// \retval            True if value was written.
//---------------------------------------------------------------------------------------------------------------------------
bool MemoryBank_Eol_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
    if ( (memoryBankEol_lockByte[channel] != 0x55) && (address != 2)) {
        return false;
    }

    switch( address ) {
    case 2:                                                                     // the lock byte
        memoryBankEol_lockByte[channel] = value;
        return true;
    case 6:
        memoryBank_Eol[channel]->enable = value;
        return true;
    case 7:
        memoryBank_Eol[channel]->eol_time = value;
#ifdef EOLtestTime
        EOLtestTime(&memoryBank_Eol[channel]->eol_time) ;
#endif
        return true;
    default:
        return false;
    }
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to set the status flags
///
/// \param channel      The channel ID of the device.
/// \param value        The status information.
///
/// \retval             none
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Eol_UpdateStatus(uint8_t channel, uint8_t value)
{
    memoryBankEol_status[channel] = value;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to inquire the enable status
///
/// \param channel     The channel ID of the device.
///
/// \retval            enable
//---------------------------------------------------------------------------------------------------------------------------
bool MemoryBank_EOL_GetEnable (uint8_t channel)
{
    return (memoryBank_Eol[channel]->enable & 0x01);
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief allow the application to inquire the EOL time
///
/// \param channel     The channel ID of the device.
///
/// \retval            EOL time
//---------------------------------------------------------------------------------------------------------------------------
uint8_t MemoryBank_EOL_GetEOLtime (uint8_t channel)
{
    return (memoryBank_Eol[channel]->eol_time);
}







