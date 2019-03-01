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
// $Revision: 107 $
// $Date: 2015-01-07 11:43:06 +0100 (mer, 07 gen 2015) $
// $Id: nvm_driver_mcu.h 107 2015-01-07 10:43:06Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Toolbox1.0/Nvm/trunk/driver_xmc1300/nvm_driver_mcu.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_DRIVER_MCU_H
#define __NVM_DRIVER_MCU_H

/** \addtogroup NvmDriverSamD21
 * \{
 *
 * \file
 * \brief Configuration file for Nvm SamD21 driver
 */

/**********************************************************************************************************************/

#include <stdint.h>

/**********************************************************************************************************************/

#define NVM_DRV_SAMD21

/**********************************************************************************************************************/

#define NVM_BASE_ADDR                               (((uint32_t)0x00000000))

#define NVM_WPAGE_SIZE_BYTE                         (64)

#define NVM_EPAGE_SIZE_BYTE                         (256)

#define NVM_EPAGE_CYCLES                            (25000)

#define NVM_SIZE_BYTE                               (1024*NVM_SIZE_KBYTE)

/**********************************************************************************************************************/

uint32_t nvm_drv_get_eeprom_fuses( void );
void nvm_drv_set_eeprom_fuses(uint32_t eeprom_fuses);

/**********************************************************************************************************************/
/** \} */
#endif