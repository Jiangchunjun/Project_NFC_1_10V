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
// $Revision: 969 $
// $Date: 2015-07-08 20:56:14 +0800 (Wed, 08 Jul 2015) $
// $Id: nvm_driver_mcu.h 969 2015-07-08 12:56:14Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/driver_stm32f0xx/nvm_driver_mcu.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_DRIVER_MCU_H
#define __NVM_DRIVER_MCU_H

/** \addtogroup NvmDriverStm32f0xx
 * \{
 *
 * \file
 * \brief Configuration file for Nvm Stm32f0xx driver
 */

/**********************************************************************************************************************/

#define NVM_DRV_STM32F0XX

/**********************************************************************************************************************/

#define NVM_BASE_ADDR                               (((uint32_t)0x08000000))

#define NVM_WPAGE_SIZE_BYTE							(2) //16 //64

#define NVM_EPAGE_SIZE_BYTE                         (1024)  //256 //256

#define NVM_EPAGE_CYCLES                            (10000)//50000//25000

#define NVM_SIZE_BYTE                               (1024*NVM_SIZE_KBYTE)

/**********************************************************************************************************************/
/** \} */
#endif