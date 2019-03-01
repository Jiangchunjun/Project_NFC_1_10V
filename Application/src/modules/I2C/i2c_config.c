/** ----------------------------------------------------------------------------
// i2c_config.c
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
// Initial version: ,
//
// $Author: g.salvador $
// $Revision: 16420 $
// $Date: 2019-01-10 21:05:27 +0800 (Thu, 10 Jan 2019) $
// $Id: i2c_config.c 16420 2019-01-10 13:05:27Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_config.c $
//
// Contains the array used for NFC chip Sectors protection

* \addtogroup I2cModule
* \{
*   \file
*   \brief Contains the array used for NFC chip Sectors protection
* \}
*/

#define MODULE_I2C
#include <stdint.h>
#include <stdbool.h>

#include "Global.h"

#include "Config.h"

#include <assert.h>


#include "i2c_driver.h"


//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
/**
 * array of NFC chip sector security status bytes
 * includes definition for password 1,2 or 3 control
 */
#if(defined (NFC_CONFIG_UNPROTECTED_LAYOUT) && defined (NFC_CONFIG_PROTECTED_LAYOUT))
  STATIC_ASSERT(0,  "Only ONE TAG protection layout configuration has to be choosen between\n "
                    "NFC_CONFIG_UNPROTECTED_LAYOUT and NFC_CONFIG_PROTECTED_LAYOUT.        \n "
                    "Please ensure to define only one of this in the entire code");
#endif

#if(defined (NFC_CONFIG_UNPROTECTED_LAYOUT)||defined(NFC_CONFIG_PROTECTED_LAYOUT))

#ifdef NFC_CONFIG_UNPROTECTED_LAYOUT
    uint8_t I2cSectorProtection[M24LR_SECTOR_COUNT] =
    {
      (M24LR_RF_ACCESS_RIGHT_1 | M24LR_RF_PASSWORD_1_CONTROL), // SECTOR 00 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_1 | M24LR_RF_PASSWORD_1_CONTROL), // SECTOR 01 ACCESS RIGHT
      0,                                                       // SECTOR 02 ACCESS RIGHT
      0,                                                       // SECTOR 03 ACCESS RIGHT
      0,                                                       // SECTOR 04 ACCESS RIGHT
      0,                                                       // SECTOR 05 ACCESS RIGHT
      0,                                                       // SECTOR 06 ACCESS RIGHT
      0,                                                       // SECTOR 07 ACCESS RIGHT
      0,                                                       // SECTOR 08 ACCESS RIGHT
      0,                                                       // SECTOR 09 ACCESS RIGHT
      0,                                                       // SECTOR 10 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 11 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 12 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 13 ACCESS RIGHT
      0,                                                       // SECTOR 14 ACCESS RIGHT
      0                                                        // SECTOR 15 ACCESS RIGHT
    };
#else
    uint8_t I2cSectorProtection[M24LR_SECTOR_COUNT] =
    {
      (M24LR_RF_ACCESS_RIGHT_1 | M24LR_RF_PASSWORD_1_CONTROL), // SECTOR 00 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_1 | M24LR_RF_PASSWORD_1_CONTROL), // SECTOR 01 ACCESS RIGHT
      0,                                                       // SECTOR 02 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 03 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 04 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 05 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 06 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 07 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 08 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 09 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 10 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 11 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 12 ACCESS RIGHT
      (M24LR_RF_ACCESS_RIGHT_3 | M24LR_RF_PASSWORD_2_CONTROL), // SECTOR 13 ACCESS RIGHT
      0,                                                       // SECTOR 14 ACCESS RIGHT
      0                                                        // SECTOR 15 ACCESS RIGHT
    };
#endif

#else
  STATIC_ASSERT(0,  "A TAG protection layout has to be choosen between NFC_CONFIG_UNPROTECTED_LAYOUT \n"
                    " and NFC_CONFIG_PROTECTED_LAYOUT. Please ensure that your project includes the \n"
                    " definition for the appropriate TAG protection layout. (usually in the Config.h)");
#endif

