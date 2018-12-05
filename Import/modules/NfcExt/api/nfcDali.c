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
// $Revision: 2927 $
// $Date: 2016-02-12 15:31:30 +0100 (ven, 12 feb 2016) $
// $Id: nfcDali.c 2927 2016-02-12 14:31:30Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/api/nfcDali.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/**********************************************************************************************************************/
#ifndef __nfcDali
#define __nfcDali

#ifdef MODULE_NFC_DALI


#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC
#include "Config.h"
#include "Global.h"

#include "ErrorHandler.h"
#include "MpcDefs.h"
#include "Mpc.h"
#include "nfc.h"
#include "nfcDali.h"
#include "driver.h"

#ifdef _X86_
#define static
#endif

/**********************************************************************************************************************/

typedef enum
{
	SYNC_LOCAL_STATE_GET_MB,
	SYNC_LOCAL_STATE_READ,
	SYNC_LOCAL_STATE_COPY,
	SYNC_LOCAL_STATE_MAX
}
sync_loc_state_t;


/**********************************************************************************************************************/

#ifdef _X86_
#undef static
#define static static
#endif

/**********************************************************************************************************************/

typedef enum
{
#ifdef MPC_MB_0_NUMBER
	INDEX_MB_0,
#endif

#ifdef MPC_MB_1_NUMBER
	INDEX_MB_1,
#endif

#ifdef MPC_CLM_NUMBER
	INDEX_MB_CLM,
#endif

#ifdef MPC_EMERGENCY_NUMBER
	INDEX_MB_EMERGENCY,
#endif

#ifdef MPC_INFO_NUMBER
	INDEX_MB_INFO,
#endif

#ifdef MPC_PASSWORD1_NUMBER
	INDEX_MB_PSW1,
#endif
 
#ifdef MPC_PASSWORD1_COPY_NUMBER
	INDEX_MB_PSW1_COPY,
#endif        

#ifdef MPC_PASSWORD2_NUMBER
	INDEX_MB_PSW2,
#endif

#ifdef MPC_PASSWORD2_COPY_NUMBER
	INDEX_MB_PSW2_COPY,
#endif      
        
#ifdef MPC_POWER_NUMBER
	INDEX_MB_POWER,
#endif

#ifdef MPC_TDC_NUMBER
	INDEX_MB_TDC,
#endif

//#ifdef MPC_DEBUG_NUMBER
//	INDEX_MB_DEBUG,
//#endif

#ifdef MPC_ASTRO_NUMBER
	INDEX_MB_ASTRO,
#endif

#ifdef MPC_CSM_NUMBER
    INDEX_MB_CSM,
#endif

#ifdef MPC_EOL_NUMBER
	INDEX_MB_EOL,
#endif

#ifdef MPC_MAINS_NUMBER
	INDEX_MB_MAINS,
#endif

#ifdef MPC_PRESDETECT_NUMBER
	INDEX_MB_PRES_DETECT,
#endif

#ifdef MPC_STEP_NUMBER
	INDEX_MB_STEP,
#endif

#ifdef MPC_STEP_NUMBER
	INDEX_MB_STEP,
#endif

#ifdef DALI_INTERFACE
	INDEX_DALI_PARAMS,
#endif

	INDEX_MB_MAX
}
mb_index_t;

/**********************************************************************************************************************/

static mem_bank_t memBank[INDEX_MB_MAX];

static uint8_t mbBuf[MB_TOT_SIZE_BYTE];

static uint32_t totMbBufSize = 0;
mem_bank_nfc_t mem_bank_nfc;//={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23},
                            //  {0,1,2,3,4,5,6,7,8,9,10,11,12,13},
                            //  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18},
                            //  {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};
mem_bank_nfc_nvm mem_bank_nfc_nvm_t;
/**********************************************************************************************************************/

uint32_t getChannelNum(mb_index_t aMBIndex);

void nfcDali_Sync_Local_MemBank(mb_index_t aMBIndex);

void nfcDali_Sync_Library_MemBank(mb_index_t aMBIndex);

/** ********************************************************************************************************************
* \brief  ...
*
* \param  ...
*         ...
*
* \retval ...
*
***********************************************************************************************************************/

void nfcDali_Init(void)
{   
	uint32_t mbSize, mbIndex, ch;

    memset(memBank, 0, INDEX_MB_MAX*sizeof(mem_bank_t));

#ifdef MPC_MB_0_NUMBER
	mbSize = Mpc_GetLength(MPC_MB_0_NUMBER);
        mbSize=24;
	memBank[(uint8_t)INDEX_MB_0].number = 0;
	memBank[(uint8_t)INDEX_MB_0].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_0].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum((mb_index_t)0));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_0].ptr, mbSize*getChannelNum((mb_index_t)0), TAG_ADDR_MB_0, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_MB_1_NUMBER
	mbSize = Mpc_GetLength(MPC_MB_1_NUMBER);
        mbSize=14;
	memBank[(uint8_t)INDEX_MB_1].number = 1;
	memBank[(uint8_t)INDEX_MB_1].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_1].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum((mb_index_t)1));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_1].ptr, mbSize*getChannelNum((mb_index_t)1), TAG_ADDR_MB_1, NFC_DATA_TYPE_QCONST);
#endif
   
#ifdef MPC_CLM_NUMBER
	mbSize = Mpc_GetLength(MPC_CLM_NUMBER);
        mbSize=11;
	memBank[(uint8_t)INDEX_MB_CLM].number = MPC_CLM_NUMBER;
	memBank[(uint8_t)INDEX_MB_CLM].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_CLM].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_CLM));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_CLM].ptr, mbSize*getChannelNum(INDEX_MB_CLM), TAG_ADDR_MB_CLM, NFC_DATA_TYPE_QCONST);

    //****** Frequently changing data init ******

    // CSM_bank[7]  -> Adjust level 1
    // CSM_bank[9]  -> Adjust level 2
    // CSM_bank[11] -> Adjust level 3
    // CSM_bank[13] -> Adjust level 4
    // CSM_bank[15] -> Adjust level 5
    // CSM_bank[17] -> Adjust level 6
    // CSM_bank[19] -> Adjust level 7
    // CSM_bank[21] -> Adjust level 8

//    for(ch = 0; ch < getChannelNum(INDEX_MB_CSM); ch++)
//    {
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (7  - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (7  - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (9  - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (9  - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (11 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (11 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (13 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (13 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        #if MPC_CLM_TABLE_ENTRIES >=5
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (15 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (15 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        #endif
//        #if MPC_CLM_TABLE_ENTRIES >=6
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (17 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (17 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        #endif
//        #if MPC_CLM_TABLE_ENTRIES >=7
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (19 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (19 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        #endif
//        #if MPC_CLM_TABLE_ENTRIES >=8
//        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CLM].ptr[(ch*mbSize) + (21 - MPC_ADDR_OFFSET)], 1, TAG_ADDR_MB_CLM + (ch*mbSize) + (21 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
//        #endif
//    }
#endif

#ifdef MPC_EMERGENCY_NUMBER
	mbSize = Mpc_GetLength(MPC_EMERGENCY_NUMBER);
	memBank[(uint8_t)INDEX_MB_EMERGENCY].number = MPC_EMERGENCY_NUMBER;
	memBank[(uint8_t)INDEX_MB_EMERGENCY].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_EMERGENCY].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_EMERGENCY));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_EMERGENCY].ptr, mbSize*getChannelNum(INDEX_MB_EMERGENCY), TAG_ADDR_MB_EMERGENCY, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_INFO_NUMBER
	mbSize = Mpc_GetLength(MPC_INFO_NUMBER);
        mbSize =14;
	memBank[(uint8_t)INDEX_MB_INFO].number = MPC_INFO_NUMBER;
	memBank[(uint8_t)INDEX_MB_INFO].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_INFO].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_INFO));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_INFO].ptr, mbSize*getChannelNum(INDEX_MB_INFO), TAG_ADDR_MB_INFO, NFC_DATA_TYPE_QCONST);

    //****** Frequently changing data init ******

    // Info_bank[5]  -> LampCounter (MSB)
    // Info_bank[6]  -> LampCounter
    // Info_bank[7]  -> LampCounter (LSB)

    for(ch = 0; ch < getChannelNum(INDEX_MB_INFO); ch++)
    {
        nfcDataInit(&memBank[(uint8_t)INDEX_MB_INFO].ptr[(ch*mbSize) + (5  - MPC_ADDR_OFFSET)], 4, TAG_ADDR_MB_INFO + (ch*mbSize) + (5 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
    }
        for(ch = 0; ch < getChannelNum(INDEX_MB_INFO); ch++)
    {
        nfcDataInit(&memBank[(uint8_t)INDEX_MB_INFO].ptr[(ch*mbSize) + (10  - MPC_ADDR_OFFSET)], 3, TAG_ADDR_MB_INFO + (ch*mbSize) + (10 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
    }
#endif

#ifdef MPC_PASSWORD1_NUMBER
	mbSize = Mpc_GetLength(MPC_PASSWORD1_NUMBER);
        mbSize =10;
	memBank[(uint8_t)INDEX_MB_PSW1].number = MPC_PASSWORD1_NUMBER;
	memBank[(uint8_t)INDEX_MB_PSW1].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_PSW1].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_PSW1));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_PSW1].ptr, mbSize*getChannelNum(INDEX_MB_PSW1), TAG_ADDR_MB_PASSWORD1, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_PASSWORD1_COPY_NUMBER
        mbSize =3;
	memBank[(uint8_t)INDEX_MB_PSW1_COPY].number = MPC_PASSWORD1_COPY_NUMBER;
	memBank[(uint8_t)INDEX_MB_PSW1_COPY].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_PSW1_COPY].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_PSW1_COPY));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_PSW1_COPY].ptr, mbSize*getChannelNum(INDEX_MB_PSW1_COPY), TAG_ADDR_MB_PSSSWORD1_COPY, NFC_DATA_TYPE_QCONST);
#endif        
        
#ifdef MPC_PASSWORD2_NUMBER
	mbSize = Mpc_GetLength(MPC_PASSWORD2_NUMBER);
        mbSize =10;
	memBank[(uint8_t)INDEX_MB_PSW2].number = MPC_PASSWORD2_NUMBER;
	memBank[(uint8_t)INDEX_MB_PSW2].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_PSW2].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_PSW2));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_PSW2].ptr, mbSize*getChannelNum(INDEX_MB_PSW2), TAG_ADDR_MB_PASSWORD2, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_PASSWORD2_COPY_NUMBER
    mbSize =3;
	memBank[(uint8_t)INDEX_MB_PSW2_COPY].number = MPC_PASSWORD2_COPY_NUMBER;
	memBank[(uint8_t)INDEX_MB_PSW2_COPY].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_PSW2_COPY].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_PSW2_COPY));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_PSW2_COPY].ptr, mbSize*getChannelNum(INDEX_MB_PSW2_COPY), TAG_ADDR_MB_PSSSWORD2_COPY, NFC_DATA_TYPE_QCONST);
#endif        
        
#ifdef MPC_POWER_NUMBER
	mbSize = Mpc_GetLength(MPC_POWER_NUMBER);
	memBank[(uint8_t)INDEX_MB_POWER].number = MPC_POWER_NUMBER;
	memBank[(uint8_t)INDEX_MB_POWER].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_POWER].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_POWER));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_POWER].ptr, mbSize*getChannelNum(INDEX_MB_POWER), TAG_ADDR_MB_POWER, NFC_DATA_TYPE_QCONST);

    //****** Frequently changing data init ******

    // Power_bank[6]  -> Power consumption (MSB)
    // Power_bank[7]  -> Power consumption
    // Power_bank[8]  -> Power consumption (LSB)

    for(ch = 0; ch < getChannelNum(INDEX_MB_POWER); ch++)
    {
        nfcDataInit(&memBank[(uint8_t)INDEX_MB_POWER].ptr[(ch*mbSize) + (6  - MPC_ADDR_OFFSET)], 3, TAG_ADDR_MB_POWER + (ch*mbSize) + (6  - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
    }

#endif

#ifdef MPC_TDC_NUMBER
	mbSize = Mpc_GetLength(MPC_TDC_NUMBER);
	memBank[(uint8_t)INDEX_MB_TDC].number = MPC_TDC_NUMBER;
	memBank[(uint8_t)INDEX_MB_TDC].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_TDC].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_TDC));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_TDC].ptr, mbSize*getChannelNum(INDEX_MB_TDC), TAG_ADDR_MB_TDC, NFC_DATA_TYPE_QCONST);
#endif

//#ifdef MPC_DEBUG_NUMBER
//	mbSize = Mpc_GetLength(MPC_DEBUG_NUMBER);
//	memBank[(uint8_t)INDEX_MB_DEBUG].number = MPC_DEBUG_NUMBER;
//	memBank[(uint8_t)INDEX_MB_DEBUG].sizeByte = mbSize;
//	memBank[(uint8_t)INDEX_MB_DEBUG].ptr = &mbBuf[totMbBufSize];
//	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_DEBUG));
//	nfcDataInit(memBank[(uint8_t)INDEX_MB_DEBUG].ptr, mbSize*getChannelNum(INDEX_MB_DEBUG), TAG_ADDR_MB_DEBUG, NFC_DATA_TYPE_QCONST);
//#endif

#ifdef MPC_ASTRO_NUMBER
#ifdef ENASTRO
	mbSize = 27; //change from 27 to 28 back to 27 2017.3.2
	memBank[(uint8_t)INDEX_MB_ASTRO].number = MPC_ASTRO_NUMBER;
	memBank[(uint8_t)INDEX_MB_ASTRO].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_ASTRO].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_ASTRO));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_ASTRO].ptr, mbSize*getChannelNum(INDEX_MB_ASTRO), TAG_ADDR_MB_ASTRO, NFC_DATA_TYPE_QCONST);
#endif
#endif
    
#ifdef MPC_CSM_NUMBER
	mbSize = Mpc_GetLength(MPC_CSM_NUMBER);
        mbSize=16;
	memBank[(uint8_t)INDEX_MB_CSM].number = MPC_CSM_NUMBER;
	memBank[(uint8_t)INDEX_MB_CSM].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_CSM].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_CSM));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_CSM].ptr, mbSize*getChannelNum(INDEX_MB_CSM), TAG_ADDR_MB_CSM, NFC_DATA_TYPE_QCONST);

    //************ CSM  frequently changing data init ***********

    // CSM_bank[15] -> Actual output current (MSB)
    // CSM_bank[16] -> Actual output current (LSB)
    // CSM_bank[17] -> Output voltage (MSB)
    // CSM_bank[18] -> Output voltage (LSB)

    for(ch = 0; ch < getChannelNum(INDEX_MB_CSM); ch++)
    {
        nfcDataInit(&memBank[(uint8_t)INDEX_MB_CSM].ptr[(ch*mbSize) + (15 - MPC_ADDR_OFFSET)], 4, TAG_ADDR_MB_CSM + (ch*mbSize) + (15 - MPC_ADDR_OFFSET), NFC_DATA_TYPE_FCHANGING);
    }
#endif

#ifdef MPC_EOL_NUMBER
	mbSize = Mpc_GetLength(MPC_EOL_NUMBER);
    mbSize=5;
	memBank[(uint8_t)INDEX_MB_EOL].number = MPC_EOL_NUMBER;
	memBank[(uint8_t)INDEX_MB_EOL].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_EOL].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_EOL));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_EOL].ptr, mbSize*getChannelNum(INDEX_MB_EOL), TAG_ADDR_MB_EOL, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_MAINS_NUMBER
	mbSize = Mpc_GetLength(MPC_MAINS_NUMBER);
	memBank[(uint8_t)INDEX_MB_MAINS].number = MPC_MAINS_NUMBER;
	memBank[(uint8_t)INDEX_MB_MAINS].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_MAINS].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_MAINS));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_MAINS].ptr, mbSize*getChannelNum(INDEX_MB_MAINS), TAG_ADDR_MB_MAINS, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_PRESDETECT_NUMBER
	mbSize = Mpc_GetLength(MPC_PRESDETECT_NUMBER);
	memBank[(uint8_t)INDEX_MB_PRES_DETECT].number = MPC_PRESDETECT_NUMBER;
	memBank[(uint8_t)INDEX_MB_PRES_DETECT].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_PRES_DETECT].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_PRES_DETECT));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_PRES_DETECT].ptr, mbSize*getChannelNum(INDEX_MB_PRES_DETECT), TAG_ADDR_MB_PRESDETECT, NFC_DATA_TYPE_QCONST);
#endif

#ifdef MPC_STEP_NUMBER
	mbSize = Mpc_GetLength(MPC_STEP_NUMBER);
	memBank[(uint8_t)INDEX_MB_STEP].number = MPC_STEP_NUMBER;
	memBank[(uint8_t)INDEX_MB_STEP].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_MB_STEP].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_MB_STEP));
	nfcDataInit(memBank[(uint8_t)INDEX_MB_STEP].ptr, mbSize*getChannelNum(INDEX_MB_STEP), TAG_ADDR_MB_STEP, NFC_DATA_TYPE_QCONST);
#endif

#ifdef DALI_INTERFACE
	mbSize = Mpc_GetLength(255);
	memBank[(uint8_t)INDEX_DALI_PARAMS].number = 255;
	memBank[(uint8_t)INDEX_DALI_PARAMS].sizeByte = mbSize;
	memBank[(uint8_t)INDEX_DALI_PARAMS].ptr = &mbBuf[totMbBufSize];
	totMbBufSize += (mbSize*getChannelNum(INDEX_DALI_PARAMS));
	nfcDataInit(memBank[(uint8_t)INDEX_DALI_PARAMS].ptr, mbSize*getChannelNum(INDEX_DALI_PARAMS), TAG_ADDR_MB_DALI, NFC_DATA_TYPE_QCONST);
#endif

    if(totMbBufSize > MB_TOT_SIZE_BYTE)
    {
        ErrorShutdown(ERR_HANDLER_NFC_DALI_TOT_SIZE);
    }

	for (mbIndex = 0; mbIndex < INDEX_MB_MAX; mbIndex++)
	{
		if(memBank[mbIndex].sizeByte > MB_MAX_SIZE_BYTE)
        {
            ErrorShutdown(ERR_HANDLER_NFC_DALI_MAX_MB_SIZE);
        }
	}
}

/** ********************************************************************************************************************
* \brief  Determines if this is a multi or single-channel mem bank
*
* \param  ...
*         ...
*
* \retval ...
*
***********************************************************************************************************************/

uint32_t getChannelNum(mb_index_t aMBIndex)
{
	uint32_t channels;

	switch (aMBIndex)
	{
		#ifdef MPC_MB_0_NUMBER
		case INDEX_MB_0:
		#endif

		#ifdef MPC_MB_1_NUMBER
		case INDEX_MB_1:
		#endif

        #ifdef MPC_PASSWORD1_NUMBER
		case INDEX_MB_PSW1:
        #endif
                  
        #ifdef MPC_PASSWORD1_COPY_NUMBER
                case INDEX_MB_PSW1_COPY:
        #endif
          
        #ifdef MPC_PASSWORD2_NUMBER
		case INDEX_MB_PSW2:
        #endif
                  
        #ifdef MPC_PASSWORD2_COPY_NUMBER
               case INDEX_MB_PSW2_COPY:
        #endif         

        #ifdef MPC_TDC_NUMBER
		case INDEX_MB_TDC:
        #endif
			channels = 1;
			break;

		default:
			channels = DEVICE_CHANNELS;
			break;
	}

	return channels;
}

/**********************************************************************************************************************/
/* Copies one Library MB in local MB
***********************************************************************************************************************/

void nfcDali_Sync_Local_MemBank(mb_index_t aMBIndex)
{
	uint32_t i, j;
	uint16_t mb_num;
	uint32_t device_ch;

	uint8_t *p = memBank[aMBIndex].ptr;

	device_ch = getChannelNum(aMBIndex);

	for (i = 0; i < device_ch; i++)
	{
		mb_num = (uint16_t)(i << 8) | (uint16_t)memBank[aMBIndex].number;

		for (j = 0; j < memBank[aMBIndex].sizeByte; j++)
		{
			Mpc_Read(mb_num, j, &p[j]);
		}

		p = p + memBank[aMBIndex].sizeByte;
	}
}

/**********************************************************************************************************************/
/* Copies one local MB in Library MB
***********************************************************************************************************************/

void nfcDali_Sync_Library_MemBank(mb_index_t aMBIndex)
{
	uint32_t i, j;
	uint16_t mb_num;
	uint32_t device_ch;

	uint8_t *p = memBank[aMBIndex].ptr;

	device_ch = getChannelNum(aMBIndex);

	if (memBank[aMBIndex].number >= 1)
	{
		for (i = 0; i < device_ch; i++)
		{
			mb_num = (uint16_t)(i << 8) | (uint16_t)memBank[aMBIndex].number;

			for (j = 0; j < memBank[aMBIndex].sizeByte; j++)
			{
				Mpc_Write(mb_num, j, p[j]);
			}

			p = p + memBank[aMBIndex].sizeByte;
		}
	}
}

/**********************************************************************************************************************/
/* Copies Library MBs' data in local MBs.To be called at power-on after dali parameters are loaded
***********************************************************************************************************************/

void nfcDali_SyncLocalData(void)
{
	uint32_t mbIndex;

	for (mbIndex = 0; mbIndex < INDEX_MB_MAX; mbIndex++)
	{
		nfcDali_Sync_Local_MemBank((mb_index_t)mbIndex);
	}
}

/**********************************************************************************************************************/
/* Copies local MBs' data in Library MBs.To be called when new NFC data is detected in the tag
***********************************************************************************************************************/

void nfcDali_SyncDaliLibrary(void)
{
	uint32_t mbIndex;

	for (mbIndex = 0; mbIndex < INDEX_MB_MAX; mbIndex++)
	{
		nfcDali_Sync_Library_MemBank((mb_index_t)mbIndex);
	}
}

/**********************************************************************************************************************/
/* Called in main loop continuously, copies one memomy bank (Dali) at time in local MB
***********************************************************************************************************************/

#ifdef FAST_LOCAL_SYNC

    void nfcDali_IncrSyncLocalData(void)
    {
        static mb_index_t MbIndex = (mb_index_t)0;

        nfcDali_Sync_Local_MemBank(MbIndex);

        if (++MbIndex == INDEX_MB_MAX)
        {
            MbIndex = (mb_index_t)0;
        }
    }

#else

    //#include "gpio_xmc1300_tssop38.h"

    void nfcDali_IncrSyncLocalData(void)
    {
        static uint8_t syncLocBuf[MB_MAX_SIZE_BYTE];
        static sync_loc_state_t syncLocalState = SYNC_LOCAL_STATE_GET_MB;
        static mb_index_t MbIdx = (mb_index_t)0;
        static uint32_t byteIdx = 0;
        static uint32_t chIdx = 0;
        static uint8_t *p;
        static uint16_t mb_num;
        static uint32_t device_ch;
        uint32_t i;

        switch (syncLocalState)
        {
            case SYNC_LOCAL_STATE_GET_MB:
                p = memBank[MbIdx].ptr;
                device_ch = getChannelNum(MbIdx);
                chIdx = 0;
                byteIdx = 0;
                mb_num = (uint16_t)memBank[MbIdx].number;
                syncLocalState = SYNC_LOCAL_STATE_READ;
                break;

            case SYNC_LOCAL_STATE_READ:
                if(byteIdx < memBank[MbIdx].sizeByte)
                {
                    syncLocBuf[byteIdx] = 0;
                    Mpc_Read(mb_num, byteIdx, &syncLocBuf[byteIdx]);
                    byteIdx++;
                }
                else
                {
                    syncLocalState = SYNC_LOCAL_STATE_COPY;
                }
                break;

            case SYNC_LOCAL_STATE_COPY:
                for (i = 0; i < memBank[MbIdx].sizeByte; i++)
                {
                    p[i] = syncLocBuf[i];   
                }

                if (++chIdx < device_ch)
                {
                    mb_num = (uint16_t)(chIdx << 8) | (uint16_t)memBank[MbIdx].number;
                    p = p + memBank[MbIdx].sizeByte;
                    byteIdx = 0;
                    syncLocalState = SYNC_LOCAL_STATE_READ;
                }
                else
                {
                    syncLocalState = SYNC_LOCAL_STATE_GET_MB;

                    if (++MbIdx == INDEX_MB_MAX)
                    {
                        //P0_5_toggle();
                        MbIdx = (mb_index_t)0;
                    }
                }
                break;
        }
    }

mpc_rw_enum Mpc_Read(uint16_t mpc_number, uint8_t address, uint8_t *value)
{
  //address += 3;
  memcpy(value,(&mem_bank_nfc.mem_bank0.GTIN[0]+nfc_mpc_addr_abs(mpc_number))+address,1);   
    
    
    // Read MPC byte

    return (mpc_rw_enum)0;
}


/**********************************************************************************************************************
* This function writes the MPC with an offset of 3 applied to the address. This function bypasses the lock byte.
*
*/

mpc_rw_enum Mpc_Write(uint16_t mpc_number, uint8_t address, uint8_t value)
{
    //address += 3;

    // Write MPC byte

  memcpy(((uint8_t*)&mem_bank_nfc+nfc_mpc_addr_abs(mpc_number)+address),&value,1);
    return (mpc_rw_enum)0;
}
#endif

#endif /* MODULE_NFC_DALI */

#endif
/**********************************************************************************************************************/