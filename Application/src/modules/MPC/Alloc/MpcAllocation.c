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
// $Author: w.limmer $
// $Revision: 11759 $
// $Date: 2018-02-02 01:27:53 +0800 (Fri, 02 Feb 2018) $
// $Id: MpcAllocation.c 11759 2018-02-01 17:27:53Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcAllocation.c $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 * \file
 * \brief Allocation of all Mpcs
 *
 * Include all Mpc header files and allocate the relevant ones (according to the config) in an array
 * array to their description tables and cmd and fallback read() and write() functions.
 */

#define MODULE_MPC
#include <Global.h>
#include "Config.h"

#include "Mpc.h"

#include "MpcInternal.h"
#include "MpcAllocation.h"

#include "MpcDescriptors.h"
#include "MpcIncludeAll.h"
#include "AclDefaults.h"

extern const mpc_descriptor_t O2T_Descriptor;
// change defined value to 1 to switch on debug print
// Always pass the debug code to the compiler, otherwise it will break sooner or later see
// http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing incl. the reasons for "do ... while(0)"
#define DEBUGPRINT_MPC
#define DEBUGPRINT(...)     do { if (  DEBUGPRINT_MPC + 0 ) { printf(__VA_ARGS__); }; } while (0)



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OTi Indoor NonSelv"
// Adjust these tables only in close cooperation with DS D EMEA Garching:
//  - Branchetti Filippo
//  - Gerber Maximilian
//  - Schwarzfischer Christof
//  - Limmer Walter


const mpc_keydata_t * MpcKeydata[] =
{
    &(MSK_Descriptor.keydata),                //  1
    &(BIO_Descriptor.keydata),                //  2
    &(MpcLegacyCsm.keydata),                  //  3
    &(MpcLegacyClm.keydata),                  //  4
    &(MpcLegacyInfo.keydata),                 //  6
    &(MpcLegacyAstro.keydata),                 //  6
    &(MpcLegacyTfm.keydata),
    &(MpcLegacyEol.keydata),
    &(O2T_Descriptor.keydata),
    &(MpcLegacyDali0.keydata),                //  9
    &(MpcLegacyDali1.keydata),                // 10
};


static const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [MPC_MSK_NUMBER]              =  1,
    [MPC_BIO_NUMBER]              =  2,
    [MPC_CSM_NUMBER]              =  3,
    [MPC_CLM_NUMBER]              =  4,
    [MPC_INFO_NUMBER]             =  5,
    [MPC_ASTRO_NUMBER]            =  6,
    [MPC_TFM_NUMBER]              =  7,
    [MPC_EOL_NUMBER]              =  8,
    [MPC_O2T_NUMBER]              =  9,
    //[MPC_TDC_NUMBER]              =  6,

    [0]                           =  10,
    [1]                           =  11,
    // No Dali mapping for the Dali-Pseudo-Mpc
};


#ifdef PLATFORM_EMEA_INDOOR_NONISOLATED
#pragma message ("Config PLATFORM_EMEA_INDOOR_NONISOLATED")

const mpc_keydata_t * MpcKeydata[] =
{
    &(MSK_Descriptor.keydata),                //  2
    &(MpcLegacyCsm.keydata),                  //  3
    &(MpcLegacyClm.keydata),                  //  4
    &(MpcLegacyInfo.keydata),                 // 29
    &(MpcLegacyTDC.keydata),                  // 31
    &(MpcLegacyGeneralFeatures.keydata),      // 15
    &(ValData_Description.keydata),           // 50
    &(MpcLegacyDali0.keydata),                //  0
    &(MpcLegacyDali1.keydata),                //  1
    &(MpcLegacyDali_T6.keydata),              //  -
};

  static const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [0]                           = 13,
    [1]                           = 14,
    // No Dali mapping for the Dali-Pseudo-Mpc
    [MPC_MSK_NUMBER]              =  1,
    [MPC_BIO_NUMBER]              =  2,
    [MPC_CSM_NUMBER]              =  3,       // index of page in above table
    [MPC_CLM_NUMBER]              =  4,       // sparse array with continuous numbers covering all
    [MPC_EMERGENCY_NUMBER]        =  5,       // entries of the mpc_nr pointers array
    [MPC_INFO_NUMBER]             =  6,       // ...
    [MPC_POWER_NUMBER]            =  7,
    [MPC_TDC_NUMBER]              =  8,
    [MPC_DIM2DARK_NUMBER]         =  9,
    [MPC_DGUARD_NUMBER]           = 10,
    [MPC_GENERALFEATURES_NUMBER]  = 11,
    [MPC_VALDATA_NUMBER]          = 12,
  };
#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OTi Indoor NonSelv Tunable White 2017/2018"
// Adjust these tables only in close cooperation with DS D EMEA Garching:
//  - Branchetti Filippo
//  - Limmer Walter

#ifdef PLATFORM_EMEA_INDOOR_NONISOLATED_TW
#pragma message ("Config PLATFORM_EMEA_INDOOR_NONISOLATED_TW")

const mpc_keydata_t * MpcKeydata[] =
{
    &(MSK_Descriptor.keydata),                //  2
    &(BIO_Descriptor.keydata),
    &(MpcLegacyClm.keydata),                  //  4
    &(MpcLegacyEmergency.keydata),            // 17
    &(MpcLegacyInfo.keydata),                 // 29
    &(MpcLegacyPower.keydata),                // 30
    &(MpcLegacyTDC.keydata),                  // 31
    &(MpcLegacyDguard.keydata),               // 18
    &(ValData_Description.keydata),           // 50
    &(MpcLegacyDali0.keydata),                //  0
    &(MpcLegacyDali1.keydata),                //  1
    &(MpcLegacyDali_T6.keydata),              //  -
};

  static const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [0]                       =  10,
    [1]                       =  11,
    // No Dali mapping for the Dali-Pseudo-Mpc
    [MPC_MSK_NUMBER]          =  1,
    [MPC_CLM_NUMBER]          =  3,        // sparse array with continuous numbers covering all
    [MPC_EMERGENCY_NUMBER]    =  4,        // entries of the mpc_nr pointers array
    [MPC_INFO_NUMBER]         =  5,        // ...
    [MPC_POWER_NUMBER]        =  6,
    [MPC_TDC_NUMBER]          =  7,
    [MPC_DGUARD_NUMBER]       =  8,
    [MPC_VALDATA_NUMBER]      =  9,
    [35]                      = 12,   // MNO dalimapping for Dali Params, for testpurposes, refactor asap
  };
#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OT Indoor Selv for Compact and Linear Designs Generation 2" (started in 2017)
// Adjust these tables only in close cooperation with DS D EMEA Garching:
//  - Max Schmidl
//  - Ralf Berchtold
//  - Joachim Eisenberg
//  - Walter Limmer

#ifdef PLATFORM_EMEA_INDOOR_ISOLATED_G2
  #pragma message ("Config PLATFORM_EMEA_INDOOR_ISOLATED_G2")
  #define LEGACY_PERM_COUNT                                 1
  const mpc_keydata_t * MpcKeydata[] = {
    &(MSK_Descriptor.keydata),                //  1
    &(BIO_Descriptor.keydata),                //  2
    &(MpcLegacyCsm.keydata),                  //  3
    &(MpcLegacyClm.keydata),                  //  4
    &(MpcLegacyDim2Dark.keydata),             //  5
    &(MpcLegacyGeneralFeatures.keydata),      //  6
    &(MpcLegacyEmergency.keydata),            //  7
    &(MpcLegacyDguard.keydata),               //  8
    &(MpcLegacyTfm.keydata),                  //  9
    &(MpcLegacyInfo.keydata),                 //  10
    &(MpcLegacyPower.keydata),                //  11
    &(MpcLegacyTDC.keydata),                  //  12
    &(MpcLegacyDali0.keydata),                //  13
    &(MpcLegacyDali1.keydata),                //  14
    &(MpcLegacyDali_T6.keydata),              //  16
//    &(MpcLegacyDebug.keydata),              //  17
  };

  const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [MPC_MSK_NUMBER]              =  1,
    [MPC_BIO_NUMBER]              =  2,
    [MPC_CSM_NUMBER]              =  3,
    [MPC_CLM_NUMBER]              =  4,
    [MPC_DIM2DARK_NUMBER]         =  5,
    [MPC_GENERALFEATURES_NUMBER]  =  6,
    [MPC_EMERGENCY_NUMBER]        =  7,
    [MPC_DGUARD_NUMBER]           =  8,
    [MPC_TFM_NUMBER]              =  9,
    [MPC_INFO_NUMBER]             = 10 ,
    [MPC_POWER_NUMBER]            = 11,
    [MPC_TDC_NUMBER]              = 12,
    [0]                           = 13,
    [1]                           = 14,
    // No Dali mapping for page 15 (Dali-internal data pseudo-Mpc)
//    [MPC_DEBUG_NUMBER]          =  16
  };

/** ONLY left for reference - is done by nfc_mpc_start_addr_create now.
  // refers to the MpcKeydata array - base adresses of the MPC data inside the tag
  const uint16_t NfcMpcTagStartAddr[] = {
    0x120,     // 288 - MSK_Description  /// bei 0x120 fängt der beschreibbare sektor an
    0x140,     // 320 - MpcLegacyCsm
    0x14C,     // 332 - MpcLegacyClm
    0x160,     // 352 - MpcLegacyDim2Dark
    0x164,     // 356 - MpcLegacyGeneralFeatures
    0x168,     // 360 - MpcLegacyEmergency
    0x170,     // 368 - MpcLegacyDguard
    0x178,     // 376 - MpcLegacyTfm
    0x184,     // 388 - MpcLegacyInfo
    0x194,     // 404 - MpcLegacyPower
    0x19C,     // 412 - MpcLegacyTDC
    0x1B0,     // 432 - MpcLegacyDali0
    0x1CC,     // 460 - MpcLegacyDali1
    0x218,     // 536 - MpcLegacyDali_T6 /// länge der Membank +2 (CRC) - bei dezimal 768 muß schluss sein, da ist dann protected Area - siehe nfc_config.h
                                         /// siehe auch 2017_04_06_NFC2_Interface_T4T_v1.8
  };
*/

#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OT Flex Signage Constant Voltage" (started/converted to DALI Lib V4.x in 2018-01)
// Adjust these tables only in close cooperation with DS D EMEA Treviso and Garching:
//  - Enrico Bortot
//  - Joachim Eisenberg
//  - Walter Limmer

#ifdef PLATFORM_EMEA_CONSTANT_VOLTAGE_FS // FS = FlexSignage
  #pragma message ("Config PLATFORM_EMEA_CONSTANT_VOLTAGE_FS")

  const mpc_keydata_t * MpcKeydata[] = {
    &(MpcLegacyAstro.keydata),                //   6
    &(MpcLegacyInfo.keydata),                 //  10
    &(MpcLegacyDali0.keydata),                //  13
    &(MpcLegacyDali1.keydata),                //  14
  };

  const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [MPC_ASTRO_NUMBER]            =  1,
    [MPC_INFO_NUMBER]             =  2,
    [0]                           =  3,
    [1]                           =  4,
  };

#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OT Outdoor 4DIM Generation 2" (started in 2016)
//
// Adjust these tables only in close cooperation with
// 1. DS D EMEA HW Treviso, Diego Fighera
// 2. DS D EMEA SW Garching, Joachim Eisenberg or Walter Limmer

#ifdef PLATFORM_OUTDOOR_4DIM_G2
  #pragma message ("Config PLATFORM_OUTDOOR_4DIM_G2")

  const mpc_keydata_t * MpcKeydata[] = {
    &(MSK_Descriptor.keydata),                // no  1          // no = index+1 as for DALI mapping array
    &(BIO_Descriptor.keydata),                // no  2
    &(MpcLegacyCsm.keydata),                  // no  3
    &(MpcLegacyClm.keydata),                  // no  4
    &(MpcLegacyTpm.keydata),                  // no  5
    &(MpcLegacyAstro.keydata),                // no  6
    &(MpcLegacyStep.keydata),                 // no  7
    &(MpcLegacyPresDet.keydata),              // no  8
    &(MpcLegacyMains.keydata),                // no  9
    &(MpcLegacyEmergency.keydata),            // no 10
    &(MpcLegacyDguard.keydata),               // no 11
    &(MpcLegacyTfm_pw1.keydata),              // no 12
    &(MpcLegacyEol.keydata),                  // no 13
    &(MpcLegacyInfo.keydata),                 // no 14
    &(MpcLegacyPower.keydata),                // no 15
    &(MpcLegacyDali0.keydata),                // no 16
    &(MpcLegacyDali1.keydata),                // no 17
    &(MpcLegacyDali_T6.keydata),              // no 18
    &(ValData_Description.keydata),           // no 19
    &(MpcLegacyDebug.keydata)                 // no 20
  };

  static const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = { // use "numbers" (not c indices startig from 0)
    [MPC_MSK_NUMBER]          =   1,
    // skip                       2,
    [MPC_CSM_NUMBER]          =   3,
    [MPC_CLM_NUMBER]          =   4,
    [MPC_TPM_NUMBER]          =   5,
    [MPC_ASTRO_NUMBER]        =   6,
    [MPC_STEP_NUMBER]         =   7,
    [MPC_PRESDETECT_NUMBER]   =   8,
    [MPC_MAINS_NUMBER]        =   9,
    [MPC_EMERGENCY_NUMBER]    =  10,
    [MPC_DGUARD_NUMBER]       =  11,
    [MPC_TFM_NUMBER]          =  12,
    [MPC_EOL_NUMBER]          =  13,
    [MPC_INFO_NUMBER]         =  14,
    [MPC_POWER_NUMBER]        =  15,
    [0]                       =  16,
    [1]                       =  17,
    // No Dali mapping for page  18,(Dali-internal data pseudo-Mpc)
    [MPC_VALDATA_NUMBER]      =  19,
    [MPC_DEBUG_NUMBER]        =  20
  };
#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Platform "OT Outdoor 1DIM Generation 1" (started in 2017)
// Nearly the same as PLATFORM_OUTDOOR_4DIM_G2, but some pages are replaced by copies
//  - with a general write protection
//  - without a DALI mapping (as long as the DALI hardware is present, this might be a good idea)
//
// Adjust these tables only in close cooperation with
// 1. DS D EMEA HW Treviso, Diego Fighera
// 2. DS D EMEA SW Garching, Joachim Eisenberg or Walter Limmer

#ifdef PLATFORM_OUTDOOR_1DIM_G1
  #pragma message ("Config PLATFORM_OUTDOOR_1DIM_G1")

  const mpc_keydata_t * MpcKeydata[] = {
    &(MSK_Descriptor.keydata),                // no  1, length =  28
    &(BIO_Descriptor.keydata),                // no  2, length =  20
    &(MpcLegacyCsm.keydata),                  // no  3, length =   8
    &(MpcLegacyClm.keydata),                  // no  4, length =  17
    &(MpcLegacyTpm.keydata),                  // no  5, length =  23
    &(MpcLegacyAstro.keydata),                // no  6, length =  26
    &(MpcLegacyStep_read_only.keydata),       // no  7, length =   9
    &(MpcLegacyPresDet_read_only.keydata),    // no  8, length =   6
    &(MpcLegacyMains_read_only.keydata),      // no  9, length =   6
    &(MpcLegacyEmergency_read_only.keydata),  // no 10, length =   3
    &(MpcLegacyDguard.keydata),               // no 11, length =   6
    &(MpcLegacyTfm_pw1.keydata),              // no 12, length =   7
    &(MpcLegacyEol_read_only.keydata),        // no 13, length =   3
    &(MpcLegacyInfo.keydata),                 // no 14, length =  12
    &(MpcLegacyPower.keydata),                // no 15, length =   4
    &(MpcLegacyDali0.keydata),                // no 16, length =  25
    &(MpcLegacyDali1.keydata),                // no 17, length =  60
    &(MpcLegacyDali_T6.keydata),              // no 18, length =  29
    &(ValData_Description.keydata),           // no 19, length =  70
    &(MpcLegacyDebug.keydata),                // no 20, length = 132
  };

  static const uint8_t dali_mpc_mapping[MPC_MAX_NUMBER + 1] = {
    [MPC_MSK_NUMBER]          =   1,
    // skip                       -,
    [MPC_CSM_NUMBER]          =   3,
    [MPC_CLM_NUMBER]          =   4,
    [MPC_TPM_NUMBER]          =   5,
    [MPC_ASTRO_NUMBER]        =   6,
    //[MPC_STEP_NUMBER]       =   7,
    //[MPC_PRESDETECT_NUMBER] =   8,
    //[MPC_MAINS_NUMBER]      =   9,
    //[MPC_EMERGENCY_NUMBER]  =  10,
    [MPC_DGUARD_NUMBER]       =  11,
    [MPC_TFM_NUMBER]          =  12,
    //[MPC_EOL_NUMBER]        =  13,
    [MPC_INFO_NUMBER]         =  14,
    [MPC_POWER_NUMBER]        =  15,
    [0]                       =  16,
    [1]                       =  17,
    // No Dali mapping for page  18 (Dali-internal data pseudo-Mpc)
    [MPC_VALDATA_NUMBER]      =  19,
    [MPC_DEBUG_NUMBER]        =  20
  };
#endif


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
#ifdef PLATFORM_NON_STANDARD
  /**********************************************************************************************************************
   *
   * In case of, this C-file (!) is included here to have a low-barrier entry point for individual configurations
   * in special setups as UNIT TESTS or during early development stages of a new platform.
   *
   * It is not intended to use this mechanism for production grade deliveries. Please adjust with the module
   * maintainers of Mpc Alloc (Info.txt of the module) to integrate a real-life platform in the common
   * code base to give the lib maintainers a little chance for an overview on what's going on.
   *
   *
   * All generic includes already are present from the beginning of this file, just do:
   * - define your specific descriptors (according includes might be needed) in the included file
   * - insert there the arrays for keydata pointers and, in case of, the Dali and Nfc mapping.
   * - MPC_*_NUMBERS, MPC_COUNT and MPC_MAX_NUMBER normally are defined by MpcDefaultConfiguration.h:
   *   These defines have to be done in the individual config file instead.
   *
   * A sample MpcAllocation.c.in-file can be found in the directory ^/Test of the Mpc Alloc module.
   *
   *********************************************************************************************************************/
#include <MpcAllocation.c.in>
#endif


/** \brief Returns the mpc_nr (= index of the MPC in MpcKeydata[]).
 *
 * Evaluates port to decide whether item_nr is a mpc_nr (index in keydata array) or a Memorybank-number (DALI mapping).
 * Always return a mpc_nr, but which might be out of range. Especially -1 flags an invalid DALI mapping
 *
 *  \param item_nr          MPC-Number or MemoryBank-Number according to port
 *  \param port mpc_port_t  Port from where the request comes to decide on the type of item_nr ( MPC_PORT_DALI ! )
 *
 *  \retval -1: For port = MPC_PORT_DALI: no valid DALI mapping
 *         >=0: mpc_nr (index to MpcKeydata-array) when item_nr is a valid DALI mapping
 *
 */
int MpcDissolveMapping(int item_nr, mpc_port_t port) {
  int32_t mpc_nr = 0;
  if (port == MPC_PORT_DALI) {
    STATIC_ASSERT( MPC_MAX_NUMBER + 1 == (sizeof(dali_mpc_mapping)/sizeof(uint8_t)), "MPC_MAX_NUMBER issue");
    if ( (item_nr >= 0) && (item_nr <= MPC_MAX_NUMBER)) {  // '<=' is ok, as the array size is MPC_MAX_NUMBER + 1
      mpc_nr = dali_mpc_mapping[item_nr] - 1; // in the array, the indices have an offset of 1 (init values are 0)
    } else {
      mpc_nr = -1;
    }
  } else { // port is not DALI
    mpc_nr = item_nr;
  }
  return mpc_nr;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/** \brief Check mpc_nr and, if valid, return a pointer to the Mpc description struct.
 *
 * \param mpc_nr  Mpc number to allocate; Checcked for the range [0; MPC_COUNT]
 *
 * \retval const mpc_descriptor_t * to the keydata struct, NULL when Mpc (no further Mpc) does exist.
 */
const mpc_descriptor_t * MpcDescriptorGet(int mpc_nr) {
  const mpc_descriptor_t * mpcDesc  = NULL; // Initialization with NULL part of algorithm
  STATIC_ASSERT( MPC_COUNT == (sizeof(MpcKeydata) / sizeof(mpc_keydata_t *)),    "MPC_COUNT/Keydata array issue");
  if ( (mpc_nr >= 0) && (mpc_nr < MPC_COUNT) ) { // can we safely access the array?
    // Ensure that we won't be hit by this pointer hack! Keydata always has to be the first component of the descriptor
    STATIC_ASSERT( offsetof(mpc_descriptor_t, keydata) == 0, "Major issue with mpc internal struct typedefs");
    mpcDesc = (mpc_descriptor_t *) MpcKeydata[mpc_nr];
  }
  return mpcDesc;
};



/** \} */
