#ifndef __SUPPLYMONITOR_LOCAL_H__
#define __SUPPLYMONITOR_LOCAL_H__

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
// $Id: supplymonitor_local.h 6164 2016-11-21 15:53:04Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/supplymonitor_local.h $

/*******************************************************************************************************************//**
* @file
* @brief Local Header file of SUPPLYMONITOR module
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup SUPPLYMONITOR SUPPLYMONITOR - SUPPLYMONITOR
* @{
***********************************************************************************************************************/
#include "global.h"
#include "supplymonitor_parameters.h"


//-----------------------------------------------------------------------------
// local defines
//-----------------------------------------------------------------------------
#define PERIOD_40_50HZ_BORDER_us          (int) (1e6 * (3 / 40.0 + 1 / 50.0) / 4 / 2) // [us] Closer to 40 than to 50Hz
#define PERIOD_50_60HZ_BORDER_us          (int) (1e6 * (1 / 50.0 + 1 / 60.0) / 2 / 2) // [us] average of 10ms and 8.33ms
#define PERIOD_60_70HZ_BORDER_us          (int) (1e6 * (1 / 60.0 + 3 / 70.0) / 4 / 2) // [us] Closer to 70 than to 60Hz
#define PERIOD_AC_NOISE_BORDER_us         (int) (1e6 / 99   / 2) // Don't accept commutations below

//-----------------------------------------------------------------------------
// local macros
//-----------------------------------------------------------------------------
#define SUPPLY_DEBUG_UART(a,b,c)         // DebugPWM(a,b)  // c = comment

//-----------------------------------------------------------------------------
// enums
//-----------------------------------------------------------------------------
typedef enum  // used as state within SupplyMonitorFSM as state
{
  sm_state_reset,
  sm_state_15V,
  sm_state_48V,
  sm_state_293V,
  sm_state_230VDCnarrow,
  sm_state_230VDCwide,
  sm_state_230VACnarrow,
  sm_state_230VACwide,
  //sm_state_overvoltage
} supply_state_t;

//-----------------------------------------------------------------------------
// structures
//-----------------------------------------------------------------------------
typedef struct {
  int32_t   polchangeMinDiffVolt;
  int32_t   sagCountMinVolt;
  int32_t   absMinVolt;
  int32_t   shutdownVoltSqrPosTh;           // not in use
  int32_t   shutdownVoltPeak;
  int32_t   maxPowerSagCount;
  int32_t   powerFailCount;
  int32_t   ac_detect_timeout;
  int32_t   state_timeout;                   // NEW OLAF used to control state machine within evaluate
  int32_t   exit_voltage;                    // NEW OLAF used to control state machine within evaluate
  uint32_t  racDetectCnt;
  uint32_t  racDiffVolt;
} supply_monitor_constants_t;



//-----------------------------------------------------------------------------
/// \ingroup SupplyMon2Intern
typedef struct {
  uint32_t  lastUline;                    ///< For sleep modes: Store Uline of last run of SupplyMonitorEval()
  uint32_t  commLastTimestamp;            ///< To detect new commutation events and to measure the period
  uint32_t  umaxTrack;                    ///< Track Umax during a (unfinshed) half cycle
  int32_t   phaseCnt;                     ///< Internal high resoultion line phase counter
  uint8_t   polarityOld;                  ///< Polarity at the end of last SupplyMonitorTimestamp()
  uint8_t   commTimeoutCnt;               ///< Counter for DC detection
  uint8_t   powerFailCnt;                 ///< Counter for missing SUPPLYMON_ABS_MIN_V -> power fail
  uint8_t   powerSagCnt;                  ///< Counter for continuos voltage decrements during sleep mode
  Filter_t  ulineRms;                     ///< RMS Value of Uline
  uint32_t  diff_abs_max;
  uint32_t  diff_abs_min;
  uint32_t  rac_detected_cnt;
} supply_monitor_intern_t;





/** @}  End of group SUPPLYMONITOR */

#endif // #ifndef __SUPPLYMONITOR_LOCAL_H__


