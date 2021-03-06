#ifndef __I2C_PARAMETERS_XMC1300_H__
#define __I2C_PARAMETERS_XMC1300_H__

/** ----------------------------------------------------------------------------
// i2c_parameters.h
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
//
// $Author: j.eisenberg $
// $Revision: 13350 $
// $Date: 2018-05-17 19:43:04 +0800 (Thu, 17 May 2018) $
// $Id: i2c_parameters_XMC1300.h 13350 2018-05-17 11:43:04Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_parameters_XMC1300.h $

* \addtogroup I2cDriverAPI
* \{
*   \file
*   \addtogroup I2cDriverXMC1300
*   \{
*       \file
*       \brief Local header file for i2c_driver_xmc1300.c
*   \}
* \}
*/

#define I2C_USIC_CH           		 XMC_USIC0_CH1

#define I2C_FIFO_TX_SIZE           4
#define I2C_FIFO_RX_SIZE           (I2C_FIFO_TX_SIZE - 1)

#define I2C_FIFO_TX_LIMIT          1
#define I2C_FIFO_RX_LIMIT          (1 << I2C_FIFO_RX_SIZE)
#define I2C_FIFO_TX_START          32
#define I2C_FIFO_RX_START          (I2C_FIFO_TX_START + (1 << I2C_FIFO_TX_SIZE))


// maximum length of a single read access to the NFC tag
// set to 256 for SAMD21!
// set to (I2C_FIFO_RX_LIMIT - 3) for XMC1300
// 3 bytes less than I2C_FIFO_RX_LIMIT, because they are needed for addressing
// the M24LR byte address
#define MAX_RX_TRANSFER_LENGTH     (I2C_FIFO_RX_LIMIT - 3)


#endif // __I2C_PARAMETERS_H__
