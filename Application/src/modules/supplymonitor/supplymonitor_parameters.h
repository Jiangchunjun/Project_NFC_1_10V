#ifndef __SUPPLYMONITOR_PARAMETERS_H__
#define __SUPPLYMONITOR_PARAMETERS_H__

// ---------------------------------------------------------------------------------------------------------------------
// Supplymonitor module for OTi NONSELV Generation 2 ballasts
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
// $Author: F.Branchetti $
// $Revision: 6164 $
// $Date: 2016-11-21 23:53:04 +0800 (Mon, 21 Nov 2016) $
// $Id: supplymonitor_parameters.h 6164 2016-11-21 15:53:04Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/supplymonitor_parameters.h $

/*******************************************************************************************************************//**
* @file
* @brief All defines, which are part of the Supplymonitor module user interface.

***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup SUPPLYMONITOR SUPPLYMONITOR - SUPPLYMONITOR
* @{
***********************************************************************************************************************/

// --> (x >> 10) * 41 --> 0,040 --> 4%
#define SUPPLYMONITOR_INPUT_VOLTAGE_DEC_SCALE                   10UL
#define SUPPLYMONITOR_INPUT_VOLTAGE_DEC_FACTOR                  21UL

#define SUPPLYMONITOR_PIMP_ULINEPK_DURATION                     (100000/395)

/** @}  End of group SUPPLYMONITOR */

#endif // #ifndef __SUPPLYMONITOR_PARAMETERS_H__
