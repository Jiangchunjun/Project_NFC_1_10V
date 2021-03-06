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
// $Revision: 864 $
// $Date: 2015-07-01 18:26:47 +0800 (Wed, 01 Jul 2015) $
// $Id: ConfigNvmTemplate.h 864 2015-07-01 10:26:47Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/api/ConfigNvmTemplate.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_CONFIG_H
#define __NVM_CONFIG_H

/** \addtogroup NvmConfig
 * \{
 *
 * \file
 * \brief A template configuration file for the Nvm module. Please copy, rename and adjust in the projects.
 *
 */

#error "Do not include this file. Copy it in your project."


/** \brief \b Total flash size of the microcontroller.
 *
 * Given by hardware. The nvm module needs this to calculate the start position of data flash area located just below
 * the end of the flash memory.
 */
#define NVM_SIZE_KBYTE          64


/** \brief Number of needed of power-on/power-off (nvm write) cycles.
*
* Defined by marketing and product development specifying the number of power-on/power-off cycles the device has to
* be able to withstand.
*
* Used by the nvm module to calculate the minimum amount of memory to be dedicated for data flash considering
* - the amount of data to be written (\ref NVM_DATA_SIZE_BYTE)
* - the minimum number of guaranteed write cycles provided by the hardware (\ref NVM_EPAGE_CYCLES).
* - the overhead for nvm internal data and address alignment
*/
#define NVM_WRITE_CYCLES       250000


/** \brief Amount of non volatile data the nvm module should provide.
*
* The real amount of data in the nvm pages will slightly differ from this number, as there is an overhead caused
* by address alignment. When the nvm module calculates write cycles or reserved flash area, this overhead (as well
* as that for nvm internal data) is taken into account (\ref NVM_WRITE_CYCLES). Variable <B>nvmData.totSizeByte</B> defined 
* in nvm.c counts at run-time the total bytes intended to be saved by nvm module. If error "Not enough space" is thrown 
* during a <B>nvmDataInit</B>() call this define must be increased.
*/
#define NVM_DATA_SIZE_BYTE        250


/** \brief Amount of non volatile data the nvm module should provide.
*
* This define specifies the number of times the nvmDataInit() function is intended to be called in the application program.
* Every time nvmDataInit() is called a new entry in a table is filled. This table is used by nvm module for data management and 
* is not accessible by the user. With this define the internal table size can be tailored to the real needs in order 
* to spare as much RAM memory as possible.
*/
#define NVM_DATA_STRUCTS_CNT_MAX   15


/** \} */
#endif
