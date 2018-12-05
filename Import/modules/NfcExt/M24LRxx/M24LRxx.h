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
// $Id: M24LRxx.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/M24LRxx/M24LRxx.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __M24LRXX_H
#define __M24LRXX_H

#ifdef MODULE_M24LRxx

#include <stdint.h>


/** \addtogroup M24LRxxAPI
 * \{
 *
 * \file
 * \brief Header file for M24LRxx.c
 *
 */

/**********************************************************************************************************************/

#ifdef M24LR64_R
    #define M24LRXX_ADDR_START              (0)
    #define M24LRXX_SIZE_BYTE               (65536)

    #define M24LRxx_ADDRESS                 (0xA0)
    #define M24LRxx_CHIP_EN_ADDR_E1_E0      M24LR64_R_CHIP_EN_ADDR_E1_E0
#endif

#ifdef M24LR16E_R
    #define M24LRXX_ADDR_START              (0)
    #define M24LRXX_SIZE_BYTE               (2048)

    #define M24LRxx_ADDRESS                 (0xA0)
    #define M24LRxx_CHIP_EN_ADDR_E1_E0      (0x03)
    #define M24LRxx_CHIP_EN_ADDR_E2         (0x07)
#endif

/**********************************************************************************************************************/

#define PAGE_SIZE_BYTE              (4)

#define M24LR_TIMEOUT               (20)    /* @Douglas: TIMEOUT LIMIT TO 20MS */                  
#define M24LR_RESET                 (100)    /* @Douglas: TIMEOUT LIMIT TO 100MS */                  

/**********************************************************************************************************************/

typedef enum
{
    M24LRxx_REG_SSS0         = 0,
    M24LRxx_REG_SSS1,
    M24LRxx_REG_SSS2,
    M24LRxx_REG_SSS3,
    M24LRxx_REG_SSS4,
    M24LRxx_REG_SSS5,
    M24LRxx_REG_SSS6,
    M24LRxx_REG_SSS7,
    M24LRxx_REG_SSS8,
    M24LRxx_REG_SSS9,
    M24LRxx_REG_SSS10,
    M24LRxx_REG_SSS11,
    M24LRxx_REG_SSS12,
    M24LRxx_REG_SSS13,
    M24LRxx_REG_SSS14,
    M24LRxx_REG_SSS15,

    M24LRxx_REG_LOCK_BITS_0  = 2048,
    M24LRxx_REG_LOCK_BITS_1  = 2049,

    M24LRxx_REG_I2C_PASSWORD = 2304,
    M24LRxx_REG_RF_PASSWORD1 = 2308,
    M24LRxx_REG_RF_PASSWORD2 = 2312,
    M24LRxx_REG_RF_PASSWORD3 = 2316,

    M24LRxx_REG_CONFIG       = 2320,

    M24LRxx_REG_UID0         = 2324,
    M24LRxx_REG_UID1         = 2328,

    M24LRxx_REG_MEM_SIZE     = 2332,

    M24LRxx_REG_CONTROL      = 2336,
    M24LRxx_REG_MAX          = 2340
}
reg_addr_t;

/**********************************************************************************************************************/

typedef enum
{
    M24LRxx_STATE_ERR,
    M24LRxx_STATE_IDLE,

    M24LRxx_STATE_RX_START,
    M24LRxx_STATE_RX_DATA,
    M24LRxx_STATE_RX_END,

    M24LRxx_STATE_TX_START,
    M24LRxx_STATE_TX_DATA,
    M24LRxx_STATE_TX_END,

    M24LRxx_STATE_MAX

}
M24LRxx_state_t;

typedef enum
{
    M24LRxx_TYPE_REG,
    M24LRxx_TYPE_USER_MEM,
    M24LRxx_TYPE_MAX
}
loc_type_t;

typedef enum
{
    M24LRxx_REQ_NOK,
    M24LRxx_REQ_DONE,
    M24LRxx_REQ_PENDING,
    M24LRxx_REQ_MAX
}
M24LRxx_req_t;

/**********************************************************************************************************************/

void M24LRxx_Init(void);
void M24LRxx_SetAddress(uint8_t aAddr);
i2c_code_t M24LRxx_PresentPassword(uint32_t aPassword);

i2c_code_t M24LRxx_ReadReg(reg_addr_t aRegName, uint32_t *pRegValue);
i2c_code_t M24LRxx_ReadByte(uint16_t aAddr, uint8_t *pBuf);
i2c_code_t M24LRxx_ReadBuffer(uint16_t aAddr, uint16_t aByteNum, uint8_t *pBuf);

i2c_code_t M24LRxx_WriteReg(reg_addr_t aRegName, uint8_t aRegValue);
i2c_code_t M24LRxx_WriteByte(uint16_t aAddr, uint8_t aValue);
i2c_code_t M24LRxx_WritePage(uint16_t aAddr, uint32_t aValue);
i2c_code_t M24LRxx_WriteBuffer(uint16_t aAddr, uint16_t aByteNum, uint32_t aValue);

M24LRxx_state_t M24LRxx_Manager(void);
M24LRxx_req_t M24LRxx_ReadReq(uint16_t aAddr, loc_type_t aLocType, uint16_t aByteNum, uint8_t *pBuf);
M24LRxx_req_t M24LRxx_WriteReq(uint16_t aAddr, loc_type_t aLocType, uint16_t aByteNum, uint8_t *pData);
M24LRxx_req_t M24LRxx_GetReqStatus(void);

void M24LRxx_Timer(void);

/**********************************************************************************************************************/
/** \} */ // M24LRxxAPI

#endif /* MODULE_M24LRxx */

#endif