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
// $Id: ConfigI2cTemplate.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/I2cExt/api/ConfigI2cTemplate.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __CONFIG_I2C_H
#define __CONFIG_I2C_H

/** \addtogroup I2cConfig
 * \{
 *
 * \file
 * \brief A template configuration file for the I2C module. Please copy, rename and adjust in the projects.
 *
 */

 #error "Do not include this file. Copy it in your project."

/***************************************************************************************************************************
 * SAMD21 I2C Hw config - Begin
 ****************************************************************************************************************************/

#define I2C_SERCOM_NUMBER           2   /**< Selects the SERCOM peripheral's number to use. */
#define I2C_CLOCK_GENERATOR         GCLK_GENERATOR_0    /**< Clock generator to connect to the selected SERCOM peripheral. */
#define I2C_GCLK_SPEED_HZ           (48E6)  /**< Speed in Hz of the GCLK selected for the SERCOM used, taking in account all dividers, etc... */
//#define I2C_TRISE_S                 (200E-9)    /**< Trise in Hz of the signal, to be measured. If not defined, a default value will be used. To be used for fine tuning. */

#define I2C_SDA_USE_ALTERNATE       1   /**< If I2C_SDA_USE_ALTERNATE is defined and >0 then the alternate sercom pin will be used for PAD0. */
#define I2C_SCL_USE_ALTERNATE       1   /**< If I2C_SCL_USE_ALTERNATE is defined and >0 then the alternate sercom pin will be used for PAD1. */

#define I2C_BAUD_RATE_HZ            (400E3) /**< Desidered baud rate of the I2C bus. */

/***************************************************************************************************************************
 * SAMD21 I2C Hw config - End
 ****************************************************************************************************************************/


/***************************************************************************************************************************
 * XMC1300 I2C Hw config - Begin
 ****************************************************************************************************************************
/** \brief Pin pair in use for I2C data (SDA) and clock (SCL) signals
*
* De-comment the MCU pin pair in use
*/

//#define I2C__SCL_PIN_2_0__SDA_PIN_2_1
#define I2C__SCL_PIN_0_8__SDA_PIN_0_6

/***************************************************************************************************************************
 * XMC1300 I2C Hw config - End
 ****************************************************************************************************************************/

/***************************************************************************************************************************
 * Common I2C Hw config - Begin
 ****************************************************************************************************************************

/** \brief I2C timer period
*
* Specify the I2C timer period
*
 \warning timer period shall be smaller than 10ms
*/

#define I2C_TIMER_PERIOD_US                     (1200)                  // [us]

/***************************************************************************************************************************
 * Common I2C Hw config - End
 ****************************************************************************************************************************

/** \} */
#endif
