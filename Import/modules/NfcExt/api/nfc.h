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
// $Id: nfc.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/api/nfc.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NFC_H
#define __NFC_H

#ifdef MODULE_NFC

#include "ConfigNfc.h"
/** \addtogroup NfcAPI
 * \{
 *
 * \file
 * \brief Header file for nfc.c
 *
 */

/**********************************************************************************************************************/

#define ERR_HANDLER_MAX_STRUCT			"Nfc structs number max exceeded"
#define ERR_HANDLER_OFFSET_ERROR    	"First byte are reserved "
#define ERR_HANDLER_MAX_STRUCT_SIZE     "Nfc struct too big"

/**********************************************************************************************************************/

typedef enum
{
    NFC_DATA_TYPE_QCONST,
    NFC_DATA_TYPE_FCHANGING,
	NFC_DATA_TYPE_DUMMY_1,
	NFC_DATA_TYPE_DUMMY_2,
    NFC_DATA_TYPE_MAX
}
nfc_data_type_t;

typedef enum
{
    NFC_REG_ADDR_ECG_ON      = 0,
    NFC_REG_ADDR_RF_REQUEST  = 1,
    NFC_REG_ADDR_MAJOR_VER   = 2,
    NFC_REG_ADDR_NEW_RF_DATA = 3,
    NFC_REG_ADDR_CHECKSUM_QC = 4,
    NFC_REG_ADDR_CHECKSUM_FC = 8,
    NFC_REG_ADDR_MAX         = 12
}
nfc_reg_t;

typedef enum
{
    NFC_GET_STATE_IDLE,
    NFC_GET_STATE_CHECK_NEW_FLAG,
    NFC_GET_STATE_GET_ADDR,
    NFC_GET_STATE_CRC,
    NFC_GET_STATE_COPY,
    NFC_GET_STATE_CLEAR_NEW_FLAG,
    NFC_GET_STATE_MAX
}
nfc_get_state_t;

typedef enum
{
    NFC_CMP_STATE_IDLE,
    NFC_CMP_STATE_READ_REG,
    NFC_CMP_STATE_ECG_ON,
    NFC_CMP_STATE_INIT,
    NFC_CMP_STATE_GET_STRUCT,
    NFC_CMP_STATE_READ,
    NFC_CMP_STATE_CALC_CRC,
    NFC_CMP_STATE_COMPARE,
    NFC_CMP_STATE_WRITE,
	NFC_CMP_STATE_GET_CRC,
	NFC_CMP_STATE_WRITE_CRC,
    NFC_CMP_STATE_REQ_CLOSE,
    NFC_CMP_STATE_MAX
}
nfc_cmp_state_t;

typedef enum
{
    NFC_MIRROR_STATE_INIT,
    NFC_MIRROR_STATE_IDLE,
    NFC_MIRROR_STATE_RF_REQ,
    NFC_MIRROR_STATE_NEW_RF_DATA,
    NFC_MIRROR_STATE_FC_CRC,
    NFC_MIRROR_STATE_CMP_RAM_NFC_DATA,
    NFC_MIRROR_STATE_MAX
}
nfc_mirror_state_t;

typedef enum
{
    NFC_RF_REQ_STATE_IDLE,
    NFC_RF_REQ_STATE_DONE,
    NFC_RF_REQ_STATE_POLL,
    NFC_RF_REQ_STATE_ACK,
    NFC_RF_REQ_STATE_ACK_REPLY,
    NFC_RF_REQ_STATE_START,
    NFC_RF_REQ_STATE_WAIT,
    NFC_RF_REQ_STATE_END,
    NFC_RF_REQ_STATE_STOP,
    NFC_RF_REQ_STATE_MAX
}
nfc_rf_req_state_t;

typedef enum
{
    NFC_CRC_STATE_NOK,
    NFC_CRC_STATE_OK,
    NFC_CRC_STATE_IDLE,
    NFC_CRC_STATE_READ,
    NFC_CRC_STATE_CALC_CRC,
    NFC_CRC_STATE_CHECK,
    NFC_CRC_STATE_MAX
}
nfc_crc_state_t;

typedef enum
{
    NFC_COPY_STATE_IDLE,
    NFC_COPY_STATE_BUSY,
    NFC_COPY_STATE_MAX
}
nfc_copy_state_t;

/**********************************************************************************************************************/

typedef struct
{
  uint8_t *ptr;
  uint16_t size;
  uint16_t tagAddr;
  nfc_data_type_t type;
  
}
nfc_item_t;

typedef struct
{
  nfc_item_t item[NFC_DATA_STRUCTS_CNT_MAX];
  uint32_t totItemNum;
  uint32_t typeItemNum[NFC_DATA_TYPE_MAX];
  uint32_t typeCrcAddr[NFC_DATA_TYPE_MAX];
  uint32_t totSizeByte;
}
nfc_data_t;


/**********************************************************************************************************************/
extern nfc_data_t nfcData;
extern nfc_get_state_t nfcGetDataState;


/**********************************************************************************************************************/void nfcInit(void);
void nfcDataInit(void *pData, uint16_t aSize, uint16_t aAddr, nfc_data_type_t aType);
void nfcManager(void);
void nfcTimer(void);
void nfcWriteAll(void);
void nfcUpdateStart(void);
void nfcUpdateStop(void);
void nfcRefreshWdt(void);
uint32_t cmpGetSizeAligned(uint32_t aStructIdx, uint32_t aAddrOffset);
/**********************************************************************************************************************/
/** \} */ // NfcAPI

#endif /* MODULE_NFC */
#endif


