
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EC - MCC
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM, BU DS. It is confidential and not
// intended for any public release. All rights reserved.
//
// Within the company the policy of a strict "who needs to know?" applies: Store this only at locations,
// where EXCLUSIVELY people who really NEED it for their DAY-TO-DAY business can access.
//
//  - Convenience or to save it for occasion are no valid arguments. Get a fresh and updated copy when needed.
//  - Neither "departments shares" nor other potential local practices normally fulfill this policy.
//  - Please, especially be careful about backups, they often are forgotten and an usual origin of diffusion.
//  - Also be careful with code coverage reporting tools: They normally include the full sources, but the
//    results often are published widely.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Change history, major milestones and credits:
//   2018-12: Filippo Branchetti, Martin Hutzl
//
//
// $Author$
// $Revision$
// $Date$
// $Id$
// $URL$
//

#ifndef __O2T_MPC_HEADER
#define __O2T_MPC_HEADER

/** \addtogroup O2T zero2ten
 * \{
 *
 * \file
 * \brief Header file
 */
#include "MpcAllocation.h"
//------------------------------------------------------------------------------
/// \brief
/// Defines the MAJOR FW Version of the Modul
/// The MAJOR needs to be incremented when something bigger has changed
/// like the data alignment or functionality
/// ( T4T for example depends on this version)
//------------------------------------------------------------------------------
#define O2T_VERSION_MAJOR 0
//------------------------------------------------------------------------------
/// \brief
/// Needs to be incremented with every released ( tagged ) version of this module
//------------------------------------------------------------------------------
#define O2T_VERSION_MINOR 0

//------------------------------------------------------------------------------
/// \brief MPC ID
//------------------------------------------------------------------------------
#define O2T_MPC_ID 34

/* Public typedef -----------------------------------------------------------*/
/* Public variables ---------------------------------------------------------*/
#if defined( MODULE_MPC )
extern const mpc_descriptor_t O2T_Descriptor;
#endif
/* Public functions ----------------------------------------------------------*/
uint8_t O2T_GetEnableConfig( void );
uint16_t O2T_GetMinDimLevel( void );

/* Public define    ----------------------------------------------------------*/
#endif
/** \} */