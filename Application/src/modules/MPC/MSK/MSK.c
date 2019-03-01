// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EC - MCC
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Initial version: 2017-01, Olaf Busse, o.busse@osram.com
//
// Change History:
//
// $Author: w.limmer $
// $Revision: 11458 $
// $Date: 2018-01-12 23:48:33 +0800 (Fri, 12 Jan 2018) $
// $Id: MSK.c 11458 2018-01-12 15:48:33Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/MSK/releases/Rel.%201.0.0/Src/MSK.c $
//

#include "Global.h"


#include "Acl.h"
#include "AclDefaults.h"
#include "nvm.h"
#include "ToolboxServices.h"
#include "Mpc.h"
#include "MpcInternal.h"
#include "MpcAllocation.h"
#include "MSK.h"
#include "MSK_local.h"
#include "MskEncryption.h"
#include "MSK_internal.h"


STATIC MSK_record_t MSK_data;
STATIC uint32_t newMasterPwTemp;
STATIC uint32_t newServicePwTemp;

static uint32_t MSK_CorrectResponse[2];
static uint32_t MSK_Response[2];

// ---------------------------------------------------------------------------------------------------------------------
/** \brief table for MPCgen
 *
 * one descriptor and 3 tables
 *
 */
// ---------------------------------------------------------------------------------------------------------------------
/** \brief 32 bit constants for this Mpc.
 *
 * An efficient storage for all constants, which are sometimes used many times.
 *
 * Possible contents:
 *  - values / data constants
 *  - permissions
 *  - pointers to variables
 *  - pointers to functions
 *
 * \note  Don't store permissions and fallback values at position 0: Index 0 is used for "n/a"
 *
 * \warning  It's very error prone to edit this table by hand. Especially when function pointers!
 *           Be very careful and try to avoid editing by hand: In best case it's created by a script
 *           translating the data model described by T4T XML files (tbd).
 */
#define MSK_MPC_TABLE_ID            27
#define MSK_MPC_TABLE_VER            1

// Inidices in the constants array
#define _P_NEVER      1
#define _P_OSRAM      2
#define _P_MASTER     3
#define _P_ALWAYS     4

#define _FFFF         5

#define _RD_MPW       6
#define _RD_NEW_MPW   8
#define _RD_NEW_MPWC 10
#define _M_STAT      12
#define _M_UNLOCK    13
#define _M_ERROR     14
#define _RD_SPW      15
#define _RD_SPWC     17

#define _S_STAT      19
#define _S_UNLOCK    20
#define _S_ERROR     21

#define _RD_PERM_U   22
#define _RD_PERM_S   24
#define _RD_UUID     26
#define _RD_SRR      27


STATIC int mskWriteMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint32_t mskReadMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWriteNewMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint32_t mskReadNewMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWriteNewMasterPasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint8_t mskReadNewMasterPasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWriteServicePassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint32_t mskReadServicePassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWriteServicePasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint8_t mskReadServicePasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWritePermissionUser(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint32_t mskReadPermissionUser(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);

STATIC int mskWritePermissionService(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);
STATIC uint32_t mskReadPermissionService(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel);


STATIC void mskFactoryReset( void );
static void mskReset( void );
static void mskPageInit(void);
static void bioPageInit(void);
static void mskAUTHInit(void);


static const MSK_NVM_record_t mskNVMData_Default= {
    .masterKey        = 0x00000000,
    .serviceKey       = 0x00000000,
    .masterUnlockCnt  = 0,
    .masterErrorCnt   = 0,
    .serviceUnlockCnt = 0,
    .serviceErrorCnt  = 0,
    .permUser         = 0xFFFFFFFF,
    .permService      = 0xFFFFFFFF
};


static const uint32_t MSK_constants[] = {
  [ 0] = 0,
  [ 1] = ACL_PERM_NEVER,  //0
  [ 2] = ACL_PERM_OSRAM,  //1
  [ 3] = ACL_PERM_MASTER, //6
  [ 4] = ACL_PERM_ALWAYS, //7
  [ 5] = 0xFFFFFFFF,
  [ 6] = (uint32_t)&mskReadMasterPassword,
  [ 7] = (uint32_t)&mskWriteMasterPassword,
  [ 8] = (uint32_t)&mskReadNewMasterPassword,
  [ 9] = (uint32_t)&mskWriteNewMasterPassword,
  [10] = (uint32_t)&mskReadNewMasterPasswordConfirmation,
  [11] = (uint32_t)&mskWriteNewMasterPasswordConfirmation,
  [12] = (uint32_t)&MSK_data.masterStatus,
  [13] = (uint32_t)&MSK_data.nvm_data.masterUnlockCnt,
  [14] = (uint32_t)&MSK_data.nvm_data.masterErrorCnt,
  [15] = (uint32_t)&mskReadServicePassword,
  [16] = (uint32_t)&mskWriteServicePassword,
  [17] = (uint32_t)&mskReadServicePasswordConfirmation,
  [18] = (uint32_t)&mskWriteServicePasswordConfirmation,
  [19] = (uint32_t)&MSK_data.serviceStatus,
  [20] = (uint32_t)&MSK_data.nvm_data.serviceUnlockCnt,
  [21] = (uint32_t)&MSK_data.nvm_data.serviceErrorCnt,
  [22] = (uint32_t)&mskReadPermissionUser,
  [23] = (uint32_t)&mskWritePermissionUser,
  [24] = (uint32_t)&mskReadPermissionService,
  [25] = (uint32_t)&mskWritePermissionService,
  [26] = (uint32_t)&MSK_data.uuid32,
  [27] = (uint32_t)&MSK_data.svn_rev_root
};


static const mpc_data_entity_t MSK_entities[] = {
  //                  type     ,len,    perm_r,    perm_w, fback,  data rdidx, multichannel
  [ 0] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_ALWAYS, _FFFF,     _RD_MPW,           0 ), // MasterPwd
  [ 1] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_MASTER, _FFFF, _RD_NEW_MPW,           0 ), // NewMasterPwd
  [ 2] = MPC_ENTITY(MPC_TYP_FUN, 1 , _P_ALWAYS, _P_MASTER, _FFFF,_RD_NEW_MPWC,           0 ), // MasterPwdConfirm
  [ 3] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_NEVER, _FFFF,     _M_STAT,           0 ), // MasterPwdStatusInfo
  [ 4] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_OSRAM, _FFFF,   _M_UNLOCK,           0 ),  // MasterPwdUnlockCounter
  [ 5] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_OSRAM, _FFFF,    _M_ERROR,           0 ),  // MasterPwdErrorCounter

  [ 6] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_ALWAYS, _FFFF,     _RD_SPW,           0 ),  // ServicePwd
  [ 7] = MPC_ENTITY(MPC_TYP_FUN, 1 , _P_ALWAYS, _P_MASTER, _FFFF,    _RD_SPWC,           0 ),  // ServicePwdConfirm
  [ 8] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_NEVER, _FFFF,     _S_STAT,           0 ),  // ServicePwdStatusInfo
  [ 9] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_OSRAM, _FFFF,   _S_UNLOCK,           0 ),  // ServicePwdUnlockCounter
  [10] = MPC_ENTITY(MPC_TYP_VAR, 1 , _P_ALWAYS,  _P_OSRAM, _FFFF,    _S_ERROR,           0 ),  // ServicePwdErrorCounter

  [11] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_MASTER, _FFFF,  _RD_PERM_U,           0 ),  // PermUser
  [12] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_MASTER, _FFFF,  _RD_PERM_S,           0 ),  // PermService

  [13] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_NEVER, _FFFF,     _RD_MPW,           0 ),  // MasterPwd encrypted RO
  [14] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_NEVER, _FFFF,     _RD_SPW,           0 ),  // ServicePwd encrypted RO
  [15] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_NEVER, _FFFF,  _RD_PERM_U,           0 ),  // PermUser RO
  [16] = MPC_ENTITY(MPC_TYP_FUN, 4 , _P_ALWAYS, _P_NEVER, _FFFF,  _RD_PERM_S,           0 ),  // PermService RO
  [17] = MPC_ENTITY(MPC_TYP_VAR, 4 , _P_ALWAYS,  _P_NEVER, _FFFF,    _RD_UUID,          0 ),  // UUID32 RO
  [18] = MPC_ENTITY(MPC_TYP_VAR, 4 , _P_ALWAYS,  _P_NEVER, _FFFF,    _RD_SRR,           0 ),  // UUID32 RO
};


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Memory layout for read access of MSK Page.
 *
 * Define all accessible addresses here.
 * Using the macros defined for this purpose.
 */
static const mpc_layout_t MSK_layout[] = {
  [ 0] = MPC_LAYOUT32_MSB           (  0 ),     // MasterPwd
  [ 1] = MPC_LAYOUT32_M2SB          (  0 ),     // MasterPwd
  [ 2] = MPC_LAYOUT32_L2SB          (  0 ),     // MasterPwd
  [ 3] = MPC_LAYOUT32_LSB           (  0 ),     // MasterPwd
  [ 4] = MPC_LAYOUT32_MSB           (  1 ),     // NewMasterPwd
  [ 5] = MPC_LAYOUT32_M2SB          (  1 ),     // NewMasterPwd
  [ 6] = MPC_LAYOUT32_L2SB          (  1 ),     // NewMasterPwd
  [ 7] = MPC_LAYOUT32_LSB           (  1 ),     // NewMasterPwd
  [ 8] = MPC_LAYOUT8                (  2 ),     // MasterPwdConfirm
  [ 9] = MPC_LAYOUT8                (  3 ),     // MasterPwdStatusInfo
  [10] = MPC_LAYOUT8                (  4 ),     // MasterPwdUnlockCounter
  [11] = MPC_LAYOUT8                (  5 ),     // MasterPwdErrorCounter
  [12] = MPC_LAYOUT32_MSB           (  6 ),     // ServicePwd
  [13] = MPC_LAYOUT32_M2SB          (  6 ),     // ServicePwd
  [14] = MPC_LAYOUT32_L2SB          (  6 ),     // ServicePwd
  [15] = MPC_LAYOUT32_LSB           (  6 ),     // ServicePwd
  [16] = MPC_LAYOUT8                (  7 ),     // ServicePwdConfirm
  [17] = MPC_LAYOUT8                (  8 ),     // ServicePwdStatusInfo
  [18] = MPC_LAYOUT8                (  9 ),     // ServicePwdUnlockCounter
  [19] = MPC_LAYOUT8                ( 10 ),     // ServicePwdErrorCounter
  [20] = MPC_LAYOUT32_MSB           ( 11 ),     // PermUser
  [21] = MPC_LAYOUT32_M2SB          ( 11 ),     // PermUser
  [22] = MPC_LAYOUT32_L2SB          ( 11 ),     // PermUser
  [23] = MPC_LAYOUT32_LSB           ( 11 ),     // PermUser
  [24] = MPC_LAYOUT32_MSB           ( 12 ),     // PermService
  [25] = MPC_LAYOUT32_M2SB          ( 12 ),     // PermService
  [26] = MPC_LAYOUT32_L2SB          ( 12 ),     // PermService
  [27] = MPC_LAYOUT32_LSB           ( 12 ),     // PermService
};


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Memory layout for read access of MSK ENC Page.
 *
 * Define all accessible addresses here.
 * Using the macros defined for this purpose.
 */

static const mpc_layout_t BIO_layout[] = {
  [  0] = MPC_LAYOUT32_MSB          (  13 ),    // MasterPwd
  [  1] = MPC_LAYOUT32_M2SB         (  13 ),    // MasterPwd
  [  2] = MPC_LAYOUT32_L2SB         (  13 ),    // MasterPwd
  [  3] = MPC_LAYOUT32_LSB          (  13 ),    // MasterPwd
  [  4] = MPC_LAYOUT32_MSB          (  14 ),    // ServicePwd
  [  5] = MPC_LAYOUT32_M2SB         (  14 ),    // ServicePwd
  [  6] = MPC_LAYOUT32_L2SB         (  14 ),    // ServicePwd
  [  7] = MPC_LAYOUT32_LSB          (  14 ),    // ServicePwd
  [  8] = MPC_LAYOUT32_MSB          (  15 ),    // PermUser
  [  9] = MPC_LAYOUT32_M2SB         (  15 ),    // PermUser
  [ 10] = MPC_LAYOUT32_L2SB         (  15 ),    // PermUser
  [ 11] = MPC_LAYOUT32_LSB          (  15 ),    // PermUser
  [ 12] = MPC_LAYOUT32_MSB          (  16 ),    // PermService
  [ 13] = MPC_LAYOUT32_M2SB         (  16 ),    // PermService
  [ 14] = MPC_LAYOUT32_L2SB         (  16 ),    // PermService
  [ 15] = MPC_LAYOUT32_LSB          (  16 ),    // PermService
  [ 16] = MPC_LAYOUT32_MSB          (  17 ),    // UUID32
  [ 17] = MPC_LAYOUT32_M2SB         (  17 ),    // UUID32
  [ 18] = MPC_LAYOUT32_L2SB         (  17 ),    // UUID32
  [ 19] = MPC_LAYOUT32_LSB          (  17 ),    // UUID32
  [ 20] = MPC_LAYOUT32_MSB          (  18 ),    // SVN REV ROOT
  [ 21] = MPC_LAYOUT32_M2SB         (  18 ),    // SVN REV ROOT
  [ 22] = MPC_LAYOUT32_L2SB         (  18 ),    // SVN REV ROOT
  [ 23] = MPC_LAYOUT32_LSB          (  18 ),    // SVN REV ROOT





};



STATIC uint32_t mskPwEncrypt(uint32_t pw) {
  #ifdef MSK_DUMMY_ENCRYPTION
    return pw + 1234;
  #else
    return MskPasswordEncrypt(pw);
  #endif
}



// -------------------------------------------------------------------------------------------------
/** \brief Mpc Cmd() function for MSK.
 *
 * Dispatch the Init and FactoryReset commands to the acc. function call sequence.
 */
mpc_returncodes_enum MSK_command(mpc_cmd_t cmd, uint8_t channel, mpc_port_t port) {
  (void)channel; (void)port; // no warnings
  switch (cmd)
  {
    case MPC_CMD_INIT:
      mskPageInit();  // First init MSK as the master of this
      bioPageInit();  // Then init BIO as a directly dependent module
      mskAUTHInit();  // Finally init AUTH functionality after BIO has picked up the challenge
      break;
    case MPC_CMD_FACTORY_RESET:
      mskFactoryReset();
      break;
    case MPC_CMD_RESET:
      mskReset();
      break;
    default:
      break;
  }

  return MPC_OK;
}

// -------------------------------------------------------------------------------------------------
/** \brief Mpc Cmd() function for BIO.
 *
 * Empty function to satisfy the system. All tasks for Init() and FactoryReset() of the BIO page
 * currently are done in MSK_command()
 * - to ensure a correct call sequence independent of the Mpc layout
 * - to have Osram Authentication also available when BIO page is not present.
 */
mpc_returncodes_enum BIO_command(mpc_cmd_t cmd, uint8_t channel, mpc_port_t port) {
  (void)channel; (void)port; // no warnings
  switch (cmd)
  {
    case MPC_CMD_INIT:
      // Done together with MSK; nothing additional here
      break;
    case MPC_CMD_FACTORY_RESET:
      // BIO page is read only, nothing is to be reset. If needed, address MSK for reset instead
      break;
    default:
      break;
  }

  return MPC_OK;
}


/** \brief Init Bio Page
 *
 * Called during device start only once (via MSK init)
 *
 * Function:
 * - fetch  ID from uC and store in uuid32
 */
static void bioPageInit(void)
{
  MSK_data.uuid32 = ControllerUniqueID();
  MSK_data.svn_rev_root = getFWRevRoot();
}


/** \brief Init MSK MPC
 *
 * Called during device start only once
 *
 * Function:
 * - set pointer for identification registers to nfc_msk_tmp
 * - init local vars but not values stored within NFC
 * - load ACL with USER and SERVICE permissions which are stored within MSK page
 */
static void mskPageInit (void) {
//    p2MSK = (MSK_NVM_record_t *) nfc_msk_tmp;
    // check if there is already data sored in persistent memory
    if( nvmDataInit(&(MSK_data.nvm_data),sizeof(MSK_NVM_record_t),NVM_MEM_CLASS_0) != NVM_INIT_OK ) {
         //  set default values if not.
         //LOCKBYTE not in useUSE.
      mskFactoryReset();

    } else {
      mskReset();
    }

}

static void mskReset(void) {
  MSK_data.masterStatus = 0;
  MSK_data.serviceStatus = 0;

  newMasterPwTemp = 0;
  newServicePwTemp = 0;
  AclDynamicPermissionsNotification(ACL_CRED_SLOT_SERVICE_KEY, MSK_data.nvm_data.permService);
  AclDynamicPermissionsNotification(ACL_CRED_SLOT_USER, MSK_data.nvm_data.permUser);
  AclCredentialNotification(ACL_CRED_SLOT_OSRAM_FACTORY_MODE, false);
}


/** \brief Resets MSK MPC to MNO FACTORY Settings
 *
 * Function:
 *  - Sets Master and Service Status to 0
 *  - Sets Permissions to current perm User and perm Service
 *  - removes FACTORY MODE
 */

STATIC void mskFactoryReset(void) {
    MSK_data.nvm_data = mskNVMData_Default;
    // static default value have to be overwritten by dynamic calculated
    // encrypted Password
    MSK_data.nvm_data.masterKey = mskPwEncrypt(0);  // encrypted content only
    MSK_data.nvm_data.serviceKey = mskPwEncrypt(0); // encrypted content only

    mskReset();
}

static void mskAUTHInit(void) {
  MskOsramAuthCalcResponses(MSK_data.uuid32, MSK_CorrectResponse);
}


//------------------------------------------------------------------------------
//-------------- Entity: MASTER PASSWORD ---------------------------------------
//------------------------------------------------------------------------------
/** \brief Function to write Master password to unlock
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - check whether key is equal to the stored one
 * - if yes -> call ACL to set Master credential
 * - increase unlock counter
 * - set status
 * - (if wrong password is written, status will be set to "locked")
 */
STATIC int mskWriteMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr); (void)(channel);

    uint32_t pwEnc;
    MSK_Response[0] = mpcIo->uint32;
    pwEnc = mskPwEncrypt(mpcIo->uint32);
    if (MSK_data.nvm_data.masterKey == pwEnc) {
      AclCredentialNotification(ACL_CRED_SLOT_MASTER_KEY, true);
      if (MSK_data.nvm_data.masterUnlockCnt < 254) {
        MSK_data.nvm_data.masterUnlockCnt++;
      }
      MSK_data.masterStatus = 1;
    }
    else {
      AclCredentialNotification(ACL_CRED_SLOT_MASTER_KEY, false);
      if (mpcIo->uint32) {  // ensure that password zero is neutral for error counter
        if (MSK_data.nvm_data.masterErrorCnt < 254) {
          MSK_data.nvm_data.masterErrorCnt++;
        }
      }
      MSK_data.masterStatus = 0;
      AclCredentialNotification(ACL_CRED_SLOT_OSRAM_FACTORY_MODE, false);
    }
    return MPC_OK;
}

/** \brief Function to read Master password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back the stored AND encrypted value of the Master Password
 */
STATIC uint32_t mskReadMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
  (void)(adr); (void)(channel); (void)(*mpcIo);

   return (MSK_data.nvm_data.masterKey);
}
//------------------------------------------------------------------------------
//-------------- Entity: NEW MASTER PASSWORD -----------------------------------
//------------------------------------------------------------------------------
/** \brief Function to write a NEW Master Password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 * The function is necessary not to store the new password readable
 * within any entity. The new password is just stored within RAM. Only if
 * confirmation is written afterwards, the password will be changed.
 *
 * \nFunction:
 * - the new pw is stored in RAM.
 */
STATIC int mskWriteNewMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
//void mskWriteNewMasterPassword(uint32_t new_master_pw) {
    (void)(adr);
    (void)(channel);
      newMasterPwTemp = mpcIo->uint32;
   return MPC_OK;
}
/** \brief Function to read Master password
 *
 * Called only by accident or for systemic reasons (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back a default value 0xFFFFFFFF
 */
STATIC uint32_t mskReadNewMasterPassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
  (void)(adr); (void)(channel); (void)(*mpcIo);

  return (MSK_data.uuid32);
}
//------------------------------------------------------------------------------
//-------------- Entity: NEW MASTER PASSWORD CONFIRMATION ----------------------
//------------------------------------------------------------------------------
/** \brief Function to write password confirmation for Master password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - check whether pw confirmation fits new_oem_pw
 * - if true store encrypt and store new
 * - Note: Master cred is necessary to write confirmation thus status of master
 * - key remains unchanged in any case!
 */
STATIC int mskWriteNewMasterPasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr);
    (void)(channel);
    uint8_t x;
    uint8_t pwc;
    pwc = (uint8_t)mpcIo->uint32;
    x = newMasterPwTemp+(newMasterPwTemp>>8)+(newMasterPwTemp>>16)+(newMasterPwTemp>>24);
    x = x/2;
    if (x == pwc) {
        MSK_data.nvm_data.masterKey = mskPwEncrypt(newMasterPwTemp);
    }
    return MPC_OK;
}
/** \brief Function to read Master password confirmation
 *
 * Called only by accident or for systemic reasons (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back a default value 0xFF
 */
STATIC uint8_t mskReadNewMasterPasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
    (void)(adr);
    (void)(channel);
    (void)(*mpcIo);
    return (0xFF);
}


STATIC int mskCheckOsramAuth(void)
{
  if ((MSK_Response[0] == MSK_CorrectResponse[0]) && (MSK_Response[1] == MSK_CorrectResponse[1]))
  {
    AclCredentialNotification(ACL_CRED_SLOT_OSRAM_FACTORY_MODE, true);
    if (MSK_data.masterStatus != 1) {
      if (MSK_data.nvm_data.masterErrorCnt >0)  MSK_data.nvm_data.masterErrorCnt--;
    }
    if (MSK_data.serviceStatus != 1) {
          if (MSK_data.nvm_data.serviceErrorCnt >0)  MSK_data.nvm_data.serviceErrorCnt--;
        }
  }
  return MPC_OK;
}

//------------------------------------------------------------------------------
//-------------- Entity: SERVICE PASSWORD --------------------------------------
//------------------------------------------------------------------------------
/** \brief Function to write a SERVICE Password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - the new pw is stored in RAM.
 * - check whether service password is equal to stored service password
 * - if yes, call ACL to change credential
 * - else increase error counter
 * - NOTE: if it is a new service pw and confirmation will be given
 *   the error counter will be decreased after confirmation
 */

STATIC int mskWriteServicePassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr); (void)(channel); uint32_t pwEnc;

    newServicePwTemp = mpcIo->uint32;
    MSK_Response[1] = mpcIo->uint32;
    pwEnc = mskPwEncrypt(mpcIo->uint32);
    if (MSK_data.nvm_data.serviceKey == pwEnc) {
        AclCredentialNotification(ACL_CRED_SLOT_SERVICE_KEY, true);
        if (MSK_data.nvm_data.serviceUnlockCnt < 254) {
          MSK_data.nvm_data.serviceUnlockCnt++;
        }
        MSK_data.serviceStatus = 1;
    }
    else {
        AclCredentialNotification(ACL_CRED_SLOT_SERVICE_KEY, false);
        AclCredentialNotification(ACL_CRED_SLOT_OSRAM_FACTORY_MODE, false);
        if (newServicePwTemp) { // ensure that password zero is neutral for error counter
          if (MSK_data.nvm_data.serviceErrorCnt < 254) {
            MSK_data.nvm_data.serviceErrorCnt++;
          }
          MSK_data.serviceStatus = 0;
        }
    }
    mskCheckOsramAuth();
    return MPC_OK;
}


/** \brief Function to read Service password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back the stored AND encrypted value of the Service Password
 */
STATIC uint32_t mskReadServicePassword(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
  (void)(adr);
  (void)(channel);
  (void)(*mpcIo);
  return (MSK_data.nvm_data.serviceKey);
}
//------------------------------------------------------------------------------
//-------------- Entity: SERVICE PASSWORD CONFIRMATION -------------------------
//------------------------------------------------------------------------------
/** \brief Function to write confirm and change SERVICE Password
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 * New serviceKey is finally encrypted and stored.
 *
 * \nFunction:
 * - check whether pw confirmation fits service password
 * - if true store new service password
 * - decrement error counter because it was incremented before
 * - (but only if temp stored password is different from new password)
 * - No credential change is necessary, due to OEM password is given anyhow
 */
//void mskWriteServicePasswordConfirmation (uint8_t pw_conf) {
STATIC int mskWriteServicePasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr);
    (void)(channel);
    uint8_t x;
    uint8_t pwc;
    uint32_t encPw;
    pwc = (uint8_t)mpcIo->uint32;
    x = newServicePwTemp+(newServicePwTemp>>8)+(newServicePwTemp>>16)+(newServicePwTemp>>24);
    x = x/2;
    encPw = mskPwEncrypt(newServicePwTemp);
    if (x == pwc) {
        if ((MSK_data.nvm_data.serviceKey)!=encPw) { // password is new
            MSK_data.nvm_data.serviceKey = encPw;
            if (MSK_data.nvm_data.serviceErrorCnt > 0)  MSK_data.nvm_data.serviceErrorCnt--;
        }
    }
    return MPC_OK;
}
/** \brief Function to read Service Password confirmation
 *
 * Called only by accident or for systemic reasons (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back a default value 0xFF
 */
STATIC uint8_t mskReadServicePasswordConfirmation(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
    (void)(adr);
    (void)(channel);
    (void)(*mpcIo);
    return (0xFF);
}
//------------------------------------------------------------------------------
//-------------- Entity: PERMISSION USER ---------------------------------------
//-----------------------------------------------------------------------------
/** \brief Function to write dynamic permissions for the User
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - copy value to internal struct
 * - call ACL function to change permission
 */
STATIC int mskWritePermissionUser(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr);
    (void)(channel);
    uint32_t perm_user;
    perm_user = mpcIo->uint32;
    MSK_data.nvm_data.permUser = perm_user;
    AclDynamicPermissionsNotification(ACL_CRED_SLOT_USER, perm_user);
    return MPC_OK;
}
/** \brief Function to read User Permissions
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back the stored User Permissions
 */
STATIC uint32_t mskReadPermissionUser(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
  (void)(adr);
  (void)(channel);
  (void)(*mpcIo);
  return (MSK_data.nvm_data.permUser);
}
//------------------------------------------------------------------------------
//-------------- Entity: PERMISSION SERVICE ------------------------------------
//-----------------------------------------------------------------------------
/** \brief Function to write dynamic permissions for the SERVICE technician
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - copy value to internal struct
 * - call ACL function to change permission
 */
STATIC int mskWritePermissionService(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
    (void)(adr);
    (void)(channel);
    uint32_t perm_service;
    perm_service = mpcIo->uint32;
    MSK_data.nvm_data.permService = perm_service;
    AclDynamicPermissionsNotification(ACL_CRED_SLOT_SERVICE_KEY, perm_service);
    return MPC_OK;
}
/** \brief Function to read Service Permissions
 *
 * Called by whoever needs it (DALI or NFC via MPCgen)
 *
 * \nFunction:
 * - gives back the stored Service Permissions
 */
STATIC  uint32_t mskReadPermissionService(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel){
  (void)(adr);
  (void)(channel);
  (void)(*mpcIo);
  return (MSK_data.nvm_data.permService);
}



// ---------------------------------------------------------------------------------------------------------------------
const mpc_descriptor_t MSK_Descriptor = {
  .keydata            = {
    .id             = 27,
    .version        = 0,
    .attributes     = 0,
    .payload_off    = 5,
    .length         = 28,
  },
  .tables_len   = (sizeof(MSK_layout) / sizeof(mpc_layout_t)),
  .entities     = MSK_entities,
  .constants    = MSK_constants,
  .layout       = MSK_layout,
  .Cmd          = &MSK_command,
};

const mpc_descriptor_t BIO_Descriptor = {
  .keydata            = {
    .id             = 29,
    .version        = 0,
    .attributes     = MPC_ATTRIB_NFC_READONLY,
    .payload_off    = 5,
    .length         = 24,
  },
  .tables_len   = (sizeof(BIO_layout) / sizeof(mpc_layout_t)),
  .entities     = MSK_entities,
  .constants    = MSK_constants,
  .layout       = BIO_layout,
  .Cmd          = &BIO_command,
};
