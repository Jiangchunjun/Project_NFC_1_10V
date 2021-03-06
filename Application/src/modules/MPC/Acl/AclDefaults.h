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
// Initial version: 2016-10, W.Limmer@osram.de
//
// Change History:
//
// $Author: w.limmer $
// $Revision: 11592 $
// $Date: 2018-01-23 23:12:20 +0800 (Tue, 23 Jan 2018) $
// $Id: AclDefaults.h 11592 2018-01-23 15:12:20Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Acl/releases/Rel.%201.0.0/Src/AclDefaults.h $
//

#ifndef __ACL_DEFAULTS_H
#define __ACL_DEFAULTS_H


/** \brief Main Index of ACL for Credentials and Slots.
 *
 * This ENUM is suitable for two purposes:
 * - 1. specify a credential which has to be enabled or disabled
 * - 2. specify a slot within dynamic permissions have to be changed
 */
typedef enum {
  ACL_CRED_SLOT_OSRAM_FACTORY_MODE,   // The Osram factory mode was entered (via 17-bit special DALI commands)
  ACL_CRED_SLOT_MASTER_KEY,           // Correct Master password is present
  ACL_CRED_SLOT_SERVICE_KEY,          // Correct Service password is present
  ACL_CRED_SLOT_USER,                 // This credential is always present
  ACL_CRED_SLOT_FIRST_UNUSED,         // Unused except for array and for-loop boundaries
}
acl_cred_slot_t;

#define ACL_CRED_SLOT_DYNAMIC_FIRST  ACL_CRED_SLOT_SERVICE_KEY

/** \brief List of all possible permissions.
 *
 * The permissions are used to:
 * - 1. create init values e.g. ACL_PERM_INIT_OSRAM_FACTORY_MODE for the
 *      aclPerm array.
 * - 2. define read and write permissions for MPC entities.
 */
#define ACL_PERM_NEVER            (1L <<  0)    // Never is never, also root can't overcome! Used for read-only.
#define ACL_PERM_OSRAM            (1L <<  1)    // For the time being: Permission for Osram factory
                                                // Allow all production test steps and calibration, counter resets, ...
                                                // Will be renamed later to ACL_PERM_OSRAM_FACTORY
#define ACL_PERM_DALI_LOCKBYTE    (1L <<  2)    // DALI Lockbyte handled by MpcAlloc

#define ACL_PERM_MASTER           (1L <<  6)    // Permissions directly bound to Master Password of MSK
                                                // e.g. for writing DALI membank 1
#define ACL_PERM_ALWAYS           (1L <<  7)

#define ACL_PERM_OPER_CURR        (1L <<  8)
#define ACL_PERM_CONST_LUMEN      (1L <<  9)
#define ACL_PERM_THERM_PROT       (1L << 10)
#define ACL_PERM_DRV_GUARD        (1L << 11)
#define ACL_PERM_TUNING_LIMITS    (1L << 12)
#define ACL_PERM_EOL              (1L << 13)
#define ACL_PERM_TUNING_FACTOR    (1L << 14)
#define ACL_PERM_LAMP_OPER_TIME   (1L << 15)
#define ACL_PERM_DC_EMERGENCY     (1L << 16)
#define ACL_PERM_OPER_MODE        (1L << 17)
#define ACL_PERM_DIMM_PARM        (1L << 18)
#define ACL_PERM_GENERAL_FEATURES (1L << 20)
#define ACL_PERM_DIM2DARK         (1L << 21)
#define ACL_PERM_O2T              (1L << 22)
#define ACL_PERM_ASTRODIM         (1L << 23)

/** \brief Permissions to access Valuable Data entities.
 *
 * Read and write are not differntiated by decission of Marketing 2017-09.
 */
#define ACL_PERM_VALDATA          (1L << 19)


// Prepare upcoming rename of ACL_PERM_OSRAM, see above
#define ACL_PERM_OSRAM_FACTORY    ACL_PERM_OSRAM

#define BIT_NUMBER(bitset) (((bitset) & 0x00000001)?0:\
                            ((bitset) & 0x00000002)?1:\
                            ((bitset) & 0x00000004)?2:\
                            ((bitset) & 0x00000008)?3:\
                            ((bitset) & 0x00000010)?4:\
                            ((bitset) & 0x00000020)?5:\
                            ((bitset) & 0x00000040)?6:\
                            ((bitset) & 0x00000080)?7:\
                            ((bitset) & 0x00000100)?8:\
                            ((bitset) & 0x00000200)?9:\
                            ((bitset) & 0x00000400)?10:\
                            ((bitset) & 0x00000800)?11:\
                            ((bitset) & 0x00001000)?12:\
                            ((bitset) & 0x00002000)?13:\
                            ((bitset) & 0x00004000)?14:\
                            ((bitset) & 0x00008000)?15:\
                            ((bitset) & 0x00010000)?16:\
                            ((bitset) & 0x00020000)?17:\
                            ((bitset) & 0x00040000)?18:\
                            ((bitset) & 0x00080000)?19:\
                            ((bitset) & 0x00100000)?20:\
                            ((bitset) & 0x00200000)?21:\
                            ((bitset) & 0x00400000)?22:\
                            ((bitset) & 0x00800000)?23:\
                            ((bitset) & 0x01000000)?24:\
                            ((bitset) & 0x02000000)?25:\
                            ((bitset) & 0x04000000)?26:\
                            ((bitset) & 0x08000000)?27:\
                            ((bitset) & 0x10000000)?28:\
                            ((bitset) & 0x20000000)?29:\
                            ((bitset) & 0x40000000)?30:\
                            ((bitset) & 0x80000000)?31:0)


/** \brief init permissions for all slots
 */

#define ACL_PERM_INIT_OSRAM_FACTORY_MODE    ( \
    ACL_PERM_OSRAM                          | \
    ACL_PERM_MASTER                         | \
    ACL_PERM_ALWAYS                         | \
    ACL_PERM_OPER_CURR                      | \
    ACL_PERM_CONST_LUMEN                    | \
    ACL_PERM_THERM_PROT                     | \
    ACL_PERM_DRV_GUARD                      | \
    ACL_PERM_TUNING_LIMITS                  | \
    ACL_PERM_EOL                            | \
    ACL_PERM_TUNING_FACTOR                  | \
    ACL_PERM_LAMP_OPER_TIME                 | \
    ACL_PERM_DC_EMERGENCY                   | \
    ACL_PERM_OPER_MODE                      | \
    ACL_PERM_DIMM_PARM                      | \
    ACL_PERM_VALDATA                        |\
	ACL_PERM_GENERAL_FEATURES               | \
    ACL_PERM_DIM2DARK                       \
)

#define ACL_PERM_INIT_MASTER_KEY            ( \
    ACL_PERM_MASTER                         | \
    ACL_PERM_ALWAYS                         | \
    ACL_PERM_OPER_CURR                      | \
    ACL_PERM_CONST_LUMEN                    | \
    ACL_PERM_THERM_PROT                     | \
    ACL_PERM_DRV_GUARD                      | \
    ACL_PERM_TUNING_LIMITS                  | \
    ACL_PERM_EOL                            | \
    ACL_PERM_TUNING_FACTOR                  | \
    ACL_PERM_LAMP_OPER_TIME                 | \
    ACL_PERM_DC_EMERGENCY                   | \
    ACL_PERM_OPER_MODE                      | \
    ACL_PERM_DIMM_PARM                      | \
    ACL_PERM_VALDATA                        |\
	ACL_PERM_GENERAL_FEATURES               | \
    ACL_PERM_DIM2DARK                       \
)

#define ACL_PERM_INIT_SERVICE_KEY           ( \
    ACL_PERM_ALWAYS                         \
)

#define ACL_PERM_INIT_USER                  ( \
    ACL_PERM_ALWAYS                         \
)

#endif  // __ACL_DEFAULTS_H
