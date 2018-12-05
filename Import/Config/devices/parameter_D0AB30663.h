#ifndef __PARAMETER_D0AB30663_H__
#define __PARAMETER_D0AB30663_H__

// ---------------------------------------------------------------------------------------------------------------------
// PARAMETER FILE for OTi DALI 75 Ultraflat
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
// $Id: parameter_D0AB30663.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/Config/devices/parameter_D0AB30663.h $

/*******************************************************************************
* @file
* @brief Parameter device file for OTi DALI 75 Ultraflat.

*******************************************************************************/

/*******************************************************************************
* @defgroup PARAMETER_D0AB30663
* @{
*******************************************************************************/

// EAN
#define GLOBAL_DEVICE_EAN                                   4052899957046   // -> 0x3AFA3A98936
#define GLOBAL_EDOS_NO                                      0xAB30663


/** Module BOOSTPFC */
#define GLOBAL_BOOSTPFC_CHOKE                               CHOKE_D30000602
#define GLOBAL_BOOSTPFC_INDUCTIVITY_uH                      1500


/** Module Buck */
#define GLOBAL_BUCK_CHOKE                                   CHOKE_D30000564
// 15% of ILED_MAX
#define GLOBAL_BUCK_PWM_LIMIT_uA                            150000UL
#define GLOBAL_BUCK_U_RAIL_COMPENSATE_DELTA_LIMIT_uV        5000000UL


/** Module CurrentSet */
// Label values
#define I_LED_MIN                                           150UL
#define I_LED_MAX                                           700UL

//ohm
#define LEDSET_SHUNT                                        3300
//[uV]  5V
#define LEDSET_VOLTAGE                                      5e6
//50000 //[uV]
#define LEDSETVOLTAGE_OFFSET                                0
//uA
#define LEDSET_MAXCURRENT                                   I_LED_MAX*1000
//uA
#define LEDSET_MINCURRENT                                   I_LED_MIN*500

//Border with hysteresis to recognize open circuit
//µV - Rset=66.7k = 75mA
#define LEDSET_UMESSMIN_U                                   6072500
//µV - Rset=95.2k = 52.5mA
#define LEDSET_UMESSMIN_O                                   5998250

//Border with hysteresis to recognize short circuit
//µV - Rset=6.0k = 840mA
#define LEDSET_UMESSMAX_U                                   8597000
//µV - Rset=5.5k = 910mA
#define LEDSET_UMESSMAX_O                                   8828000


/** Module Kernel */
///< [W] Max output power at the LED string
#define GLOBAL_POWER_OUTPUT_MAX_W                           75
// (3.45V/4096) * 1e6 * (1/12)* (1/0.20R) = 351
#define GLOBAL_IBUCK_ADC_FACTOR                             351


/** @}  PARAMETER_D0AB30663 */

#endif // #ifndef __PARAMETER_D0AB30663_H__
