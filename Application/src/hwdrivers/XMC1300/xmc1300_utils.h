// ---------------------------------------------------------------------------------------------------------------------
// Firmware for OTi NONSELV Generation 2 Ballasts
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
// Initial version: M. Gerber, 2014-04
// $Author: F.Branchetti $
// $Revision: 5700 $
// $Date: 2016-10-13 15:39:00 +0800 (Thu, 13 Oct 2016) $
// $Id: xmc1300_utils.h 5700 2016-10-13 07:39:00Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_utils.h $

/*******************************************************************************************************************//**
* @file
* @brief Header file for general setup of XMC1300 microcontroller and helper functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup HW HW - Hardware drivers and hardware dependent parameters
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

void ControllerInit(void);

#endif

/** @}  End of group HW */
