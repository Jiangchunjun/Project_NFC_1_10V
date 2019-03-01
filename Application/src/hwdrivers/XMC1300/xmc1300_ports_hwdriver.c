// ---------------------------------------------------------------------------------------------------------------------
// OS3 module for OTi NONSELV Generation 2 ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     LE D - M
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: C. Schwarzfischer, 2014-10
// $Author: c.schwarzfischer $
// $Revision: 6684 $
// $Date: 2017-01-10 15:45:30 +0800 (Tue, 10 Jan 2017) $
// $Id: xmc1300_ports_hwdriver.c 6684 2017-01-10 07:45:30Z c.schwarzfischer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_ports_hwdriver.c $

/*******************************************************************************************************************//**
* @file
* @brief XMC1300 hardware driver for OS3 module
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup OS3 OS3 - 'O'perating 'S'ystem for 'OS'RAM 'O'ptotronic 'S'ings Module
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

#include "global.h"
#include "gpio_xmc1300_tssop38.h"
#include "xmc1300_ports_hwdriver.h"

//------------------------------------------------------------------------------
void hw_P1_0_set(void)
{
    P1_0_set();
}

//------------------------------------------------------------------------------
void hw_P1_0_reset(void)
{
    P1_0_reset();
}

//------------------------------------------------------------------------------
void hw_P0_13_set(void)
{
    P0_13_set();
}

//------------------------------------------------------------------------------
void hw_P0_13_reset(void)
{
    P0_13_reset();
}

//------------------------------------------------------------------------------
void hw_P0_8_set(void)
{
    P0_8_set();
}

//------------------------------------------------------------------------------
void hw_P0_8_reset(void)
{
    P0_8_reset();
}

#endif


/** @}  End of group OS3 */
