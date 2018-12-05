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
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: parameter.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/Config/parameter.h $

/*******************************************************************************
* @file
* @brief Master file to select the device type.

*******************************************************************************/

/*******************************************************************************
* @defgroup PARAMETER
* @{
*******************************************************************************/
/** Should be included in the ...._userinterface.h */

//******************************************************************************
//************************* 21mm ***********************************************
//******************************************************************************

/** Select device type */
//#define D0AB33078 // OTi DALI 35 21mm
//#define D30003606 // OTi DALI 60 21mm
//#define D0AB33079 // OTi DALI 90 21mm

//#define D30007860 // OTi DALI 35 UF
#define D0AB30663 // OTi DALI 75 UF


// ----------------- OTi DALI 35 21mm D0AB33078 --------------------------------
#ifdef D0AB33078
    // validate multi device defines
    #ifdef device_type_is_defined
        #error "(parameter.h): ERROR - more than 1 device type is defined !!"
    #else
        #define device_type_is_defined
        #pragma message("(parameter.h):device D0AB33078 is selected !")
    #endif

    #include "devices/parameter_D0AB33078.h"

#endif


// ----------------- OTi DALI 60 21mm D30003606 --------------------------------
#ifdef D30003606
    // validate multi device defines
    #ifdef device_type_is_defined
        #error "(parameter.h): ERROR - more than 1 device type is defined !!"
    #else
        #define device_type_is_defined
        #pragma message("(parameter.h):device D30003606 is selected !")
    #endif

    #include "devices/parameter_D30003606.h"
#endif


// ----------------- OTi DALI 90 21mm D0AB33079 --------------------------------
#ifdef D0AB33079
    // validate multi device defines
    #ifdef device_type_is_defined
        #error "(parameter.h): ERROR - more than 1 device type is defined !!"
    #else
        #define device_type_is_defined
        #pragma message("(parameter.h):device D0AB33079 is selected !")
    #endif

    #include "devices/parameter_D0AB33079.h"
#endif


//******************************************************************************
//************************* ULTRAFLAT*******************************************
//******************************************************************************

// ----------------- OTi DALI 35 UF D30007860 --------------------------------
#ifdef D30007860
    // validate multi device defines
    #ifdef device_type_is_defined
        #error "(parameter.h): ERROR - more than 1 device type is defined !!"
    #else
        #define device_type_is_defined
        #pragma message("(parameter.h):device D30007860 is selected !")
    #endif

    #include "devices/parameter_D30007860.h"
#endif


// ----------------- OTi DALI 75 UF D0AB30663 --------------------------------
#ifdef D0AB30663
    // validate multi device defines
    #ifdef device_type_is_defined
        #error "(parameter.h): ERROR - more than 1 device type is defined !!"
    #else
        #define device_type_is_defined
        #pragma message("(parameter.h):device D0AB30663 is selected !")
    #endif

    #include "devices/parameter_D0AB30663.h"
#endif

// ----------------- No device defined ------------------------------
#ifndef device_type_is_defined
    #error "(parameter.h): ERROR - No device type is defined !!"
#endif

/** @}  PARAMETER */

#endif // #ifndef __PARAMETER_H__
