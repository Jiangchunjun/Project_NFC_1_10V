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
// $Revision: 107 $
// $Date: 2015-01-07 11:43:06 +0100 (mer, 07 gen 2015) $
// $Id: flash_driver_xmc1300.c 107 2015-01-07 10:43:06Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Toolbox1.0/Nvm/trunk/driver_xmc1300/flash_driver_xmc1300.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \addtogroup NvmDriver
 * \{
 * \defgroup NvmDriverSamD21 Nvm driver SamD21
 * \{
 * \brief Flash driver for SamD21
 *
 * \file
 * \brief Nvm driver for SamD21
 */

#include <stdint.h>
#include <stdbool.h>
#include <clock.h>

#define MODULE_NVM
#include "Config.h"

#include "Global.h"
#include "nvm_driver_mcu.h"
#include "nvm_driver.h"
#include "ErrorHandler.h"

/**********************************************************************************************************************/

#define NVM_MEMORY          ((volatile uint16_t *)NVM_BASE_ADDR)

/**********************************************************************************************************************/

Nvmctrl *const nvm_module = NVMCTRL;    // Get a pointer to the module hardware instance

/**********************************************************************************************************************/

typedef enum
{
    NVM_CMD_ERASE_EPAGE    = NVMCTRL_CTRLA_CMD_ER,
    NVM_CMD_WRITE_WPAGE    = NVMCTRL_CTRLA_CMD_WP,
    NVM_CMD_ERASE_AUX_ROW  = NVMCTRL_CTRLA_CMD_EAR,
    NVM_CMD_PAGE_BUF_CLEAR = NVMCTRL_CTRLA_CMD_PBC,
    NVM_CMD_WRITE_AUX_PAGE = NVMCTRL_CTRLA_CMD_WAP,
    NVM_CMD_MAX
}
nvm_cmd_t;

/**********************************************************************************************************************/

typedef enum nvm_sleep_power_mode
{
    NVM_SLEEP_PWR_WAKE_ON_ACCESS = NVMCTRL_CTRLB_SLEEPPRM_WAKEONACCESS_Val,
    NVM_SLEEP_PWR_WAKEUP_INSTANT = NVMCTRL_CTRLB_SLEEPPRM_WAKEUPINSTANT_Val,
    NVM_SLEEP_PWR_ALWAYS_AWAKE   = NVMCTRL_CTRLB_SLEEPPRM_DISABLED_Val,
    NVM_SLEEP_PWR_MAX
}
nvm_sleep_pwr_t;

/**********************************************************************************************************************/

typedef enum
{
	/** The NVM Controller (cache system) does not insert wait states on
	 *  a cache miss. Gives the best system performance.
	 */
	NVM_CACHE_READMODE_NO_MISS_PENALTY,
	/** Reduces power consumption of the cache system, but inserts a
	 *  wait state each time there is a cache miss
	 */
	NVM_CACHE_READMODE_LOW_POWER,
	/** The cache system ensures that a cache hit or miss takes the same
	 *  amount of time, determined by the number of programmed flash
	 *  wait states.
	 */
	NVM_CACHE_READMODE_DETERMINISTIC,
}
nvm_cache_readmode;

/**********************************************************************************************************************/

struct nvm_config {
	/** Power reduction mode during device sleep. */
	nvm_sleep_pwr_t sleep_power_mode;
	/** Manual write mode; if enabled, pages loaded into the NVM buffer will
	 *  not be written until a separate write command is issued. If disabled,
	 *  writing to the last byte in the NVM page buffer will trigger an automatic
	 *  write.
	 *
	 *  \note If a partial page is to be written, a manual write command must be
	 *        executed in either mode.
	 */
	bool manual_page_write;
	/** Number of wait states to insert when reading from flash, to prevent
	 *  invalid data from being read at high clock frequencies.
	 */
	uint8_t wait_states;

	/**
	 * Setting this to true will disable the pre-fetch cache in front of the
	 * nvm controller.
	 */
	bool disable_cache;

	/**
	 * Select the mode for  how the cache will pre-fetch data from the flash.
	 */
	nvm_cache_readmode cache_readmode;
};

/** ********************************************************************************************************************
* \brief Checks whether nvm is ready
*
* \retval TRUE if ready
*
***********************************************************************************************************************/
static inline bool nvm_is_ready(void)
{
	return (nvm_module->INTFLAG.reg)&NVMCTRL_INTFLAG_READY;
}

/** ********************************************************************************************************************
* \brief Executes nvm command
*
* \param command
*        nvm command:   NVM_CMD_ERASE_EPAGE <BR>
*                       NVM_CMD_WRITE_WPAGE   <BR>
*                       NVM_CMD_ERASE_AUX_ROW  <BR>
*                       NVM_CMD_PAGE_BUF_CLEAR <BR>
*                       NVM_CMD_WRITE_AUX_PAGE <BR>
*
* \retval STATUS_OK if command executed successfully
*
***********************************************************************************************************************/

static enum status_code nvm_cmd(nvm_cmd_t command, const uint32_t address)
{
	uint32_t temp;

	temp = nvm_module->CTRLB.reg;
	nvm_module->CTRLB.reg = temp | NVMCTRL_CTRLB_CACHEDIS;                              // Turn off cache before issuing flash commands

	nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;                                      // Clear error flags

	if (!nvm_is_ready())                                                                // Check if the module is busy
    {
		return STATUS_BUSY;
	}

	switch (command)                                                                    // Commands requiring address (protected)
    {
		case NVM_CMD_ERASE_EPAGE:
		case NVM_CMD_WRITE_WPAGE:
        case NVM_CMD_ERASE_AUX_ROW:
        case NVM_CMD_PAGE_BUF_CLEAR:
        case NVM_CMD_WRITE_AUX_PAGE:
			nvm_module->ADDR.reg = (uintptr_t)&NVM_MEMORY[address / 4];
			break;

		default:
			return STATUS_ERR_INVALID_ARG;
	}

	nvm_module->CTRLA.reg = command | NVMCTRL_CTRLA_CMDEX_KEY;                          // Set command

	while (!nvm_is_ready()) {}

	nvm_module->CTRLB.reg = temp;                                                       // Restore the setting

	return STATUS_OK;
}


/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief Initializes nv memory driver
*
***********************************************************************************************************************/

void nvm_drv_init( void )
{
    static struct nvm_config config;

    config.sleep_power_mode  = NVM_SLEEP_PWR_WAKE_ON_ACCESS;
    config.manual_page_write = false;
    config.wait_states       = NVMCTRL->CTRLB.bit.RWS;
    config.disable_cache     = true;
    config.cache_readmode    = NVM_CACHE_READMODE_NO_MISS_PENALTY;

    PM->APBBMASK.reg |= PM_APBBMASK_NVMCTRL;                                        // APB set clock

	nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;                                  // Clear error flags

	if (!nvm_is_ready())                                                            // Check if the module is busy
    {
		ErrorShutdown("Nvm not ready during init");
	}

	nvm_module->CTRLB.reg =    NVMCTRL_CTRLB_SLEEPPRM(config.sleep_power_mode)     // Writing configuration to the CTRLB register
                           | ((config.manual_page_write & 0x01) << NVMCTRL_CTRLB_MANW_Pos)
                           |   NVMCTRL_CTRLB_RWS(config.wait_states)
                           | ((config.disable_cache & 0x01) << NVMCTRL_CTRLB_CACHEDIS_Pos)
                           |   NVMCTRL_CTRLB_READMODE(config.cache_readmode);

}

/** ********************************************************************************************************************
* \brief Get EEPROM fuses configuration
*
* \retval fuses configuration
*
***********************************************************************************************************************/

uint32_t nvm_drv_get_eeprom_fuses(void)
{
    uint32_t eepromFuses;

    while (!nvm_is_ready()) {}

    eepromFuses = (uint32_t)(((((uint16_t)NVM_MEMORY[NVMCTRL_USER / 2])&NVMCTRL_FUSES_EEPROM_SIZE_Msk)>> NVMCTRL_FUSES_EEPROM_SIZE_Pos));
    eepromFuses &= 0x00000007;

    return eepromFuses;
}

/** ********************************************************************************************************************
* \brief Sets EEPROM fuses
*
* \param eeprom_fuses
*        fuses configuration
*
***********************************************************************************************************************/

void nvm_drv_set_eeprom_fuses(uint32_t eeprom_fuses)
{
    if (nvm_module->STATUS.reg & NVMCTRL_STATUS_SB)
    {
        ErrorShutdown("Nvn fuses error");
    }

    uint32_t fuseBits00_31 = *((uint32_t *)NVMCTRL_AUX0_ADDRESS);                   // Save actual user row
    uint32_t fuseBits32_63 = *(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1);

    fuseBits00_31 &= ~NVMCTRL_FUSES_EEPROM_SIZE_Msk;
    fuseBits00_31 |= (eeprom_fuses << NVMCTRL_FUSES_EEPROM_SIZE_Pos);               // Set EEPROM configuration

    nvm_cmd(NVM_CMD_ERASE_AUX_ROW, NVMCTRL_AUX0_ADDRESS);                           // Erase auxiliary row
    nvm_cmd(NVM_CMD_PAGE_BUF_CLEAR, NVMCTRL_AUX0_ADDRESS);                          // Clear page buffer

    *((uint32_t *)NVMCTRL_AUX0_ADDRESS)       = fuseBits00_31;                      // Set new user page
    *(((uint32_t *)NVMCTRL_AUX0_ADDRESS) + 1) = fuseBits32_63;

     nvm_cmd(NVM_CMD_WRITE_AUX_PAGE, NVMCTRL_AUX0_ADDRESS);                         // Write aux page

    NVIC_SystemReset();                                                             // Reset to apply new fuses
}

/** ********************************************************************************************************************
* \brief Locks nv memory
*
***********************************************************************************************************************/

void nvm_drv_lock(void)
{

}

/** ********************************************************************************************************************
* \brief Unlocks nv memory
*
***********************************************************************************************************************/

void nvm_drv_unlock(void)
{

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
        if(nvm_cmd(NVM_CMD_ERASE_EPAGE, epage_addr) != STATUS_OK)          // Erase row
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
    uint16_t data;
	uint16_t *ptr;

    addr_check = (NVM_WPAGE_SIZE_BYTE*(wpage_addr/NVM_WPAGE_SIZE_BYTE));        // Addr must be multiple of w-page size

    if(addr_check != wpage_addr)
    {
        err_code = NVM_ERR_ADDR;
    }
	else
    {
        ptr = (uint16_t *)pdata;

        while (!nvm_is_ready())                                                 // Check if the module is busy
        {}

        nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;                          // Clear error flags

        uint32_t nvm_address = wpage_addr/2;

        for (uint32_t i = 0; i < NVM_WPAGE_SIZE_BYTE; i += 2)
        {
            data = *ptr;

            NVM_MEMORY[nvm_address] = data;

            nvm_address++;
            ptr++;
        }

        if(nvm_cmd(NVM_CMD_WRITE_WPAGE, wpage_addr) != STATUS_OK)
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

nvm_err_t nvm_drv_read_byte( uint32_t addr, uint8_t *pdata )
{
    nvm_err_t err_code = NVM_ERR_NO_ERR;

    if((NVM_BASE_ADDR && (addr < NVM_BASE_ADDR)) || (addr > ( NVM_BASE_ADDR + NVM_SIZE_BYTE - 1)))
    {
        err_code = NVM_ERR_ADDR;
    }
    else
    {
        if (!nvm_is_ready())                                                    // Check if the module is busy
        {
            err_code = NVM_ERR_READ;
        }
        else
        {
            nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;                      // Clear error flags

            *pdata = (*(volatile uint8_t *)(addr));
		}
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

    if((NVM_BASE_ADDR && (addr < NVM_BASE_ADDR)) || ((addr + size) > ( NVM_BASE_ADDR + NVM_SIZE_BYTE)))
    {
        err_code = NVM_ERR_ADDR;
    }
    else
    {
        if (!nvm_is_ready())                                                    // Check if the module is busy
        {
            err_code = NVM_ERR_READ;
        }
        else
        {
            nvm_module->STATUS.reg |= NVMCTRL_STATUS_MASK;                      // Clear error flags

			p = (uint8_t *)pdata;

			for(i = 0; i < size; i++)
			{
				*p = (*(volatile uint8_t *)(addr + i));
				p++;
			}
		}
    }

    return err_code;
}

/**********************************************************************************************************************/

/** \} */ // NvmDriverSamD21
/** \} */ // NvmDriver

