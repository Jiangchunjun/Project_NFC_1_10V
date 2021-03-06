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
// Initial version: 2016-09, W.Limmer@osram.de
//
// Change History:
//
// $Author: w.limmer $
// $Revision: 11592 $
// $Date: 2018-01-23 23:12:20 +0800 (Tue, 23 Jan 2018) $
// $Id: Acl.c 11592 2018-01-23 15:12:20Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Acl/releases/Rel.%201.0.0/Src/Acl.c $
//

#include "Global.h"
#include "Acl.h"



// ---------------------------------------------------------------------------------------------------------------------
/** \brief Cache for all \b credentials present at a given time.
*
* This variable stores all credentials as a bit in an integer variable. This is done, because all credentials
* are independent from each other and quite often different credentials result in the same permissions:
* Therefore it's necessary to know about this to rebuild the permissions after a credential is withdrawn.
*
* This cache significantly speeds-up this rebuild (an quite expensive operation nevertheless).
*/
//STATIC uint32_t aclCredentailsCache = (1UL << ACL_CRED_SLOT_USER); // user only
STATIC uint32_t aclCredentialsCache;

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Cache for all \b permissions present at a given time.
*
* This variable stores all permissions as a bit in an integer variable. This is done to have all permissions
* available immediately when a permission should be checked, the vast majority of calls to ACL.
*
* Without this cache, every permissions check would have to ask all credentials for their specific permissions,
* which would be an quite expensive operation (for-loop).
*
* Set and cleared in \ref AclCredentialNotification(), queried in \ref AclCheckPermissions().
*/
STATIC uint32_t aclPermissionsCache;

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Array to store the mask of permissions, which come with one credential
 *
 * For every slot (source of credentials), there is one word to store the associated permissions.
 */
static uint32_t aclPerm[ACL_CRED_SLOT_FIRST_UNUSED];


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Rebuild aclPermissionsCache (internal function).
 *
 * Logically OR all permissions in aclPerm[] of all given credentials to an internal cache varaible. This has
 * to be done after a credential is given, withdrawn or when dynamic permissions are changed.
 *
 * Called by:
 * - AclCredentialNotification or
 * - AclDynamicPermissionsNotification
 */
STATIC void rebuildPermissionCache(void) {
  uint32_t new_perm = 0;

  for (int i = 0; i < ACL_CRED_SLOT_FIRST_UNUSED; i++) {
    if (aclCredentialsCache & (1 << i)) {
      new_perm |= aclPerm[i];
    }
  }
  aclPermissionsCache = new_perm;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Init ACL module variables
 *
 * Called by the application once after uC ramp up
 *
 * Function:
 * - sets init permissions for all slots
 * - sets no credential available, except user credential
 * - calculation of permission cache
 */
void AclInit (void) {
    aclPerm[ACL_CRED_SLOT_OSRAM_FACTORY_MODE] = ACL_PERM_INIT_OSRAM_FACTORY_MODE;
    aclPerm[ACL_CRED_SLOT_MASTER_KEY] = ACL_PERM_INIT_MASTER_KEY;
    aclPerm[ACL_CRED_SLOT_SERVICE_KEY] = ACL_PERM_INIT_SERVICE_KEY;
    aclPerm[ACL_CRED_SLOT_USER] = ACL_PERM_INIT_USER;

    aclCredentialsCache = (1UL << ACL_CRED_SLOT_USER); // user only

    rebuildPermissionCache();
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Receive notifications about credentials provided or withdrawn.
 *
 * Called by any MPC or other code which can provide a sort of authentication, e.g.
 * - New user and tool authentication mpc
 * - Legacy password pages
 * - Tbd. MPCs for sellable features (cryptographic strong hashes)
 * - ProductionTest (calibration mode)
 * - What ever will come in the future when new decisions about balance safety vs. convenience are needed
 *
 * Function:
 * - Set or clear a bit in \ref aclCredentailsCache upon the message
 * - Rebuild \ref aclPermissionsCache
 *
 * \param cred            Number of the credential involved (acl_credential_enum)
 * \param cred_new_state  True: Credential now is present; False: credential is withdrawn
 */
void AclCredentialNotification(acl_cred_slot_t cred, bool cred_new_state) {

  uint32_t cred_mask = (1 << cred); // No overflow check needs to be done
  if (cred_new_state) {
    aclCredentialsCache |= cred_mask;
  }
  else {
    aclCredentialsCache &= ~(cred_mask);
  }
  rebuildPermissionCache();
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Store dynamically managed permissions in a defined slot.
 *
 * Called by any NEW Password MPC (11/2016 or later) !!
 * - as soon as new dynamic perm_mask data was written to New Psswort MPC
 * - this function has to be called to create "Permission Cache" based on
 * - new perm_mask.
 * - slot gives the name dynamic perm_mask e.g. Service, User
 * - test comment to check SVN 2017 01 02
 *
 * \param slot
 * \param perm_mask
 */
void AclDynamicPermissionsNotification(acl_cred_slot_t slot, acl_perm_t perm_mask) {

  if ((slot >= ACL_CRED_SLOT_DYNAMIC_FIRST) && (slot < ACL_CRED_SLOT_FIRST_UNUSED)) {
    perm_mask     &= ~0x000000FF;   // clear the fixed/non-dynamic input permissions
    aclPerm[slot] &= ~0xFFFFFF00;   // clear the dynamic part of the available slot

    aclPerm[slot] |= perm_mask;

    // Better version with ACL_PERM_ALWAYS == 0 would be:
    // aclPerm[slot] = (perm_mask & 0xFFFFFF00);
    // printf("\tACL Permission Slot %d: 0x%08x\n", slot, aclPerm[slot]);
  }
  rebuildPermissionCache();
}

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Check whether all permissions required for an operation are present.
 *
 * Typically called by a MPC before a protected data entity is read or written. But also any other code
 * can invoke this function to allow or to deny an operation depending on the permissions managed by the ACL.
 *
 * Port ROOT: Everything is allowed except ACL_PERM_NEVER (e.g. write access to constants).
 * Port DALI: \todo -- to be reworded after code re-factoring--
 *            Usually permission DALI_LOCKBYTE is given to everybody. But permission
 *            is removed in case of port == DALI. This allows the calling MPC to
 *            decide whether access is denied by setting the DALI_LOCKBYTE
 *            permission within the required permission.
 * Port DALI: Thus proceed as follows within MPC (pseudocode)":
 *   if ((DALI_Lockbyte_required_by_entity) && (DALI_Lockbyte != 0x55)) {
 *     perm_required |= ACL_PERM_DALI_LOCKBYTE;  // no access if port=DALI
 *   }
 *   else {
 *     perm_required &= ~ACL_PERM_DALI_LOCKBYTE; //
 *   }
 *
 * \param perm_required
 * \param port
 */
acl_check_t AclCheckPermissions(acl_perm_t perm_required, acl_port_t port) {
  uint32_t aclPermissionsCacheTemp;
  aclPermissionsCacheTemp = aclPermissionsCache;

  if (ACL_PORT_ROOT == port) {
    aclPermissionsCacheTemp = 0xFFFFFFFF;
    aclPermissionsCacheTemp &= ~ ACL_PERM_NEVER;
  }

  /// \todo Re-factor this to check on != PORT_DALI
  else if (port == ACL_PORT_USER) {
    //aclPermissionsCacheTemp = aclPermissionsCache;
    // add Lockbyte Permission to normal User thus
    // MPCgen cannot block by requesting this permission
    aclPermissionsCacheTemp |= ACL_PERM_DALI_LOCKBYTE;
  }

  // Normal Access, e.g. USER or NFC
  if ( (uint32_t)perm_required == ( aclPermissionsCacheTemp & (uint32_t)perm_required )) {
    return ACL_GRANT;   // All required permissions are present
  }
  else {
    return ACL_DENY;
  }
}
