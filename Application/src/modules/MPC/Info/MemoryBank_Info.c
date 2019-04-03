// ---------------------------------------------------------------------------------------------------------------------
// MemoryBank_Info.c
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
// $Revision: 10732 $
// $Date: 2017-11-13 19:48:15 +0800 (Mon, 13 Nov 2017) $
// $Id: MemoryBank_Info.c 10732 2017-11-13 11:48:15Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Info/tags/V3_3/Source/MemoryBank_Info.c $
//
//
// Implementation of the memory bank "General Info Page" (2784480 000 01). To enable define:
// #define MPC_INFO_NUMBER       [bank number]
// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>


#define MODULE_MPC                                                                 // to #include "MemoryBanksDefault.h" in config.h                                                       
#include "Config.h"
#include "MpcDefs.h"
#include "nvm.h"

#include "DaliServices.h"
#include "MemoryBank_Info.h"
#include "MpcInfo_Services.h" 
#ifdef MPC_CLM_NUMBER
  #include "MemoryBank_Clm.h"
#endif

#define MPC_INFO_STT_PER_MINUTE       (uint16_t) (60000/MPC_SlowTIME_TICK_ms)
#define MPC_INFO_OV_DURATION_LIMIT		(uint16_t) ((2^16) - 2)                      ///< \brief overvoltage duration limit in minutes
#define MPC_INFO_OV_SHUTDOWN_COUNTER	(uint8_t)  ((2^8) - 2)


uint32_t readingPortBufferTemperature[AVAILABLE_PORTS_NUMBER];
uint32_t readingPortBufferOvervoltage[AVAILABLE_PORTS_NUMBER];
uint32_t readingPortBufferCounter[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];
uint32_t writingPortBufferCounter[DEVICE_CHANNELS][AVAILABLE_PORTS_NUMBER];


typedef enum {
  overvoltage,
  no_overvoltage
} mpcInfo_flags_t;


typedef struct{                                                                    ///< \brief The lamp ops counter
  uint16_t ticks;
  uint32_t minutes;                                                                ///< \brief Minutes counter (total).
} infoCounter_t;


typedef struct {
  infoCounter_t lampCounter[DEVICE_CHANNELS];                                      ///< \brief The lamp ops counter in minutes.
  uint8_t maxTemperature;
  uint32_t temperatureTimeIntegrator;
  uint8_t maxSupplyVoltage;
  uint8_t overvoltageShutdownEvents;
  uint16_t totalOvervoltageDuration;
  uint16_t overvoltageTimeTicks;                                                   ///< \brief overvoltage duration ticks
} mpcInfo_nvm_t;


typedef struct {
  mpcInfo_nvm_t nvm;
  uint8_t lockByte[DEVICE_CHANNELS];                                               ///< \brief memory bank lock byte is in RAM
  uint8_t actTemperature;
  uint32_t readTemperatureTimeIntegrator;
  uint16_t readtotalOvervoltageDuration;
  uint32_t readCounter[DEVICE_CHANNELS];
  uint32_t writeCounter[DEVICE_CHANNELS];
  mpcInfo_flags_t internal_state;
} mpcInfo_t;
mpcInfo_t mpcInfo;


//----------------------------------------------------------------------------------------------------------------------
/// \brief    For external access: Returns the Lamp operating counter in minutes.
/// \param    channel: Device-channel to be queried
/// \retval   Lamp operating counter in minutes (value is limited to 0xFFFFFE, 0xFFFFFF means not supported
//----------------------------------------------------------------------------------------------------------------------

uint32_t InfoMPC_GetLampOnMinutes(uint8_t channel) {
  return mpcInfo.nvm.lampCounter[channel].minutes;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    For external access: Returns the Lamp operating counter in ticks.
/// \param    channel: Device-channel to be queried
/// \retval   Lamp operating counter in ticks 
//----------------------------------------------------------------------------------------------------------------------

uint32_t InfoMPC_GetLampOnTicks(uint8_t channel) {
  return mpcInfo.nvm.lampCounter[channel].ticks;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    weak defined function for return of the overvoltage shut down state. Used if no such function is defined
//            in the application. sets overvoltage shut down events and overvoltage duration to their defaults (0xFF).
/// \param    none
/// \retval   false
//----------------------------------------------------------------------------------------------------------------------

__weak bool MpcInfoServices_GetOvervoltageShutDownState( void )
{
  mpcInfo.nvm.overvoltageShutdownEvents  = 0xFF; 
  mpcInfo.nvm.totalOvervoltageDuration   = 0xFFFF; 
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    weak defined function for return of the actual temperature. Used if no such function is defined
//            in the application.
/// \param    none
/// \retval   0xFF
//----------------------------------------------------------------------------------------------------------------------

__weak uint8_t MpcInfoServices_GetActualTemperature( void ) 
{
  return 0xFF;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    weak defined function for return of the actual supply voltage. Used if no such function is defined
//            in the application.
/// \param    none
/// \retval   0xFF
//----------------------------------------------------------------------------------------------------------------------
__weak uint8_t MpcInfoServices_GetSupplyVoltage( void )
{
  return 0xFF;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief    weak defined function for return of the light level state (on or off). Used if no such function is defined
//            in the application.
/// \param    none
/// \retval   false
//----------------------------------------------------------------------------------------------------------------------

__weak bool MpcInfoServices_GetLightLevelState( void ) {
  return false;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialises the memory bank "General Info Page" (default settings, also for optional parameters)
///
/// \param       None
///
/// \retval      None
//----------------------------------------------------------------------------------------------------------------------

void MemoryBank_Info_Init( void )
{
  uint8_t i;
    
  if (nvmDataInit ( (void *)(&(mpcInfo.nvm)), sizeof(mpcInfo_nvm_t), NVM_MEM_CLASS_0) != NVM_INIT_OK) 
  {
    for( i = 0; i < DEVICE_CHANNELS; i++ )
    {
      mpcInfo.nvm.lampCounter[i].ticks = 0;
      mpcInfo.nvm.lampCounter[i].minutes = 0;
    }
    mpcInfo.nvm.maxTemperature             = MpcInfoServices_GetActualTemperature();        
    mpcInfo.nvm.temperatureTimeIntegrator  = 0xFFFFFF; // not supported in this MPC version
    mpcInfo.nvm.maxSupplyVoltage           = MpcInfoServices_GetSupplyVoltage();
    mpcInfo.nvm.overvoltageShutdownEvents  = 0x00; 
    mpcInfo.nvm.totalOvervoltageDuration   = 0x0000; 
    mpcInfo.nvm.overvoltageTimeTicks       = 0;
  }
  mpcInfo.actTemperature                   = 0xFF;
  mpcInfo.internal_state = no_overvoltage;
  
  for( i = 0; i < DEVICE_CHANNELS; i++ )
  {
    mpcInfo.lockByte[i] = 255;
#ifdef MPC_CLM_NUMBER
    MemoryBank_Clm_Init( mpcInfo.nvm.lampCounter[i].minutes, i);
#endif
  }
}



//----------------------------------------------------------------------------------------------------------------------
/// \brief  Reset the memory Bank.
/// \retval None
//----------------------------------------------------------------------------------------------------------------------
void MemoryBank_Info_Reset( uint8_t channel )                                    
{
  // no variables of MPC have to be reset
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief Function for all slowly time depended actions related to the General Info Page.
///
/// This function must be called in the action timer interval and is responsible for
/// - updating the external lamp operations timer
/// - other actions to be implemented later
///
/// \retval      nothing
//---------------------------------------------------------------------------------------------------------------------------
void MemoryBank_Info_SlowTimer( void )
{
  uint8_t i;
  uint8_t supplyVoltage;
  uint8_t actualTemperature;
  uint16_t *totalOvervoltageDuration = &mpcInfo.nvm.totalOvervoltageDuration;
  uint16_t *ovTimeTicks = &mpcInfo.nvm.overvoltageTimeTicks;

  for( i = 0; i < DEVICE_CHANNELS; i++ ) {
    if ( DaliServices_GetOperatingMode( i ) != 0)                                 // lamp is ON if operating mode is set to AM or PWM
    {  infoCounter_t *actCounter = &mpcInfo.nvm.lampCounter[i];
      actCounter->ticks++;
      if (actCounter->ticks >= MPC_INFO_STT_PER_MINUTE) {
        if (actCounter->minutes < 0x00FFFFFE)
        {         
          actCounter->ticks = 0;
          actCounter->minutes++;
#ifdef MPC_CLM_NUMBER                                                             // JA added
          if ( (uint8_t)(actCounter->minutes) == 0)                               // Update mpcClm-adjustFactor each 256 minutes (= 4,27 hours)
          {  
            MemoryBank_Clm_UpdateAdjustFactor(actCounter->minutes, i);
          }
#endif
        } 
      } 
    } 
    
    // get overvoltage shut down flag and add it to the stored value
    // if no overvoltage condition was active before and DALI Level is > 0
    if ( MpcInfoServices_GetOvervoltageShutDownState() == true ) { 
      if ( mpcInfo.nvm.overvoltageShutdownEvents < MPC_INFO_OV_SHUTDOWN_COUNTER ) {
        if ( mpcInfo.internal_state == no_overvoltage ) {
          mpcInfo.nvm.overvoltageShutdownEvents++;                                // increase OV event counter only if OV is detected after normal operation
          mpcInfo.internal_state = overvoltage;
        }
      }  
      
      (*ovTimeTicks)++;                                                            // increasee overvoltage time ticks
      if ( *ovTimeTicks >= MPC_INFO_STT_PER_MINUTE) {
        *ovTimeTicks = 0;            
        if ( *totalOvervoltageDuration < MPC_INFO_OV_DURATION_LIMIT ) {
          (*totalOvervoltageDuration)++;                                           // add one minute
        }     
      }
    }
    else {
      mpcInfo.internal_state = no_overvoltage;
    }  

    // get actual temperature and compare with stored max value
    actualTemperature = MpcInfoServices_GetActualTemperature();    
    if ( actualTemperature < 255 ) {
      mpcInfo.actTemperature = actualTemperature;
      if ( actualTemperature > mpcInfo.nvm.maxTemperature ) {
        mpcInfo.nvm.maxTemperature = actualTemperature;
      }
    }

    // get actual supply voltage and compare with stored max value
    supplyVoltage =  MpcInfoServices_GetSupplyVoltage();
    if ( ( supplyVoltage < 255 ) && ( supplyVoltage > mpcInfo.nvm.maxSupplyVoltage ) ) {
      mpcInfo.nvm.maxSupplyVoltage = supplyVoltage;
    }    
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "General Info Page".
///
/// The data will be collected from the real device.
/// The checksum will be handle in MemoryBank.c
///
/// \param address    The address in the selected memory bank.
/// \param *pValue   Pointer to a memory address to get the result.
/// \param channel    The device channel.
///
/// \retval      True in case of a successful address access.
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Info_GetValue_port( uint8_t address, uint8_t *pValue, uint8_t channel, uint8_t port )
{
  if ( port >= AVAILABLE_PORTS_NUMBER )
  {
      return MPC_RW_OUT_OF_RANGE;
  }

    switch( address )
    {
      case 0:                                                                       
        *pValue = MPC_Info_LastADDRESS;
        return MPC_RW_OK;
      case 1:                                                                       
        *pValue = 0;
        return MPC_RW_OK;
      case 2:                                                                       
        *pValue = mpcInfo.lockByte[channel];
        return MPC_RW_OK;
      case 3:                                                                     
        *pValue = MPC_Info_ID;
        return MPC_RW_OK;
      case 4:                                                                     
        *pValue = MPC_Info_VERSION;
        return MPC_RW_OK;
      case 5:                                                                       // lamp operation timer
        mpcInfo.readCounter[channel] = mpcInfo.nvm.lampCounter[channel].minutes;    // latch value when reading MSB
        readingPortBufferCounter[channel][port] = mpcInfo.readCounter[channel];
        *pValue = (uint8_t)(readingPortBufferCounter[channel][port] >>16);          // return MSB of latched value
        return MPC_RW_OK;
      case 6:
        *pValue = (uint8_t)(readingPortBufferCounter[channel][port] >>8);
        return MPC_RW_OK;
      case 7:
        *pValue = (uint8_t)readingPortBufferCounter[channel][port];                 // return LSB of latched value
        return MPC_RW_OK;
      case 8:
        *pValue = mpcInfo.actTemperature ;
        return MPC_RW_OK;
      case 9:
        *pValue = mpcInfo.nvm.maxTemperature;
        return MPC_RW_OK;
      case 10:
        mpcInfo.readTemperatureTimeIntegrator = mpcInfo.nvm.temperatureTimeIntegrator; // latch value, when reading MSB
        readingPortBufferTemperature[port] = mpcInfo.readTemperatureTimeIntegrator;
        *pValue = (uint8_t)( readingPortBufferTemperature[port] >>16 );             // return MSB of latched value
        return MPC_RW_OK;
      case 11:
        *pValue = (uint8_t)(readingPortBufferTemperature[port] >> 8 );
        return MPC_RW_OK;
      case 12:
        *pValue = (uint8_t)readingPortBufferTemperature[port];                      // return LSB of latched value
        return MPC_RW_OK;
      case 13:
        *pValue = (uint8_t) mpcInfo.nvm.overvoltageShutdownEvents;
        return MPC_RW_OK;
      case 14:
        *pValue = (uint8_t) mpcInfo.nvm.maxSupplyVoltage;
        return MPC_RW_OK;
      case 15:
        mpcInfo.readtotalOvervoltageDuration = mpcInfo.nvm.totalOvervoltageDuration;    // latch value, when reading MSB
        readingPortBufferOvervoltage[port] = mpcInfo.readtotalOvervoltageDuration;
        *pValue = (uint8_t)(readingPortBufferOvervoltage[port] >>8);               // return MSB of latched value
        return MPC_RW_OK;
      case 16:
        *pValue = (uint8_t)(readingPortBufferOvervoltage[port]);                   // return LSB of latched value
        return MPC_RW_OK;
      default:
        return MPC_RW_OUT_OF_RANGE;
  }
}


//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank "General Info Page".
///
/// \param address    The address in the selected memory bank.
/// \param value      The value to write.
/// \param channel    The device channel.
///
/// \retval      True if value was written.
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Info_SetValue_port( uint8_t address, uint8_t value, uint8_t channel, uint8_t port )
{
  if ( port >= AVAILABLE_PORTS_NUMBER )
  {
      return MPC_RW_OUT_OF_RANGE;
  }

  if ( (mpcInfo.lockByte[channel] != MPC_UNLOCK) && (address != 2))
  {
      return MPC_RW_DENY;
  }

  switch( address ) {
    case 2:                                                                         
      mpcInfo.lockByte[channel] = value;
      return MPC_RW_OK;
    case 5:                                                                         
      writingPortBufferCounter[channel][port] = (uint32_t)value <<16;
      return MPC_RW_OK;
    case 6:
      writingPortBufferCounter[channel][port] &= 0x00FF00FF;                                   // to avoid overrun ..
      writingPortBufferCounter[channel][port] += (uint32_t)value <<8;                          // .. by multiple additions
      return MPC_RW_OK;
    case 7:
      writingPortBufferCounter[channel][port] &= 0x00FFFF00;
      writingPortBufferCounter[channel][port] += value;
      mpcInfo.writeCounter[channel] = writingPortBufferCounter[channel][port];
      mpcInfo.nvm.lampCounter[channel].minutes = writingPortBufferCounter[channel][port];
      mpcInfo.nvm.lampCounter[channel].ticks = 0;
#ifdef MPC_CLM_NUMBER
      MemoryBank_Clm_UpdateAdjustFactor(mpcInfo.writeCounter[channel], channel);
#endif
      return MPC_RW_OK;
  default:
      return MPC_RW_OUT_OF_RANGE;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "General Info Page".
///
/// \param address    selected memory bank address
/// \param *pValue    memory address pointer to return value.
/// \param channel    device channel
///
/// \retval           'MPC_RW_OK' in case of a successful address access
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Info_GetValue( uint8_t address, uint8_t *pValue, uint8_t channel )
{
  return MemoryBank_Info_GetValue_port( address, pValue, channel, 0 );
}



//---------------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value to memory bank "General Info Page".
///
/// \param address    selected memory bank address
/// \param value      value to be written
/// \param channel    device channel
///
/// \retval           'MPC_RW_OK' if value was written
//---------------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Info_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
  return MemoryBank_Info_SetValue_port( address, value, channel, 0 );
}


mpc_rw_enum MemoryBank_Info_SetMinute_value(uint32_t time)

{
  mpcInfo.nvm.lampCounter[0].minutes=time;
  
  return MPC_RW_OK;
}
