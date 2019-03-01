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
// $Author: m.nordhausen $
// $Revision: 8138 $
// $Date: 2017-04-28 15:37:17 +0800 (Fri, 28 Apr 2017) $
// $Id: Acl.h 8138 2017-04-28 07:37:17Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Acl/releases/Rel.%201.0.0/Src/Acl.h $
//

#ifndef __ACL_H
#define __ACL_H

// Not the very best style, but fine to keep the interface clear and to allow
// a script generated credentials/permissions table
#include "AclDefaults.h"

typedef uint32_t acl_perm_t;

typedef enum {
  ACL_GRANT,
  ACL_DENY
} acl_check_t;

typedef enum {
  ACL_PORT_ROOT,
  ACL_PORT_DALI,
  ACL_PORT_USER
} acl_port_t;

void        AclInit(void);
void        AclCredentialNotification(acl_cred_slot_t cred, bool cred_new_state);
void        AclDynamicPermissionsNotification(acl_cred_slot_t slot, acl_perm_t perm_mask);
acl_check_t AclCheckPermissions(acl_perm_t perm_required, acl_port_t port);

#endif
