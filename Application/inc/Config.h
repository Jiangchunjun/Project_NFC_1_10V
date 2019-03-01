#ifndef __CONFIG_H__
#define __CONFIG_H__

/******************** (C) COPYRIGHT 2012 OSRAM AG *********************************************************************************
* File Name         : Config.h
* Description       : Definitions of device-specific properties of a "Dali Control Gear" Device (defining the specific hardware+features of the device)
*                   : device <write device name here>.
* Author(s)         : Template: Jutta Avril (JA), PL-LMS TRT
* Created           : 12.12.2012 (JA)
* Modified by       : 16.1.2015  (JE)
**********************************************************************************************************************************/

#include <stdint.h>
#include <parameter.h>
//#define  configXMC1302

#define OSRAM_MANUFACTURER_CODE             1                                   // OSRAM NFC Manufacturer code (it was 12)

#define THIS_DEVICE_EAN                                     GLOBAL_DEVICE_EAN

//======================================================================================================================
//     P r o d u c t   P r o p e r t i e s
//======================================================================================================================

#define DEVICE_CHANNELS                                                       1

#define DEVICE_TOUCHDIM

#define DEVICE_SYSCLK_MHz                                                   (32)
#define DEVICE_PERIPHCLK_MHz                               (2*DEVICE_SYSCLK_MHz)


//======================================================================================================================
//     I n c u d e - F i l e s ,   t o   b e   c o n f i g u r e d
//======================================================================================================================

#include "ConfigDali.h"                                                                                                 // included only if "#define MODULE_DALI" in the c-File
#include "ConfigMpc.h"                                                                                                  // included only if "#define MODULE_MPC" in the c-File
#include "ConfigNvm.h"                                                                                                  // included only if "#define MODULE_NVM" in the c-File
#include "ConfigNfc.h"                                                                                                  // included only if "#define MODULE_NVM" in the c-File


#endif // #ifndef __CONFIG_H__
