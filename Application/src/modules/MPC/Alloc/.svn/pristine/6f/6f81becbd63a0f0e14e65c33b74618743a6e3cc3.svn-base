/******************** (C) COPYRIGHT 2015 - 2016 OSRAM AG **********************************************************************************
* File Name         : Mpc.c
* Description       : Providing read and write access to MultiPurposeCenters
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 11.12.2015 (JA)
* Modified          : Date Author Description
******************************************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#include "MpcDefs.h"
#include "DaliDevice_Mpc.h"
#include "MemoryBank.h"                                                         // interface to DALI-MemoryBanks ID=0 and ID=1
#include "MemoryBanksGeneric.h"                                                 // interface to OSRAM-MemoryBanks ID >=2
#include "MpcLegacy.h"

mpc_rw_enum Mpc_Write(uint16_t mpc_number, uint8_t address, uint8_t value) {
  mpc_rw_enum mpcWriteAck = MPC_RW_OUT_OF_RANGE;                                //
  uint8_t lockbyte;
  uint16_t temp16 = mpc_number;
  uint8_t bank = (uint8_t)temp16;
  uint8_t channel = (uint8_t)(temp16>>8);
  switch (bank) {
  case 0:
    break;
  case 1:
    MemoryBank_GetValue( 1, MBANK_LOCK_ADDR, &lockbyte, channel );
    MemoryBank_SetValue( 1, MBANK_LOCK_ADDR, MPC_UNLOCK, channel );             // unlock MPC for writing
    if (MemoryBank_SetValue( bank, address+MPC_ADDR_OFFSET, value, channel ) ) {
      mpcWriteAck = MPC_RW_OK;
    } // no else: set as default
    MemoryBank_SetValue( 1, MBANK_LOCK_ADDR, lockbyte, channel );               // restore lock-state of MPC
    break;
  case 255:
    mpcWriteAck = DaliDevice_WriteMpc(address, value, channel);
    break;
  default:
    MemoryBankGeneric_GetValue( bank, MBANK_LOCK_ADDR, &lockbyte, channel );
    MemoryBankGeneric_SetValue( bank, MBANK_LOCK_ADDR, MPC_UNLOCK, channel );   // unlock MPC for writing
    mpcWriteAck = MemoryBankGeneric_SetValue( bank, address + MPC_ADDR_OFFSET, value, channel );
    MemoryBankGeneric_SetValue( bank, MBANK_LOCK_ADDR, lockbyte, channel );     // restore lock-state of MPC
    break;
  }
  return mpcWriteAck;
}

mpc_rw_enum Mpc_Read(uint16_t mpc_number, uint8_t address, uint8_t *value) {
  mpc_rw_enum mpcRead = MPC_RW_OUT_OF_RANGE;
  uint16_t temp16 = mpc_number;
  uint8_t bank = (uint8_t)temp16;
  uint8_t channel = (uint8_t)(temp16>>8);
  switch (bank) {
  case 0:
  case 1:
    if (MemoryBank_GetValue( bank, address+MPC_ADDR_OFFSET, value, channel ) ) {
      mpcRead = MPC_RW_OK;
    } // no else: set as default
    break;
  case 255:
    mpcRead = DaliDevice_ReadMpc(address, value, channel);
    break;
  default:
    mpcRead = MemoryBankGeneric_GetValue( (uint8_t)mpc_number, address+MPC_ADDR_OFFSET, value, (uint8_t)(mpc_number>>8) );

  }
  return mpcRead;
}

uint8_t Mpc_GetLength(uint16_t mpc_number) {
  uint8_t length;
  mpc_rw_enum readResult;
  uint8_t bank = (uint8_t)mpc_number;

  switch (bank) {
  case 0:
  case 1:
    MemoryBank_GetValue( bank, MBANK_LENGTH_ADDR, &length, 0 );
    return (length+1 - MPC_ADDR_OFFSET);
  case 255:
    return DaliDevice_MpcLength();
  default:
    readResult = MemoryBankGeneric_GetValue( bank, MBANK_LENGTH_ADDR, &length, 0 );  // length is identical fo all channels, read always channel 0
    if (readResult == MPC_RW_OK)  { return (length+1 - MPC_ADDR_OFFSET);}
    else                          { return 0;}
  }
}
