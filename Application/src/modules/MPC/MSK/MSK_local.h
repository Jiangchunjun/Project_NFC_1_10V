// ---------------------------------------------------------------------------------------------------------------------
// OSK_local.h
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
// Initial version: 2017-01, Olaf Busse, o.busse@osram.com
//
// $Author: m.nordhausen $
// $Revision: 11144 $
// $Date: 2017-12-07 17:42:47 +0800 (Thu, 07 Dec 2017) $
// $Id: MSK_local.h 11144 2017-12-07 09:42:47Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/MSK/releases/Rel.%201.0.0/Src/MSK_local.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>



/** \brief MSK data to be stored within NVM
 * The struct contains:
 * - some module internal data
 * - some entities / properties
*/
typedef struct {
  uint32_t     masterKey;  // encrypted content only
  uint32_t     serviceKey; // encrypted content only

  uint8_t      masterUnlockCnt;
  uint8_t      masterErrorCnt;

  uint8_t      serviceUnlockCnt;
  uint8_t      serviceErrorCnt;

  uint32_t     permUser;
  uint32_t     permService;

} MSK_NVM_record_t;



/** \brief MSK data to be stored within NVM

 */
typedef struct {
  MSK_NVM_record_t  nvm_data;
  uint8_t           masterStatus;
  uint8_t           serviceStatus;
  uint32_t          uuid32;
  uint32_t          svn_rev_root;

} MSK_record_t;
