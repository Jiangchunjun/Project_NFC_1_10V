#ifndef __XMC1300_PORTS_HWDRIVER_H__
#define __XMC1300_PORTS_HWDRIVER_H__


// ---------------------------------------------------------------------------------------------------------------------
// Xmc1300_ports_hwdriver for OTi NONSELV Generation 2 ballasts
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
// Initial version: S. Mayer, 2014-11
// $Author: c.schwarzfischer $
// $Revision: 6684 $
// $Date: 2017-01-10 15:45:30 +0800 (Tue, 10 Jan 2017) $
// $Id: xmc1300_ports_hwdriver.h 6684 2017-01-10 07:45:30Z c.schwarzfischer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_ports_hwdriver.h $

/*******************************************************************************************************************//**
* @file
* @brief Header file for user interface of (hardware independent) Xmc1300_ports_hwdriver module.
* Basic hardware independent functions for Xmc1300_ports_hwdriver.
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup XMC1300_PORTS_HWDRIVER XMC1300_PORTS_HWDRIVER - XMC1300_PORTS_HWDRIVER
* @{
***********************************************************************************************************************/

#include "global.h"

void hw_P1_0_set(void);
void hw_P1_0_reset(void);

void hw_P0_13_set(void);
void hw_P0_13_reset(void);

void hw_P0_8_set(void);
void hw_P0_8_reset(void);

/** @}  End of group XMC1300_PORTS_HWDRIVER */

#endif // #ifndef __XMC1300_PORTS_HWDRIVER_H__
