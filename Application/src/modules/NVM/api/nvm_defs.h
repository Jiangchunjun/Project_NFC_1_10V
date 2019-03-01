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
// $Revision: 1003 $
// $Date: 2015-07-10 21:09:45 +0800 (Fri, 10 Jul 2015) $
// $Id: nvm_defs.h 1003 2015-07-10 13:09:45Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/api/nvm_defs.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_DEFS_H
#define __NVM_DEFS_H

/** \addtogroup NvmInternal
 * \{
 *
 * \file
 * \brief Defines file for nvm.c
 *
 * \defgroup NvmInternalDefines Nvm Defines
 * \brief Nvm module implementation details: defines and assertions
 * \{
 */

/**********************************************************************************************************************/

#include "nvm_driver_mcu.h"

/***************************************************** Macros *********************************************************/

#define IS_NOT_POWER_OF_2(v) (((v) & ((v) - 1)) && (v))

/************************************************ Config data check ***************************************************/

#if IS_NOT_POWER_OF_2(NVM_WPAGE_SIZE_BYTE)
#error !!! NVM_WPAGE_SIZE_BYTE must be a power of 2 !!!
#endif

#if IS_NOT_POWER_OF_2(NVM_EPAGE_SIZE_BYTE)
#error !!! NVM_EPAGE_SIZE_BYTE must be a power of 2 !!!
#endif

#if NVM_WPAGE_SIZE_BYTE < 2
#error !!! NVM_WPAGE_SIZE_BYTE must be > 1 !!!
#endif

#if NVM_EPAGE_SIZE_BYTE < 2
#error !!! NVM_EPAGE_SIZE_BYTE must be > 1 !!!
#endif

#ifndef NVM_DATA_SIZE_BYTE
#error !!! NVM_DATA_SIZE_BYTE not defined !!!
#endif

#ifndef NVM_WRITE_CYCLES
#error !!! NVM_WRITE_CYCLES not defined !!!
#endif

#ifndef NVM_DATA_STRUCTS_CNT_MAX
#error !!! NVM_DATA_STRUCTS_CNT_MAX not defined !!!
#endif

#ifndef NVM_SIZE_BYTE
#error !!! NVM_SIZE_KBYTE not defined !!!
#endif

/**********************************************************************************************************************/

#define NVM_DATA_WORD_ALIGNED_SIZE_BYTE				(4*((NVM_DATA_SIZE_BYTE + NVM_DATA_STRUCTS_CNT_MAX + 3)/4))         // Data area multiple of 4 bytes

#define NVM_OFFSET_DPAGE_INFO                       NVM_DATA_WORD_ALIGNED_SIZE_BYTE						// [.]

//-----------

#define NVM_OFFSET_TOT_WRITE_CYCLES                 (NVM_OFFSET_DPAGE_INFO)                             // [.]
#define NVM_OFFSET_PAGE_ERASE_CNT                   (NVM_OFFSET_TOT_WRITE_CYCLES + 4)                   // [.]
//#define NVM_OFFSET_TOT_CRC_ERR_CNT                  (NVM_OFFSET_PAGE_ERASE_CNT + 4)                     // [.]
#define NVM_OFFSET_DATA_SIZE                        (NVM_OFFSET_PAGE_ERASE_CNT + 4) //(NVM_OFFSET_TOT_CRC_ERR_CNT + 4)                    // [.]
#define NVM_OFFSET_PAGE_CRC                         (NVM_OFFSET_DATA_SIZE + 4)                          // [.]

//-----------

#define NVM_DPAGE_INFO_SIZE_BYTE                    ((NVM_OFFSET_PAGE_CRC + 4) - NVM_OFFSET_DPAGE_INFO)	// [.]

/**********************************************************************************************************************/

#define ODD_EVEN_EPAGE_SIZE_BYTE					(2*(NVM_EPAGE_SIZE_BYTE/2))

#if ( (ODD_EVEN_EPAGE_SIZE_BYTE != NVM_EPAGE_SIZE_BYTE) && (NVM_EPAGE_SIZE_BYTE != 1) )
#error !!! NVM_EPAGE_SIZE_BYTE must be even !!!
#endif

#define ODD_EVEN_WPAGE_SIZE_BYTE                    (2*(NVM_WPAGE_SIZE_BYTE/2))

#if ( (ODD_EVEN_WPAGE_SIZE_BYTE != NVM_WPAGE_SIZE_BYTE) && (NVM_WPAGE_SIZE_BYTE != 1) )
#error !!! NVM_WPAGE_SIZE_BYTE must be even !!!
#endif

/**********************************************************************************************************************/

#define NVM_DATA_PLUS_INFO_PAGE_SIZE_BYTE		    (NVM_DATA_WORD_ALIGNED_SIZE_BYTE + NVM_DPAGE_INFO_SIZE_BYTE)

#define NVM_WPAGE_CNT_PER_DPAGE						((NVM_DATA_PLUS_INFO_PAGE_SIZE_BYTE + (NVM_WPAGE_SIZE_BYTE - 1)) / NVM_WPAGE_SIZE_BYTE)
#define __NVM_DPAGE_SIZE_BYTE						(NVM_WPAGE_CNT_PER_DPAGE * NVM_WPAGE_SIZE_BYTE)

/**********************************************************************************************************************/

#define __NVM_DPAGE_CNT_PER_EPAGE					( NVM_EPAGE_SIZE_BYTE/__NVM_DPAGE_SIZE_BYTE )

#if ( __NVM_DPAGE_CNT_PER_EPAGE == 0 )

    #define __NVM_EPAGE_CNT_PER_DPAGE               (( __NVM_DPAGE_SIZE_BYTE + (NVM_EPAGE_SIZE_BYTE - 1)) / NVM_EPAGE_SIZE_BYTE)
    #define DPAGE_TOT_CNT_CANDIDATE				    ((__NVM_EPAGE_CNT_PER_DPAGE * ((NVM_WRITE_CYCLES + (NVM_EPAGE_CYCLES - 1)) / NVM_EPAGE_CYCLES))/__NVM_EPAGE_CNT_PER_DPAGE)

    #if (DPAGE_TOT_CNT_CANDIDATE < 4)
        #define __NVM_DPAGE_TOT_CNT                 (4)         // At least 4 d-pages needed when more than 1 e-page is used to cover the complete d-page
        #define NVM_EPAGE_TOT_CNT                   (__NVM_DPAGE_TOT_CNT * __NVM_EPAGE_CNT_PER_DPAGE)
    #else
        #define NVM_EPAGE_TOT_CNT				    (__NVM_EPAGE_CNT_PER_DPAGE * ((NVM_WRITE_CYCLES + (NVM_EPAGE_CYCLES - 1)) / NVM_EPAGE_CYCLES))
        #define __NVM_DPAGE_TOT_CNT			        (NVM_EPAGE_TOT_CNT/__NVM_EPAGE_CNT_PER_DPAGE)
    #endif

#else

	#define __NVM_EPAGE_CNT_PER_DPAGE               (1)
    #define DPAGE_TOT_CNT_CANDIDATE 			    (((NVM_WRITE_CYCLES + ((NVM_EPAGE_CYCLES * __NVM_DPAGE_CNT_PER_EPAGE) - 1)) / (__NVM_DPAGE_CNT_PER_EPAGE * NVM_EPAGE_CYCLES)) * __NVM_DPAGE_CNT_PER_EPAGE)

    #if( DPAGE_TOT_CNT_CANDIDATE <= ( 4 * __NVM_DPAGE_CNT_PER_EPAGE) )
        #define NVM_EPAGE_TOT_CNT				    (4)			// At least 4 e-pages needed when e-page is divided in several d-pages
    #else
        #define NVM_EPAGE_TOT_CNT				    ((NVM_WRITE_CYCLES + ((NVM_EPAGE_CYCLES * __NVM_DPAGE_CNT_PER_EPAGE) - 1 )) / ( __NVM_DPAGE_CNT_PER_EPAGE * NVM_EPAGE_CYCLES))
    #endif

    #define __NVM_DPAGE_TOT_CNT				        (__NVM_DPAGE_CNT_PER_EPAGE * NVM_EPAGE_TOT_CNT)

#endif

/**********************************************************************************************************************/

#define __NVM_AREA_TOT_SIZE_BYTE  				    ( NVM_EPAGE_TOT_CNT * NVM_EPAGE_SIZE_BYTE )

#define __NVM_DATA_BASE_ADDR    		            ( ( NVM_BASE_ADDR + NVM_SIZE_BYTE ) - __NVM_AREA_TOT_SIZE_BYTE )

#if ( __NVM_AREA_TOT_SIZE_BYTE > NVM_SIZE_BYTE )
    #error !!! Data size and writing-cycles requirements cannot be fulfilled !!!
#endif

/**********************************************************************************************************************/

#ifdef NVM_DRV_SAMD21
    #if (__NVM_AREA_TOT_SIZE_BYTE > 16384)
        #error !!! Requirements cannot be fulfilled (SAM D21)!!!
    #else
        #if   ( __NVM_AREA_TOT_SIZE_BYTE > 8192)
            #define EEPROM_SIZE_CFG                     (0)
        #elif ( __NVM_AREA_TOT_SIZE_BYTE > 4096)
            #define EEPROM_SIZE_CFG                     (1)
        #elif ( __NVM_AREA_TOT_SIZE_BYTE > 2048)
            #define EEPROM_SIZE_CFG                     (2)
        #elif ( __NVM_AREA_TOT_SIZE_BYTE > 1024)
            #define EEPROM_SIZE_CFG                     (3)
        #elif ( __NVM_AREA_TOT_SIZE_BYTE > 512)
            #define EEPROM_SIZE_CFG                     (4)
        #elif ( __NVM_AREA_TOT_SIZE_BYTE > 256)
            #define EEPROM_SIZE_CFG                     (5)
        #else
            #define EEPROM_SIZE_CFG                     (6)
        #endif
    #endif
#endif

/**********************************************************************************************************************/

/** \} */ // NvmInternalDefines
/** \} */ // NvmInternal
#endif


