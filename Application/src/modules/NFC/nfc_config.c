// ---------------------------------------------------------------------------------------------------------------------
// nfc_config.c
// ---------------------------------------------------------------------------------------------------------------------
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
// Initial version: Sammy el Baradie,  27/07/2016
//
// $Author: g.salvador $
// $Revision: 16253 $
// $Date: 2018-12-18 17:59:27 +0800 (Tue, 18 Dec 2018) $
// $Id: nfc_config.c 16253 2018-12-18 09:59:27Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nfc/tags/v2.7/Src/nfc_config.c $
//
// ---------------------------------------------------------------------------------------------------------------------


#define MODULE_NFC
#include "Global.h"
#include "Config.h"
#include "ErrorHandler.h"
#include "Acl.h"
#include "Mpc.h"
#include "MpcInternal.h"
#include "MpcAllocation.h"

#include "Nfc.h"
#include "nfc_config.h"
#include "nfc_local.h"
#include "nfc_parameters.h"


/** \file
 * \brief Define device specific parameters for Device Identification Registers
 *        and TOC. Create NFC tag start addresses for each MPC instance.
 */

//-----------------------------------------------------------------------------
// constants
//-----------------------------------------------------------------------------

/** NFC_CONFIG_STD1 defines specific start addresses for data assignment to
 * different read / write protection classes
 */



#if(defined (NFC_CONFIG_UNPROTECTED_LAYOUT) && defined(NFC_CONFIG_PROTECTED_LAYOUT))
  STATIC_ASSERT(0,  "Only ONE TAG protection layout configuration has to be choosen between\n "
                    "NFC_CONFIG_UNPROTECTED_LAYOUT and NFC_CONFIG_PROTECTED_LAYOUT.        \n "
                    "Please ensure to define only one of this in the entire code");
#endif



#if(defined (NFC_CONFIG_UNPROTECTED_LAYOUT)||defined(NFC_CONFIG_PROTECTED_LAYOUT))

// Define standard parameters fitting to several platforms
#ifdef NFC_CONFIG_UNPROTECTED_LAYOUT

    // First Mpc Start Address is right after the TOC (BIO page)
    // It is the read-only sector, protected by RF Passowrd 1
    // Its length vary depending on the TOC-length!
    #define NFC_FIRST_MPC_START_ADDR            ((NFC_TOC_START_ADDR + \
                                                NFC_TAG_TOC_REGISTER_BYTE_COUNT + \
                                                NFC_CRC_LENGTH + 4) & 0xFFFC)
    // Fix address of the Status Register
    #define NFC_STATUS_REGISTER_START_ADDR      252

    // Fix address of the Control Register
    #define NFC_CONTROL_REGISTER_START_ADDR     256

    // Protected memory area start address.
    // It is protected by RF Password 2
    // It has a fixed length of 3 sectors (11, 12 and 13 -> 128*3= 384 bytes)
    #define NFC_PROTECTED_MEMORY_START_ADDR    (NFC_CONTROL_REGISTER_START_ADDR + \
                                                9*NFC_TAG_SECTOR_LENGTH)
#else
    // First Mpc Start Address is right after the TOC (BIO page)
    // It is the read-only sector, protected by RF Passowrd 1
    // Its length vary depending on the TOC-length!
    #define NFC_FIRST_MPC_START_ADDR            ((NFC_TOC_START_ADDR + \
                                                NFC_TAG_TOC_REGISTER_BYTE_COUNT + \
                                                NFC_CRC_LENGTH + 4) & 0xFFFC)
    // Fix address of the Status Register
    #define NFC_STATUS_REGISTER_START_ADDR      252

    // Fix address of the Control Register
    #define NFC_CONTROL_REGISTER_START_ADDR     256

    // Protected memory area start address.
    // It is protected by RF Password 2
    // It has a fixed length of 11 sectors (from 3 to 13 -> 128*11= 1408 bytes)
    #define NFC_PROTECTED_MEMORY_START_ADDR    (NFC_CONTROL_REGISTER_START_ADDR + \
                                                NFC_TAG_SECTOR_LENGTH)
#endif

#else
  STATIC_ASSERT(0,  "A TAG protection layout has to be choosen between NFC_CONFIG_UNPROTECTED_LAYOUT \n"
                    " and NFC_CONFIG_PROTECTED_LAYOUT. Please ensure that your project includes the \n"
                    " definition for the appropriate TAG protection layout. (usually in the Config.h)");
#endif


//-----------------------------------------------------------------------------
/**
 * array of special NFC TAG addresses
 *
 * \note this array assigns defined tag address values
 * representing different read / write protection classes
 */
const nfc_tag_addr_config_t nfc_tag_addr_config =
{
   .first_mpc_start_addr          = NFC_FIRST_MPC_START_ADDR,
   .status_register_start_addr    = NFC_STATUS_REGISTER_START_ADDR,
   .control_register_start_addr   = NFC_CONTROL_REGISTER_START_ADDR,
   .protected_memory_start_addr   = NFC_PROTECTED_MEMORY_START_ADDR
};


//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------

/**
 * NFC TAG Copy Memory
 * externally defined to minimize ram size if NFC is part of a library
 */
uint8_t nfc_tag_copy[];

/**
 * pointer to externally declared NFC tag start address array
 * used in this file to assign a defined array length
 */
uint16_t * NfcMpcTagStartAddr;

/**
 * pointer to TOC items array
 * used in this file to assign a defined array length
 */
toc_item_t * toc_items;


#ifdef NFC_TAG_COPY_BYTE_SIZE
   uint8_t  nfc_tag_copy[NFC_TAG_COPY_BYTE_SIZE];
#else
  STATIC_ASSERT(0,  "Parameter NFC_TAG_COPY_BYTE_SIZE is not defined. Please ensure that your project\n"
                    " - either defines an appropriate platform, which comes along with this parameter\n"
                    " - or has a ConfigNfc.h (normally included by Config.h) defining this parameter\n"
                    " - or has this definition somewhere else.");
#endif



// -----------------------------------------------------------------------------
/**
* \brief
* - MPC_COUNT corresponds to the number of implemented MPCs and
*   is defined in MpcDefaultConfiguration.h
* - MPC_NFC_INST enumerates MPC_COUNT with all its instances
* - For single channel devices, MPC_NFC_INST equals MPC_COUNT
*/

//------------------------------------------------------------------------------
/**
 * \brief define array with defined length for assignment to NfcMpcTagStartAddr
*/
uint16_t nfc_p[MPC_NFC_INST];


/**
 * \brief define array with defined length for assignment to toc_items
*/
toc_item_t toc_p[MPC_NFC_INST];


//------------------------------------------------------------------------------
// global functions
//------------------------------------------------------------------------------
/**
 * Return NFC tag copy memory size
 *
 * Normally this parameter is not needed anywhere except here. Therefore,
 * this function should not be used/linked in production code.
 *
 * Nevertheless add this function for test purposes to provide the authoritative
 * answer about this parameter also in case of config file issues.
 */
// LCOV_EXCL_START : Normally unneeded function tested inherently if really used
uint16_t nfc_tag_copy_size ( void )
{
  return NFC_TAG_COPY_BYTE_SIZE;
}
// LCOV_EXCL_STOP


// -----------------------------------------------------------------------------
/**
 * \brief initialize arrays NfcMpcTagStartAddr[] and toc_items[]
 * \note The function is needed to allow different values in unit tests for
 * MPC_COUNT and MPC_NFC_INST
 * - assign pointers with defined lengths in project firmware
 * - assign pointers with changeable lengths in unit tests
 * \param length: Needed for unit tests only to assign variable array lengths
 */
void nfc_init_arrays ( uint8_t length )
{
  #ifdef DEVICE_UNDER_TEST
    #if defined (__ICCARM__) || defined (ECG_SIMULATOR)
      #error "This code is for unit tests only; It never ever should be compiled for real life or even the Simulator."
    #endif
    #include <stdlib.h>
    NfcMpcTagStartAddr = malloc(length * sizeof(uint16_t));
    toc_items = malloc(length * sizeof(toc_item_t));
  #else
    NfcMpcTagStartAddr = nfc_p;
    toc_items = toc_p;
  #endif
}

