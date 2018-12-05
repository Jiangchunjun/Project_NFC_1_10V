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
// $Id: i2c.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/I2cExt/api/i2c.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __I2C_H
#define __I2C_H

#ifdef MODULE_I2C

/** \addtogroup I2cAPI
 * \{
 *
 * \file
 * \brief Header file for i2c.c
 *
 */

/**********************************************************************************************************************/

void I2C_Init(void);
i2c_code_t* I2C_MasterTransmit(uint16_t aSlaveAddr, bool aStop, uint8_t *pTxBuf, uint32_t aTxBufSize);
i2c_code_t* I2C_MasterReceive (uint16_t aSlaveAddr, bool aStop, uint8_t *pRxBuf, uint32_t aRxBufSize);
bool I2C_IsBusy(void);
i2c_code_t I2C_GetCode(void);
uint32_t I2C_GetDataCount(void);
void I2C_Timer(void);

/**********************************************************************************************************************/
/** \} */ // I2cAPI

#endif /* MODULE_I2C */

#endif


