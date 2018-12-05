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

// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2014-12, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: nfc.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/api/nfc.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \defgroup Nfc Nfc module
 * \{
 * \brief Hardware abstraction external NFC tag
 *
 * Nfc module implements the functionalities apt to read and write data in external NFC tags that in turn
 * realize a bidirectional RF port for ECGs.
 *
 * \defgroup NfcAPI Nfc API
 * \brief Nfc module user interface
 *
 * \file
 * \brief Central functions for Nfc handling.
 *
 * \defgroup NfcConfig Nfc Configuration
 * \brief Parameters to configure the Nvm module
 *
 * \{
 */

#ifdef MODULE_NFC

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "Global.h"

#include "Config.h"

#include "crc.h"
#include "ErrorHandler.h"
#include "i2c_driver.h"

#include "nfcDali.h"
#include "M24LRxx.h"
#include "nfc.h"

/**********************************************************************************************************************/

#define NFC_MODULE_VERSION                  (1.1)

/**********************************************************************************************************************/

#define NFC_REG_FALSE                       (0x00)
#define NFC_REG_TRUE                        (0X00)//0XAA

#define NFC_REG_WRITE_REQ                   (0x96)
#define NFC_REG_UPDATE_REQ                  (0xC3)

#define NFC_REG_NEW_DATA_CODE               (0x50)
#define NFC_REG_RF_START                    (0xCC)

/**********************************************************************************************************************/

#define NFC_QCONST_DATA_TIMEOUT_MS          (1000)              // [ms]

#define NFC_RF_REQ_TIMEOUT_MS               (5000)              // [ms]
#define NFC_RF_REQ_POLL_PERIOD_MS           (400)               // [ms]

/**********************************************************************************************************************/

#define NFC_FCHANG_DATA_TIMEOUT_INIT_MS     (10000)
#define NFC_FCHANG_DATA_TIMEOUT_MIN         (10)

#define NFC_FC_CRC_CHECK_PERIOD_MS          (5000)
#define NFC_FC_FORCE_TIMEOUT_MS             (1000)

#define NFC_WRITE_ALL_TIMEOUT_MS            (2000)

#define NFC_TIMER_PERIOD_US					I2C_TIMER_PERIOD_US

/**********************************************************************************************************************/

#define NFC_QCONST_DATA_TIMEOUT				((1000*NFC_QCONST_DATA_TIMEOUT_MS)/NFC_TIMER_PERIOD_US)					// Timer ticks

#define NFC_RF_REQ_TIMEOUT					((1000*NFC_RF_REQ_TIMEOUT_MS)/NFC_TIMER_PERIOD_US)						// Timer ticks

#define NFC_RF_REQ_POLL_PERIOD				((1000*NFC_RF_REQ_POLL_PERIOD_MS)/NFC_TIMER_PERIOD_US)					// Timer ticks

#define NFC_FCHANG_DATA_TIMEOUT_INIT		((1000*NFC_FCHANG_DATA_TIMEOUT_INIT_MS)/NFC_TIMER_PERIOD_US)			// Timer ticks

#define NFC_FCHANG_DATA_TIMEOUT				((1000*(60*1000*NFC_FCHANG_DATA_TIMEOUT_MIN)/NFC_TIMER_PERIOD_US))		// Timer ticks

#define NFC_FC_CRC_CHECK_PERIOD				((1000*NFC_FC_CRC_CHECK_PERIOD_MS)/NFC_TIMER_PERIOD_US)					// Timer ticks

#define NFC_FC_FORCE_TIMEOUT				((1000*NFC_FC_FORCE_TIMEOUT_MS)/NFC_TIMER_PERIOD_US)					// Timer ticks

#define NFC_WRITE_ALL_TIMEOUT               ((1000*NFC_WRITE_ALL_TIMEOUT_MS)/NFC_TIMER_PERIOD_US)					    // Timer ticks

/**********************************************************************************************************************/
//
//typedef enum
//{
//    NFC_REG_ADDR_ECG_ON      = 0,
//    NFC_REG_ADDR_RF_REQUEST  = 1,
//    NFC_REG_ADDR_MAJOR_VER   = 2,
//    NFC_REG_ADDR_NEW_RF_DATA = 3,
//    NFC_REG_ADDR_CHECKSUM_QC = 4,
//    NFC_REG_ADDR_CHECKSUM_FC = 8,
//    NFC_REG_ADDR_MAX         = 12
//}
//nfc_reg_t;
//
//typedef enum
//{
//    NFC_GET_STATE_IDLE,
//    NFC_GET_STATE_CHECK_NEW_FLAG,
//    NFC_GET_STATE_GET_ADDR,
//    NFC_GET_STATE_CRC,
//    NFC_GET_STATE_COPY,
//    NFC_GET_STATE_CLEAR_NEW_FLAG,
//    NFC_GET_STATE_MAX
//}
//nfc_get_state_t;
//
//typedef enum
//{
//    NFC_CMP_STATE_IDLE,
//    NFC_CMP_STATE_READ_REG,
//    NFC_CMP_STATE_ECG_ON,
//    NFC_CMP_STATE_INIT,
//    NFC_CMP_STATE_GET_STRUCT,
//    NFC_CMP_STATE_READ,
//    NFC_CMP_STATE_CALC_CRC,
//    NFC_CMP_STATE_COMPARE,
//    NFC_CMP_STATE_WRITE,
//	NFC_CMP_STATE_GET_CRC,
//	NFC_CMP_STATE_WRITE_CRC,
//    NFC_CMP_STATE_REQ_CLOSE,
//    NFC_CMP_STATE_MAX
//}
//nfc_cmp_state_t;
//
//typedef enum
//{
//    NFC_MIRROR_STATE_INIT,
//    NFC_MIRROR_STATE_IDLE,
//    NFC_MIRROR_STATE_RF_REQ,
//    NFC_MIRROR_STATE_NEW_RF_DATA,
//    NFC_MIRROR_STATE_FC_CRC,
//    NFC_MIRROR_STATE_CMP_RAM_NFC_DATA,
//    NFC_MIRROR_STATE_MAX
//}
//nfc_mirror_state_t;
//
//typedef enum
//{
//    NFC_RF_REQ_STATE_IDLE,
//    NFC_RF_REQ_STATE_DONE,
//    NFC_RF_REQ_STATE_POLL,
//    NFC_RF_REQ_STATE_ACK,
//    NFC_RF_REQ_STATE_ACK_REPLY,
//    NFC_RF_REQ_STATE_START,
//    NFC_RF_REQ_STATE_WAIT,
//    NFC_RF_REQ_STATE_END,
//    NFC_RF_REQ_STATE_STOP,
//    NFC_RF_REQ_STATE_MAX
//}
//nfc_rf_req_state_t;
//
//typedef enum
//{
//    NFC_CRC_STATE_NOK,
//    NFC_CRC_STATE_OK,
//    NFC_CRC_STATE_IDLE,
//    NFC_CRC_STATE_READ,
//    NFC_CRC_STATE_CALC_CRC,
//    NFC_CRC_STATE_CHECK,
//    NFC_CRC_STATE_MAX
//}
//nfc_crc_state_t;
//
//typedef enum
//{
//    NFC_COPY_STATE_IDLE,
//    NFC_COPY_STATE_BUSY,
//    NFC_COPY_STATE_MAX
//}
//nfc_copy_state_t;
//
///**********************************************************************************************************************/
//
//typedef struct
//{
//    uint8_t *ptr;
//    uint16_t size;
//    uint16_t tagAddr;
//    nfc_data_type_t type;
//
//}
//nfc_item_t;
//
//typedef struct
//{
//	nfc_item_t item[NFC_DATA_STRUCTS_CNT_MAX];
//    uint32_t totItemNum;
//    uint32_t typeItemNum[NFC_DATA_TYPE_MAX];
//    uint32_t typeCrcAddr[NFC_DATA_TYPE_MAX];
//	uint32_t totSizeByte;
//}
//nfc_data_t;

/**********************************************************************************************************************/

#define NFC_USER_MEM_START_ADDR             NFC_REG_ADDR_MAX

/**********************************************************************************************************************/

static nfc_get_state_t nfcGetNewData(void);
static nfc_cmp_state_t cmpRamNfcData(nfc_data_type_t aDataType);
static void nfcSwap(void* p1, void* p2, void* tmp, uint32_t aSize);
static void nfcTableAddItem(void *pNewItem, uint16_t aNewItemTagAddr, uint16_t aNewItemSize, nfc_data_type_t aNewItemType);
static uint32_t nfcGetOuterStructIdx(void *pNewData, uint16_t aNewDataSize);
static uint32_t nfcGetInnerStructIdx(void *pNewData, uint16_t aNewDataSize);
//static uint32_t cmpGetSizeAligned(uint32_t aStructIdx, uint32_t aAddrOffset);
static bool isDataEqual(uint8_t *p1, uint8_t *p2, uint32_t *pAddrOffset, uint32_t *pDataSizeByte);
static nfc_data_type_t getNextType(uint8_t aTypeMask);
static nfc_rf_req_state_t nfcRfRequest(uint32_t *pTimeOut);
uint32_t getStructIndex(nfc_data_type_t aType);
static nfc_copy_state_t copyDataToRam(nfc_data_type_t aDataType);
static bool dataFcCrcTimeout(uint32_t aTimer);

/**********************************************************************************************************************/
nfc_data_t nfcData;
static uint8_t rfPendingRequest;
static uint8_t nfcBuf[NFC_STRUCT_MAX_SIZE_BYTE];
static uint8_t ramBuf[NFC_STRUCT_MAX_SIZE_BYTE];

static uint32_t nfcQConstDataTimeout = NFC_QCONST_DATA_TIMEOUT;
static uint32_t nfcFChangDataTimeout = NFC_FCHANG_DATA_TIMEOUT_INIT;
static uint32_t nfcRfRequestTimeout  = NFC_RF_REQ_POLL_PERIOD;
static uint32_t nfcWriteAllTimeout   = 0;
static uint32_t tagDataCrc           = 0;
static uint32_t nfcFcDataActualCrc   = 0;

static bool cmpStateStart     = false;
static bool crcStateStart     = false;
static bool copyStateStart    = false;
static bool rfReqStateStart   = false;
static bool getDataStateStart = false;

nfc_get_state_t nfcGetDataState = NFC_GET_STATE_IDLE;
nfc_mirror_state_t nfcMirrorState = NFC_MIRROR_STATE_INIT;

extern uint8_t g_nfc_update_flag;
extern uint8_t g_nfc_start_flag;
extern uint8_t g_psw_update_flag;
//extern mem_bank_nfc_t mem_bank_nfc;
extern uint8_t g_pwm_update_flag;
/** ********************************************************************************************************************
* \brief Swaps data structs
*
* \param p1
*        pointer to first data struct
*
* \param p2
*        pointer to second data struct
*
* \param tmp
*        pointer to temporary data struct used as buffer
*
* \param size
*        p1, p2, tmp struct size
*
***********************************************************************************************************************/

static void nfcSwap(void* p1, void* p2, void* tmp, uint32_t aSize)
{
    memcpy(tmp, p1, (size_t)aSize);
    memcpy(p1, p2, (size_t)aSize);
    memcpy(p2 , tmp, (size_t)aSize);
}

/** ********************************************************************************************************************
* \brief Looks for data struct containing the new element
*
* This function checks whether the incoming data struct is contained in a data struct already initialized with nfcDataInit().
* If the return index == nfcData.totItemNum the struct is not contained in any other.
*
* \param pNewData
*        pointer to new data struct
*
* \param aNewDataSize
*        new data struct size
*
* \retval index where new data struct can be inserted
*
***********************************************************************************************************************/

static uint32_t nfcGetOuterStructIdx(void *pNewData, uint16_t aNewDataSize)
{
    uint32_t tableIdx = 0;
    uint32_t addr;
	uint32_t size;
    bool structFound = false;

    addr = (uint32_t)nfcData.item[0].ptr;
	size = (uint32_t)nfcData.item[0].size;

    while((structFound == false) && (tableIdx < nfcData.totItemNum))
    {
        if(((uint32_t)pNewData >= addr) && (((uint32_t)pNewData + aNewDataSize) <= (addr + size)))
        {
            structFound = true;
        }
        else
        {
            tableIdx++;                                                                                // Look for index in the table
			addr = (uint32_t)nfcData.item[tableIdx].ptr;
			size = (uint32_t)nfcData.item[tableIdx].size;
        }
    }

    return tableIdx;
}

/** ********************************************************************************************************************
* \brief Looks for data struct contained the new element
*
* This function checks whether the incoming data struct contains a data struct already initialized with nfcDataInit().
* If the return index == nfcData.totItemNum no inner structs were found.
*
* \param pNewData
*        pointer to new data struct
*
* \param aNewDataSize
*        new data struct size
*
* \retval index where new data struct can be inserted
*
***********************************************************************************************************************/

static uint32_t nfcGetInnerStructIdx(void *pNewData, uint16_t aNewDataSize)
{
    uint32_t tableIdx = 0;
    uint32_t addr;
	uint32_t size;
    bool structFound = false;

    addr = (uint32_t)nfcData.item[0].ptr;
	size = (uint32_t)nfcData.item[0].size;

    while((structFound == false) && (tableIdx < nfcData.totItemNum))
    {
        if((addr >= (uint32_t)pNewData) && ((addr + size) <= ((uint32_t)pNewData + aNewDataSize)))
        {
            structFound = true;
        }
        else
        {
            tableIdx++;                                                                             // Look for index in the table
            addr = (uint32_t)nfcData.item[tableIdx].ptr;
			size = (uint32_t)nfcData.item[tableIdx].size;
        }
    }

    return tableIdx;
}

/** ********************************************************************************************************************
* \brief Inserts a new item in the items' table
*
* \param pNewItem
*        pointer to new item
*
* \param aNewItemTagAddr
*        new item address in the tag
*
* \param aNewItemSize
*        new item size
*
* \param aNewItemType
*        NFC_DATA_TYPE_QCONST or NFC_DATA_TYPE_FCHANGING
*
***********************************************************************************************************************/

static void nfcTableAddItem(void *pNewItem, uint16_t aNewItemTagAddr, uint16_t aNewItemSize, nfc_data_type_t aNewItemType)
{
    uint32_t idx;
    uint32_t size;
    nfc_item_t tmp;

    idx = nfcGetOuterStructIdx(pNewItem, aNewItemSize);

    if(idx == nfcData.totItemNum)
    {
        // No outer struct found
        idx = nfcGetInnerStructIdx(pNewItem, aNewItemSize);

        if(idx == nfcData.totItemNum)
        {
            // No inner struct found -> No nested structs
            // Add new item at the end of table
            nfcData.item[nfcData.totItemNum].ptr     = (uint8_t *)pNewItem;
            nfcData.item[nfcData.totItemNum].tagAddr = aNewItemTagAddr;
            nfcData.item[nfcData.totItemNum].size    = aNewItemSize;
            nfcData.item[nfcData.totItemNum].type    = aNewItemType;
			nfcData.totSizeByte += aNewItemSize;
			nfcData.totItemNum++;
        }
        else
        {
            // New item contains an existing data struct
            size = (uint32_t)nfcData.item[idx].ptr - (uint32_t)pNewItem;        // Size of first part

            // First part
            nfcData.item[nfcData.totItemNum].ptr     = (uint8_t *)pNewItem;
            nfcData.item[nfcData.totItemNum].tagAddr = aNewItemTagAddr;
            nfcData.item[nfcData.totItemNum].size    = size;
            nfcData.item[nfcData.totItemNum].type    = aNewItemType;
			nfcData.totSizeByte += size;
            nfcData.totItemNum++;

            // Second part
            nfcData.item[nfcData.totItemNum].ptr     = (uint8_t *)((uint32_t)nfcData.item[idx].ptr + nfcData.item[idx].size);         // Second part at the end of existing struct
            nfcData.item[nfcData.totItemNum].tagAddr = aNewItemTagAddr + size + nfcData.item[idx].size;
            nfcData.item[nfcData.totItemNum].size    = aNewItemSize - size - nfcData.item[idx].size;
            nfcData.item[nfcData.totItemNum].type    = aNewItemType;
			nfcData.totSizeByte += nfcData.item[nfcData.totItemNum].size;
            nfcData.totItemNum++;

            if(size == 0)
            {
                // First part covered by existing struct
                nfcData.totItemNum--;
                nfcSwap(&nfcData.item[nfcData.totItemNum], &nfcData.item[nfcData.totItemNum - 1], &tmp, sizeof(nfc_item_t));
            }
            else if(nfcData.item[nfcData.totItemNum - 1].size == 0)
            {
                // Second part covered by existing struct -> discard last element
                nfcData.totItemNum--;
            }
        }
    }
    else
    {
        // New item contained in an existing data struct
        // Add new item at the end of table
        size = (uint32_t)pNewItem - (uint32_t)nfcData.item[idx].ptr;        // Size of first part

        nfcData.item[nfcData.totItemNum].ptr     = (uint8_t *)pNewItem;
        nfcData.item[nfcData.totItemNum].tagAddr = aNewItemTagAddr;
        nfcData.item[nfcData.totItemNum].size    = aNewItemSize;
        nfcData.item[nfcData.totItemNum].type    = aNewItemType;
        nfcData.totItemNum++;

        nfcData.item[nfcData.totItemNum].ptr     = (uint8_t *)((uint32_t)pNewItem + aNewItemSize);
        nfcData.item[nfcData.totItemNum].tagAddr = nfcData.item[idx].tagAddr + size + aNewItemSize;
        nfcData.item[nfcData.totItemNum].size    = nfcData.item[idx].size - size - aNewItemSize;
        nfcData.item[nfcData.totItemNum].type    = nfcData.item[idx].type;
        nfcData.totItemNum++;

		nfcData.item[idx].size = size;

        if(size == 0)
        {
            // First part covered by new item
            nfcData.totItemNum--;
            nfcSwap(&nfcData.item[nfcData.totItemNum - 1], &nfcData.item[idx], &tmp, sizeof(nfc_item_t));
            nfcSwap(&nfcData.item[nfcData.totItemNum], &nfcData.item[nfcData.totItemNum - 1], &tmp, sizeof(nfc_item_t));     // Last item moved one position backwards
        }

		if(nfcData.item[nfcData.totItemNum - 1].size == 0)
        {
            // Second part covered by existing struct -> discard last element
            nfcData.totItemNum--;
        }
    }
}

/** ********************************************************************************************************************
* \brief Sorts table. First NFC_DATA_TYPE_QCONST items then NFC_DATA_TYPE_FCHANGING ones
*
***********************************************************************************************************************/

static void nfcTableSort(void)
{
	uint32_t i, j;
	uint32_t qConstDataCnt = 0;
	nfc_item_t tmp;

    //------------- Place Quasi-const items in first part of array ------------
    for(i = 0; i < nfcData.totItemNum; i++)
    {
        if(nfcData.item[i].type == NFC_DATA_TYPE_QCONST)
        {
            if(i != qConstDataCnt)
            {
                nfcSwap(&nfcData.item[i], &nfcData.item[qConstDataCnt], &tmp, sizeof(nfc_item_t));
            }

            qConstDataCnt++;
        }
    }

	//-------------- Sort Quasi-const data by increasing TagAddr --------------
	for(j = 0; j < qConstDataCnt; j++)
	{
		for(i = 0; i < qConstDataCnt; i++)
		{
			if(nfcData.item[i].tagAddr > nfcData.item[j].tagAddr)
			{
				nfcSwap(&nfcData.item[i], &nfcData.item[j], &tmp, sizeof(nfc_item_t));
			}
		}
	}

	//------------- Sort Freq-chaning data by increasing TagAddr --------------
	for(j = qConstDataCnt; j < nfcData.totItemNum; j++)
	{
		for(i = qConstDataCnt; i < nfcData.totItemNum; i++)
		{
			if(nfcData.item[i].tagAddr > nfcData.item[j].tagAddr)
			{
				nfcSwap(&nfcData.item[i], &nfcData.item[j], &tmp, sizeof(nfc_item_t));
			}
		}
	}


    nfcData.typeItemNum[NFC_DATA_TYPE_QCONST]    = qConstDataCnt;
    nfcData.typeItemNum[NFC_DATA_TYPE_FCHANGING] = nfcData.totItemNum - qConstDataCnt;
}


/** ********************************************************************************************************************
* \brief Selects incrementally the next struct to read according to the bit mapped.
*
* \param aTypeMask
*        bit map
*
* \retval data type of the next struct
*
***********************************************************************************************************************/

static nfc_data_type_t getNextType(uint8_t aTypeMask)
{
    uint8_t testBit = 0x01;
	uint32_t typeCnt = 0;
    nfc_data_type_t type = (nfc_data_type_t)0;

    while(((aTypeMask & testBit) == 0) && (type < NFC_DATA_TYPE_MAX))
    {
        typeCnt++;
        type = (nfc_data_type_t)typeCnt;

        testBit <<= 1;
    }

    return type;
}

/** ********************************************************************************************************************
* \brief Gets index of the first element of the specified data type
*
* \param aType
*        data type selected
*
* \retval next struct index
*
***********************************************************************************************************************/

 uint32_t getStructIndex(nfc_data_type_t aType)
{
    uint32_t idx = 0;

    while((nfcData.item[idx].type != aType) &&  (idx < nfcData.totItemNum))
    {
        idx++;
    }

    return idx;
}

/** ********************************************************************************************************************
* \brief Copies data of specified type from tag to RAM
*
* \param aDataType
*        tag data type to copy in RAM
*
* \retval state machine state. NFC_COPY_STATE_IDLE if copy is done
*
***********************************************************************************************************************/

static nfc_copy_state_t copyDataToRam(nfc_data_type_t aDataType)
{
    static uint32_t structIdx = 0;
    static uint32_t typeItemCnt = 0;
    static nfc_copy_state_t nfcCopyState = NFC_COPY_STATE_IDLE;

    if(copyStateStart)
    {
        copyStateStart = false;
        nfcCopyState  = NFC_COPY_STATE_IDLE;
    }

    switch(nfcCopyState)
    {
        case NFC_COPY_STATE_IDLE:
            structIdx   = getStructIndex(aDataType);
            typeItemCnt = 0;

            if(structIdx < nfcData.totItemNum)                              // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
            {
                nfcCopyState = NFC_COPY_STATE_BUSY;
            }
            break;

        case NFC_COPY_STATE_BUSY:
            if(M24LRxx_ReadReq(NFC_USER_MEM_START_ADDR + nfcData.item[structIdx + typeItemCnt].tagAddr, M24LRxx_TYPE_USER_MEM, nfcData.item[structIdx + typeItemCnt].size, nfcBuf) == M24LRxx_REQ_DONE)
            {
                memcpy(nfcData.item[structIdx + typeItemCnt].ptr, nfcBuf, nfcData.item[structIdx + typeItemCnt].size);

                if(++typeItemCnt == nfcData.typeItemNum[aDataType])                                                      // All items of same type copied
                {
                    nfcCopyState = NFC_COPY_STATE_IDLE;
                }
            }
            break;

        default:
            nfcCopyState = NFC_COPY_STATE_IDLE;
            break;
    }

    return nfcCopyState;
}

/** ********************************************************************************************************************
* \brief Checks nfc data integrity by comparing calculated CRC of tag data with CRC stored in tag.
*
* \param aDataType
*        tag data type to check
*
* \param pTagCrc
*        pointer to struct where CRC read from tag is copied
*
* \retval state machine state. NFC_CRC_STATE_OK if CRC is ok otherwise NFC_CRC_STATE_NOK
*
***********************************************************************************************************************/

nfc_crc_state_t nfcDataCrc(nfc_data_type_t aDataType, uint32_t *pTagCrc)
{
    static nfc_crc_state_t nfcCrcState = NFC_CRC_STATE_IDLE;
    static uint32_t typeItemCnt = 0;
    static uint32_t structIndex = 0;
    static uint32_t crc;
    static uint32_t i;

    if(crcStateStart)
    {
        crcStateStart = false;
        nfcCrcState  = NFC_CRC_STATE_IDLE;
    }

    switch(nfcCrcState)
    {
        case NFC_CRC_STATE_NOK:
            break;

        case NFC_CRC_STATE_OK:
            nfcCrcState = NFC_CRC_STATE_IDLE;
            break;

        case NFC_CRC_STATE_IDLE:
            crcReset();
            typeItemCnt = 0;
            structIndex = getStructIndex(aDataType);

            if(structIndex < nfcData.totItemNum)                // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
            {
                nfcCrcState = NFC_CRC_STATE_READ;
            }
            break;

        case NFC_CRC_STATE_READ:
            if(M24LRxx_ReadReq(NFC_USER_MEM_START_ADDR + nfcData.item[structIndex + typeItemCnt].tagAddr, M24LRxx_TYPE_USER_MEM, nfcData.item[structIndex + typeItemCnt].size, nfcBuf) == M24LRxx_REQ_DONE)
            {
                i = 0;
                nfcCrcState = NFC_CRC_STATE_CALC_CRC;
                     }
            break;

        case NFC_CRC_STATE_CALC_CRC:
            crcLoadByte(nfcBuf[i]);

            if(++i >= nfcData.item[structIndex + typeItemCnt].size)
            {
                if(++typeItemCnt >= nfcData.typeItemNum[aDataType])                                                      // All items of same type done
                {
                    nfcCrcState = NFC_CRC_STATE_CHECK;
                }
                else
                {
                    nfcCrcState = NFC_CRC_STATE_READ;
                }
            }
            break;

        case NFC_CRC_STATE_CHECK:
            if(M24LRxx_ReadReq(nfcData.typeCrcAddr[aDataType], M24LRxx_TYPE_USER_MEM, 4, (uint8_t *)&crc) == M24LRxx_REQ_DONE)
            {
                if(crc != crcGet())
                {
                    nfcCrcState = NFC_CRC_STATE_NOK;                                                                    // Bad crc -> do not copy
                }
                else
                {
                    nfcCrcState = NFC_CRC_STATE_OK;
                    *pTagCrc = crc;
                }
            }
            break;

        default:
            nfcCrcState = NFC_CRC_STATE_IDLE;
            break;
    }

    return nfcCrcState;
}

/** ********************************************************************************************************************
* \brief Loads data from nfc tag to RAM
*
* \retval state machine state. NFC_GET_STATE_IDLE when done
*
***********************************************************************************************************************/

static nfc_get_state_t nfcGetNewData(void)
{
    static uint32_t nfcRegister;
    static uint8_t newRfDataReg;
    static bool newRfDataCorrupted = false;
    static nfc_data_type_t dataType = (nfc_data_type_t)0;
    //static uint8_t s_arr[]={24,49,38,0,61};
     //uint8_t i;
    //static nfc_get_state_t nfcGetDataState = NFC_GET_STATE_IDLE;
    nfc_crc_state_t crc_state;

    if(getDataStateStart)
    {
        getDataStateStart = false;
        nfcGetDataState   = NFC_GET_STATE_IDLE;
    }

    switch(nfcGetDataState)
    {
        case NFC_GET_STATE_IDLE:
            if(nfcData.totItemNum)
            {
                nfcGetDataState = NFC_GET_STATE_CHECK_NEW_FLAG;
            }
            break;

        case NFC_GET_STATE_CHECK_NEW_FLAG:
            if(M24LRxx_ReadReq(NFC_REG_ADDR_NEW_RF_DATA, M24LRxx_TYPE_USER_MEM, 1, (uint8_t *)&newRfDataReg) == M24LRxx_REQ_DONE)
            {
                if((newRfDataReg & 0xF0) == NFC_REG_NEW_DATA_CODE)
                {
                    newRfDataCorrupted = false;
                    dataType = (nfc_data_type_t)0;
                    crcReset();
                    nfcGetDataState = NFC_GET_STATE_GET_ADDR;
                }
                else
                {   
                    g_nfc_start_flag=1;
                    nfcGetDataState = NFC_GET_STATE_IDLE;
                }
            }
            break;

        case NFC_GET_STATE_GET_ADDR:
            if((newRfDataReg & ((0x01) << (uint8_t)dataType)) == 0)
            {
                dataType = getNextType(newRfDataReg);
            }
            else if(nfcData.typeItemNum[dataType])
            {
                nfcGetDataState = NFC_GET_STATE_CRC;
                crcStateStart   = true;
            }
            else
            {
                newRfDataReg &= ~((0x01) << (uint8_t)dataType);                                                         // Flag set but no elements present -> go to next dataType
            }

            if(dataType == NFC_DATA_TYPE_MAX)                                                                           // All structs done
            {
                nfcGetDataState = NFC_GET_STATE_CLEAR_NEW_FLAG;
            }
            break;

        case NFC_GET_STATE_CRC:
            crc_state = nfcDataCrc(dataType, &tagDataCrc);

            if(crc_state == NFC_CRC_STATE_NOK)
            {
                newRfDataCorrupted = true;
                newRfDataReg &= ~((0x01) << (uint8_t)dataType);
                nfcGetDataState = NFC_GET_STATE_GET_ADDR;                                                               // Bad crc -> do not copy
            }
            else if(crc_state == NFC_CRC_STATE_IDLE)
            {
                copyStateStart  = true;
                nfcGetDataState = NFC_GET_STATE_COPY;

                if(dataType == NFC_DATA_TYPE_FCHANGING)
                {
                    nfcFcDataActualCrc = tagDataCrc;
                }
            }
            break;

        case NFC_GET_STATE_COPY:                                                                                        // Copy data from nfc
            if(copyDataToRam(dataType) == NFC_COPY_STATE_IDLE)
            {
                newRfDataReg &= ~((0x01) << (uint8_t)dataType);
                nfcGetDataState = NFC_GET_STATE_GET_ADDR;
            }
            break;

        case NFC_GET_STATE_CLEAR_NEW_FLAG:
            nfcRegister = (uint32_t)NFC_REG_FALSE;
            if(M24LRxx_WriteReq(NFC_REG_ADDR_NEW_RF_DATA, M24LRxx_TYPE_USER_MEM, 1, (uint8_t *)&nfcRegister) == M24LRxx_REQ_DONE)
            {
                if(newRfDataCorrupted == false)
                {
                    //nfcDali_SyncDaliLibrary();
                }
                //g_nfc_update_flag=1;
                g_pwm_update_flag=1;
                g_nfc_start_flag=1;
                g_psw_update_flag=1;
//                for(i = 0; i < nfcData.totItemNum; i++)
//                {
//                 //memcpy((uint8_t*)&mem_bank_nfc+s_arr[i],nfcData.item[i].ptr,nfcData.item[i].size);
//                }
                nfcGetDataState = NFC_GET_STATE_IDLE;
            }
            break;

        default:
            nfcGetDataState = NFC_GET_STATE_IDLE;
            break;
    }

    return nfcGetDataState;
}

/** ********************************************************************************************************************
* \brief Gets data to compare incrementally by keeping 4 bytes alignment
*
* \param aStructIdx
*        struct index
*
* \param aAddrOffset
*        address offset
*
* \retval next 4-bytes aligned address
*
***********************************************************************************************************************/

 uint32_t cmpGetSizeAligned(uint32_t aStructIdx, uint32_t aAddrOffset)
{
    uint32_t startAddr = 0;
    uint32_t endAddr = 0;
    uint32_t size;

    size = nfcData.item[aStructIdx].size - aAddrOffset;

    if(size > PAGE_SIZE_BYTE)
    {
        size = PAGE_SIZE_BYTE;
    }

    if(size)
    {
        startAddr = NFC_USER_MEM_START_ADDR + nfcData.item[aStructIdx].tagAddr + aAddrOffset;                // If data accross memory aligment bounduary (4 bytes aligment) -> split data write
        endAddr   = startAddr + size - 1;

        startAddr = ((startAddr >> 2) << 2);
        endAddr   = ((endAddr >> 2) << 2);

        if(startAddr != endAddr)
        {
            size = endAddr - (NFC_USER_MEM_START_ADDR + nfcData.item[aStructIdx].tagAddr + aAddrOffset);
        }
    }

    return size;
}

/** ********************************************************************************************************************
* \brief Compares two data struct and gives back the offset where the first mismatch is found.
*
* \param p1
*        pointer to first struct
*
* \param p1
*        pointer to second struct
*
* \param pAddrOffset
*        pointer to first mismatched byte
*
* \param pDataSizeByte
*        pointer to size remaining
*
* \retval TRUE if data match
*
***********************************************************************************************************************/

static bool isDataEqual(uint8_t *p1, uint8_t *p2, uint32_t *pAddrOffset, uint32_t *pDataSizeByte)
{
    uint32_t i = 0;
    bool isEqual = true;

    while((i < *pDataSizeByte) && isEqual)
    {
        if(p1[*pAddrOffset + i] != p2[*pAddrOffset + i])
        {
            isEqual = false;
        }
        else
        {
            i++;
        }
    }

    *pDataSizeByte -= i;
    *pAddrOffset   += i;

    return isEqual;
}

/** ********************************************************************************************************************
* \brief Compares RAM with nfc data. Structs are loaded one by one and compared with RAM copies. When a mismatch is
*        found data is written in nfc tag. Writing in nfc tag must respect 4 bytes alignment. If data is across the
*        bounduary the writing operation is split in two parts.
*
* \param aDataType
*        data type to compare
*
* \retval state machine state. NFC_CMP_STATE_IDLE when done
*
***********************************************************************************************************************/

static nfc_cmp_state_t cmpRamNfcData(nfc_data_type_t aDataType)
{
    static nfc_cmp_state_t nfcCmpState = NFC_CMP_STATE_IDLE;
    static uint32_t data;
    static uint32_t addrOffset = 0;
	static uint32_t structCnt = 0;
    static uint32_t structIdx = 0;
    static uint32_t dataSizeByte;
    static uint32_t ramCrc;
    static uint32_t i;
#ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,61,87,92};// this is for nfcData.item to mem_bank_nfc
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    if(cmpStateStart)
    {
        cmpStateStart = false;
        nfcCmpState  = NFC_CMP_STATE_IDLE;
    }

    switch(nfcCmpState)
    {
        case NFC_CMP_STATE_IDLE:
            nfcCmpState = NFC_CMP_STATE_READ_REG;
            break;

        case NFC_CMP_STATE_READ_REG:                                                                                    // Check ON flag and NFC version
            if(M24LRxx_ReadReq(NFC_REG_ADDR_ECG_ON, M24LRxx_TYPE_USER_MEM, 3, (uint8_t *)&data) == M24LRxx_REQ_DONE)
            {
                if(( 0x00FF00FF & data) != (((uint32_t)NFC_MODULE_VERSION << 16) | NFC_REG_TRUE) )
                {
                    data &= 0xFF00FF00;
                    data |= (((uint32_t)NFC_MODULE_VERSION << 16) | NFC_REG_TRUE);//NFC_REG_ADDR_MAJOR_VER
                    nfcCmpState = NFC_CMP_STATE_ECG_ON;
                }
                else
                {
                    nfcCmpState = NFC_CMP_STATE_INIT;
                }

                //nfcCmpState = (((uint8_t)data != NFC_REG_TRUE) ? NFC_CMP_STATE_ECG_ON : NFC_CMP_STATE_INIT);
            }
            break;

        case NFC_CMP_STATE_ECG_ON:
            if(M24LRxx_WriteReq(NFC_REG_ADDR_ECG_ON, M24LRxx_TYPE_USER_MEM, 3, (uint8_t *)&data) == M24LRxx_REQ_DONE)   // Write ON flag and NFC version
            {
                nfcCmpState = NFC_CMP_STATE_INIT;
            }
            break;

        case NFC_CMP_STATE_INIT:
            if(nfcData.typeItemNum[(uint32_t)aDataType])
            {
                structCnt = 0;
                structIdx = getStructIndex(aDataType);
                dataSizeByte = 0;
                addrOffset = 0;
                crcReset();
                nfcCmpState = NFC_CMP_STATE_GET_STRUCT;
            }
            else
            {
                nfcCmpState = ( (rfPendingRequest == NFC_REG_UPDATE_REQ) ? NFC_CMP_STATE_REQ_CLOSE : NFC_CMP_STATE_IDLE );
            }
            break;

        case NFC_CMP_STATE_GET_STRUCT:
            if(addrOffset == nfcData.item[structIdx].size)
            {
                structCnt++;
                structIdx++;
            }

            if(structCnt == nfcData.typeItemNum[(uint32_t)aDataType])
            {
                nfcCmpState = NFC_CMP_STATE_GET_CRC;
            }
            else
            {
                nfcCmpState = NFC_CMP_STATE_READ;
                memcpy(ramBuf,s_arr[structIdx]+(uint8_t*)&mem_bank_nfc,nfcData.item[structIdx].size);//memcpy(ramBuf, nfcData.item[structIdx].ptr, nfcData.item[structIdx].size);                              // Take a snapshot of RAM data in a local buffer. The buffer is used for comparison because RAM data can be changed by Dali
                    }
            break;

        case NFC_CMP_STATE_READ:
            if(M24LRxx_ReadReq(NFC_USER_MEM_START_ADDR + nfcData.item[structIdx].tagAddr, M24LRxx_TYPE_USER_MEM, nfcData.item[structIdx].size, nfcBuf) == M24LRxx_REQ_DONE)
            {
                addrOffset = 0;
                dataSizeByte = nfcData.item[structIdx].size;
                nfcCmpState = NFC_CMP_STATE_CALC_CRC;
                i = 0;
            }
            break;

        case NFC_CMP_STATE_CALC_CRC:
            crcLoadByte(ramBuf[i]);

            if(++i >= dataSizeByte)
            {
                nfcCmpState = NFC_CMP_STATE_COMPARE;
            }
            break;

        case NFC_CMP_STATE_COMPARE:
            if(isDataEqual(ramBuf, nfcBuf, &addrOffset, &dataSizeByte))
            {
				nfcCmpState = NFC_CMP_STATE_GET_STRUCT;
            }
            else
            {
                dataSizeByte = cmpGetSizeAligned(structIdx, addrOffset);
                nfcCmpState = NFC_CMP_STATE_WRITE;
                }
            break;

        case NFC_CMP_STATE_WRITE:
            if(M24LRxx_WriteReq(NFC_USER_MEM_START_ADDR + nfcData.item[structIdx].tagAddr + addrOffset, M24LRxx_TYPE_USER_MEM, dataSizeByte, &ramBuf[addrOffset]) == M24LRxx_REQ_DONE)   // Set I2C bus as busy
            {
                addrOffset += dataSizeByte;
                dataSizeByte = nfcData.item[structIdx].size - addrOffset;
                nfcCmpState = ((dataSizeByte == 0) ? NFC_CMP_STATE_GET_STRUCT : NFC_CMP_STATE_COMPARE);
            }
            break;

		case NFC_CMP_STATE_GET_CRC:
			if(M24LRxx_ReadReq(nfcData.typeCrcAddr[(uint32_t)aDataType], M24LRxx_TYPE_USER_MEM, 4, (uint8_t *)&data) == M24LRxx_REQ_DONE)
            {
                ramCrc = crcGet();

                if(data == ramCrc)
                {
                    nfcCmpState = ( (rfPendingRequest == NFC_REG_UPDATE_REQ) ? NFC_CMP_STATE_REQ_CLOSE : NFC_CMP_STATE_IDLE );  // If crc already ok go to idle
                }
                else
                {
                    nfcCmpState = NFC_CMP_STATE_WRITE_CRC;
                }

                if(aDataType == NFC_DATA_TYPE_FCHANGING)
                {
                    nfcFcDataActualCrc = ramCrc;
                }
            }
			break;

		case NFC_CMP_STATE_WRITE_CRC:
			if(M24LRxx_WriteReq(nfcData.typeCrcAddr[(uint32_t)aDataType], M24LRxx_TYPE_USER_MEM, 4, (uint8_t *)&ramCrc) == M24LRxx_REQ_DONE)
            {
                if(rfPendingRequest == NFC_REG_UPDATE_REQ)
                {
                    nfcCmpState = NFC_CMP_STATE_REQ_CLOSE;
                }
                else
                {
                    nfcCmpState = NFC_CMP_STATE_IDLE;
                }
            }
			break;

        case NFC_CMP_STATE_REQ_CLOSE:
            data = (uint32_t)NFC_REG_FALSE;
            if(M24LRxx_WriteReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, (uint8_t *)&data) == M24LRxx_REQ_DONE)
            {
                rfPendingRequest = NFC_REG_FALSE;
                nfcCmpState      = NFC_CMP_STATE_IDLE;
            }
            break;

        default:
            nfcCmpState = NFC_CMP_STATE_IDLE;
            break;
    }

    return nfcCmpState;
}

/** ********************************************************************************************************************
* \brief Sets ECG_ON register TRUE and checks if data structs sizes are ok.
*
* \retval TRUE if power-on succeeded
*
***********************************************************************************************************************/

static bool nfcPowerOn(void)
{
    uint32_t i;
    uint32_t reg = 0;
    bool isInitDone = false;

    reg = ((uint32_t)NFC_MODULE_VERSION << 16) | NFC_REG_TRUE;
    if(M24LRxx_WriteReq(NFC_REG_ADDR_ECG_ON, M24LRxx_TYPE_USER_MEM, 3, (uint8_t *)&reg) == M24LRxx_REQ_DONE)
    {
        for(i = 0; i < nfcData.totItemNum; i++)
        {
            if(nfcData.item[i].size > NFC_STRUCT_MAX_SIZE_BYTE)
            {
                ErrorShutdown(ERR_HANDLER_MAX_STRUCT_SIZE);
            }
        }

        isInitDone = true;
    }

    return isInitDone;
}

/** ********************************************************************************************************************
* \brief Checks every whether a new RF request (write or update) has arrived and serves it
*
* \param pTimeOut
*        pointer to timer
*
* \retval state machine state. NFC_RF_REQ_STATE_IDLE or NFC_RF_REQ_STATE_DONE when finished
*
***********************************************************************************************************************/

static nfc_rf_req_state_t nfcRfRequest(uint32_t *pTimeOut)
{
    static nfc_rf_req_state_t nfcRfReqState = NFC_RF_REQ_STATE_IDLE;
    static uint8_t reqReg;
    static uint8_t ackByte;
    static uint32_t lastTimer;

    if(rfReqStateStart)
    {
        rfReqStateStart = false;
        nfcRfReqState   = NFC_RF_REQ_STATE_IDLE;
    }

    switch(nfcRfReqState)
    {
        case NFC_RF_REQ_STATE_DONE:
        case NFC_RF_REQ_STATE_IDLE:
            nfcRfReqState = NFC_RF_REQ_STATE_POLL;
            break;

        case NFC_RF_REQ_STATE_POLL:
            if(M24LRxx_ReadReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &reqReg) == M24LRxx_REQ_DONE)
            {
                rfPendingRequest = reqReg;
                ackByte = nfcFChangDataTimeout ^ reqReg;                    // Avoid 0xC3 to be presented as random byte

                if(rfPendingRequest == NFC_REG_WRITE_REQ)
                {
                    nfcRfReqState = NFC_RF_REQ_STATE_ACK;
                }
                else if(rfPendingRequest == NFC_REG_UPDATE_REQ)
                {
                    nfcRfReqState = NFC_RF_REQ_STATE_ACK;
                }
                else
                {
                    rfPendingRequest = NFC_REG_FALSE;
                    *pTimeOut        = NFC_RF_REQ_POLL_PERIOD;              // Poll again after  xx ms
                    nfcRfReqState    = NFC_RF_REQ_STATE_IDLE;
                }
            }
            break;

        case NFC_RF_REQ_STATE_ACK:
            if(M24LRxx_WriteReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &ackByte) == M24LRxx_REQ_DONE)
            {
                *pTimeOut = NFC_RF_REQ_TIMEOUT;                             // Give 5s to complete
                lastTimer = *pTimeOut;
                nfcRfReqState = NFC_RF_REQ_STATE_ACK_REPLY;
            }
            break;

        case NFC_RF_REQ_STATE_ACK_REPLY:
            if((lastTimer - *pTimeOut) > NFC_RF_REQ_POLL_PERIOD)            // Check every 100ms if Rf is done
            {
                if(M24LRxx_ReadReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &reqReg) == M24LRxx_REQ_DONE)
                {
                    if(reqReg != ackByte)
                    {
                        if(reqReg == (uint8_t)(~ackByte + 1))             	// Two's complement
                        {
                            *pTimeOut = NFC_RF_REQ_TIMEOUT;                 // Give 5s to complete
                            nfcRfReqState = NFC_RF_REQ_STATE_START;

                            if(rfPendingRequest == NFC_REG_WRITE_REQ)
                            {
                                nfcUpdateStart();
                            }
                        }
                        else
                        {
                            nfcRfReqState = NFC_RF_REQ_STATE_END;
                        }
                    }

                    lastTimer = *pTimeOut;
                }
            }

            if(*pTimeOut == 0)
            {
                nfcUpdateStop();
                nfcRfReqState = NFC_RF_REQ_STATE_STOP;
            }
            break;

        case NFC_RF_REQ_STATE_START:
            reqReg = NFC_REG_RF_START;
            if(M24LRxx_WriteReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &reqReg) == M24LRxx_REQ_DONE)        // Grant Rf write access (Dali off, ECG standby ...)
            {
                *pTimeOut = NFC_RF_REQ_TIMEOUT;
                lastTimer = NFC_RF_REQ_TIMEOUT;

                if(rfPendingRequest == NFC_REG_WRITE_REQ)
                {
                    nfcRfReqState = NFC_RF_REQ_STATE_WAIT;
                }
                else if(rfPendingRequest == NFC_REG_UPDATE_REQ)
                {
                    *pTimeOut     = NFC_RF_REQ_POLL_PERIOD;                 // Poll again after  xx ms
                    nfcRfReqState = NFC_RF_REQ_STATE_IDLE;                  // Back to idle
                }
            }
            break;

        case NFC_RF_REQ_STATE_WAIT:
            if(*pTimeOut == 0)
            {
                nfcUpdateStop();
                nfcRfReqState = NFC_RF_REQ_STATE_STOP;
            }
            else if((lastTimer - *pTimeOut) > NFC_RF_REQ_POLL_PERIOD)        // Check every 250ms if Rf is done
            {
                nfcRfReqState = NFC_RF_REQ_STATE_END;
            }
            break;

        case NFC_RF_REQ_STATE_END:
            if(M24LRxx_ReadReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &reqReg) == M24LRxx_REQ_DONE)
            {
                lastTimer = *pTimeOut;

                if(reqReg == NFC_REG_FALSE)
                {
                    nfcUpdateStop();
                    *pTimeOut     = NFC_RF_REQ_POLL_PERIOD;
                    nfcRfReqState = NFC_RF_REQ_STATE_DONE;
                }
                else
                {
                    nfcRfReqState = NFC_RF_REQ_STATE_WAIT;
                }
            }
            break;

        case NFC_RF_REQ_STATE_STOP:
            reqReg = NFC_REG_FALSE;
            if(M24LRxx_WriteReq(NFC_REG_ADDR_RF_REQUEST, M24LRxx_TYPE_USER_MEM, 1, &reqReg) == M24LRxx_REQ_DONE)    // Used to tell RF reader that abort/timeout occurred
            {
                *pTimeOut     = NFC_RF_REQ_POLL_PERIOD;
                nfcRfReqState = NFC_RF_REQ_STATE_IDLE;
            }
            break;

        default:
            nfcUpdateStop();
            *pTimeOut = NFC_RF_REQ_POLL_PERIOD;
            nfcRfReqState = NFC_RF_REQ_STATE_STOP;
            break;
    }

    return nfcRfReqState;
}

/** ********************************************************************************************************************
* \brief Checks whether timeout for checking "frequently changing" parameters CRC has expired
*
* \param aTimer
*        pointer to timer
*
* \retval TRUE is time has expired
*
***********************************************************************************************************************/

static bool dataFcCrcTimeout(uint32_t aTimer)
{
    bool isTimeExpired = false;
    static uint32_t lastFcTimer = NFC_FCHANG_DATA_TIMEOUT_INIT_MS;

    if(aTimer > lastFcTimer)
    {
        lastFcTimer = aTimer;
    }

    if((lastFcTimer - aTimer) > NFC_FC_CRC_CHECK_PERIOD)
    {
        lastFcTimer   = aTimer;
        isTimeExpired = true;
    }

    return isTimeExpired;
}

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief Nfc module initializazion
*
***********************************************************************************************************************/

void nfcInit(void)
{
    crcInit();

    M24LRxx_Init();

    memset(&nfcData, 0, sizeof(nfc_data_t));

    nfcData.typeCrcAddr[NFC_DATA_TYPE_QCONST]    = NFC_REG_ADDR_CHECKSUM_QC;
    nfcData.typeCrcAddr[NFC_DATA_TYPE_FCHANGING] = NFC_REG_ADDR_CHECKSUM_FC;

    //M24LRxx_PresentPassword(0);
    //M24LRxx_I2CwritePassword(0x12345678);
    //M24LRxx_SectorPassword(0X12345678);
    //M24LRxx_WritePassword(0X12345679);
    //M24LRxx_WriteReg(M24LRxx_REG_SSS15,0X0D);
    //M24LRxx_WriteReg(M24LRxx_REG_LOCK_BITS_0,0X00);
    //M24LRxx_WriteReg(M24LRxx_REG_LOCK_BITS_1,0X00);
}

/** ********************************************************************************************************************
* \brief Initializes RAM data struct as nfc data
*
* \param pdata
*        Pointer to data
*
* \param aSize
*        data size [byte]
*
* \param aTagAddr
*        absolute address
*
* \param aType
*        type of data
*
***********************************************************************************************************************/

void nfcDataInit(void *pData, uint16_t aSize, uint16_t aTagAddr, nfc_data_type_t aType)
{
    //---------- Store data info -----------
    if(aTagAddr < NFC_USER_MEM_START_ADDR)
    {
        ErrorShutdown(ERR_HANDLER_OFFSET_ERROR);
    }
    else if(nfcData.totItemNum < (NFC_DATA_STRUCTS_CNT_MAX - 1))        // Worst case two new entries needed (data structs split in two part)
    {
        aTagAddr -= NFC_USER_MEM_START_ADDR;
        nfcTableAddItem(pData, aTagAddr, aSize, aType);
    }
    else
    {
        ErrorShutdown(ERR_HANDLER_MAX_STRUCT);
    }

    nfcTableSort();
}

/** ********************************************************************************************************************
* \brief Nfc functionality manager
*
* This fucntion executes the following tasks:
*   1) At power-on notifies ECG is on
*   2) Compares RAM data with Nfc data
*   3) Handles RF requests (write & update)
*   4) Periodically cheecks FC data CRC
*
***********************************************************************************************************************/

void nfcManager(void)
{
    static nfc_data_type_t dataType = NFC_DATA_TYPE_QCONST;
    nfc_rf_req_state_t rf_req_state;
    nfc_crc_state_t crc_state;

    M24LRxx_Manager();      // Max 30us execution time

    switch(nfcMirrorState)
    {
        case NFC_MIRROR_STATE_INIT:
            if(nfcPowerOn())
            {
				nfcDali_SyncLocalData();
                getDataStateStart = true;
                nfcMirrorState    = NFC_MIRROR_STATE_NEW_RF_DATA;
            }
            break;

        case NFC_MIRROR_STATE_IDLE:
            if((nfcFChangDataTimeout == 0) || (rfPendingRequest == NFC_REG_UPDATE_REQ))
            {
                cmpStateStart  = true;
                dataType       = NFC_DATA_TYPE_FCHANGING;
                nfcMirrorState = NFC_MIRROR_STATE_CMP_RAM_NFC_DATA;                 // Compare RAM with NFC qconst data every 15 mins
            }
            else if(nfcQConstDataTimeout == 0)
            {
                cmpStateStart  = true;
                dataType       = NFC_DATA_TYPE_QCONST;
                nfcMirrorState = NFC_MIRROR_STATE_CMP_RAM_NFC_DATA;                 // Compare RAM with NFC fchang data every 1s
            }
            else if(nfcRfRequestTimeout == 0)
            {
                rfReqStateStart = true;                                             // Rf request arrived?
                nfcMirrorState  = NFC_MIRROR_STATE_RF_REQ;
            }
            else if(dataFcCrcTimeout(nfcFChangDataTimeout))
            {
                crcStateStart  = true;                                              // Check Fc data integrity
                nfcMirrorState = NFC_MIRROR_STATE_FC_CRC;
            }

            //nfcDali_IncrSyncLocalData();                                            // Max execution time = 280us with 27 bytes memory bank
            break;

        case NFC_MIRROR_STATE_RF_REQ:
            rf_req_state = nfcRfRequest(&nfcRfRequestTimeout);
            if(rf_req_state == NFC_RF_REQ_STATE_DONE)                               // Polls every 100ms for new RF request
            {
                getDataStateStart = true;
                nfcMirrorState    = NFC_MIRROR_STATE_NEW_RF_DATA;
            }
            else if(rf_req_state == NFC_RF_REQ_STATE_IDLE)
            {
                nfcMirrorState = NFC_MIRROR_STATE_IDLE;
            }
            break;

        case NFC_MIRROR_STATE_NEW_RF_DATA:
            if(nfcGetNewData() == NFC_GET_STATE_IDLE)
            {
                nfcMirrorState = NFC_MIRROR_STATE_IDLE;                             // Finished
//                g_nfc_start_flag=1;
//                g_pwm_update_flag=1;
            }
            break;

        case NFC_MIRROR_STATE_FC_CRC:
            crc_state = nfcDataCrc(NFC_DATA_TYPE_FCHANGING, &tagDataCrc);

            if(crc_state == NFC_CRC_STATE_NOK)
            {
                nfcMirrorState       = NFC_MIRROR_STATE_IDLE;
                nfcFChangDataTimeout = NFC_FC_FORCE_TIMEOUT;                        // Force RAM to tag data check
            }
            else if(crc_state == NFC_CRC_STATE_IDLE)
            {
                if(tagDataCrc != nfcFcDataActualCrc)                                // Crc doesn't match
                {
                    nfcMirrorState       = NFC_MIRROR_STATE_IDLE;
                    nfcFChangDataTimeout = NFC_FC_FORCE_TIMEOUT;                    // Force RAM to tag data check
                }
                else
                {
                    nfcMirrorState = NFC_MIRROR_STATE_IDLE;
                }
            }
            break;

        case NFC_MIRROR_STATE_CMP_RAM_NFC_DATA:
        if(cmpRamNfcData(dataType) == NFC_CMP_STATE_IDLE)                           // Max execution time: 40us with MemBank of 27 bytes
            {
                nfcMirrorState = NFC_MIRROR_STATE_IDLE;

                if(dataType == NFC_DATA_TYPE_FCHANGING)
                {
                    nfcFChangDataTimeout = NFC_FCHANG_DATA_TIMEOUT;
                }
                else
                {
                    nfcQConstDataTimeout = NFC_QCONST_DATA_TIMEOUT;
                }
            }
            break;

        default:
            nfcMirrorState       = NFC_MIRROR_STATE_IDLE;
            nfcQConstDataTimeout = NFC_QCONST_DATA_TIMEOUT;
            nfcFChangDataTimeout = NFC_FCHANG_DATA_TIMEOUT;
            break;
    }
}

/** ********************************************************************************************************************
* \brief Writes all RAM data in tag. Blocking function.
*
* If a large amount of data must be written in Nfc a watchdog refresh is needed (see nfcRefreshWdt). Data writing must
* come to end within 5s.
*
***********************************************************************************************************************/

void nfcWriteAll(void)
{
    uint32_t i, j;
    uint32_t itemTypeNum = 0;
    uint32_t byteCnt;
    uint32_t byteOffset;
    uint8_t *p;
    nfc_data_type_t type = (nfc_data_type_t)0;
    uint32_t value;

    crcReset();

    nfcWriteAllTimeout = NFC_WRITE_ALL_TIMEOUT;

    for(i = 0; i < nfcData.totItemNum; i++)
    {
        byteOffset = 0;

		memcpy(ramBuf, nfcData.item[i].ptr, nfcData.item[i].size);												        // Get RAM data

		M24LRxx_ReadBuffer(NFC_USER_MEM_START_ADDR + nfcData.item[i].tagAddr, nfcData.item[i].size, nfcBuf);	        // Get NFC data

		do
		{
			byteCnt = cmpGetSizeAligned(i, byteOffset);

            if(byteCnt)
            {
                p = &ramBuf[byteOffset];

                memcpy(&value, p, byteCnt) ;

                if(memcmp(&nfcBuf[byteOffset], p, byteCnt) != 0)									                    // If (RAM data) != (NFC data) -> write
                {
                    M24LRxx_WriteBuffer(NFC_USER_MEM_START_ADDR + nfcData.item[i].tagAddr + byteOffset, byteCnt, value);
                }

                byteOffset += byteCnt;

                for(j = 0; j < byteCnt; j++)
                {
                    crcLoadByte(*p);															        // Calculate crc
    				p++;
	            }
            }

            if(nfcWriteAllTimeout)
            {
                nfcRefreshWdt();
            }

		}
		while(byteCnt);

        if(++itemTypeNum >= nfcData.typeItemNum[type])
        {
            M24LRxx_WriteBuffer(nfcData.typeCrcAddr[type], 4, crcGet());										        // Write crc type

            crcReset();
            itemTypeNum = 0;
            type = (nfc_data_type_t)((uint32_t)type + 1);
        }
    }

    M24LRxx_WriteByte(NFC_REG_ADDR_ECG_ON, NFC_REG_FALSE);														        // Going off
}

/** ********************************************************************************************************************
* \brief Timer function handling all internal time bases and timeouts.
*
* Must be called at interrupt with a period < 10ms. Period configuration in ConfigI2C.h
*
***********************************************************************************************************************/

void nfcTimer(void)
{
    if(nfcQConstDataTimeout)
    {
        nfcQConstDataTimeout--;
    }

    if(nfcFChangDataTimeout)
    {
        nfcFChangDataTimeout--;
    }

    if(nfcRfRequestTimeout)
    {
        nfcRfRequestTimeout--;
    }

    if(nfcWriteAllTimeout)
    {
        nfcWriteAllTimeout--;
    }

    M24LRxx_Timer();
}


/**********************************************************************************************************************/

void nfcUpdateStart(void)
{

}

/**********************************************************************************************************************/

void nfcUpdateStop(void)
{

}

#endif /* MODULE_NFC */

/**********************************************************************************************************************/

/** \} */ // NfcAPI
/** \} */ // Nfc