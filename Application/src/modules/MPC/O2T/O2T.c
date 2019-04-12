// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EC - MCC
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM, BU DS. It is confidential and not
// intended for any public release. All rights reserved.
//
// Within the company the policy of a strict "who needs to know?" applies: Store this only at locations,
// where EXCLUSIVELY people who really NEED it for their DAY-TO-DAY business can access.
//
//  - Convenience or to save it for occasion are no valid arguments. Get a fresh and updated copy when needed.
//  - Neither "departments shares" nor other potential local practices normally fulfill this policy.
//  - Please, especially be careful about backups, they often are forgotten and an usual origin of diffusion.
//  - Also be careful with code coverage reporting tools: They normally include the full sources, but the
//    results often are published widely.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Change history, major milestones and credits:
//   2018-12: Filippo Branchetti, Martin Hutzl
//
//
// $Author$
// $Revision$
// $Date$
// $Id$
// $URL$
//
#define MODULE_O2T
#define MODULE_MPC
#include <string.h>
#include "Global.h"
#include "Config.h"
#include "AclDefaults.h"
#include "MpcInternal.h"
#include "Acl.h"
#include "Mpc.h"
#include "nvm.h"
#include "O2T.h"
#include "O2T_private.h"

/* Warning  ------------------------------------------------------------------*/
#warning message( "Do Not use this version: Module under construction ")
#warning message( " Ask for released and tagged version  ")

/* Private makro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
zero2ten_config_t o2t;
/* Public variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private constants ---------------------------------------------------------*/

// ---------------------------------------------------------------------------------------------------------------------
// This table is auto-generated by TabTools; Please don't edit manually, all changes will be discarded by next make

static const uint32_t o2t_constants[] = {
    [  0] = (uint32_t) &o2t.nvm.enable,                                 // used for entity   0   VAR O2TENABLE
    [  1] = (uint32_t) (ACL_PERM_ALWAYS),                               // used for permissions Rd000 Wr000 Rd001 Wr001 Rd002 Wr002 Rd003 Wr003 Rd004 Wr004 Rd005 Wr005 Rd006 Wr006 Rd007 Wr007 Rd008 Wr008
    [  2] = (uint32_t) (ACL_PERM_NEVER),                                // used for permissions
    [  3] = (uint32_t) (0xffffffff),                                    // used for fallbacks En000 En001 En002 En003 En004 En005 En006 En007 En008
    [  4] = (uint32_t) &o2t.nvm.minDimLevel,                            // used for entity   1   VAR MINDIMLEVEL
};


// ---------------------------------------------------------------------------------------------------------------------
// This table is auto-generated by TabTools; Please don't edit manually, all changes will be discarded by next make

static const mpc_data_entity_t o2t_entities[] = {
    [  0] = MPC_ENTITY( MPC_TYP_VAR, 1,  1,  1,  3,   0,  0),           // D2WENABLE
    [  1] = MPC_ENTITY( MPC_TYP_VAR, 2,  1,  1,  3,   4,  0),           // MINDIMLEVEL
};


// ---------------------------------------------------------------------------------------------------------------------
// This table is auto-generated by TabTools; Please don't edit manually, all changes will be discarded by next make

static const mpc_layout_t o2t_layout[] = {
    [  0] = MPC_LAYOUT8                              (   0 ),           // O2TENABLE
    [  1] = MPC_LAYOUT16_MSB                         (   1 ),           // MINDIMLEVEL
    [  2] = MPC_LAYOUT16_LSB                         (   1 ),           // MINDIMLEVEL
};


//------------------------------------------------------------------------------
/// \brief Data stored in NVM
//------------------------------------------------------------------------------
STATIC void InitNvmData ( void )
{
  memset( &o2t.nvm, 0x00, sizeof( o2t.nvm ) );
  o2t.nvm.enable = MPC_O2T_CONFIG_ENABLED;
  o2t.nvm.minDimLevel = MPC_O2T_CONFIG_MINDIMLEVEL;
}

//------------------------------------------------------------------------------
/// \brief Data stored in RAM
//------------------------------------------------------------------------------
STATIC void InitRamData ( void )
{
  // memset( &d2w.ram, 0x00, sizeof( d2w.ram ) );
}

//------------------------------------------------------------------------------
/// \brief Inits data after power up
//------------------------------------------------------------------------------
STATIC void PageInit ( void ) {
  if ( nvmDataInit(&o2t.nvm ,sizeof( o2t.nvm  ),NVM_MEM_CLASS_0) != NVM_INIT_OK ) // Normal startup -> init if flash corrupted
  {
    InitNvmData();
  }
  InitRamData();
}

//------------------------------------------------------------------------------
/// \brief Executed when CMD == MPC_CMD_RESET
//------------------------------------------------------------------------------
STATIC void PageReset (void) {
  InitRamData();
}

//------------------------------------------------------------------------------
/// \brief Executed when CMD == MPC_CMD_FACTORY_RESET
//------------------------------------------------------------------------------
STATIC void PageFactoryReset ( void ) {

  InitNvmData();
  InitRamData();
}

//------------------------------------------------------------------------------
/// \brief CMD D2W MPC
//------------------------------------------------------------------------------
STATIC mpc_returncodes_enum O2T_command(mpc_cmd_t cmd, uint8_t channel, mpc_port_t port) {
  (void) cmd;
  (void) channel;
  (void) port;

  if ( cmd == MPC_CMD_INIT )
  {
    PageInit();
  }
  else if ( cmd == MPC_CMD_RESET ) {
    PageReset();
  }
  else if ( cmd == MPC_CMD_FACTORY_RESET ) {
    PageFactoryReset();
  }
  return MPC_OK;
};

//------------------------------------------------------------------------------
/// \brief Page Description
//------------------------------------------------------------------------------
const mpc_descriptor_t O2T_Descriptor = {
  .keydata            = {
    .id             = O2T_MPC_ID,
    .version        = O2T_VERSION_MAJOR,
    .attributes     = 0,
    .payload_off    = 5,
    .length         = 3,
  },
  .tables_len   = (sizeof(o2t_layout) / sizeof(mpc_layout_t)),
  .entities     = o2t_entities,
  .constants    = o2t_constants,
  .layout       = o2t_layout,
  .Cmd          = &O2T_command,
};

// ----------------------------------------------------------------------------------
/// \brief Getter function for O2T configuration
//-----------------------------------------------------------------------------------
uint8_t O2T_GetEnableConfig( void ) {
  return ( o2t.nvm.enable );
}

// ----------------------------------------------------------------------------------
/// \brief Getter function for O2T configuration
//-----------------------------------------------------------------------------------
uint16_t O2T_GetMinDimLevel( void ) {
  if ( o2t.nvm.minDimLevel > MPC_O2T_CONFIG_MINDIMLEVEL_MAX ) {
    o2t.nvm.minDimLevel = MPC_O2T_CONFIG_MINDIMLEVEL_MAX;
  }
  else if ( o2t.nvm.minDimLevel < MPC_O2T_CONFIG_MINDIMLEVEL_MIN ) {
    o2t.nvm.minDimLevel = MPC_O2T_CONFIG_MINDIMLEVEL_MIN;
  }
  return ( o2t.nvm.minDimLevel );
}