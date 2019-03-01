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
// $Author: m.nordhausen $
// $Revision: 10979 $
// $Date: 2017-11-28 15:10:46 +0800 (Tue, 28 Nov 2017) $
// $Id: MpcIncludeAll.h 10979 2017-11-28 07:10:46Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcIncludeAll.h $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 * \file
 * \brief Include all single Mpc headers.
 *
 * Include a header only when the according allocation is defined.
 */

#ifndef MPC_INCLUDE_ALL_H__
#define MPC_INCLUDE_ALL_H__


#define __STRINGIFY_NS_MPC(TEXT) #TEXT
#define __MESSAGE_NS_MPC(TEXT)   _Pragma(__STRINGIFY_NS_MPC(message (#TEXT)))


#define MPC_DEFINE_INFO(TEXT)
// #define MPC_DEFINE_INFO(TEXT) __MESSAGE_NS_MPC(Defined #TEXT = TEXT)

#ifdef MPCs_DALI
  MPC_DEFINE_INFO(0)                      // Dali MemoryBank 0
  MPC_DEFINE_INFO(1)                      // Dali MemoryBank 1
  MPC_DEFINE_INFO(MPC_DALI_NUMBER)        // MPC of Dali parameters (not available as Dali MemoryBank)
  #include "MemoryBanks_Dali.h"
  #include "MemoryBank_DaliParams.h"
#endif

#if defined(MPC_MSK_NUMBER) || defined(MSK_PAGE)
  MPC_DEFINE_INFO(MPC_MSK_NUMBER)
#include "Mpc.h"
#include "MpcInternal.h"
#include "MpcAllocation.h"
  #include "MSK.h"
  #define NFC_TAG_MSK_TABLE_ENTRY         NFC_TAG_MSK_LOCATION ,        // yes, there is a entry (value to be calculated separately)
  #define NFC_TAG_MSK_RAW_LENGHT          MSK_PAYLOAD_LENGTH            // raw length without crc or padding
#else
  #define NFC_TAG_MSK_TABLE_ENTRY         // no entry for a non existing page
  #define NFC_TAG_MSK_RAW_LENGHT          // as well as no length
#endif

// Not existent for the time being, just to prove NFC tag allocation for this case
#if defined(HAS_MPC_AUTH)
  MPC_DEFINE_INFO(MPC_AUTH_DALI_MAPPING)
  #include "Auth.h"
  #define NFC_TAG_AUTH_TABLE_ENTRY        NFC_TAG_AUTH_LOCATION ,       // yes, there is a entry (value to be calculated separately)
  #define NFC_TAG_AUTH_RAW_LENGHT         AUTH_PAYLOAD_LENGTH           // raw length without crc or padding
#else
  #define NFC_TAG_AUTH_TABLE_ENTRY        // no entry for a non existing page
  #define NFC_TAG_AUTH_RAW_LENGHT         // as well as no length
#endif

#if defined(MPC_CSM_NUMBER) || defined(MPC_CSM_NUMBER_WP)
  MPC_DEFINE_INFO(MPC_CSM_NUMBER)
  #include "MemoryBank_Csm.h"
  #define CSM_PAYLOAD_LENGTH             (MPC_CurrentSetting_LastADDRESS - 4)
  #define NFC_TAG_CSM_TABLE_ENTRY         NFC_TAG_CSM_LOCATION ,        // yes, there is a entry (value to be calculated separately)
  #define NFC_TAG_CSM_RAW_LENGHT          CSM_PAYLOAD_LENGTH            // raw length without crc or padding
#else
  #define NFC_TAG_CSM_TABLE_ENTRY         // no entry for a non existing page
  #define NFC_TAG_CSM_RAW_LENGHT          // as well as no length
#endif

#ifdef MPC_CLM_NUMBER
  MPC_DEFINE_INFO(MPC_CLM_NUMBER)
  #include "MemoryBank_Clm.h"
  #define CLM_PAYLOAD_LENGTH             (MPC_ConstantLumen_LastADDRESS - 4)
  #define NFC_TAG_CLM_TABLE_ENTRY         NFC_TAG_CLM_LOCATION ,        // yes, there is a entry (value to be calculated separately)
  #define NFC_TAG_CLM_RAW_LENGHT          CLM_PAYLOAD_LENGTH            // raw length without crc or padding
#else
  #define NFC_TAG_CLM_TABLE_ENTRY         // no entry for a non existing page
  #define NFC_TAG_CLM_RAW_LENGHT          // as well as no length
#endif

#ifdef MPC_EMERGENCY_NUMBER
  MPC_DEFINE_INFO(MPC_EMERGENCY_NUMBER)
  #include "MemoryBank_Emergency.h"
#endif

#ifdef MPC_INFO_NUMBER
  MPC_DEFINE_INFO(MPC_INFO_NUMBER)
  #include "MemoryBank_Info.h"
#endif

#ifdef MPC_PRESDETECT_NUMBER
MPC_DEFINE_INFO(MPC_PRESDETECT_NUMBER)
#include "MemoryBank_PresDet.h"
#endif

#ifdef  MPC_PASSWORD1_NUMBER
  MPC_DEFINE_INFO(MPC_PASSWORD1_NUMBER)
  MPC_DEFINE_INFO(MPC_PASSWORD2_NUMBER)
  #include "MemoryBank_Password.h"
#endif

#ifdef MPC_OT_NUMBER
  MPC_DEFINE_INFO(MPC_OT_NUMBER)
  #include "MemoryBank_Ot.h"
#endif

#ifdef MPC_POWER_NUMBER
  MPC_DEFINE_INFO(MPC_POWER_NUMBER)
  #include "MemoryBank_Power.h"
#endif

#ifdef MPC_TDC_NUMBER
  MPC_DEFINE_INFO(MPC_TDC_NUMBER)
  #include "TouchDim_MPC.h"
#endif

#ifdef MPC_DIM2DARK_NUMBER
  MPC_DEFINE_INFO(MPC_DIM2DARK_NUMBER)
  #include "MemoryBank_Dim2Dark.h"
#endif

#ifdef MPC_GENERALFEATURES_NUMBER
  MPC_DEFINE_INFO(MPC_GENERALFEATURES_NUMBER)
  #include "MemoryBank_GeneralFeatures.h"
#endif

#ifdef MPC_DEBUG_NUMBER
  MPC_DEFINE_INFO(MPC_DEBUG_NUMBER)
  // To use the debug bank, the *appplication* (not the library) has to provide DebugMpc.h!
  #include "DebugMpc.h"
#endif

//--- O u t d o o r   M e m o r y   B a n k s ---------------------------------------------------------------------------

#ifdef MPC_ASTRO_NUMBER
  MPC_DEFINE_INFO(MPC_ASTRO_NUMBER)
  #include "MemoryBank_Astro.h"
#endif

#ifdef MPC_EOL_NUMBER
  MPC_DEFINE_INFO(MPC_EOL_NUMBER)
  #include "MemoryBank_Eol.h"
#endif

#ifdef MPC_MAINS_NUMBER
  MPC_DEFINE_INFO(MPC_MAINS_NUMBER)
  #include "MemoryBank_Mains.h"
#endif

#ifdef MPC_STEP_NUMBER
  MPC_DEFINE_INFO(MPC_STEP_NUMBER)
  #include "MemoryBank_Step.h"
#endif

#ifdef MPC_TPM_NUMBER
  MPC_DEFINE_INFO(MPC_TPM_NUMBER)
  #include "MemoryBank_Tpm.h"
#endif

#ifdef MPC_TFM_NUMBER
  MPC_DEFINE_INFO(MPC_TFM_NUMBER)
  #include "MemoryBank_Tfm.h"
#endif

#ifdef MPC_DGUARD_NUMBER
  MPC_DEFINE_INFO(MPC_DGUARD_NUMBER)
  #include "MemoryBank_Dguard.h"
#endif

#ifdef MPC_VALDATA_NUMBER
  MPC_DEFINE_INFO(MPC_VALDATA_NUMBER)
  #include "ValData.h"
#endif


MPC_DEFINE_INFO(MPC_COUNT)
MPC_DEFINE_INFO(MPC_MAX_NUMBER)

#endif
/** \}*/
