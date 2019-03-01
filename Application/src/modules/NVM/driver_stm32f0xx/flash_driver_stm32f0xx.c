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
// $Id: flash_driver_stm32f0xx.c 1003 2015-07-10 13:09:45Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/driver_stm32f0xx/flash_driver_stm32f0xx.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \addtogroup NvmDriver
 * \{
 * \defgroup NvmDriverStm32f0xx Nvm driver Stm32f0xx
 * \{
 * \brief Flash driver for Stm32f0xx
 *
 * \file
 * \brief Nvm driver for Stm32f0xx
 */

#define MODULE_NVM
#include "Config.h"

#include "Global.h"
#include "nvm_driver_mcu.h"
#include "nvm_driver.h"
#include "stm32f0xx_flash.h"			// ST standard peripheral library

/**********************************************************************************************************************/

#ifndef _X86_
    #define FLASH_READ_BYTE( a )        (*(volatile uint8_t *)( a ))
#endif

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief Initializes nv memory driver
*
***********************************************************************************************************************/

void nvm_drv_init( void )
{

}

/** ********************************************************************************************************************
* \brief Locks nv memory
*
***********************************************************************************************************************/

void nvm_drv_lock(void)
{
    FLASH_Lock();
}

/** ********************************************************************************************************************
* \brief Unlocks nv memory
*
***********************************************************************************************************************/

void nvm_drv_unlock(void)
{
    FLASH_Unlock();
}

/** ********************************************************************************************************************
* \brief Erases an e-page
*
* \param epage_addr
*        e-page address
*
* \retval NVM_ERR_NO_ERR if e-page erased successfully
*
***********************************************************************************************************************/

nvm_err_t nvm_drv_erase_epage( uint32_t epage_addr )
{
    nvm_err_t err_code = NVM_ERR_NO_ERR;
    uint32_t addr_check;

    addr_check = (NVM_EPAGE_SIZE_BYTE*(epage_addr/NVM_EPAGE_SIZE_BYTE));        // Addr must be multiple of e-page size

    if(addr_check != epage_addr)
    {
        err_code = NVM_ERR_ADDR;
    }
	else
    {
        if(FLASH_ErasePage(epage_addr) != FLASH_COMPLETE)
        {
            err_code = NVM_ERR_ERASE;
        }
    }

    return err_code;
}

/** ********************************************************************************************************************
* \brief Writes a w-page
*
* \param wpage_addr
*        w-page address
*
* \param pdata
*        pointer to data to write
*
* \warning pdata shall pointer to a memory area as big as a w-page
*
* \retval NVM_ERR_NO_ERR if w-page written successfully
*
***********************************************************************************************************************/

nvm_err_t nvm_drv_write_wpage( uint32_t wpage_addr, const void *pdata )
{
    nvm_err_t err_code = NVM_ERR_NO_ERR;
    uint32_t addr_check;

    addr_check = (NVM_WPAGE_SIZE_BYTE*(wpage_addr/NVM_WPAGE_SIZE_BYTE));        // Addr must be multiple of w-page size

    if(addr_check != wpage_addr)
    {
        err_code = NVM_ERR_ADDR;
    }
	else
    {
        if(FLASH_ProgramHalfWord(wpage_addr, *(uint16_t *)pdata) != FLASH_COMPLETE)
        {
            err_code = NVM_ERR_WRITE;
        }
    }

    return err_code;
}

/** ********************************************************************************************************************
* \brief Reads a byte
*
* \param addr 
*        byte address
*
* \param pdata 
*        pointer to byte where data is copied
*
* \retval NVM_ERR_NO_ERR if byte read successfully
*
***********************************************************************************************************************/

nvm_err_t nvm_drv_read_byte( uint32_t addr, uint8_t *pdata)
{
    nvm_err_t err_code = NVM_ERR_NO_ERR;

    if((addr < NVM_BASE_ADDR) || (addr > ( NVM_BASE_ADDR + NVM_SIZE_BYTE - 1)))
    {
        err_code = NVM_ERR_ADDR;
    }
    else
    {
        *pdata = FLASH_READ_BYTE(addr);
    }

    return err_code;
}

/** ********************************************************************************************************************
* \brief Reads a block of memory
*
* \param addr 
*        starting address
*
* \param size 
*        number of byte to read
*
* \param pdata 
*        pointer to block of memory where data is copied
*
* \retval NVM_ERR_NO_ERR if byte read successfully
*
***********************************************************************************************************************/

nvm_err_t nvm_drv_read_buffer( uint32_t addr, uint32_t size, void *pdata )
{
    uint32_t i;
    uint8_t *p;
    nvm_err_t err_code = NVM_ERR_NO_ERR;

    if((addr < NVM_BASE_ADDR) || ((addr + size) > ( NVM_BASE_ADDR + NVM_SIZE_BYTE)))
    {
        err_code = NVM_ERR_ADDR;
    }
    else
    {
        p = (uint8_t *)pdata;

        for(i = 0; i < size; i++)
        {
            *p = FLASH_READ_BYTE(addr + i);
            p++;
        }
    }

    return err_code;
}

/**********************************************************************************************************************/

/** \} */ // NvmDriverStm32f0xx
/** \} */ // NvmDriver

