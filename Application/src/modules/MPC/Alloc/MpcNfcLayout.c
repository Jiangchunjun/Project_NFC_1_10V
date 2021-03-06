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
// Change history, major milestones and credits:
//    2017-04, W.Limmer@osram.de: Initial version
//
// $Author: m.nordhausen $
// $Revision: 9516 $
// $Date: 2017-08-09 19:13:08 +0800 (Wed, 09 Aug 2017) $
// $Id: MpcNfcLayout.c 9516 2017-08-09 11:13:08Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcNfcLayout.c $
//

/** \addtogroup MPC Multi Purpose Centers
 * \{
 *
 * \file
 * \brief Defines an array for the NFC layout of the Mpcs: Will be moved sooner or later to MppcAllocation.c
 */


#include "Global.h"
#include "Config.h"

// -----------------------------------------------------------
// Only for a very quick&dirty test
#define MSK_PAGE                                2
#define MPC_CSM_NUMBER                          3
#define MPC_CLM_NUMBER                          4
// Add reasonable unit tests instead!
// -----------------------------------------------------------


#include "Mpc.h"                // Generic application interface
#include "MpcInternal.h"        // Internal interface to the Mpc implementations
#include "MpcAllocation.h"      // Generic application interface
#include "MpcIncludeAll.h"


// #ifndef PLATFORM_NON_STANDARD

#define NFC_TAG_SECTION_RW_START      0x120
#define NFC_TAG_SECTION_PROT_START    0x300

MPC_DEFINE_INFO(NFC_TAG_SECTION_RW_START);
MPC_DEFINE_INFO(NFC_TAG_SECTION_PROT_START);

// Padding / alignment in the NFC sections
#define NFC_CRC_LENGTH                       2
#define NFC_ADDR_ALIGN                      16
#define NFC_ADDR_PADDING(START, RAW_LENGTH) (  ((START) + (RAW_LENGTH)                    \
                                               + ((RAW_LENGTH) ? (NFC_CRC_LENGTH) : 0 )   \
                                               + (NFC_ADDR_ALIGN) - 1                     \
                                             ) & ( (~ (NFC_ADDR_ALIGN)) + 1) )

// Define locations successive one after the other with reference to the last one
// A empty entry also will have a location, but this location/start point will used/overwritten by the next one again
#define NFC_TAG_MSK_LOCATION    NFC_ADDR_PADDING(NFC_TAG_SECTION_RW_START,  0) // First entry, nothing to be added from last entry
#define NFC_TAG_AUTH_LOCATION   NFC_ADDR_PADDING(NFC_TAG_MSK_LOCATION,      NFC_TAG_MSK_RAW_LENGHT + 0)
#define NFC_TAG_CSM_LOCATION    NFC_ADDR_PADDING(NFC_TAG_AUTH_LOCATION,     NFC_TAG_AUTH_RAW_LENGHT + 0)
#define NFC_TAG_CLM_LOCATION    NFC_ADDR_PADDING(NFC_TAG_CSM_LOCATION,      NFC_TAG_CSM_RAW_LENGHT + 0)
#define NFC_TAG_SECTION_RW_END  NFC_ADDR_PADDING(NFC_TAG_CLM_LOCATION,      NFC_TAG_CLM_RAW_LENGHT + 0) - 1

STATIC_ASSERT(NFC_TAG_SECTION_RW_END < NFC_TAG_SECTION_PROT_START, "NFC tag layout issue, overlapping sections: rw and prot");

// Build the table with pre-defined entries: Be careful, no commas here, they have to be part of the defines
const uint16_t NfcMpcTagLayout[] = {
  NFC_TAG_MSK_TABLE_ENTRY
  NFC_TAG_AUTH_TABLE_ENTRY
  NFC_TAG_CSM_TABLE_ENTRY
  NFC_TAG_CLM_TABLE_ENTRY
};


#include <stdio.h>
int nfc_alloc_print (int argc, char ** argv) {
  printf("\n\n\n" );
  (void) argc; (void) argv;
  for (unsigned i = 0; i < sizeof(NfcMpcTagLayout) / 2; i++) {
    printf("Index %d: Start at %d (0x%04x)\n", i, NfcMpcTagLayout[i], NfcMpcTagLayout[i]);
  }
  return 0;
}
/** \} */
