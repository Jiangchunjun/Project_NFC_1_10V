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
// Initial version: 2016-07, W.Limmer@osram.de
//
// Change History:
//
// $Author: w.limmer $
// $Revision: 11759 $
// $Date: 2018-02-02 01:27:53 +0800 (Fri, 02 Feb 2018) $
// $Id: MpcDefaultConfiguration.h 11759 2018-02-01 17:27:53Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcDefaultConfiguration.h $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 *
 * \file
 * \brief Authoriative definition of the existence and DALI mapping of Membank/Mpc of the aligned platforms.
 *
 * As said, this file provides only the existence, dali mapping and count of the banks, but no parameters for them.
 * The intention is to have this consolidated at one point without any overhead as data types or other higher
 * level information as provided with MpcAllocation.h or similar.
 *
 * Also the sequence of them (in snese of MpcGeneric and NFC) is not defined here, but can be found in
 * MpcAllocation.c, where the tables are layed down in flash memory.
 */

#ifndef __MPC_DEFAULT_CONFIG_H
#define __MPC_DEFAULT_CONFIG_H


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Real life platforms

#ifdef PLATFORM_EMEA_INDOOR_NONISOLATED

  #define MPC_MAX_NUMBER                         50   // Highest membank number in the DALI address room
  #define MPC_COUNT                              15   // Number of Mpcs allocated

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_MSK_NUMBER                          2
  #define MPC_CSM_NUMBER                          3
  #define MPC_CLM_NUMBER                          4
  #define MPC_BIO_NUMBER                          6
  #define MPC_DIM2DARK_NUMBER                    14
  #define MPC_GENERALFEATURES_NUMBER             15
  #define MPC_EMERGENCY_NUMBER                   17
  #define MPC_DGUARD_NUMBER                      18
  #define MPC_INFO_NUMBER                        29
  #define MPC_POWER_NUMBER                       30
  #define MPC_TDC_NUMBER                         31
  #define MPC_VALDATA_NUMBER                     50

#endif


// ---------------------------------------------------------------------------------------------------------------------
#ifdef PLATFORM_EMEA_INDOOR_ISOLATED_G2

  #define MPC_MAX_NUMBER                         31   // Highest membank number in the DALI address room
  #define MPC_COUNT                              15   // Number of Mpcs allocated

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_MSK_NUMBER                          2
  #define MPC_CSM_NUMBER                          3
  #define MPC_CLM_NUMBER                          4
  #define MPC_BIO_NUMBER                          6
  #define MPC_DIM2DARK_NUMBER                    14
  #define MPC_GENERALFEATURES_NUMBER             15
  #define MPC_EMERGENCY_NUMBER                   17
  #define MPC_DGUARD_NUMBER                      18
  #define MPC_TFM_NUMBER                         19
  #define MPC_INFO_NUMBER                        29
  #define MPC_POWER_NUMBER                       30
  #define MPC_TDC_NUMBER                         31

#endif


// ---------------------------------------------------------------------------------------------------------------------
#if defined PLATFORM_EMEA_CONSTANT_VOLTAGE_FS // FS = FlexSignage

  #define MPC_MAX_NUMBER                         29   // Highest membank number in the DALI address room
  #define MPC_COUNT                               4   // Number of Mpcs allocated

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_ASTRO_NUMBER                       10
  #define MPC_INFO_NUMBER                        29

#endif


// ---------------------------------------------------------------------------------------------------------------------
#if defined PLATFORM_OUTDOOR_1DIM_G1

  #define MPC_MAX_NUMBER                         50   // Highest membank number in the DALI address room
  #define MPC_COUNT                              20   // Number of Mpcs allocated

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_MSK_NUMBER                          2
  #define MPC_CSM_NUMBER                          3
  #define MPC_CLM_NUMBER                          4
  #define MPC_TPM_NUMBER                          5
  // #define MPC_BIO_NUMBER                          6
  #define MPC_ASTRO_NUMBER                       10
  #define MPC_STEP_NUMBER                        11   // read-only in 1DIM
  #define MPC_PRESDETECT_NUMBER                  12   // read-only in 1DIM
  #define MPC_MAINS_NUMBER                       13   // read-only in 1DIM
  #define MPC_EMERGENCY_NUMBER                   17   // read-only in 1DIM
  #define MPC_DGUARD_NUMBER                      18
  #define MPC_TFM_NUMBER                         19
  #define MPC_EOL_NUMBER                         21   // read-only in 1DIM
  #define MPC_DEBUG_NUMBER                       25
  #define MPC_INFO_NUMBER                        29
  #define MPC_POWER_NUMBER                       30
  #define MPC_VALDATA_NUMBER                     50

#endif


// ---------------------------------------------------------------------------------------------------------------------
#if defined PLATFORM_OUTDOOR_4DIM_G2

  #define MPC_MAX_NUMBER                         50   // Highest membank number in the DALI address room
  #define MPC_COUNT                              20   // Number of Mpcs allocated

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_MSK_NUMBER                          2
  #define MPC_CSM_NUMBER                          3
  #define MPC_CLM_NUMBER                          4
  #define MPC_TPM_NUMBER                          5
  // #define MPC_BIO_NUMBER                          6
  #define MPC_ASTRO_NUMBER                       10
  #define MPC_STEP_NUMBER                        11   // read-only in 1DIM
  #define MPC_PRESDETECT_NUMBER                  12   // read-only in 1DIM
  #define MPC_MAINS_NUMBER                       13   // read-only in 1DIM
  #define MPC_EMERGENCY_NUMBER                   17   // read-only in 1DIM
  #define MPC_DGUARD_NUMBER                      18
  #define MPC_TFM_NUMBER                         19
  #define MPC_EOL_NUMBER                         21   // read-only in 1DIM
  #define MPC_DEBUG_NUMBER                       25
  #define MPC_INFO_NUMBER                        29
  #define MPC_POWER_NUMBER                       30
  #define MPC_VALDATA_NUMBER                     50

#endif


// ---------------------------------------------------------------------------------------------------------------------
#ifdef PLATFORM_EMEA_INDOOR_NONISOLATED_TW

  #define USES_NEW_TABLE_MPCS                      // \todo nutzen checken

  #define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
  #define MPC_MSK_NUMBER                          2
  #define MPC_CLM_NUMBER                          4
  #define MPC_EMERGENCY_NUMBER                   17
  #define MPC_DGUARD_NUMBER                      18
  #define MPC_INFO_NUMBER                        29
  #define MPC_POWER_NUMBER                       30
  #define MPC_TDC_NUMBER                         31
  #define MPC_VALDATA_NUMBER                     50

  // #include "ConfigMpc_TW.h"

#endif





// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// Test platforms; Move this to private locations soon, but not before the new config scheme is in place

#ifdef PLATFORM_TEST_MSK_MPC_ACL
  #define MPC_COUNT         3
  #define MSK_PAGE          5
  #define MPC_CSM_NUMBER    6
#endif

#ifdef PLATFORM_DALI_INDOOR_SAMPLE_OTI15
  #define MPC_COUNT        8    ///< Number of Mpcs allocated (e.g. for TOC size of NFC)
#endif

#ifdef PLATFORM_DALI_OUTDOOR_SAMPLE_4DIM
  #define MPC_COUNT       16    ///< Number of Mpcs allocated (e.g. for TOC size of NFC)
#endif

#ifdef PLATFORM_DALI_OUTDOOR_SAMPLE_4DIM_TEMP
  #define MPC_COUNT        8    ///< Number of Mpcs allocated (e.g. for TOC size of NFC)
#endif


#endif // __MPC_DEFAULT_CONFIG_H

/** \} */
