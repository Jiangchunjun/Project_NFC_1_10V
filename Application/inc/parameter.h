#ifndef __PARAMETER_H__
#define __PARAMETER_H__

// ---------------------------------------------------------------------------------------------------------------------
// PARAMETER FILE for OTi NONSELV Generation 2 ballasts
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
// $Revision: 13049 $
// $Date: 2018-04-25 16:29:28 +0800 (Wed, 25 Apr 2018) $
// $Id: parameter.h 13049 2018-04-25 08:29:28Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/Config/parameter.h $

/*******************************************************************************
* @file
* @brief Master file to select the device type.

*******************************************************************************/

/*******************************************************************************
* @defgroup PARAMETER
* @{
*******************************************************************************/

#define DEVICE_FW_VERSION_MAJOR                                             FW_HIGH
#define DEVICE_FW_VERSION_MINOR                                             FW_LOW

#define LEDSET_R_CORRECTION                                                 1

#define CALIBRATION_PEAK_CAPTURE_DELAY_MS                                   20
#define CALIBRATION_MAXIMUM_SAMPLE_TIME_MS                                  450

//******************************************************************************
//*************************** OT PSE *******************************************
//******************************************************************************


#ifdef OT_NFC_IP67_200W
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): OTi PSE 50W is selected! ::::::")
#endif

#include "power_config_200w.h"
#endif

#ifdef OT_NFC_IP67_100W
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): OTi PSE 50W is selected! ::::::")
#endif



#include "power_config_100w.h"
#endif


//******************************************************************************
//************************* ULTRAFLAT ******************************************
//******************************************************************************

// ----------------- OTi DALI 35 UF Analog Dim D30016207 -----------------------
#ifdef D30016207
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30016207 is selected! ::::::")
#endif

#include "devices/parameter_D30016207.h"
#endif


// ----------------- OTi DALI 75 UF Analog Dim D30010259 -----------------------
#ifdef D30010259
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30010259 is selected! ::::::")
#endif

#include "devices/parameter_D30010259.h"
#endif


//******************************************************************************
//*************************   21 mm   ******************************************
//******************************************************************************

// ----------------- OTI DALI 35 400mA D LT2 L GIII D30018481 ------------------
#ifdef D30018481
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30018481 is selected! ::::::")
#endif

#include "devices/parameter_D30018481.h"
#endif


// ----------------- OTI DALI 60 550mA D LT2 L GIII D30017937 ------------------
#ifdef D30017937
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30017937 is selected! ::::::")
#endif

#include "devices/parameter_D30017937.h"
#endif


// ----------------- OTI DALI 90 700mA D LT2 L GIII D30018482 ------------------
#ifdef D30018482
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30018482 is selected! ::::::")
#endif

#include "devices/parameter_D30018482.h"
#endif


// ----------------- OTI DALI 90 1000mA D LT2 L GIII D30018483 ------------------
#ifdef D30018483
// validate multi device defines
#ifdef device_type_is_defined
#error "(parameter.h): ERROR - more than 1 device type is defined !!"
#else
#define device_type_is_defined
#pragma message(":::::: (parameter.h): device D30018483 is selected! ::::::")
#endif

#include "devices/parameter_D30018483.h"
#endif


// ----------------- No device defined ------------------------------
#ifndef device_type_is_defined
#error "(parameter.h): ERROR - No device type is defined !!"
#endif

/** @}  PARAMETER */

//static_assert( sizeof(GLOBAL_BASIC_CODE_STRING)!=0, "Basic code string is too short" );

#endif // #ifndef __PARAMETER_H__
