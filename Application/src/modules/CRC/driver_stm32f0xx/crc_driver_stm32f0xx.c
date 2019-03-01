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
// $Revision: 368 $
// $Date: 2015-04-23 13:52:50 +0800 (Thu, 23 Apr 2015) $
// $Id: crc_driver_stm32f0xx.c 368 2015-04-23 05:52:50Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Crc/tags/v1.3/driver_stm32f0xx/crc_driver_stm32f0xx.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \addtogroup CrcDriver
 * \{
 * \defgroup CrcDriverSTM32F0xx CRC driver - STM32F0xx
 * \brief Software driver for CRC peripheral - STM32F0xx mcu
 * \{
 *
 * \file
 * \brief Central functions for CRC peripheral.
 */

#include "Global.h"

#include "crc_driver.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_crc.h"			// ST standard peripheral library

/** ********************************************************************************************************************
* \brief CRC driver init - stm32
*
* \warning Reset CRC to 0xFFFFFFFF.
***********************************************************************************************************************/

void crc_drv_init( void )
{
   RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

   CRC_DeInit();
}

/** ********************************************************************************************************************
* \brief Resets CRC to 0xFFFFFFFF
*
***********************************************************************************************************************/

void crc_drv_reset(void)
{
    CRC_ResetDR();
}

/** ********************************************************************************************************************
* \brief Computes the 32-bits CRC of the given word
*
* \param data
*        word to be added
*
* \retval CRC
***********************************************************************************************************************/

uint32_t crc_drv_calc(uint32_t data)
{
    CRC->DR = data;

    return CRC->DR;
}

/** ********************************************************************************************************************
* @brief Gets CRC
*
* @return CRC
***********************************************************************************************************************/

uint32_t crc_drv_get(void)
{
    return (CRC->DR);
}

/**********************************************************************************************************************/

/** \} */ // CrcDriverSTM32F0xx
/** \} */ // CrcDriver

