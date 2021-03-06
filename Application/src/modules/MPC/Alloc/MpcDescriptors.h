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
// $Revision: 11657 $
// $Date: 2018-01-29 16:02:08 +0800 (Mon, 29 Jan 2018) $
// $Id: MpcDescriptors.h 11657 2018-01-29 08:02:08Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcDescriptors.h $
//


/** \addtogroup MPC
 * \{
 * \file
 * \brief Descriptors for legacy pages (which don't implement these descriptors by themselves)
 */

#ifndef MPC_LEGACY_DESCRIPTORS_H__
#define MPC_LEGACY_DESCRIPTORS_H__

extern const mpc_descriptor_t MpcLegacyDali0;
extern const mpc_descriptor_t MpcLegacyDali1;
extern const mpc_descriptor_t MpcLegacyDali1_with_perm_master;
extern const mpc_descriptor_t MpcLegacyDali_T6;
extern const mpc_descriptor_t MpcLegacyDebug;
extern const mpc_descriptor_t MpcLegacyInfo;
extern const mpc_descriptor_t MpcLegacyCsm_pw1;
extern const mpc_descriptor_t MpcLegacyCsm;
extern const mpc_descriptor_t MpcLegacyTfm_pw1;
extern const mpc_descriptor_t MpcLegacyTfm;
extern const mpc_descriptor_t MpcLegacyClm_pw1;
extern const mpc_descriptor_t MpcLegacyClm;
extern const mpc_descriptor_t MpcLegacyTpm;
extern const mpc_descriptor_t MpcLegacyEol;
extern const mpc_descriptor_t MpcLegacyEol_read_only;
extern const mpc_descriptor_t MpcLegacyMains;
extern const mpc_descriptor_t MpcLegacyMains_read_only;
extern const mpc_descriptor_t MpcLegacyPresDet;
extern const mpc_descriptor_t MpcLegacyPresDet_read_only;
extern const mpc_descriptor_t MpcLegacyStep;
extern const mpc_descriptor_t MpcLegacyStep_read_only;
extern const mpc_descriptor_t MpcLegacyPassword1;
extern const mpc_descriptor_t MpcLegacyPassword2;
extern const mpc_descriptor_t MpcLegacyEmergency;
extern const mpc_descriptor_t MpcLegacyEmergency_pw2;
extern const mpc_descriptor_t MpcLegacyEmergency_read_only;
extern const mpc_descriptor_t MpcLegacyPower;
extern const mpc_descriptor_t MpcLegacyDim2Dark_pw1;
extern const mpc_descriptor_t MpcLegacyDim2Dark;
extern const mpc_descriptor_t MpcLegacyGeneralFeatures_pw1;
extern const mpc_descriptor_t MpcLegacyGeneralFeatures;
extern const mpc_descriptor_t MpcLegacyTDC;
extern const mpc_descriptor_t MpcLegacyAstro_pw1;
extern const mpc_descriptor_t MpcLegacyAstro;
extern const mpc_descriptor_t MpcLegacyDguard;


//----------------------------------------------------------------------------------------------------------------------
// Legacy section;
// - allow refactoring of the descriptors
// - but not to break all the platforms
/// \todo remove all things here as soon as appropriate

// #define MpcLegacyDaliParams  MpcLegacyDali_T6
// #define MpcLegacyTfmWithPerm MpcLegacyTfm_pw1

#endif

/** \} */
