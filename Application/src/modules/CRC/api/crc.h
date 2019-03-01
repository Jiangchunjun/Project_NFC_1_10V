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
// $Revision: 108 $
// $Date: 2015-01-07 18:46:04 +0800 (Wed, 07 Jan 2015) $
// $Id: crc.h 108 2015-01-07 10:46:04Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Crc/tags/v1.3/api/crc.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __CRC_H
#define __CRC_H

/** \addtogroup CrcAPI
 * \{
 *
 * \file
 * \brief Header file for crc.c
 *
 */

//**********************************************************************************************************************

void crcInit ( void );

void crcReset ( void );

uint32_t crcCalcBuf ( const void *pdata, uint32_t sizeByte );

void crcLoadWord( uint32_t value );

void crcLoadByte( uint8_t value );

uint32_t crcGet ( void );

//**********************************************************************************************************************
/** \} */ // CrcAPI
#endif


