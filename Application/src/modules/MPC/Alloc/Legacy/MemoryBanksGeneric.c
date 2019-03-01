// ---------------------------------------------------------------------------------------------------------------------
// MemoryBanksGeneric.c
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
// Initial version: Jutta Avril, 14.03.2014
//
// $Author: w.limmer $
// $Revision: 11112 $
// $Date: 2017-12-06 05:58:10 +0800 (Wed, 06 Dec 2017) $
// $Id: MemoryBanksGeneric.c 11112 2017-12-05 21:58:10Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/Legacy/MemoryBanksGeneric.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/// \file
/// \brief Implementation of memory banks > 1
///
///
/// \par Bank 2 ... n
/// All other banks are in seperate files. Only the init and the connection to the DALI read and write commands are in this file.

#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "MpcDefs.h"                          // only: use definition of DALI_FastTIME_TICK_ms, DALI_SlowTIME_TICK_ms

#include "Mpc.h"

#include "MemoryBanksGeneric.h"
#ifdef MPC_INFO_NUMBER
 #include "MemoryBank_Info.h"
#endif
#include "Global.h"

#ifdef MPC_PASSWORD2_NUMBER
STATIC_ASSERT( (MPC_PASSWORD1_NUMBER + 2) <= MPC_PASSWORD2_NUMBER, \
                "MPC_PASSWORD2_NUMBER must be greater or equal to MPC_PASSWORD1_NUMBER + 2 to protect at least 1 MPC");
#endif


//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the memory Banks
///
/// \param  none. All channels are initialised
///
/// \retval      Return true, the persistent data are changed, else the persistent data are unchanged.
//----------------------------------------------------------------------------------------------------------------------
void MemoryBankGeneric_Init( void )
{
  MpcInit();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Sets the reset values of the memory bank upon Dali-Reset (at least reset lockMpc).
/// \param bank        Selection of the bank to be reset (if bank = 0: reset all).
/// \param channel    The selected device channel.
/// \retval      none.
//----------------------------------------------------------------------------------------------------------------------
//
void MemoryBankGeneric_Reset( uint8_t bank_nr, uint8_t channel )
{
  uint8_t selectBank, firstBank, lastBank, lockMPC;

  if (bank_nr == 0) {
    firstBank = 2;
    lastBank = MPC_MAX_NUMBER;
  }
  else {
    firstBank = lastBank = bank_nr;
  }

  for (selectBank = firstBank; selectBank <= lastBank; selectBank++) {
    MpcReadDali(selectBank, 0x02, channel, &lockMPC);
    // A reset via DALI is allowed only when a Mpc is unlocked and not password protected
    if (lockMPC == 0x55) {
      // But skip extra password lock query here: By contract, the lockbyte always reads as 0x00
      // when a password lock is present for a Mpc: The if(lockMPC == 0x55) is sufficient
      MpcCmd (bank_nr, MPC_CMD_RESET, MPC_PORT_DALI, channel);
      MpcWriteDali(selectBank, 0x02, channel, 0xFF);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Reads a value from a DALI memory bank.
///
/// The address 0x00 contains the last accessible memory location and the address 0x01 the checksum of the memory bank.
/// Attention: recursive function! To calculate the checksum function calls itself.
///
/// \param bank      The memory bank number.
/// \param address    The address in the selected memory bank.
/// \param value    Pointer to a memory address to save the result.
/// \param channel    The device channel.
///
/// \retval      MPC_RW_OK = value read / MPC_RW_OUT_OF_RANGE.
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBankGeneric_GetValue( uint8_t bank, uint8_t address, uint8_t *pValue, uint8_t channel )
{
  mpc_returncodes_enum res = MpcReadDali(bank, address, channel, pValue);
  if (res > 0) {
    return MPC_RW_OK;
  }
  else {
    return MPC_RW_OUT_OF_RANGE;
  }
}



//----------------------------------------------------------------------------------------------------------------------
/// \brief Writes a value in a DALI memory bank.
///
/// The bank number in pDaliDevice->dtr1, address in pDaliDevice->dtr and channel in pDaliDevice->channel.
///
/// \param bank      The memory bank number.
/// \param address    The address in the selected memory bank.
/// \param value    The value to write.
/// \param channel    The device channel.
///
/// \retval      MPC_RW_OK = value written / MPC_RW_DENY = write not possible / MPC_RW_OUT_OF_RANGE..
//----------------------------------------------------------------------------------------------------------------------
mpc_rw_enum MemoryBankGeneric_SetValue( uint8_t bank, uint8_t address, uint8_t value, uint8_t channel )
{
  mpc_rw_enum mpcWrite = MPC_RW_OUT_OF_RANGE;
  mpc_returncodes_enum mpcNewWrite = MPC_OUT_OF_RANGE;

  mpcNewWrite = MpcWriteDali (bank, address, channel, value);

  if (mpcNewWrite >= 0) {
    mpcWrite = MPC_RW_OK;
  }else
  {
    mpcWrite = (mpcNewWrite == MPC_OK)?MPC_RW_OK:((mpcNewWrite == MPC_DENY)?  \
              MPC_RW_DENY:MPC_RW_OUT_OF_RANGE);
  }
  if (mpcWrite == MPC_RW_OK) {
    MpcIncChangeCounter();
  }
  return mpcWrite;
}



void MemoryBankGeneric_SlowTimer( uint8_t channels ) {
#ifdef MPC_PASSWORD1_NUMBER                                                     // MPC_PASSWORD2_NUMBER may be optionally defined in addition
  MemoryBank_Password_SlowTimer();
#endif
#ifdef MPC_INFO_NUMBER
  MemoryBank_Info_SlowTimer();
#endif
}
