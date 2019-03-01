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
// $Revision: 109 $
// $Date: 2015-01-07 18:46:31 +0800 (Wed, 07 Jan 2015) $
// $Id: crc_driver.h 109 2015-01-07 10:46:31Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Crc/tags/v1.3/driver_api/crc_driver.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __CRC_DRIVER_H
#define __CRC_DRIVER_H

/** \addtogroup CrcDriverAPI
 * \{
 *
 * \file
 * \brief Header file for crc_driver.c
 *
 */

/**********************************************************************************************************************/

void crc_drv_init(void);
void crc_drv_reset(void);
uint32_t crc_drv_calc(uint32_t data);
uint32_t crc_drv_get(void);

/**********************************************************************************************************************/
/** \} */ // CrcDriverAPI
#endif