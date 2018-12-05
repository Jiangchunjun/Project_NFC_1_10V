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
//#include "parameter.h"
#define  configXMC1302

//======================================================================================================================
//     P r o d u c t   I d e n t i f i c a t i o n
//======================================================================================================================

#define DEVICE_FW_VERSION_MAJOR                                               1
#define DEVICE_FW_VERSION_MINOR                                               3

//======================================================================================================================
//     P r o d u c t   P r o p e r t i e s
//======================================================================================================================

#define DEVICE_CHANNELS                                                       1
// #define DEVICE_CHANNELS_MAX

#define DEVICE_TOUCHDIM

#define DEVICE_SYSCLK_MHz                                                   (32)
#define DEVICE_PERIPHCLK_MHz                               (2*DEVICE_SYSCLK_MHz)

#define POWERon_DELAY_TIME_ms                                               (20)                                        // time after mains on until the µC starts

// Fuses
// in case of XMC1xxx


//======================================================================================================================
//     I n c u d e - F i l e s ,   t o   b e   c o n f i g u r e d
//======================================================================================================================

#ifdef MODULE_DALI
#include "ConfigDali.h"                                                                                                 // included only if "#define MODULE_DALI" in the c-File
#endif

#ifdef MODULE_MPC
#include "ConfigMpc.h"                                                                                                  // included only if "#define MODULE_MPC" in the c-File
#endif

#ifdef MODULE_NVM
#include "ConfigNvm.h"                                                                                                  // included only if "#define MODULE_NVM" in the c-File
#endif

#ifdef MODULE_I2C
#include "ConfigI2c.h"
#endif

#ifdef MODULE_M24LRxx
#include "ConfigM24LRxx.h"
#endif

#ifdef MODULE_NFC
#include "ConfigNfc.h"                                                                                                  // included only if "#define MODULE_NVM" in the c-File
#endif

#ifdef MODULE_NFC_DALI
#include "ConfigNfcDali.h"                                                                                                  // included only if "#define MODULE_NVM" in the c-File
#endif

#endif // #ifndef __CONFIG_H__
