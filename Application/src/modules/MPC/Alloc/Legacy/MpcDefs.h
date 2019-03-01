/******************** (C) COPYRIGHT 2012 - 2016 OSRAM AG **********************************************************************************
* File Name         : MpcDefs.h
* Description       : Typedefs for MemoryBanks / MPC based on C99 data types
* Author(s)         : Jutta Avril (JA), PDS D LMS-COM DE-2
* Created           : 12.12.2013 (JA)
* Modified by       : 10.12.2015 (JA)       revision of types, defines added
******************************************************************************************************************************************/

#ifndef _MPC_DEFS__H
#define _MPC_DEFS__H

#define MBANK_LENGTH_ADDR     0
#define MBANK_LOCK_ADDR       2
#define MPC_ID_ADDR           3
#define MPC_VERSION_ADDR      4

#define MPC_UNLOCK         0x55

#define MPC_ADDR_OFFSET       MPC_ID_ADDR
#define MPC_FastTIME_TICK_ms        (2.500)                                     // = DALI_FastTIME_TICK_ms if called from DaliBallastLibrary
#define MPC_SlowTIME_TICK_ms        (10.0*MPC_FastTIME_TICK_ms)                 // = DALI_SlowTIME_TICK_ms if called from DaliBallastLibrary

typedef enum {
  MPC_RW_OUT_OF_RANGE,    ///< The addressed location is not provided (either bank not existent or address out of range)
  MPC_RW_OK,              ///< The mpc r/w operation was done successfully
  MPC_RW_DENY             ///< The mpc r/w operation was denied (write: blocked or read-only, read: not replied)
} mpc_rw_enum;

#endif
