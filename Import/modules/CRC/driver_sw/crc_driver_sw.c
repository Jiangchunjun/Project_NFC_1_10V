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
// $Id: crc_driver_sw.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/CRC/driver_sw/crc_driver_sw.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \addtogroup CrcDriver
 * \{
 * \defgroup CrcDriverSw CRC driver - SW emulation
 * \brief Software driver emulation for CRC peripheral 
 * \{
 *
 * \file
 * \brief Central functions for CRC peripheral emulation.
 */

#define MODULE_CRC
#include "Config.h"

#include "Global.h"
#include "crc_driver.h"

/** ********************************************************************************************************************
* \brief Table for CRC calculation
*
***********************************************************************************************************************/

const uint32_t CrcTable[] =		    // Nibble lookup table for 0x04C11DB7 polynomial
{

	0x00000000,0x04C11DB7,0x09823B6E,0x0D4326D9,
	0x130476DC,0x17C56B6B,0x1A864DB2,0x1E475005,
	0x2608EDB8,0x22C9F00F,0x2F8AD6D6,0x2B4BCB61,
	0x350C9B64,0x31CD86D3,0x3C8EA00A,0x384FBDBD

};

/** ********************************************************************************************************************
* \brief CRC
*
***********************************************************************************************************************/

static uint32_t crc = 0xFFFFFFFF;

/** ********************************************************************************************************************
* \brief CRC driver init - sw emulation
*
* \warning Reset CRC to 0xFFFFFFFF.
***********************************************************************************************************************/

void crc_drv_init( void )
{
	crc = 0xFFFFFFFF;
}

/** ********************************************************************************************************************
* \brief Resets CRC to 0xFFFFFFFF
*
***********************************************************************************************************************/

void crc_drv_reset( void )
{
	crc = 0xFFFFFFFF;
}

/** ********************************************************************************************************************
* \brief Computes the 32-bit CRC of a given data word.
*
* \param data
*        word to be added
*
* \retval CRC
***********************************************************************************************************************/

uint32_t crc_drv_calc(uint32_t data)
{
	crc = crc ^ data; // Apply all 32-bits

	// Process 32-bits, 4 at a time, or 8 rounds

	crc = (crc << 4) ^ CrcTable[crc >> 28]; // Assumes 32-bit reg, masking index to 4-bits
	crc = (crc << 4) ^ CrcTable[crc >> 28]; //  0x04C11DB7 Polynomial used in STM32
	crc = (crc << 4) ^ CrcTable[crc >> 28];
	crc = (crc << 4) ^ CrcTable[crc >> 28];
	crc = (crc << 4) ^ CrcTable[crc >> 28];
	crc = (crc << 4) ^ CrcTable[crc >> 28];
	crc = (crc << 4) ^ CrcTable[crc >> 28];
	crc = (crc << 4) ^ CrcTable[crc >> 28];

	return(crc);
}

/** ********************************************************************************************************************
* \brief Gets actual 32-bits CRC
*
* \retval CRC
***********************************************************************************************************************/


uint32_t crc_drv_get(void)
{
	return crc;
}

/**********************************************************************************************************************/

/** \} */ // CrcDriverSw
/** \} */ // CrcDriver