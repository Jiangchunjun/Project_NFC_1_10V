// ---------------------------------------------------------------------------------------------------------------------
// Firmware for SSL Full Digital M2 SELV Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     LE D - M
//     Hellabrunner Strasse 1
//     81536 Munich
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: W. Limmer, 2014-08
// $Author: F.Branchetti $
// $Revision: 6164 $
// $Date: 2016-11-21 23:53:04 +0800 (Mon, 21 Nov 2016) $
// $Id: SupplyMonitorStructsInit.c 6164 2016-11-21 15:53:04Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/SupplyMonitorStructsInit.c $
//

/// \addtogroup SupplyMon2Intern
/// \{
/// \file
/// \brief Constants for the module SupplyMonitor
///
/// This file only adds a bit of constant data to be linked to SupplyMonitor to enable this module to be
/// delivered as an object file resp. as a library without source code, but nevertheless to be able to
/// include project specific, compile time constants.

#include "global.h"
//#include "kernel_parameters.h"

//#include "analog_userinterface.h"
#include "supplymonitor_userinterface.h"
#include "supplymonitor_local.h"
#include "SupplyMonitorDefaults.h"
#define SQUARE(a) ((a) * (a))   ///< Square done by preprocessor

/** \hideinitializer
 * Constants for the SupplyMonitor with 230V/ 50/60Hz line voltages and according DC supplies
 */
//sm_state_reset,
//  sm_state_15V,
//  sm_state_48V,
//  sm_state_293V,
//  sm_state_230VDCnarrow,
//  sm_state_230VDCwide
//  sm_state_230VACnarrow,
//  sm_state_230VACwide,
//} supply_state_t;

// OLAF_tmp: Calculation for ulineRms is as follows
// OLAF_tmp: Uline1.last + Uline2.last -> uline_diff -> ulinesquare -> ulineRms
// OLAF_tmp: ulineRms -> via filter -> ulineRms.mean (155ms)
// OLAF_tmp: Problem 1: this too slow for faster overvoltage and therefore exit_voltage (solution pending)
// OLAF_tmp: Possible soltion: Walter agrees to do it faster; perhaps exit.
// OLAF_tmp: Problem 2: there is a build in hysteresis (will be changed)


const supply_monitor_constants_t supplyMonConstants15V = {
  .polchangeMinDiffVolt = VOLTS(5),                                 // 30V resonable phase shifts
  .sagCountMinVolt      = VOLTS(30),        // 30V above this voltage the boost discharges the Xcap....
  .absMinVolt           = VOLTS(0),                                 // 160V SUPPLYMON_POWERFAIL_ms
  .shutdownVoltSqrPosTh = SQUARE( VOLTS(22) >> 16 ) << 6,           // not in use
  .shutdownVoltPeak     = VOLTS(22),                                // used for NEW OVERVOLTAGE
  .powerFailCount       = 1000L * 30            / (5*400),       //    30ms        / 5*400us
  .ac_detect_timeout    = 1000L * 34    / (5*400),       //    34ms
  .maxPowerSagCount     = 1000L * 26       / (5*400),       //    26ms
  .state_timeout        = 1000L * 1000                              / (5*400),
  .exit_voltage         = VOLTS(25),                                // when this modus is left, only meaningfull for 15V 48V mode
  .racDetectCnt         = 0,
  .racDiffVolt          = VOLTS(1000)
};
const supply_monitor_constants_t supplyMonConstants48V = {
//  .polchangeMinDiffVolt = VOLTS(15),                                // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(35),                                // 160V SUPPLYMON_POWERFAIL_ms
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(60) >> 16 ) << 6,           // not in use
//  .shutdownVoltPeak     = VOLTS(60),                                // used for NEW OVERVOLTAGE
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 40                                / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(100),                               // when this modus is left, only meaningfull for 15V 48V mode
//  .racDetectCnt         = 0,
   .racDiffVolt          = VOLTS(1000)
};
const supply_monitor_constants_t supplyMonConstants293V = {
//  .polchangeMinDiffVolt = VOLTS(SUPPLYMON_POL_MINDIFF_V),           // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(380),                               // means 270Vrms @ sinewave
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(330) >> 16 ) << 6,          // not in use
//  .shutdownVoltPeak     = VOLTS(453),                               // used for NEW OVERVOLTAGE
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 3000                              / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(442),                               // 293VAC * 1,41 * 1,07 (7%) = 442Vpk
//  .racDetectCnt         = 0,
  .racDiffVolt          = VOLTS(1000)
};
const supply_monitor_constants_t supplyMonConstants230VDCnarrow = {
//  .polchangeMinDiffVolt = VOLTS(SUPPLYMON_POL_MINDIFF_V_DC_230),           // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(174),                               // means 176V @ DC
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(276) >> 16 ) << 6,          // not in use
//  .shutdownVoltPeak     = VOLTS(360),                               // used for NEW OVERVOLTAGE (253V+2V for Emergency)
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 40                                / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(600),                               // when this modus is left, only meaningfull for 15V 48V mode
//  .racDetectCnt         = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms,
  .racDiffVolt          = VOLTS(80)
};
const supply_monitor_constants_t supplyMonConstants230VDCwide = {
//  .polchangeMinDiffVolt = VOLTS(SUPPLYMON_POL_MINDIFF_V_DC_230),           // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(162),                               // 10V hysteresis @ DC  // hys incresed 4V
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(286) >> 16 ) << 6,          // not in use
//  .shutdownVoltPeak     = VOLTS(374),                               // used for NEW OVERVOLTAGE (253V+2V + Hys for Emergency)
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 40                                / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(600),                               // when this modus is left, only meaningfull for 15V 48V mode
//  .racDetectCnt         = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms,
  .racDiffVolt          = VOLTS(80)
};
const supply_monitor_constants_t supplyMonConstants230VACnarrow = {
//  .polchangeMinDiffVolt = VOLTS(SUPPLYMON_POL_MINDIFF_V),           // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(246),                               // means 176V @ AC
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(266) >> 16 ) << 6,          // not in use
//    .shutdownVoltPeak     = VOLTS(379),                               // used for NEW OVERVOLTAGE (264V+4V for Emergency)
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 40                                / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(600),                               // when this modus is left, only meaningfull for 15V 48V mode
//  .racDetectCnt         = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms,
  .racDiffVolt          = VOLTS(80)
};
const supply_monitor_constants_t supplyMonConstants230VACwide = {
//  .polchangeMinDiffVolt = VOLTS(SUPPLYMON_POL_MINDIFF_V),           // 30V resonable phase shifts
//  .sagCountMinVolt      = VOLTS(SUPPLYMON_XCAP_DISCHARGE_V),        // 30V above this voltage the boost discharges the Xcap....
//  .absMinVolt           = VOLTS(224),                               // means 166V @ AC 10 V hyteresis
//  .shutdownVoltSqrPosTh = SQUARE( VOLTS(276) >> 16 ) << 6,          // not in use
//    .shutdownVoltPeak     = VOLTS(390),                               // used for NEW OVERVOLTAGE (276V+2V + hys for Emergency)
//  .powerFailCount       = 1000L * SUPPLYMON_POWERFAIL_ms            / SUPPLYMON_EVAL_us,       //    30ms        / 5*400us
//  .ac_detect_timeout    = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms
//  .maxPowerSagCount     = 1000L * SUPPLYMON_SLEEP_POWERSAG_ms       / SUPPLYMON_EVAL_us,       //    26ms
//  .state_timeout        = 1000L * 40                                / SUPPLYMON_EVAL_us,
//  .exit_voltage         = VOLTS(600),                               // when this modus is left, only meaningfull for 15V 48V mode
//  .racDetectCnt         = 1000L * SUPPLYMON_DC_DETECT_TIMEOUT_ms    / SUPPLYMON_EVAL_us,       //    34ms,
  .racDiffVolt          = VOLTS(80)
};

/// \todo Clarify, why moving this struct into an extra file takes 32 bytes of RAM area? A copy and paste back to
/// SupplyMonitor.c (*exactly* the same code, with identical header files) saves this it again !!!

/// \}  //defgroup SupplyMon2
