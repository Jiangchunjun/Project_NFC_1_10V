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
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: crc_driver.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/CRC/driver_api/crc_driver.h $
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