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
// Initial version: 2015-05, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: ConfigNfcTemplate.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/api/ConfigNfcTemplate.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NFC_CONFIG_H
#define __NFC_CONFIG_H

/** \addtogroup NfcConfig
 * \{
 *
 * \file
 * \brief A template configuration file for the Nfc module. Please copy, rename and adjust in the projects.
 *
 */

#error "Do not include this file. Copy it in your project."

/** \brief Size max of data structs initialized as nfc data by nfcDataInit() can have.
*
* Data initialized as nfc (i.e. stored in nfc tag) is continuously compared with tag data. Comparisons are performed on copies
* of respective data i.e. before doing the comparison tag data is copied in a buffer and RAM copy as well. In order to save 
* as much RAM as possible these buffers have size able to contain the biggest nfc data struct declared. 
*/
#define NFC_STRUCT_MAX_SIZE_BYTE        (128)


/** \brief Number of data structs declared as nfc data
*
* This define specifies the number of times the nfcDataInit() function is intended to be called in the application program.
* Every time nfcDataInit() is called a new entry in a table is filled. This table is used by nfc module for data management and 
* is not accessible by the user. With this define the internal table size can be tailored to the real needs in order 
* to spare as much RAM memory as possible.
*
 \warning if an inner struct is declared this number has to be increased by 1.
*/
#define NFC_DATA_STRUCTS_CNT_MAX        (64)


/** \} */
#endif
