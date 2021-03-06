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
// Initial version: 2016-07 W.Limmer@osram.de
//
// Change History:
//
// $Author: m.schmidl $
// $Revision: 10915 $
// $Date: 2017-11-23 21:26:27 +0800 (Thu, 23 Nov 2017) $
// $Id: MpcPerm4Legacy.c 10915 2017-11-23 13:26:27Z m.schmidl $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcPerm4Legacy.c $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 * \file
 * \brief Definition of all arrays needed to legacy membanks with more than one permission.
 *
 * Defined arrays give indirect access to permissions needed for address in Membank.
 * Arrays are linked to descriptors. Index of Permission bit is Address in Membank + 1.
 * First element contains length of array, to test whether queried address is in range.
 * Content is number of bits to shift left for permission.
 *
 */

#include <Global.h>

#define MODULE_MPC
#include "Config.h"

#include "AclDefaults.h"
#include "MpcPerm4Legacy.h"
#include "MpcDefaultConfiguration.h"

#define DEBUGPRINT(...)
// #define DEBUGPRINT(...)         printf(__VA_ARGS__)

#ifdef MPC_TFM_NUMBER
uint8_t tfmpermissions[6] = {
    [0]  = 4,
    [1]  = BIT_NUMBER(ACL_PERM_TUNING_LIMITS),  // adr 0x05
    [2]  = BIT_NUMBER(ACL_PERM_TUNING_FACTOR),  //     0x06
    [3]  = BIT_NUMBER(ACL_PERM_TUNING_LIMITS),  //     0x07
    [4]  = BIT_NUMBER(ACL_PERM_TUNING_LIMITS),  //     0x08
};
#endif



#ifdef MPC_STEP_NUMBER
uint8_t steppermissions[11] = {
    [0]  = 9,      // last adr.
    [1]  = BIT_NUMBER(ACL_PERM_NEVER),      //    0x05
    [2]  = BIT_NUMBER(ACL_PERM_OPER_MODE),      // Adr 0x06
    [3]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x07
    [4]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x08
    [5]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x09
    [6]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0A
    [7]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0B
    [8]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0C
    [9] = BIT_NUMBER(ACL_PERM_DIMM_PARM)       //     0x0D
};
#endif

#ifdef MPC_ASTRO_NUMBER
uint8_t astropermissions[28] = {
    [0]  = 26,   // last adr.
    [1]  = BIT_NUMBER(ACL_PERM_NEVER),    //    0x05
    [2]  = BIT_NUMBER(ACL_PERM_OPER_MODE),      // Adr 0x06
    [3]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x07
    [4]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x08
    [5]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x09
    [6]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0A
    [7]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0B
    [8]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0C
    [9] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0D
    [10] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0E
    [11] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0F
    [12] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x10
    [13] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x11
    [14] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x12
    [15] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x13
    [16] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x14
    [17] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x15
    [18] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x16
    [19] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x17
    [20] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x18
    [21] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x19
    [22] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x1A
    [23] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x1B
    [24] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x1C
    [25] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x1D
    [26] = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x1E
};
#endif

#ifdef MPC_MAINS_NUMBER
uint8_t mainspermissions[8] = {
    [0]  = 6,   // last adr.
    [1]  = BIT_NUMBER(ACL_PERM_NEVER),    //    0x05
    [2]  = BIT_NUMBER(ACL_PERM_OPER_MODE),      // Adr 0x06
    [3]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x07
    [4]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x08
    [5]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x09
    [6]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0A

};
#endif

#ifdef MPC_PRESDETECT_NUMBER
uint8_t presdetectpermissions[8] = {
    [0]  = 6,   // last adr.
    [1]  = BIT_NUMBER(ACL_PERM_NEVER),    //    0x05
    [2]  = BIT_NUMBER(ACL_PERM_OPER_MODE),      // Adr 0x06
    [3]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x07
    [4]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x08
    [5]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x09
    [6]  = BIT_NUMBER(ACL_PERM_DIMM_PARM),      //     0x0A

};
#endif

/** \} */
