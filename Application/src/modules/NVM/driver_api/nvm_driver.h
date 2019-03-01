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
// $Revision: 106 $
// $Date: 2015-01-07 18:42:28 +0800 (Wed, 07 Jan 2015) $
// $Id: nvm_driver.h 106 2015-01-07 10:42:28Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/driver_api/nvm_driver.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_DRIVER_H
#define __NVM_DRIVER_H

/** \addtogroup NvmDriverAPI
 * \{
 *
 * \file
 * \brief Header file for flash_driver_xx.c
 *
 */

/**********************************************************************************************************************/

#include <stdint.h>

/**********************************************************************************************************************/

typedef enum
{
    NVM_ERR_NO_ERR = 0,
	NVM_ERR_ADDR,
	NVM_ERR_LOCKED,
	NVM_ERR_ERASE,
	NVM_ERR_WRITE,
	NVM_ERR_READ,
    NVM_ERR_MAX
}
nvm_err_t;

/**********************************************************************************************************************/

void nvm_drv_init( void );
void nvm_drv_lock(void);
void nvm_drv_unlock(void);
nvm_err_t nvm_drv_erase_epage( uint32_t epage_addr );
nvm_err_t nvm_drv_write_wpage( uint32_t wpage_addr, const void *pdata );
nvm_err_t nvm_drv_read_byte( uint32_t addr, uint8_t *pdata);
nvm_err_t nvm_drv_read_buffer( uint32_t addr, uint32_t size, void *pdata );

/**********************************************************************************************************************/
/** \} */ // NvmDriverAPI
#endif

