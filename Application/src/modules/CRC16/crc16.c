// -----------------------------------------------------------------------------
// crc16.c
// -----------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: Sammy el Baradie,  27/07/2016
// $Author: g.salvador $
// $Revision: 16296 $
// $Date: 2018-12-19 22:00:30 +0800 (Wed, 19 Dec 2018) $
// $Id: crc16.c 16296 2018-12-19 14:00:30Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Crc16/tags/v1.0/Src/crc16.c $
//
// -----------------------------------------------------------------------------

/** \addtogroup Crc16
 * \{
 * \file
 * \brief Central functions for CRC 16 calculation
 * Table based implementation to calculate CRC-16 - CCITT
 * using the polynomial: 0x1021
 * start value: 0
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#include "Global.h"

#include "crc16.h"



#define MAX_CRC_INSTANCES                                                     5

//-----------------------------------------------------------------------------
// local constants
//-----------------------------------------------------------------------------
static const uint16_t crc16_table[] =
{
  0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50a5, 0x60c6, 0x70e7,
  0x8108, 0x9129, 0xa14a, 0xb16b, 0xc18c, 0xd1ad, 0xe1ce, 0xf1ef
};

//-----------------------------------------------------------------------------
// local variables
//-----------------------------------------------------------------------------

typedef struct
{
    uint16_t crc16;
    bool    is_busy;
} checksum_instance_t;

static checksum_instance_t checksum_instance[MAX_CRC_INSTANCES];

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 * \brief Initialize the given instance of the CRC-16 Checksum to start a new
 * calculation
 * \param instance    -> the instance of the CRC16 calculation used
 * \param start_value -> the start value for the CRC16 calculaton
 * \return -1 if the instance is out of range, the used instance otherwise
 */
int8_t Crc16Init_Inst(uint8_t instance, uint16_t start_value)
{
   // uint8_t i = 0;

    if( instance < MAX_CRC_INSTANCES)
    {
        //if(checksum_instance[instance].is_busy == false)
        {
            checksum_instance[instance].crc16 = start_value;
         //   checksum_instance[instance].is_busy = true;
            return instance;
        }
    }
    return -1;
}

//-----------------------------------------------------------------------------
/**
 * \brief Initialize the default instance (which is 0) of the CRC-16 Checksum to
 * start a new calculation
 * \param start_value
 * \note this is simply a wrapper function to mantain the compatibility with the
 * code that use previous versions of this module.
 */
void Crc16Init(uint16_t start_value)
{
    Crc16Init_Inst(0, start_value);
}


//-----------------------------------------------------------------------------
/**
 * \brief Update the given instance of the CRC-16 checksum with data byte
 * \param instance  -> the instance of the CRC16 calculation used
 * \param data_byte -> data byte to update CRC-16 checksum
 */
void Crc16AppendByte_Inst(uint8_t instance, uint8_t data_byte)
{
    // byte - high nibble
    checksum_instance[instance].crc16 =
            crc16_table[(checksum_instance[instance].crc16 >> 12)
                    ^ (data_byte >> 4)]
                    ^ (checksum_instance[instance].crc16 << 4);

    // byte - low nibble
    checksum_instance[instance].crc16 =
            crc16_table[(checksum_instance[instance].crc16 >> 12)
                    ^ (data_byte & 0xF)]
                    ^ (checksum_instance[instance].crc16 << 4);
}

//-----------------------------------------------------------------------------
/**
 * \brief Update the default instance (which is 0) of the CRC-16 checksum with
 * data byte
 * \param data_byte -> data byte to update CRC-16 checksum
 * \note this is simply a wrapper function to mantain the compatibility with the
 * code that use previous versions of this module.
 */
void Crc16AppendByte(uint8_t data_byte)
{
    Crc16AppendByte_Inst(0, data_byte);
}


//-----------------------------------------------------------------------------
/**
 * \brief Update the given instance of the CRC-16 checksum with data byte array
 * \param instance -> the instance of the CRC16 calculation used
 * \param data     -> pointer to data byte array
 * \param length   -> length of data byte array
 */
void Crc16AppendByteArray_Inst(uint8_t instance, uint8_t *data, uint8_t length)
{
    uint8_t i1;
    uint8_t *data_ptr;

    data_ptr = data;

    for (i1 = 0; i1 < length; ++i1)
    {
        Crc16AppendByte_Inst(instance, *(data_ptr++));
    }
}

//-----------------------------------------------------------------------------
/**
 * \brief Update the default instance (which is 0) of the CRC-16 checksum with
 * data byte array
 * \param data   -> pointer to data byte array
 * \param length -> length of data byte array
 * \note this is simply a wrapper function to mantain the compatibility with the
 * code that use previous versions of this module.
 */
void Crc16AppendByteArray(uint8_t *data, uint8_t length)
{
    Crc16AppendByteArray_Inst(0, data, length);
}

//-----------------------------------------------------------------------------
/**
 * \brief Return the given instance of the calculated CRC-16 checksum
 * \param instance   -> the instance of the CRC16 calculation used
 * \param endianness -> can be set to crc16_big_endian or crc16_little_endian
 * \return the calculated CRC-16 checksum
 * \note the endianness is calculated using the macro REV_BYTE_ORDER16
 * which is supported by the ARM assembler code for CORTEX M0
 */
uint16_t Crc16Checksum_Inst(uint8_t instance, crc16_endianness_t endianness)
{
    if(crc16_big_endian == endianness)
    {
        return REV_BYTE_ORDER16(checksum_instance[instance].crc16);
    }

    //checksum_instance[instance].is_busy = false;


    return checksum_instance[instance].crc16;
}

//-----------------------------------------------------------------------------
/**
 * \brief Return the default instance (which is 0) of the calculated CRC-16
 * checksum
 * \param endianness -> can be set to crc16_big_endian or crc16_little_endian
 * \return the calculated CRC-16 checksum
 * \note the endianness is calculated using the macro REV_BYTE_ORDER16
 * which is supported by the ARM assembler code for CORTEX M0
 */
uint16_t Crc16Checksum(crc16_endianness_t endianness)
{
    return Crc16Checksum_Inst(0, endianness);
}

//-----------------------------------------------------------------------------

/** \} */ // Crc16
