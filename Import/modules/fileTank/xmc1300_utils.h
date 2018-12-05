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
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: xmc1300_utils.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/fileTank/xmc1300_utils.h $

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