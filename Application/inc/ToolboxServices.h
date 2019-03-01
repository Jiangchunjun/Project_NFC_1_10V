// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
//  (c) Osram GmbH
//      DS D EC - MCC
//      Parkring 33
//      85748 Garching
//
//
//  The content of this file is intellectual property of OSRAM, BU DS. It is confidential and not
//  intended for any public release. All rights reserved.
//
//  Within the company the policy of a strict "who needs to know?" applies: Store this only at locations,
//  where EXCLUSIVELY people who really NEED it for their DAY-TO-DAY business can access.
//
//   - Convenience or to save it for occasion are no valid arguments. Get a fresh and updated copy when needed.
//   - Neither "departments shares" nor other potential local practices normally fulfill this policy.
//   - Please, especially be careful about backups, they often are forgotten and an usual origin of diffusion.
//   - Also be careful with code coverage reporting tools: They normally include the full sources, but the
//     results often are published widely.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Change history, major milestones and credits:
//    2017-10, W.Limmer@osram.de: Initial version, maintanance
//
//
// $Author: F.Branchetti $
// $Revision: 13732 $
// $Date: 2018-06-13 21:47:16 +0800 (Wed, 13 Jun 2018) $
// $Id: ToolboxServices.h 13732 2018-06-13 13:47:16Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/kernel/ToolboxServices.h $
//

#ifndef __TOOLBOX_SERVICES_H
#define __TOOLBOX_SERVICES_H


#define toolboxservices_version  00001 // 1.09.01


/** \addtogroup Kernel
 * \{
 * \defgroup ToolboxServices Toolbox Services
 * \{
 * \brief Declaration of functions needed by the Toolbox and other basic OT Library modules.
 *
 * This header file provides the declaration of generic functions needed by the Kernel and other OT library
 * modules, and which <b> the application and/or its hardware drivers have to provide</b>.
 *
 * It's a bit of a melting pot for different purposes, when no clear assignment (as with e.g. DaliServices.h)
 * is possible. Sometimes even the borderline to special hardware drivers or such is difficult.
 *
 * Include this file in any c-files implementing or using one of the functions.
 *
 * \file
 */



// ---------------------------------------------------------------------------------------------------------------------
/** \brief Provide the unique ID of the microcontroller.
 *
 * All microcontrollers currently supported (SAM D21, XMC1000, STM32) provide a "serial number", "unique chip ID",
 * "device electronic signature", "chip identification number", ... which is unique for every single chip.
 * Normally this ID is significantly longer than 4 bytes and may contain date code, lot number, x- and y-
 * coordinates of the chip in the wafer, package and temperature options or what ever.
 *
 * This function is to read this (hardware dependent) data and to shrink and to provide it as a (hardware
 * independent and maybe no longer completely unique) 32 bit integer.
 *
 * At the moment (2017-10), there are 2 main use cases
 * - The Osram Authentication feature implemented by MSK/BIO.
 *   Here it's used to generate a challenge for this authentication. By design decision, this
 *   challenge should be constant for one ballast, but different from ballast to ballast.
 *   Otherwise a very simple replay-attack to Osram authentication would be possible: Without
 *   different challenges all devices would have the same response (aka key or password) to unlock.
 * - Test setups can use this number to relate test results to a distinct hardware and/or to reveal
 *   when a device with a probale strange behaviour is tested more often.
 *
 * \note For Atmel SAM D21 there already is an implementation available. See
 *       `^/Platforms/10 Isolated/OTI_DALI_COMPACT_G2/Line/trunk/src/HW_Drv/SAMD21_Utils.c` for reference.
 *
 * \warning As mentioned above: A dummy for this function "returning only a constant" would \b compromise
 *          the \b Osram \b authentication for a platform.
 */
uint32_t  ControllerUniqueID (void);


/** \brief Do what is needed to switch on the NFC-Chip.
 *
 *  Toggles pin or what ever is needed.
 *  \note For Atmel SAM D21 there already is an implementation available. See
 *       `^/Platforms/10 Isolated/OTI_DALI_COMPACT_G2/Line/trunk/src/HW_Drv/SAMD21_Utils.c` for reference.
 */
void NfcTagPowerOn(void);

/** \brief Do what is needed to switch off the NFC-Chip.
 *
 *  Toggles pin or what ever is needed.
 *  \note For Atmel SAM D21 there already is an implementation available. See
 *       `^/Platforms/10 Isolated/OTI_DALI_COMPACT_G2/Line/trunk/src/HW_Drv/SAMD21_Utils.c` for reference.
 */
void NfcTagPowerOff(void);



/** \brief Returns the SVN Revision Number of the Firmware. Additional information, if it fits inside 32B bit is possible.
 *
 */
uint32_t getFWRevRoot(void);


/** \} defgroup KernelServices */
/** \} addtogroup Kernel */

#endif
