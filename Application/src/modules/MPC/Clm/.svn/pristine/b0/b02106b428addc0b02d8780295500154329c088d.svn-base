//----------------------------------------------------------------------------------------------------------------------
///   \file   MemoryBank_Clm.c
///   \since  2012-11-12
///   \brief  MultiPurposeCenter ContantLumenOutput (CLM). MPC-ID = 6. Specified by 2D1 2786696-01
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is confidential and not intended for public release.
///   All rights reserved. Copyright © Osram GmbH
//----------------------------------------------------------------------------------------------------------------------


#define MODULE_MPC
#define MODULE_CLM

#include "Global.h"
#include "MpcDefs.h"
#include "Config.h"                                                             // may define device-specific #defines for clm-table ..
#include "MemoryBank_Clm.h"                                                     // .. otherwise the default-#defines for clm-table are used, which are included here
#include "nvm.h"


//---   A d j u s t   s e t t i n g s   --------------------------------------------------------------------------------
#define MPC_CLM_ENABLE_FLAG           0x01
#define MPC_CLM_RELOAD_FACTORY_FLAG   0x80

#define MPC_CLM_TABLE_MaxFACTOR       (uint16_t)((MPC_CLM_100PERCENT_FACTOR * MPC_CLM_TABLE_MaxLEVEL)/100.0)
#define MPC_CLM_TABLE_MinFACTOR       (uint16_t)((MPC_CLM_100PERCENT_FACTOR * MPC_CLM_TABLE_MinLEVEL)/100.0)

#ifndef MPC_CLM_TABLE_ENABLE
  #define MPC_CLM_ENABLE              0                                         // default = disabled
#else
  #define MPC_CLM_ENABLE              MPC_CLM_ENABLE_FLAG
#endif

//---   C h e c k   o f   s e t t i n g s   ----------------------------------------------------------------------------
STATIC_ASSERT( (MPC_CLM_TABLE_ENTRIES == 4) || (MPC_CLM_TABLE_ENTRIES == 8), "MPC_CLM_TABLE_ENTRIES must be 4 or 8");
STATIC_ASSERT( MPC_CLM_TABLE_MinLEVEL <= MPC_CLM_TABLE_MaxLEVEL, "MPC_CLM_TABLE_MaxLEVEL less than MPC_CLM_TABLE_MinLEVEL");
STATIC_ASSERT( MPC_CLM_TABLE_MaxLEVEL <= 150, "MPC_CLM_TABLE_MaxLEVEL to big");
STATIC_ASSERT( MPC_CLM_TABLE_MinLEVEL >= 50, "MPC_CLM_TABLE_MinLEVEL to small");
STATIC_ASSERT( MPC_ConstantLumen_VERSION == 3, "Wrong version number in header");

//---   D a t a   s t r u c t u r e   ----------------------------------------------------------------------------------
typedef struct {
  uint8_t khours;
  uint8_t adjustFactor;
} TypeClmTable;

typedef struct {
  uint8_t enableClm;
  TypeClmTable table[MPC_CLM_TABLE_ENTRIES];
} TypeClmMpcNvm;

typedef struct {
  TypeClmMpcNvm nvm;
  uint8_t lockByte;                                                             ///< \brief The lock byte of the memory bank is stored in RAM
  uint32_t actTime_mins;
  uint16_t actAdjustFactor;
} TypeMemoryBank_Clm;

TypeMemoryBank_Clm memoryBank_Clm[DEVICE_CHANNELS];
uint16_t  ClmCalcAdjustlevel (uint32_t runTime_mins, TypeClmTable *pClmTable);


//----------------------------------------------------------------------------------------------------------------------
/// \brief Factory reset values (const array in flash memory) for a CLM memory bank
const TypeClmMpcNvm clmMpcNvmDefault = {
  .enableClm = MPC_CLM_ENABLE,
  .table[0].khours = MPC_CLM_TABLE_0_kHOURS,
  .table[0].adjustFactor = MPC_CLM_TABLE_0_FACTOR,
  .table[1].khours = MPC_CLM_TABLE_1_kHOURS,
  .table[1].adjustFactor = MPC_CLM_TABLE_1_FACTOR,
  .table[2].khours = MPC_CLM_TABLE_2_kHOURS,
  .table[2].adjustFactor = MPC_CLM_TABLE_2_FACTOR,
  .table[3].khours = MPC_CLM_TABLE_3_kHOURS,
  .table[3].adjustFactor = MPC_CLM_TABLE_3_FACTOR,
  #if MPC_CLM_TABLE_ENTRIES == 8
   .table[4].khours = MPC_CLM_TABLE_4_kHOURS,
   .table[4].adjustFactor = MPC_CLM_TABLE_4_FACTOR,
   .table[5].khours = MPC_CLM_TABLE_5_kHOURS,
   .table[5].adjustFactor = MPC_CLM_TABLE_5_FACTOR,
   .table[6].khours = MPC_CLM_TABLE_6_kHOURS,
   .table[6].adjustFactor = MPC_CLM_TABLE_6_FACTOR,
   .table[7].khours = MPC_CLM_TABLE_7_kHOURS,
   .table[7].adjustFactor = MPC_CLM_TABLE_7_FACTOR,
  #endif
};


//----------------------------------------------------------------------------------------------------------------------
/// \brief    Initialise and reset (in case of MPC_CLM_RELOAD_FACTORY_FLAG is set) the memory Bank and the clm-factor.
/// \param    runTime_mins    time to calculate the clm-factor
/// \param    channel         The device channel.
/// \retval   none
//----------------------------------------------------------------------------------------------------------------------
void MemoryBank_Clm_Init( uint32_t runTime_mins, uint8_t channel )
{
  TypeMemoryBank_Clm *pClmMPC = &memoryBank_Clm[channel];
  nvm_init_t nvm_init_state =  nvmDataInit ( (void *)(&(pClmMPC->nvm)), sizeof(TypeClmMpcNvm), NVM_MEM_CLASS_0);
  if ( ( nvm_init_state != NVM_INIT_OK) || ((pClmMPC->nvm.enableClm & MPC_CLM_RELOAD_FACTORY_FLAG) !=0 ) ) {
    pClmMPC->nvm = clmMpcNvmDefault;
  }

  memoryBank_Clm[channel].lockByte = 255;                                       // init RAM-value
  pClmMPC->actTime_mins = runTime_mins;                                         // store time from InfoMPC to enable update upon parameter change
  if ( (pClmMPC->nvm.enableClm & MPC_CLM_ENABLE_FLAG) != 0 ) {
    pClmMPC->actAdjustFactor = ClmCalcAdjustlevel(runTime_mins, pClmMPC->nvm.table);
  }
  else {
    pClmMPC->actAdjustFactor = MPC_CLM_100PERCENT_FACTOR;                      // set to 100 % = 2^14
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Bank.
///
/// \todo Clarify relation of this function to MemoryBank_Clm_FactoryReset()
///
/// \param channel    The device channel.
/// \retval      none
//----------------------------------------------------------------------------------------------------------------------
void MemoryBank_Clm_Reset( uint8_t channel )                                       // reset only MPC variables
{
  // no variables of MPC have to be reset
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Factory reset (all channels) of CLM memory bank(s)
///
/// Resets all variables stored in nvm memory to the defaults as defined during the compilation. For a
/// full reset, also a power-off and power-on cycle is needed to reset the ram variables too.
///
/// Purpose of this function:
/// - Reset a ballasts returning from the field to allow that it can be sold again
/// - Full reset of a ballast in regression tests (separate test cases from each other)
///
/// \todo Clarify relation of this function to MemoryBank_Clm_Reset(ch)
//----------------------------------------------------------------------------------------------------------------------
void MemoryBank_Clm_FactoryReset( void )
{
  for (int i = 0; i < DEVICE_CHANNELS; i++) {
    memoryBank_Clm[i].nvm = clmMpcNvmDefault;
  }
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from the memory bank "CLM Page".
///
/// The data will be collected from the real device.
/// The checksum will be handle in MemoryBank.c
///
/// \param address    The address in the selected memory bank.
/// \param value    Pointer to a memory address to save the result.
/// \param channel    The device channel.
///
/// \retval      True if successful and value valid.
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Clm_GetValue( uint8_t address, uint8_t *value, uint8_t channel )
{
  switch( address ) {
  case 0:                                                                       // last addressable memory bank location
    *value = MPC_ConstantLumen_LastADDRESS;
    return MPC_RW_OK;
  case 1:                                                                       // return indicator byte
    *value = 0;
    return MPC_RW_OK;
  case 2:                                                                       // lock byte
    *value = memoryBank_Clm[channel].lockByte;
    return MPC_RW_OK;
  case 3:                                                                       // identification number of the multipurpose center
    *value = MPC_ConstantLumen_ID;
    return MPC_RW_OK;
  case 4:                                                                       // version of the multipurpose center implementation
    *value = MPC_ConstantLumen_VERSION;
    return MPC_RW_OK;
  case 5:                                                                       //version of the multipurpose center implementation
    *value = memoryBank_Clm[channel].nvm.enableClm;
    return MPC_RW_OK;

  case 6:
  case 8:
  case 0x0A:
  case 0x0C:
#if MPC_CLM_TABLE_ENTRIES == 8
  case 0x0E:
  case 0x10:
  case 0x12:
  case 0x14:
#endif
    *value = memoryBank_Clm[channel].nvm.table[(address-6)>>1].khours;
    return MPC_RW_OK;

  case 7:
  case 9:
  case 0x0B:
  case 0x0D:
#if MPC_CLM_TABLE_ENTRIES == 8
  case 0x0F:
  case 0x11:
  case 0x13:
  case 0x15:
#endif
    *value = memoryBank_Clm[channel].nvm.table[(address-6)>>1].adjustFactor;
    return MPC_RW_OK;
  default:
    return MPC_RW_OUT_OF_RANGE;                                                 // ignore all others
  }
}



//----------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a the memory bank "CLM Page".
///
/// The bank number in pDaliDevice->dtr1, address in pDaliDevice->dtr and channel in pDaliDevice->channel.
///
/// \param address    The address in the selected memory bank.
/// \param value    The value to write.
/// \param channel    The device channel.
///
/// \retval      True if value was written.
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBank_Clm_SetValue( uint8_t address, uint8_t value, uint8_t channel )
{
  TypeMemoryBank_Clm* pClmMPC = &memoryBank_Clm[channel];
  if (address == 2) {                                                           // no check of lock byte for 1 parameter
    pClmMPC->lockByte = value;                                                  // write RAM-value
    return MPC_RW_OK;                                                           // leave after update
  }
  else {
    if( pClmMPC->lockByte != MPC_UNLOCK ) {                                     // if mpc is locked ..
      return MPC_RW_DENY;                                                       // .. leave without change
    }
    else {                                                                      // else (i.e. if mpc is un-locked): change value
      switch( address ) {
      case 5:
        pClmMPC->nvm.enableClm = value & (MPC_CLM_ENABLE_FLAG|MPC_CLM_RELOAD_FACTORY_FLAG);
        break;                                                                  // continue with update of actAdjustFactor at any change after switch
      case 6:
      case 8:
      case 0x0A:
      case 0x0C:
#if MPC_CLM_TABLE_ENTRIES == 8
      case 0x0E:
      case 0x10:
      case 0x12:
      case 0x14:
#endif
        pClmMPC->nvm.table[(address-6)>>1].khours = value;                       // time value at even addresses
        break;                                                                  // continue

      case 7:
      case 9:
      case 0x0B:
      case 0x0D:
#if MPC_CLM_TABLE_ENTRIES == 8
      case 0x0F:
      case 0x11:
      case 0x13:
      case 0x15:
#endif
        pClmMPC->nvm.table[(address-6)>>1].adjustFactor = value;                 // adjustFactors at odd addresses
        break;                                                                  // continue
      default:
        return MPC_RW_OUT_OF_RANGE;                                             // leave without change
      }
      if ( (pClmMPC->nvm.enableClm & MPC_CLM_ENABLE_FLAG) != 0 ) {
        pClmMPC->actAdjustFactor = ClmCalcAdjustlevel(pClmMPC->actTime_mins, pClmMPC->nvm.table);
      }
      else {
        pClmMPC->actAdjustFactor = MPC_CLM_100PERCENT_FACTOR;
      }
      return MPC_RW_OK;                                                         // leave after update
    }
  }
}


void MemoryBank_Clm_UpdateAdjustFactor (uint32_t runTime_mins, uint8_t channel) {
  memoryBank_Clm[channel].actTime_mins = runTime_mins;                          // store to RAM, needed if update of CLM-table
  if ( (memoryBank_Clm[channel].nvm.enableClm & MPC_CLM_ENABLE_FLAG) != 0 ) {
    memoryBank_Clm[channel].actAdjustFactor = ClmCalcAdjustlevel(runTime_mins, memoryBank_Clm[channel].nvm.table);
  } // no else: actAdjustFactor is ignored in ConstantLumen_GetFactor, if CLM is disabled
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief    Returns the pre-calculated CLM-factor, if CLM is enabled. Else return 100 %
/// \param    channel    The device channel.
///
/// \retval   factor in resolution 100 % = 2^14
//----------------------------------------------------------------------------------------------------------------------
uint16_t ConstantLumen_GetFactor (uint8_t channel) {
  if ( memoryBank_Clm[channel].nvm.enableClm & MPC_CLM_ENABLE_FLAG ) {
    return memoryBank_Clm[channel].actAdjustFactor;                             // return pre-calulated factor ..
  }
  else {
    return MPC_CLM_100PERCENT_FACTOR;                                           // .. or 100 %
  }
}

//------------- " Private functions
//----------------------------------------------------------------------------------------------------------------------
/// \brief get an adjustment level for given operation hours from the "CLM Page".
//
//----------------------------------------------------------------------------------------------------------------------
//
uint16_t ClmCalcAdjustlevel(uint32_t runTime_mins, TypeClmTable *pClmTable){
  uint16_t returnFactor = MPC_CLM_100PERCENT_FACTOR;                            // default: set to 100% (= return value, if disabled by table(!) values)
  uint8_t adjustFactorLeft, adjustFactorRight;
  uint32_t timeLeft_mins, timeRight_mins;
  const uint16_t scaleLevel = (uint16_t)164;                                    // scale 100 % to 2^14 (correct value 163.84, leads to deviation of < +1/1000

  if ( runTime_mins >= 0xFFFFFF) { return returnFactor; }                       // lampOn counter not active: set factor = 100 %

  adjustFactorRight = pClmTable[0].adjustFactor;
  if ( (pClmTable[0].khours != 0xFF) && (adjustFactorRight != 0xFF) )   {       // calculate adjustFactor, if ContantLumenMPC is NOT disabled by table(!) values
    timeRight_mins = (uint32_t)pClmTable[0].khours * (uint32_t)60000;               // start value for timeRight_mins
    if (runTime_mins < timeRight_mins) {                                        // if time at first entry != 0 ..
      returnFactor = (uint16_t)adjustFactorRight * scaleLevel;                  // .. [returnFactor]16bit = [adjustFactor]8bit * [scaleLevel]8bit. Limitation done before return!
    }
    else {
      uint8_t index = 0;
      uint8_t khours;
      bool endOfTable = false;
      uint32_t diffActTime;

      do {
        khours = pClmTable[++index].khours;                                     // check next entry
        adjustFactorLeft = adjustFactorRight;
        adjustFactorRight = pClmTable[index].adjustFactor;
        if ((khours == 255) || (adjustFactorRight == 255)
            || (khours <= pClmTable[index-1].khours) || (index >= MPC_CLM_TABLE_ENTRIES)
            || (adjustFactorLeft > adjustFactorRight) ) {                       // According to spec. (interpolation is tested for "adjustFactorLeft > adjustFactorRight" too)
          endOfTable = true;
        }
        else {
          timeLeft_mins = timeRight_mins;
          timeRight_mins = (uint32_t)khours * (uint32_t)60000;
        }
      }
      while ( (runTime_mins > timeRight_mins) && (endOfTable == false) );

      if (endOfTable) {                                                         // use last entry as constant factor for the rest of live-time
        returnFactor = ((uint16_t)(adjustFactorLeft) * scaleLevel);
      }
      else {
        uint32_t diffTableTime;
        uint8_t i;

        uint8_t tempFactor;
        uint32_t dividend, divisor, result =0, factor32;
        int32_t diff;

        diffActTime = runTime_mins - timeLeft_mins;
        if (adjustFactorLeft > MPC_CLM_TABLE_MaxLEVEL) {adjustFactorLeft = MPC_CLM_TABLE_MaxLEVEL;}
        if (adjustFactorRight > MPC_CLM_TABLE_MaxLEVEL) {adjustFactorRight = MPC_CLM_TABLE_MaxLEVEL;}
        if (adjustFactorLeft < MPC_CLM_TABLE_MinLEVEL) {adjustFactorLeft = MPC_CLM_TABLE_MinLEVEL;}
        if (adjustFactorRight < MPC_CLM_TABLE_MinLEVEL) {adjustFactorRight = MPC_CLM_TABLE_MinLEVEL;}
        if (adjustFactorRight >= adjustFactorLeft) {                            // factor is increasing? (stay positive)
          tempFactor = adjustFactorRight - adjustFactorLeft;
        }
        else {
          tempFactor = adjustFactorLeft - adjustFactorRight;                    // could never be reached: endOfTable = true in this case
        }
        dividend = diffActTime * tempFactor;                                    // [dividend]32bit = diffActTime]24bit * [tempFactor]8bit
        if (dividend == 0) {
          returnFactor = ((uint16_t)(adjustFactorLeft) * scaleLevel);
        }
        else {
          diffTableTime = timeRight_mins - timeLeft_mins;
          divisor = diffTableTime <<8;                                          // [divisor]32bit = [diffTableTime]24bit <<8. Never zero (guarantied in while-loop)
          for (i=14; i>0; i--) {                                                // accuracy of 11 bit is sufficient
            divisor >>= 1;                                                      // always: divisor > dividend, make 1st shift first
            result <<= 1;
            diff = dividend - divisor;
            if (diff >= 0) {
              dividend = diff;
              result++;
            }
          }
          result <<=2;
          factor32 = (uint32_t)(adjustFactorLeft) <<8;
          if (adjustFactorRight > adjustFactorLeft) {                           //
            factor32 += result;
          }
          else {
            factor32 -= result;
          }
          factor32 *= scaleLevel;                                               // [factor32]24bit = [factor32]16bit * [scaleLevel]8
          returnFactor = (uint16_t)(factor32 >>8);
        } // else to if (dividend == 0)
      } // else to if: ((endOfTable = false) || (actDiffTime <= 0))
    } // not before 1st entry
    if (returnFactor > MPC_CLM_TABLE_MaxFACTOR) {returnFactor = MPC_CLM_TABLE_MaxFACTOR;}
    if (returnFactor < MPC_CLM_TABLE_MinFACTOR) {returnFactor = MPC_CLM_TABLE_MinFACTOR;}
  } // no else: return default
  return returnFactor;
}
