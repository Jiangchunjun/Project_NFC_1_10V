
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

#ifndef __O2T_PRIVATE_H
#define __O2T_PRIVATE_H

/** \addtogroup O2T zero2ten
 * \{
 *
 * \file
 * \brief Header file
 */

/* Private makro -------------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
//------------------------------------------------------------------------------
/// \brief Default settings, used if MPC_O2T_DEFAULT_SETTINGS / MPC_O2T_BOUNDARIES_SETTINGS is defined by the user.
///        Otherwise the user has to define these values himself.
//------------------------------------------------------------------------------
#ifdef MPC_O2T_DEFAULT_SETTINGS
  #define MPC_O2T_CONFIG_ENABLED            1
  #define MPC_O2T_CONFIG_MINDIMLEVEL        3276
#endif
#ifdef MPC_O2T_DEFAULT_BOUNDARIES
  #define MPC_O2T_CONFIG_MINDIMLEVEL_MIN        3276
  #define MPC_O2T_CONFIG_MINDIMLEVEL_MAX        32768 
#endif
/* Private typedef -----------------------------------------------------------*/
/** \brief Struct to store one node for the zero2ten characteristic
 */
typedef struct {
  struct {
    uint8_t enable;
    uint16_t minDimLevel;
  } nvm;
} zero2ten_config_t;
/* Private variables ---------------------------------------------------------*/
extern zero2ten_config_t o2t;
#endif
/** \} */