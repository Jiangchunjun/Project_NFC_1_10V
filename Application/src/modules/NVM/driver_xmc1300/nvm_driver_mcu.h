// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram spa
//     Via Castagnole 65/a
//     31100 Treviso (I)
//
//
// The content of this file is intellectual property of OSRAM spa. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2014-12, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 970 $
// $Date: 2015-07-08 21:02:46 +0800 (Wed, 08 Jul 2015) $
// $Id: nvm_driver_mcu.h 970 2015-07-08 13:02:46Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/driver_xmc1300/nvm_driver_mcu.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_DRIVER_MCU_H
#define __NVM_DRIVER_MCU_H

/** \addtogroup NvmDriverXmc1300
 * \{
 *
 * \file
 * \brief Configuration file for Nvm Xmc1300 driver
 */

/**********************************************************************************************************************/

#define NVM_DRV_XMC1300

/**********************************************************************************************************************/

#define NVM_BASE_ADDR                               (((uint32_t)0x10001000))

#define NVM_WPAGE_SIZE_BYTE                         (16)

#define NVM_EPAGE_SIZE_BYTE                         (256)

#define NVM_EPAGE_CYCLES                            (50000)

#define NVM_SIZE_BYTE                               (1024*NVM_SIZE_KBYTE)


/**********************************************************************************************************************/
/** \} */
#endif