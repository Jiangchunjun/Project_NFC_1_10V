#ifndef __I2C_CONFIG_H__
#define __I2C_CONFIG_H__

/** ----------------------------------------------------------------------------
// i2c_config.h
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
// Initial version:
//
// $Author: g.salvador $
// $Revision: 12832 $
// $Date: 2018-04-11 22:26:14 +0800 (Wed, 11 Apr 2018) $
// $Id: i2c_config.h 12832 2018-04-11 14:26:14Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_config.h $
//
// Header file required by the user to get access to the array used for NFC chip
// Sectors protection

* \addtogroup I2cModule
* \{
* \file
* \brief User header file for i2c_config.c
*\}
*/


//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------
/**
 * I2C
 * external defined to minimize ram size if I2C is part of a library
 */
extern uint8_t I2cSectorProtection[];


#endif // __I2C_CONFIG_H__
