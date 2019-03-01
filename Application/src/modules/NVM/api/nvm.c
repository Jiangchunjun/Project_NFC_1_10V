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
// $Revision: 2995 $
// $Date: 2016-02-22 15:35:26 +0800 (Mon, 22 Feb 2016) $
// $Id: nvm.c 2995 2016-02-22 07:35:26Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/api/nvm.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \defgroup Nvm Nonvolatile Memory
 * \{
 * \brief Hardware abstraction for non-volatile memory
 *
 * Nvm module provides an interface for the configuration and management of non-volatile memories for initialization,
 * reading and writing of data. The module is thought to handle data on flash memories but it can be extended to EEPROM
 * or other kinds of non-volatile storage supports. It adopts the so called "paging" strategy typically used for EEPROM
 * simulation in flash and consisting in dividing the flash memory in several pages (partitions) able to contain all
 * the needed data. Every time data is intended to be saved a new page is selected, erase and written. Whenever data
 * needs to be retrieved the most recent saved page is selected. The candidate page for erasing is instead the one
 * with the oldest data. Nv(m) stands for non-volatile (memory).
 *
 * Typically flash memories allow erase and write operations on partitions with different size i.e. the minimum
 * erasable area is different in size from the minimum writeable area. The nvm module extends therefore the concept of
 * pages to handle data properly by defining writeable, erasable and data pages:
 *
 * e-page = minimum erasable memory partition   <BR>
 * w-page = minimum writeable memory partition  <BR>
 * d-page = minimum writeable partition able to contain all data intended to be saved in nvm  <BR>
 *
 * E-page and w-page sizes are fixed and MCU dependent while the d-page size is determined by the amount of data
 * the user wants to store in nvm. The table below shows the pages' characteristics for three MCUs.
 *
|                |   STM32F0xx   |    XMC1000    |  SAM D10/21   |
|:---------------|:-------------:|:-------------:|:-------------:|
|w-page          |2 bytes        |16 bytes       |64 bytes       |
|d-page          |(Nx2) bytes    |(Nx16) bytes   |(Nx64) bytes   |
|e-page          |1 Kbyte        |256 bytes      |256 bytes      |
|e-page cycles   |10K            |50K            |25K            |
 *
 * D-pages are multiples of w-pages by design while in a single e-page several d-pages can find place or if the
 * d-page is bigger than an e-page more e-pages might be required to cover the complete d-page.
 * <BR><BR><B>Example (XMC100):</B> if data to save is 600 bytes, the d-page must be (16 x 38) = 608 bytes and therefore 3 e-pages
 * are needed (256 x 3 = 768 bytes).
 *
 * \dot
 * digraph G
 * {
 *  rankdir="UD"
 *   node1
 *   [
 *       shape = none
 *       label =<  <table  BORDER="0" CELLSPACING="1" >
 *	            <tr><td port="port0" border="0"                  height="15"    >           </td></tr>
 *              <tr><td port="port0" border="1" bgcolor="orange" height="64" 	>  d-page   </td></tr>
 *               </table> >
 *   ]
 *
 *   node2
 *   [
 *       shape = none
 *       label = <  <table title="test_title" border="0" cellspacing="0" >
 *	            <tr><td port="port0" border="0"                     height="30"	    >          </td></tr>
 *              <tr><td port="port0" border="1" bgcolor="turquoise" height="25"   	> e-page 0 </td></tr>
 *	            <tr><td port="port1" border="1" bgcolor="turquoise" height="25" 	> e-page 1 </td></tr>
 *	            <tr><td port="port2" border="1" bgcolor="turquoise" height="25" 	> e-page 2 </td></tr>
 *               </table>>
 *   ]
 * }
 * \enddot
 * <BR><B>Example (SAM D10/21):</B> if data to save is 100 bytes, the d-page must be (64 x 2) = 128 bytes and therefore
 * one e-page can be divided in two parts. In this case a new e-page will be erased every two writing cycles.
 * \dot
 * digraph G
 * {
 * rankdir="UD"
 * node1
 * [
 *      shape = none
 *      label =<  <table  BORDER="0" CELLSPACING="2" >
 *	                    <tr><td port="port0" border="0" height="6"                      >            </td></tr>
 *                   	<tr><td port="port1" border="1" bgcolor="orange" height="25" 	>  d-page 0  </td></tr>
 *	                    <tr><td port="port2" border="1" bgcolor="orange" height="25" 	>  d-page 1  </td></tr>
 *               </table> >
 *   ]
 *
 *   node2
 *   [
 *       shape = none
 *       label = <  <table title="test_title" border="0" cellspacing="0" >
 *	                    <tr><td port="port0" border="0"                     height="28"	    >          </td></tr>
 *                   	<tr><td port="port1" border="1" bgcolor="turquoise" height="58"   	> e-page 0 </td></tr>
 *               </table>>
 *   ]
 * }
 * \enddot
 * Because flash memory partitions must be erased before being written, in order to safely save data in flash, at least
 * two d-pages are needed: one that contains saved data and one ready to be written.
 * The actual number of d-pages required to fulfil the application needs is calculated according to the configuration
 * parameters defined by the user in NvmConfig.h i.e. number of writing cycles and data size to be saved. In any case
 * to guarantee a certain level of redundancy of persistent data at least 4 d-pages are configured.
 *
 * At power-on RAM data is initialized with the most recent saved d-page provided that data integrity is verified (valid
 * CRC). A new d-page, the one containing the least recent data, is then erased, ready to be used at next data saving.
 * Data is typically saved in flash when a power-failure is detected.
 *
 * \dot
 * digraph G
 * {
 *    rankdir = LR;
 *    node1
 *    [
 *        shape = none
 *        label = <<table border="0" cellspacing="2">
 *
 * 	                    <tr><td port="Title1" border="0"		                        >   Flash    </td></tr>
 *                    	<tr><td port="port0"  border="1" bgcolor="orange"	            >  d-page 0  </td></tr>
 *                    	<tr><td port="port1"  border="1" bgcolor="orange"	            >  d-page 1  </td></tr>
 *                    	<tr><td port="port2"  border="2" bgcolor="turquoise"      	    >  d-page 2  </td></tr>
 *                    	<tr><td port="port3"  border="1" bgcolor="grey"          	    >  d-page 3  </td></tr>
 *		                <tr><td port="port4"  border="1" bgcolor="orange"         	    >  d-page 4  </td></tr>
 *		                <tr><td port="port5"  border="1" bgcolor="green"  height="100"  >    code    </td></tr>
 *
 *                </table>>
 *    ]
 *
 *    node2
 *    [
 *        shape = none
 *        label = <<table border="0" cellspacing="0">
 *
 *                      <tr><td port="title2" border="0"		                >    RAM    </td></tr>
 *                    	<tr><td port="port0" border="2" bgcolor="turquoise"	    >page mirror</td></tr>
 *
 *                </table>>
 *    ]
 *
 *
 *	node1:port2 -> node2:port0 [label="Power-on"   color="green"]
 *	node2:port0 -> node1:port3 [label="Power-down" color="red"]
 *	node2:port0:s -> node2:port0:e [label="Run-time"   color="black" constraint="false"]
 * }
 * \enddot
 *
 * <B>Module usage</B> <BR>
 * In order to use the non-volatile memory module the following files must be included in the project:
 *
 * nvm.c    <BR>
 * crc.c
 *
 * and the related driver files depending on the MCU in use. Crc.c and the related driver files belong to crc module. <BR>
 * <I>Drivers for STM32F0xx </I>:
 *
 * flash_driver_stm32f0xx.c <BR>
 * crc_driver_stm32f0xx.c
 *
 * together with ST standard peripheral library v1.0.0:
 *
 * stm32f0xx_flash.c	<BR>
 * stm32f0xx_crc.c		<BR>
 * stm32f0xx_rcc.c		<BR>
 *
 * <I>Drivers for XMC1000</I>:
 *
 * flash_driver_xmc1300.c   <BR>
 * crc_driver_sw.c
 *
 * <I>Drivers for SAM D10/21</I>:
 *
 * flash_driver_samd21.c    <BR>
 * crc_driver_sw.c
 *
 * The nvm module requires then the definitions of some parameters. See Nvm configuration. Once compiled the module
 * offers some informations on the memory structure to save nv data with the following symbols (defines): <BR>
 *
 Symbol                    | Description
-------------------------- | ----------------------------------------------------
__NVM_DPAGE_TOT_CNT        | Number of d-pages used
__NVM_DPAGE_SIZE_BYTE      | Size [byte] of a d-page
__NVM_DPAGE_CNT_PER_EPAGE  | Number of d-pages contained in an e-page
__NVM_EPAGE_CNT_PER_DPAGE  | Number of e-pages needed to cover a d-page
__NVM_DATA_TOT_SIZE_BYTE   | Total size [bytes] of flash memory used for nv data
__NVM_DATA_BASE_ADDR       | Starting address of nv data area
 *
 * The linker checks whether the flash memory area for nv data overlaps the code area.
 *
 * \file
 * \brief Central functions for Nvm handling.
 *
 * \defgroup NvmInternal Nvm Internals
 * \brief Nvm module implementation details
 *
 * \defgroup NvmConfig Nvm Configuration
 * \brief Parameters to configure the Nvm module
 *
 * \defgroup NvmDriverAPI Nvm Driver API
 * \brief Nvm module hardware driver interface
 *
 * \defgroup NvmAPI Nvm API
 * \brief Nvm module user interface
 *
 * \{
 */

#define MODULE_NVM
#include "Config.h"

#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#include "Global.h"

#include "nvm_defs.h"
#include "nvm_driver.h"
#include "nvm.h"

#include "crc.h"
#include "ErrorHandler.h"

/**********************************************************************************************************************/

#include "message_flash_size.h"

#define XSTR(x) STR(x)
#define STR(x) #x

#pragma message ("NVM data flash size [byte]: 0x" XSTR(NVM_SIZE_BYTE3S) XSTR(NVM_SIZE_BYTE2S) XSTR(NVM_SIZE_BYTE1S) "0")

/**********************************************************************************************************************/

#ifdef _X86_
	#include "stm32f0xx_flash.h"	    		// Driver double
#else
    #define GET_NVM_ADDR( a )   ( a )		    // Do nothing
#endif

/** ********************************************************************************************************************
 * \ingroup NvmAPI
 * \brief Mock array to reserve the space for nvm data flash area.
 *
 * The concept of the nvm module is to use a part of program memory to store the non-volatile data (data flash).
 * But neither the compiler nor the linker know anything about this usage and there is the risk, that the nvm module
 * and the program memory overlap. If this silently happens, the nvm module would erase portions of the code
 * sooner or later. <b> This must not happen.</b>
 *
 * To prevent this, this array is to reserve the space in flash memory (at compile/link time) at exactly the
 * same position, which will be used later on by the nvm module (during runtime):
 * A linker error will raise if program code and nvm data flash overlap.
 *
 * \warning The compiler/linker does not check, whether there is really flash memory at the absolute address assigned
 * to this array. Normally the programming tool will warn in case of.
 *
 * \note Temporarily uncomment this array definition to preserve data flash content over re-programming cycles and
 * to speed up program download.
 */

#ifndef _X86_
extern const uint8_t nvm_dataflash_area[]; // extern declaration not needed, but present to help doxygen

// object_attribute=__root: Include this object in hex code even if it's unreferenced in the program
#pragma object_attribute=__root
const uint8_t nvm_dataflash_area[__NVM_AREA_TOT_SIZE_BYTE] @ (__NVM_DATA_BASE_ADDR) = {0};
#endif

/*************************************************** Structs **********************************************************/

typedef enum
{
	CMD_DPAGE_FAILED,
	CMD_DPAGE_OK,
	CMD_DPAGE_MAX
}
cmd_dpage_t;

/*************************************************** Structs **********************************************************/

typedef struct
{
    uint32_t totWriteCycles;
    uint32_t eraseCycles;
//    uint32_t totCrcErrCnt;
    uint32_t dataSizeByte;
    uint32_t crc;
}
dpage_info_t;

//-----------------------------

typedef struct
{
    uint32_t index;
    uint32_t addr;
    nvm_state_t state;
    dpage_info_t info;
}
dpage_t;

//-----------------------------

typedef struct
{
    uint8_t  buf[NVM_WPAGE_SIZE_BYTE];
    uint32_t addr;
    uint32_t byteCnt;
}
wpage_t;

//-----------------------------

typedef struct
{
	struct
	{
		uint8_t *ptr;
		uint32_t size;

	} item[NVM_DATA_STRUCTS_CNT_MAX];

    uint32_t idxMax;
	uint32_t totSizeByte;
}
nvm_data_t;

/************************************************ Local functions *****************************************************/

static uint32_t getDpageAddr( uint32_t dPageIdx );
static cmd_dpage_t setupNextDpage( dpage_t *pNextDpage );
static bool isDpageBlank( uint32_t dPageIdx );
static bool isDpageCRCok( uint32_t dPageIdx );
static cmd_dpage_t eraseDpage(uint32_t dPageIdx);
static void updateNvmData( const dpage_t *pActualDpage, const dpage_t *pNextDpage );
static void crcErrCount( bool *crcOkBuf, const dpage_t *pActualDpage );
static void getActualDpage( dpage_t *pDpage );
static void getNextDpage( const dpage_t *pActualDpage, dpage_t *pNextDpage );
static void getDpageInfo( dpage_info_t *pDpage, uint32_t dPageIdx );
static void resetNvmErrors( void );
static nvm_err_t wPageAddByte( uint8_t value );

/**************************************************** Local data ******************************************************/

static nvm_t		nvm;
static nvm_data_t	nvmData;
static dpage_t		actualDpage;
static dpage_t		nextDpage;
static wpage_t		wpage;

/** ********************************************************************************************************************
* \brief Gets d-page address
*
* Adress of the first byte of the struct
*
* \param dPageIdx
*        d-page index
*
* \retval d-page address
*
***********************************************************************************************************************/

static uint32_t getDpageAddr( uint32_t dPageIdx )
{
    uint32_t startAddr;

#if (__NVM_DPAGE_CNT_PER_EPAGE == 0)

    startAddr = __NVM_DATA_BASE_ADDR + dPageIdx * ( __NVM_EPAGE_CNT_PER_DPAGE * NVM_EPAGE_SIZE_BYTE );

#else

    uint32_t ePageCnt = ( dPageIdx / __NVM_DPAGE_CNT_PER_EPAGE );
    uint32_t dPageCnt = ( dPageIdx - ( __NVM_DPAGE_CNT_PER_EPAGE * ePageCnt) );

    startAddr = __NVM_DATA_BASE_ADDR + ( ePageCnt * NVM_EPAGE_SIZE_BYTE ) + ( dPageCnt * __NVM_DPAGE_SIZE_BYTE );

#endif

    return startAddr;
}

/** ********************************************************************************************************************
* \brief Gets d-page infos
*
*  Gets d-page footer with meaningful infos about actual d-page
*
* \param pInfoDpage
*        pointer to info struct where d-page footer will be copied
*
* \param dPageIdx
*        d-page index
*
***********************************************************************************************************************/

static void getDpageInfo( dpage_info_t *pInfoDpage, uint32_t dPageIdx )
{
    uint32_t dpageInfoAddr = getDpageAddr(dPageIdx) + NVM_OFFSET_DPAGE_INFO;

    nvm_drv_read_buffer(dpageInfoAddr, sizeof(dpage_info_t), pInfoDpage);
}

/** ********************************************************************************************************************
* \brief Checks CRC of the specified d-page
*
* \param dPageIdx
*        d-page index
*
* \retval TRUE if CRC ok
*
***********************************************************************************************************************/

static bool isDpageCRCok( uint32_t dPageIdx )
{
	uint32_t addr;
    dpage_info_t iPage;
    bool crcOk = false;

    getDpageInfo(&iPage, dPageIdx);

    addr = getDpageAddr(dPageIdx);

	addr = GET_NVM_ADDR(addr);

    if(crcCalcBuf((void *)(addr), (NVM_DATA_PLUS_INFO_PAGE_SIZE_BYTE - 4)) == iPage.crc)              // CRC bytes not counted
    {
        crcOk = true;
    }

    return crcOk;
}

/** ********************************************************************************************************************
* \brief Checks whether d-page is blank
*
* \param dPageIdx
*        d-page index
*
* \retval TRUE if d-page is blank (all 0xFF)
*
***********************************************************************************************************************/

static bool isDpageBlank( uint32_t dPageIdx )
{
    bool dpageBlank = true;
    uint16_t value;
    uint32_t i, addr;

    addr = getDpageAddr(dPageIdx);

    for(i = 0; i < (__NVM_DPAGE_SIZE_BYTE/2); i++)
    {
        nvm_drv_read_buffer(addr + 2*i, 2 , &value);                        // D-page is a multiple of w-page. W-page size is even -> check on half-word (16 bits)
        if(value != 0xFFFF)
        {
            dpageBlank = false;
        }
    }

    return dpageBlank;
}

/** ********************************************************************************************************************
* \brief Erases specified d-page
*
* \param dPageIdx
*        d-page struct
*
* \retval CMD_DPAGE_OK if erase succeeded otherwise CMD_DPAGE_FAILED
*
***********************************************************************************************************************/

static cmd_dpage_t eraseDpage(uint32_t dPageIdx)
{
	uint32_t i;
	uint32_t addr;
	cmd_dpage_t eraseDpage = CMD_DPAGE_OK;

	addr = getDpageAddr(dPageIdx);

	for(i = 0; i < __NVM_EPAGE_CNT_PER_DPAGE; i++)                                              // D-page might be covered by multiple e-pages
	{
		nvm_drv_unlock();

		if(nvm_drv_erase_epage(addr + (i*NVM_EPAGE_SIZE_BYTE)) != NVM_ERR_NO_ERR)
		{
			eraseDpage = CMD_DPAGE_FAILED;
		}

		nvm_drv_lock();
	}

	return eraseDpage;
}

/** ********************************************************************************************************************
* \brief Checks if d-page is already erased. If not erases d-page, whenever possible
*
* \param pNextDpage
*        pointer to a d-page struct
*
* \retval CMD_DPAGE_OK if erase succeeded otherwise CMD_DPAGE_FAILED
*
***********************************************************************************************************************/

static cmd_dpage_t setupNextDpage( dpage_t *pNextDpage )
{
    dpage_info_t iPage;
    cmd_dpage_t dpageBlank = CMD_DPAGE_FAILED;

    #if (__NVM_DPAGE_CNT_PER_EPAGE > 1)
	uint32_t firstDpageIdx;

	if((pNextDpage->index) % __NVM_DPAGE_CNT_PER_EPAGE)			// Not first d-page inside e-page
	{
        if(isDpageBlank((pNextDpage->index)))					// Page must be already erased
		{
			dpageBlank = CMD_DPAGE_OK;
			firstDpageIdx =__NVM_DPAGE_CNT_PER_EPAGE * (pNextDpage->index / __NVM_DPAGE_CNT_PER_EPAGE); // Index of first page (inside same e-page)

			if(isDpageCRCok(firstDpageIdx))
			{
				getDpageInfo(&iPage, firstDpageIdx);					// Reload info of first d-page (CRC ok)
				pNextDpage->info.eraseCycles = iPage.eraseCycles;
			}
			else
			{
				pNextDpage->info.eraseCycles = 1;
			}
		}
    }
	else
    #endif
	{
		getDpageInfo(&iPage, pNextDpage->index);

		if((iPage.crc == 0xFFFFFFFF) && (iPage.eraseCycles == 0xFFFFFFFF))	// Is this a blank d-page?
		{
			pNextDpage->info.eraseCycles = 1;

			if(isDpageBlank(pNextDpage->index))								// Check done when eraseCycle = 0xFFFFFFFF (factory new)
			{
				dpageBlank = CMD_DPAGE_OK;
			}
			else
			{
				dpageBlank = eraseDpage(pNextDpage->index);
			}
		}
		else
		{
			if(isDpageCRCok(pNextDpage->index))
			{
				if(pNextDpage->info.eraseCycles < NVM_EPAGE_CYCLES)			// CRC ok -> check erase cycles
				{
					pNextDpage->info.eraseCycles += 1;
					dpageBlank = eraseDpage(pNextDpage->index);
				}
			}
			else
			{
				pNextDpage->info.eraseCycles = 1;
				dpageBlank = eraseDpage(pNextDpage->index);
			}
		}
	}

    return dpageBlank;
}

/** ********************************************************************************************************************
* \brief Counts CRC errors
*
* \param crcOkBuf
*        pointer to array CRC buffer
*
* \param pActualDpage
*        pointer to actual d-page
*
***********************************************************************************************************************/

static void crcErrCount( bool *crcOkBuf, const dpage_t *pActualDpage )
{
    uint32_t i;

    for(i=0; i < __NVM_DPAGE_TOT_CNT; i++)                                                  // Log CRC errors
    {
        if(crcOkBuf[i] == false)
        {
            nvm.error.crc++;

            if(pActualDpage->state == NVM_STATE_OK)
            {
//                page->info.totCrcErrCnt++;
            }
        }
    }
}

/** ********************************************************************************************************************
* \brief Gets d-page with most recent valid data
*
* The function looks for the d-page with greatest totWriteCycles i.e. the last d-page written; if totWriteCycles equals
* 0xFFFFFFFF the d-page is considered blank, just erased and is ignored. If the found d-page has  wrong CRC the function
* ignores this d-page too and starts the research again. The process continues until a d-page with valid CRC is found or
* until all the pages have been checked.
* If a valid d-page is found:       pDpage->state == NVM_STATE_OK <BR>
* If all d-pages are blank:         pDpage->state == NVM_STATE_NO_DATA <BR>
* If a valid d-pages is not found:  pDpage->state == NVM_STATE_ERR
*
* \param pDpage
*        pointer to d-page where to store the actual d-page
*
***********************************************************************************************************************/

static void getActualDpage( dpage_t *pDpage )
{
    uint32_t i,j;
    dpage_info_t iPage;
    bool crcDpageOk[ __NVM_DPAGE_TOT_CNT ];

    memset(pDpage, 0, sizeof(dpage_t));                                                     // External struct init

	for(i=0; i < __NVM_DPAGE_TOT_CNT; i++)
	{
		crcDpageOk[i] = true;
	}

    pDpage->state = NVM_STATE_NO_DATA;
    i = __NVM_DPAGE_TOT_CNT;

    do                                                                                      // Look for page with valid CRC
    {
        i--;
        pDpage->index = 0;
        pDpage->info.totWriteCycles = 0;

        //-------- Search d-page with greatest number of power-down cycles --------
        for(j = 0; j < __NVM_DPAGE_TOT_CNT; j++)
        {
            if(crcDpageOk[j] == true)
            {
                getDpageInfo(&iPage, j);                                                    // Load d-page

                if(iPage.totWriteCycles == 0xFFFFFFFF)                                      // Page never used
                {
                    iPage.totWriteCycles = 0;
                }

                if(iPage.totWriteCycles > pDpage->info.totWriteCycles)
                {
                    pDpage->info.totWriteCycles = iPage.totWriteCycles;
                    pDpage->index = j;
                    pDpage->state = NVM_STATE_OK;
                }
            }
        }

        //---------------------- Check candidate d-page CRC -----------------------
        if(pDpage->state == NVM_STATE_OK)
        {
            if(isDpageCRCok(pDpage->index) == true)                                         // CRC check is time consuming -> done only once at the end of search
            {
                getDpageInfo(&(pDpage->info), pDpage->index);                               // Load d-page
            }
            else
            {
                pDpage->state = NVM_STATE_ERR;                                              // Wrong CRC
                crcDpageOk[pDpage->index] = false;
            }
        }
    }
    while((i > 0) && (pDpage->state == NVM_STATE_ERR));                                     // Cycle over all pages

    crcErrCount(crcDpageOk, pDpage);

    pDpage->addr = getDpageAddr(pDpage->index);

    if(pDpage->state != NVM_STATE_OK)
    {
        pDpage->info.totWriteCycles = 0;
		//pDpage->info.totCrcErrCnt = 0;
    }
}

/** ********************************************************************************************************************
* \brief Identifies d-page with least recent data and erases it
*
* Actual d-page (if present) cointains most recent data therefore is not included in the research. This function looks
* for the d-page with the smallest eraseCycles number i.e. the d-page that cointains the "oldest" data. According to
* d-page and e-page sizes different scenarios can be found. If an e-page contains more d-pages than every e-page erasure
* prepares as many d-pages for writing as the number of d-pages contained in the e-page.
* If a d-page is ready for writing:         pNextDpage->state == NVM_STATE_OK <BR>
* If no d-page is available for writing:    pNextDpage->state == NVM_STATE_ERR
*
* \param pActualDpage
*        pointer to actual d-page
*
* \param pNextDpage
*        pointer to next d-page (erased)
*
***********************************************************************************************************************/

static void getNextDpage( const dpage_t *pActualDpage, dpage_t *pNextDpage )
{
    uint32_t i, j;
    dpage_info_t iPage;
    bool doSearch = false;
    bool dPageValid[ __NVM_DPAGE_TOT_CNT ];

    memset(pNextDpage, 0, sizeof(dpage_t));                                                             // Next page init

	for(i = 0; i < __NVM_DPAGE_TOT_CNT; i++)
	{
		dPageValid[i] = true;
	}

    if(pActualDpage->state != NVM_STATE_OK)  // i.e. -> NVM_STATE_ERR or NVM_STATE_NO_DATA
    {
        doSearch = true;
    }
    else if((pActualDpage->state == NVM_STATE_OK) && (pActualDpage->info.totWriteCycles < NVM_WRITE_CYCLES))
    {
        dPageValid[pActualDpage->index] = false;                                                        // Actual d-page must NOT be erased
        doSearch = true;

        #if (__NVM_DPAGE_CNT_PER_EPAGE > 1)
        dPageValid[__NVM_DPAGE_CNT_PER_EPAGE *(pActualDpage->index/__NVM_DPAGE_CNT_PER_EPAGE)] = false; // ... also first d-page of e-page must NOT be erased
        #endif

		pNextDpage->info.totWriteCycles = pActualDpage->info.totWriteCycles;							// Load number of cycles of actual page
	}

	pNextDpage->index = 0;
	pNextDpage->state = NVM_STATE_ERR;

    if(doSearch)
    {
		i = __NVM_DPAGE_TOT_CNT;
        do
        {
            i--;
            pNextDpage->index = 0;
            pNextDpage->info.eraseCycles = 0xFFFFFFFF;

            //-------- Search d-page with smallest number of page-erase cycles --------
            for(j = 0; j < __NVM_DPAGE_TOT_CNT; j++)
            {
                if(dPageValid[j] == true)
                {
                    getDpageInfo(&iPage, j);

                    if(iPage.eraseCycles > NVM_EPAGE_CYCLES)                                    // Page never used (0xFFFFFFFF) or corrupted
                    {
                        iPage.eraseCycles = 0;
                    }

                    if(iPage.eraseCycles < pNextDpage->info.eraseCycles)
                    {
                        pNextDpage->info.eraseCycles = iPage.eraseCycles;
                        pNextDpage->index = j;
                        pNextDpage->state = NVM_STATE_OK;
                    }
                }
            }

            if(pNextDpage->state == NVM_STATE_OK)
            {
                if(setupNextDpage(pNextDpage) == CMD_DPAGE_OK)
                {
                    pNextDpage->info.totWriteCycles = pActualDpage->info.totWriteCycles + 1;
					//pNextDpage->info.totCrcErrCnt   = actualDpage.info.totCrcErrCnt;
                }
                else
                {
                    nvm.error.erase++;
                    pNextDpage->state = NVM_STATE_ERR;
                    dPageValid[(pNextDpage->index)] = false;                                // Erase failed d-page invalid

                    #if (__NVM_DPAGE_CNT_PER_EPAGE > 1)
                    if(((pNextDpage->index) % __NVM_DPAGE_CNT_PER_EPAGE) == 0)
                    {
                        for(j = 1; j < __NVM_DPAGE_CNT_PER_EPAGE; j++)
                        {
                            dPageValid[(pNextDpage->index) + j] = false;                    // If first d-page erase failed -> all d-pages contained in e-page are invalid
                        }
                    }
                    #endif
                }
            }
        }
        while((i > 0) && (pNextDpage->state != NVM_STATE_OK));
    }

    pNextDpage->addr = getDpageAddr(pNextDpage->index);
}

/** ********************************************************************************************************************
* \brief Updates Nvm state according to actual and next d-pages states
*
* If (actual d-page == no data) and (next d-page == error) then     nvm.state = NVM_STATE_ERR
* If (actual d-page == no data) and (next d-page == ok)    then     nvm.state = NVM_STATE_NO_DATA
* If (actual d-page == ok)      and (next d-page == error) then     nvm.state = NVM_STATE_READ_ONLY
* If (actual d-page == ok)      and (next d-page == ok)    then     nvm.state = NVM_STATE_OK
* If (actual d-page == error)   and (next d-page == ok)    then     nvm.state = NVM_STATE_NO_DATA
* If (actual d-page == error)   and (next d-page == error) then     nvm.state = NVM_STATE_NO_ERR
*
* \param pActualDpage
*        pointer to actual d-page
*
* \param pNextDpage
*        pointer to next d-page (erased)
*
***********************************************************************************************************************/

static void updateNvmData( const dpage_t *pActualDpage, const dpage_t *pNextDpage )
{
    switch(pActualDpage->state)
    {
        case NVM_STATE_NO_DATA:                                     // No valid data present
            if(pNextDpage->state == NVM_STATE_ERR)
            {
                nvm.state = NVM_STATE_ERR;                          // NO read, no write
            }
            else
            {
                nvm.state = NVM_STATE_NO_DATA;
            }
            break;

        case NVM_STATE_OK:                                          // Valid data present
            nvm.dbg.readPageCycles = pActualDpage->info.eraseCycles;
            nvm.dbg.readPageIndex  = pActualDpage->index;

            if(pNextDpage->state == NVM_STATE_ERR)
            {
                nvm.state = NVM_STATE_READ_ONLY;                    // Read only
            }
            else
            {
                nvm.state = NVM_STATE_OK;
            }
            break;

        default:
        case NVM_STATE_ERR:											// Pages with wrong crc
            if(pNextDpage->state == NVM_STATE_OK)
            {
                nvm.state = NVM_STATE_NO_DATA;                      // No valid data present
            }
            else
            {
                nvm.state = NVM_STATE_ERR;
            }
            break;
    }

    nvm.writeCycles = pActualDpage->info.totWriteCycles;

    if(pNextDpage->state == NVM_STATE_OK)
    {
        nvm.dbg.writePageCycles = pNextDpage->info.eraseCycles;
        nvm.dbg.writePageIndex  = pNextDpage->index;
    }
}

/** ********************************************************************************************************************
* \brief Resets nvm errors
*
***********************************************************************************************************************/

static void resetNvmErrors( void )
{
    memset(&nvm.error, 0, sizeof(nvm_err_t));
}

/** ********************************************************************************************************************
* \brief  Adds a byte to the w-page buffer. If buffer is full w-page is written in nv memory
*
* \param value
*        byte to add
*
* \retval error structs
*
***********************************************************************************************************************/

static nvm_err_t wPageAddByte( uint8_t value )
{
    nvm_err_t err = NVM_ERR_NO_ERR;

    wpage.buf[wpage.byteCnt] = value;

    if(++wpage.byteCnt == NVM_WPAGE_SIZE_BYTE)                     // If w-page full -> write it and increment addr offset
    {
        nvm_drv_unlock();

        err = nvm_drv_write_wpage(wpage.addr, (uint8_t *)wpage.buf);

        nvm_drv_lock();

        wpage.addr += NVM_WPAGE_SIZE_BYTE;

        memset(wpage.buf, 0, NVM_WPAGE_SIZE_BYTE);

        wpage.byteCnt = 0;
    }

    return err;
}

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief Initializes the nv memory at system power-on.
*
* Initializes hardware and internal data structures of the nvm module.
*
* It identifies the d-page where data has been saved during most recent nvmWriteAll() call. If the CRC of this page is
* corrupted the previously used page is addressed.
*
* It prepares the nv memory to be written again by erasing the d-page used least recently.
* If a d-page has bad CRC it is erased and used.
*
* \warning This routine must be called during initialization (after reset) \b exactly \b once before any other
* Nvm API usage.
*
* \warning It may take several tens of ms and may block the controller completely, even interrupt execution might
* be not possible during this function call.
*
* \warning This function decreases the number of writing cycles by one because a new memory page is erased in order
* to be used at next nvmWriteAll() call.
*
* \warning This function changes <B>(Atmel SAM-Dxx MCUs only)</B> the fuses settings according to the required flash
* memory size.
*
** \retval constant pointer to nvm_t struct (see nvm.h).
*
***********************************************************************************************************************/

const nvm_t* nvmInit( void )
{
	nvm_drv_init();

    #ifdef NVM_DRV_SAMD21
    if(nvm_drv_get_eeprom_fuses() > EEPROM_SIZE_CFG)
    {
        nvm_drv_set_eeprom_fuses(EEPROM_SIZE_CFG);                      // Change fuses
    }
    #endif

    crcInit();

    if(sizeof(dpage_info_t) != NVM_DPAGE_INFO_SIZE_BYTE)                // Check dpage_info_t struct size according to user config
    {
        ErrorShutdown("Dpage size error");
    }

	memset(&nvm,     0, sizeof(nvm_t));
	memset(&nvmData, 0, sizeof(nvm_data_t));

	getActualDpage(&actualDpage);
    getNextDpage(&actualDpage, &nextDpage);

    updateNvmData(&actualDpage, &nextDpage);

	return &nvm;
}

/** ********************************************************************************************************************
* \brief Resumes nv module after nvmWriteAll() call
*
* Re-initializes internal data structures and whenever needed prepares a new page to be written at next nvmWriteAll()
* call.
*
* \warning This routine must be called after every nvmWriteAll() call before any further Nvm API usage <B>if and only if
* the MCU does not reset i.e. does not execute nvmInit()</B>.
*
* \warning It may take several tens of ms and may block the controller completely, even interrupt execution might
* be not possible during this function call.
*
* \warning This function decreases the number of writing cycles by one because a new memory page is erased in order
* to be used at next nvmWriteAll() call.
*
***********************************************************************************************************************/

void nvmResume( void )
{
    resetNvmErrors();

    getActualDpage(&actualDpage);

    if(actualDpage.index != nextDpage.index)
    {
		nvm.error.write++;
    }

    getNextDpage(&actualDpage, &nextDpage);

    updateNvmData(&actualDpage, &nextDpage);
}

/** ********************************************************************************************************************
* \brief Declares and initializes data as nvm data
*
* In order to make data non-volatile this function records address and size of data structures; these information are
* then used when nvmWriteAll() is called in order to save data in nv memory. If the nv memory has valid data (CRC is ok)
* this function loads data from nv memory to RAM structures.
*
* \warning If the specified maximum number of initializations is exceeded i.e. nvmDataInit() has been called more than
* NVM_DATA_STRUCTS_CNT_MAX times, an error is thrown.
*
* \warning If the specified total maximum size of nv data i.e. NVM_DATA_SIZE_BYTE is exceeded then the function throws
* an error.
*
* \warning During initialization nvmDataInit() might be called several times but the calling sequence shall remain the
* same over multiple power cycles. Conditional calls of nvmDataInit() are therefore not allowed.
*
* \param pdata
*        pointer to data to initialize
*
* \param size
*        data size [byte]
*
* \param mc
*        memory class (currently unused)
*
* \retval NVM_INIT_OK when data has been successfully initialized, NVM_INIT_NEW if data not initialized.
*
***********************************************************************************************************************/

nvm_init_t nvmDataInit( void *pdata, uint32_t size, nvm_memclass_t mc )
{
    nvm_init_t nvmDataInit = NVM_INIT_NEW;

    resetNvmErrors();

    //---------- Store data info -----------
    if(nvmData.idxMax < NVM_DATA_STRUCTS_CNT_MAX)
    {
        nvmData.item[nvmData.idxMax].ptr = (uint8_t *)pdata;
        nvmData.item[nvmData.idxMax].size = size;

        nvmData.idxMax++;
    }
    else
    {
        ErrorShutdown("Structs number max exceeded");
    }

    //--------- Load data from nvm ---------
    if((nvm.state == NVM_STATE_OK) || (nvm.state == NVM_STATE_READ_ONLY))
    {
		if((nvmData.totSizeByte + size) <= actualDpage.info.dataSizeByte)
		{
			nvm_drv_read_buffer((actualDpage.addr + nvmData.totSizeByte), size, pdata);

			nvmDataInit = NVM_INIT_OK;
		}
    }

	nvmData.totSizeByte += size;

	if(size != (2*(size>>1)))						// Even alignment check
	{
		nvmData.totSizeByte += 1;
	}

    if(nvmData.totSizeByte > NVM_DATA_WORD_ALIGNED_SIZE_BYTE)
    {
        ErrorShutdown("Not enough space");
    }

    return nvmDataInit;
}

/** ********************************************************************************************************************
* \brief Writes all RAM data initialized with nvmDataInit() in nv memory.
*
* This function is intended to be called when a power-failure is detected. After that the MCU is expected to switch-off
* or,whenever the power-supply is restored, an nvmResume() call is needed in order to set-up the nvm memory again.
*
* \warning It may take several ms and may block the controller completely, even interrupt execution might
* be not possible during each w-page writing.
*
* \warning If nvmWriteAll() is used during a power-failure event, it must be guaranteed under every circumstance to
* bring the function call to conclusion otherwise data in flash might result incomplete or corrupted.
*
***********************************************************************************************************************/

void nvmWriteAll( void )
{
    uint8_t *p;
    uint32_t byteCnt;
    uint32_t dataIdx = 0;

    resetNvmErrors();

    if((nvm.state == NVM_STATE_OK) || (nvm.state == NVM_STATE_NO_DATA))
    {
        crcReset();

        memset(&wpage, 0, sizeof(wpage_t));
        wpage.addr = nextDpage.addr;

        nextDpage.info.dataSizeByte = 0;

        //--------------------- Write data structs -------------------
        for(dataIdx = 0; dataIdx < nvmData.idxMax; dataIdx++)
        {
            p = (uint8_t *)nvmData.item[dataIdx].ptr;

            for(byteCnt = 0; byteCnt < nvmData.item[dataIdx].size; byteCnt++, p++)
            {
				if(wPageAddByte(*p) != NVM_ERR_NO_ERR)
				{
					nvm.error.write++;
				}

                crcLoadByte(*p);
            }

            nextDpage.info.dataSizeByte += nvmData.item[dataIdx].size;

            if(nvmData.item[dataIdx].size != (2*(nvmData.item[dataIdx].size >> 1)))
            {
				if(wPageAddByte(0) != NVM_ERR_NO_ERR)
				{
					nvm.error.write++;
				}

                crcLoadByte(0);
                nextDpage.info.dataSizeByte += 1;
            }
        }

        //----------------- Dummy bytes up to info page --------------
        for(byteCnt = nextDpage.info.dataSizeByte; byteCnt < NVM_OFFSET_DPAGE_INFO; byteCnt++)
        {
			if(wPageAddByte(0) != NVM_ERR_NO_ERR)
			{
				nvm.error.write++;
			}

            crcLoadByte(0);
        }

        //------------------------- Info page ------------------------
        p = (uint8_t *)&nextDpage.info;

        for(byteCnt = 0; byteCnt < (NVM_DPAGE_INFO_SIZE_BYTE - 4); byteCnt++, p++)          // Do not load CRC yet
        {
			if(wPageAddByte(*p) != NVM_ERR_NO_ERR)
			{
				nvm.error.write++;
			}

            crcLoadByte(*p);
        }

        //------------------------ Write CRC -------------------------
        nextDpage.info.crc = crcGet();                                                     // Data is mutliple of 4 byte -> CRC calculation must be word aligned
        p = (uint8_t *)&nextDpage.info.crc;
        for(byteCnt = 0; byteCnt < 4; byteCnt++, p++)
        {
			if(wPageAddByte(*p) != NVM_ERR_NO_ERR)
			{
				nvm.error.write++;
			}
        }

        //--------------- Dummy bytes up to end of d-page -------------
        for(byteCnt = NVM_DATA_PLUS_INFO_PAGE_SIZE_BYTE; byteCnt < __NVM_DPAGE_SIZE_BYTE; byteCnt++)
        {
			if(wPageAddByte(0) != NVM_ERR_NO_ERR)
			{
				nvm.error.write++;
			}
        }

        if(wpage.addr != (nextDpage.addr + __NVM_DPAGE_SIZE_BYTE))                        // Check if all data has been written
        {
            nvm.error.addr++;
        }
    }
    else
    {
        nvm.error.write++;
    }
}

/** ********************************************************************************************************************
* \brief Compares nv RAM data with corresponding data stored in nv memory
*
* \param pdata
*        Pointer to data
*
* \param sizeByte
*        data size
*
* \retval NVM_CMP_EQUAL if data matches otherwise NVM_CMP_NOT_EQUAL
*
***********************************************************************************************************************/

nvm_cmp_t nvmDataCompare( const void *pdata, uint32_t sizeByte )
{
    uint32_t i;
	uint32_t addr;
    nvm_cmp_t dataCmp = NVM_CMP_NOT_EQUAL;
	uint32_t addrOffset;

    resetNvmErrors();
    i = 0;
    addrOffset = 0;

    while(((uint8_t *)pdata) != nvmData.item[i].ptr && (i < nvmData.idxMax))							// Look for struct with same address
    {
		addrOffset += nvmData.item[i].size;

		if(nvmData.item[i].size != (2*(nvmData.item[i].size >> 1)))                                     // Odd size
		{
			addrOffset += 1;
		}

		i++;
    }

    if((i < nvmData.idxMax) && (sizeByte <= nvmData.item[i].size))										// Check data size
    {
		addr = actualDpage.addr + addrOffset;

		addr = GET_NVM_ADDR(addr);

        if(memcmp(pdata, (void *)(addr), sizeByte) == 0)												// Compare
        {
            dataCmp = NVM_CMP_EQUAL;
        }
    }

    return dataCmp;
}

/** ********************************************************************************************************************
* \brief Returns the total number of write-cycles already done.
*
* If writeCycles > NVM_WRITE_CYCLES the number is invalid.
*
* \retval tot write-cycles
*
***********************************************************************************************************************/

uint32_t nvmGetWriteCycles(void)
{
    return nvm.writeCycles;
}

/**********************************************************************************************************************/

/** \} */ // NvmAPI
/** \} */ // Nvm