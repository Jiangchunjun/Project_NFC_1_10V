// -----------------------------------------------------------------------------
#ifndef __CRC16_H__
#define __CRC16_H__

// crc16.h
// -----------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 4 spaces per indentation level
//
// Initial version: Sammy el Baradie,  2016-12
//
// $Author: g.salvador $
// $Revision: 16296 $
// $Date: 2018-12-19 22:00:30 +0800 (Wed, 19 Dec 2018) $
// $Id: crc16.h 16296 2018-12-19 14:00:30Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Crc16/tags/v1.0/Src/crc16.h $
//
// -----------------------------------------------------------------------------

#include <stdint.h>


//------------------------------------------------------------------------------
// parameters
//------------------------------------------------------------------------------
#define CRC16_CCITT_FALSE_START_VALUE                                     0xFFFF

//------------------------------------------------------------------------------
// enums
//------------------------------------------------------------------------------
typedef enum
{
    crc16_big_endian,
    crc16_little_endian
} crc16_endianness_t;

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// global functions
//------------------------------------------------------------------------------
int8_t Crc16Init_Inst(uint8_t instance, uint16_t start_value);
void Crc16AppendByte_Inst(uint8_t instance, uint8_t data_byte);
void Crc16AppendByteArray_Inst(uint8_t instance, uint8_t *data, uint8_t length);
uint16_t Crc16Checksum_Inst(uint8_t instance, crc16_endianness_t endianness);

void Crc16Init(uint16_t start_value);
void Crc16AppendByte(uint8_t data_byte);
void Crc16AppendByteArray(uint8_t *data, uint8_t length);
uint16_t Crc16Checksum(crc16_endianness_t endianness);

#endif // __CRC16_H__
