// ---------------------------------------------------------------------------------------------------------------------
// XMC1300 hardware driver for currentset subsystem for OTi NONSELV Generation 2 ballasts
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
// Initial version: O. Busse, M. Gerber, 2014-05
// $Author: F.Branchetti $
// $Revision: 5700 $
// $Date: 2016-10-13 15:39:00 +0800 (Thu, 13 Oct 2016) $
// $Id: xmc1300_currentset_hw_driver.c 5700 2016-10-13 07:39:00Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_currentset_hw_driver.c $

/*******************************************************************************************************************//**
* @file
* @brief XMC1300 hardware driver for currentset subsystem
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup XMC1300_HWDRIVER_CURRENTSET HWDRIVER - CURRENTSET - XMC1300 Hardware Driver for currentset subsystem
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

#include "currentset_hal.h"
#include "xmc1300.h"

#define ROM_CalcTemperature \
        (*((uint32_t (**)(void))0x0000010cU))


uint32_t Currentset_HwReadTempSensorKelvinValue (void)
{
    uint32_t read_value;
    read_value = ROM_CalcTemperature();
    return read_value;
}


void Currentset_HwActivateTSE (void)
{
    // Activate TSE Temperature Sensor
    SET_BIT(SCU_ANALOG->ANATSECTRL ,SCU_ANALOG_ANATSECTRL_TSE_EN_Pos);
}


void Currentset_HwDeactivateTSE (void)
{
    // Deactivate TSE Temperature Sensor
    CLR_BIT(SCU_ANALOG->ANATSECTRL ,SCU_ANALOG_ANATSECTRL_TSE_EN_Pos);
}


#endif
/** @}  End of group XMC1300_HWDRIVER_CURRENTSET */
