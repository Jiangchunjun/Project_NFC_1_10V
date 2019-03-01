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
// $Revision: 9844 $
// $Date: 2017-09-19 15:26:38 +0800 (Tue, 19 Sep 2017) $
// $Id: MpcPerm4Legacy.h 9844 2017-09-19 07:26:38Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcPerm4Legacy.h $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 * \file
 * \brief Deklaration of Legacy permission arrays
 *
 *
 */

#ifndef MPCPERM4LEGACY_H
#define MPCPERM4LEGACY_H
/*****************************************************
 * Order of needed includes before:
  #include <Global.h>
  #include "Config.h"
 ****************************************************/



#define DEBUGPRINT(...)
// #define DEBUGPRINT(...)         printf(__VA_ARGS__)

#ifdef MPC_TFM_NUMBER
extern uint8_t tfmpermissions[];
#endif

#ifdef MPC_STEP_NUMBER
extern uint8_t steppermissions[];
#endif

#ifdef MPC_ASTRO_NUMBER
extern uint8_t astropermissions[];
#endif

#ifdef MPC_MAINS_NUMBER
extern uint8_t mainspermissions[];
#endif

#ifdef MPC_PRESDETECT_NUMBER
extern uint8_t presdetectpermissions[];
#endif
#endif

/** \} */
