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
// $Id: crc.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/CRC/api/crc.h $
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


