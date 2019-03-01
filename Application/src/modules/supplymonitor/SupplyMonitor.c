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
// Initial version: W. Limmer, 2013-07
// $Author: Chunjun.Jiang $
// $Revision: 14014 $
// $Date: 2018-06-29 10:48:23 +0800 (Fri, 29 Jun 2018) $
// $Id: SupplyMonitor.c 14014 2018-06-29 02:48:23Z Chunjun.Jiang $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/SupplyMonitor.c $
//

/**
 * \defgroup SupplyMon2 Supply Monitor 2
 * \{
 * \brief Supervise supply voltage (input power) and provide its key characteristics (2 voltage dividers implementation)
 *
 * \note - This module is the version working with 2 voltage divider strings, one at each input terminals (before
 * the bridge rectification).
 *
 * \note - It's the full-featured implementation wich was developed also in respect to the
 * possibility to send data to the ballast via modulated line voltage.
 *
 * Process all data about input power supply and provide important characteristics (AC/DC, power fail, period, ...).
 * See \ref supply_monitor_state_t and \ref supply_flags_t for details.
 *
 * Relies on a working analog input system providing filtered measurements of BOTH line voltage terminals and of the
 * rail voltage.
 *
 * Sooner or later provide additionally:
 *  - Line synchronization for the line phase angle
 *  - A provision and interface for phase cut measurement
 *  - Data extraction for communication to the ballast (QTi calibration procedure)
 *
 * \warning As said in the log messages, the current version of this module is a starting point for the integration
 * in the XMC project. It even does NOT compile as it is.
 *
 * \defgroup SupplyMon2Api Supply Monitor API
 * \brief User interface for Supply Monitor
 *
 * \defgroup SupplyMon2Parameter  Supply Monitor Configuration
 * \brief Parameters to configure the Supply Monitor
 *
 * \defgroup SupplyMon2Intern Supply Monitor Internals
 * \brief Supply Monitor implementation details
 *
 * \}
 *
 * \addtogroup SupplyMon2Intern
 * \{
 * \file
 * \brief Watch and evaluate input power supply.
 */

/**
\brief
   access from extern to the infos:
Supply.flags.AC
Supply.flags.AC50Hz
Supply.flags.AC60Hz
Supply.flags.Powerfail
Supply.uline_peak
Supply.phase
Supply.period.mean
etc.
   */

#include <string.h>                 // for memset()
#include <stdbool.h>

#include "global.h"
//#include "kernel_parameters.h"


#include "assert.h"
//#include "analog_userinterface.h"
#include "filter.h"


#include "gpio_xmc1300_tssop38.h"

#include "supplymonitor_parameters.h"
#include "supplymonitor_userinterface.h"
#include "supplymonitor_local.h"
#include "xmc1300_ports_hwdriver.h"

//----------------------------------------------------------------------------------------------------------------------
// Global variables
//----------------------------------------------------------------------------------------------------------------------
supply_monitor_state_t Supply;    ///< Supply Monitor API exports
uint32_t TimebaseSinceStart_us=0;
static uint32_t check_cmph_prev;
static uint32_t check_cmph_active;
static uint32_t check_loss_of_vin;
static uint32_t input_voltage_prev;
static uint32_t monitor_duration_cnt;
static uint32_t loss_of_vin_detected_cnt;
uint32_t check_cmph_event_cnt;
uint32_t check_cmph_total_fail_time_cnt;
uint32_t check_cmph_fail_time_cnt;
uint32_t check_cmph_max_fail_time_cnt;
bool check_cmph_failure;
static uint32_t pimp_ulinepk_cnt;


//----------------------------------------------------------------------------------------------------------------------
// extern data structs
//----------------------------------------------------------------------------------------------------------------------
//extern const supply_monitor_constants_t supplyMonConstantsStandard;
//extern const supply_monitor_constants_t supplyMonConstantsLowVoltPrg;

extern const supply_monitor_constants_t supplyMonConstants15V;
extern const supply_monitor_constants_t supplyMonConstants48V;
extern const supply_monitor_constants_t supplyMonConstants293V;
extern const supply_monitor_constants_t supplyMonConstants230VDCnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VDCwide;
extern const supply_monitor_constants_t supplyMonConstants230VACnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VACwide;


//----------------------------------------------------------------------------------------------------------------------
// Static data types, variables and functions
//

//STATIC_ASSERT((int)(SUPPLYMON_EVAL_us < (1e6 / SUPPLYMON_AC_MAXFREQ_Hz * 0.4)),
//               "Nyquist condition for SUPPLYMON_AC_MAXFREQ_Hz isn't fulfilled");

//#define SUPPLY_DEBUG_UART(a,b,c)          putchar(a);

//----------------------------------------------------------------------------------------------------------------------
// local variables
//----------------------------------------------------------------------------------------------------------------------

static supply_monitor_intern_t supplyMonitorPriv;

static const supply_monitor_constants_t * suMoCoP;

static supply_state_t sm_state;
static uint32_t fsm_cycle_cnt;
static uint32_t evaluate_timer;
static uint32_t no_commutation_detected_cnt;
static uint32_t supplymonitor_deviation_cnt;
static uint32_t supplymonitor_Ulinepk_delta;

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
/*! Evaluate the supply voltage characteristics.
 * \ingroup SupplyMon2Api
 * Olaf 2015.01.12
 * Two routines are called by timebase in roughly 2 ms intervals to deeper evaluate the parameters provided by
 * SupplyMonitorTimestamp() to provide the global flags in supply_flags_t for the rest of the system.
 *   SupMonEvaFailACDC() supply_powertype_t SM_POWERTYPE;
 *   SupMonEvaOvervoltage() supply_overvoltage_flag_t SM_OVERVOLTAGE;
 *
 * To reach PF, AC or DC from NA it always will take some time. If no decision is possible the old value is kept.
 * If you do set new suMoCoP values please be aware that one has to reset the analysis.
 * State changes in the state machine can only be done if one detects PF, AC, DC but not NA!!!!!
 * For this one hat to reset SM_POWERTYP to NA!!!!!
 * Some hints on how recognition works:
 * PF: Inputvoltage has to be below a peak level for roughly 30ms.
 * At low AC voltage: once a halfwave input voltage exceeded the low level threshold
 * a) powerFailCnt=0;
 * b) commTimeoutCnt++; for DC detection
 * c) check since last threshold ... if time is long enough: AC is set and commTimeoutCnt=0;
 * d) if commTimeoutCnt>~30ms DC
 * At high AC voltage: very often input voltage exceeded the low level threshold
 * a) powerFailCnt=0;
 * b) commTimeoutCnt++; for DC detection (will happen much often, but without effect, cause gets reset at zerocrossing)
 * c) check since last threshold ... if time is long enough: AC is set and commTimeoutCnt=0;
 * d) if commTimeoutCnt>~30ms DC
 * Note: AC is set after first valid communtation, but DC or PF later!
 * Note: If RAC, DC will be detected, cause no commutation will be detectetd (uline1, uline2)
 * Note: RAC could be detected
 *
 *
 *
 * \tparam None
 * \retval None
 */


static void SupMonEvaFailACDC (void){
  //supply_flags_t  tmp_flags = {0};              // temporary flags to be copied in the global flags later on (if needed)
  uint32_t commutation_per;
  //Supply.flags.SM_POWERTYPE = NA;
  // Test for power fail: True in all operation modes, if there isn't a sufficient line voltage for a long time
  if (Supply.uline_diff_abs < suMoCoP->absMinVolt) {     // Absolute min level valid for DC- as well as for AC-supply  //OLAF TODO check uline2/uline1
    supplyMonitorPriv.powerFailCnt++;
    if (supplyMonitorPriv.powerFailCnt >= suMoCoP->powerFailCount) {    // Wait a bit more than two half cycles
      supplyMonitorPriv.powerFailCnt = suMoCoP->powerFailCount;         // Avoid overflow
      //Supply.flags.powerfail = PF;
      Supply.flags.SM_POWERTYPE = PF;
      if (supplyMonitorPriv.rac_detected_cnt > 0)
        {
          supplyMonitorPriv.rac_detected_cnt --;
        }
     }
  }
  else {
    // Line input voltage is reasonable high
    // High input voltage, no commutation, no sleep mode: We obviously have supply, so we
    supplyMonitorPriv.powerFailCnt  = 0;      // ... can/should reset power fail count
    //supplyMonitorPriv.powerSagCnt   = 0;      // ... can reset (sleep mode) power sag counter
    // powerSagCnt is used for detection line off or DC to generate a trigger to switch on Boost!!!
    supplyMonitorPriv.commTimeoutCnt++;       // ... and can/should increment DC detection counter

    //  First, check whether there was a commutation and process it in case of
    commutation_per = Supply.comm_ts - supplyMonitorPriv.commLastTimestamp;
    if (commutation_per) {                        // If there was a commutation
      // Check, whether the period was in a reasonable range
      if (commutation_per > PERIOD_AC_NOISE_BORDER_us) {
        SUPPLY_DEBUG_UART('c', 8, "commutation");
        supplyMonitorPriv.commLastTimestamp = Supply.comm_ts;
        supplyMonitorPriv.commTimeoutCnt    = 0;  // Clear DC detection counter when we had a reasonable comm. period
        supplyMonitorPriv.rac_detected_cnt  = 0;
        Supply.flags.SM_POWERTYPE = AC;

        FilterInvoke(&Supply.period, commutation_per);
        commutation_per = Supply.period.mean >> 16;
        if(commutation_per > PERIOD_60_70HZ_BORDER_us && commutation_per < PERIOD_40_50HZ_BORDER_us ) {
          if ( commutation_per > PERIOD_50_60HZ_BORDER_us ) {
            // period is longer than the half of 50Hz/60Hz
            //tmp_flags.AC50Hz = 1;
          }
          else {
            //tmp_flags.AC60Hz = 1;
          }
        }
      }
      else {
        SUPPLY_DEBUG_UART('?', 8, "dropped commutation");
      }
    }



    // Voltage ok, no commutation detected: Distinguish between DC and RAC
    if (Supply.uline_diff_abs_diff_max_min > suMoCoP->racDiffVolt)
    {
      supplyMonitorPriv.rac_detected_cnt += 10;
      supplyMonitorPriv.commTimeoutCnt = 0;                                     // "suppress" DC
    }
    else if (supplyMonitorPriv.rac_detected_cnt > 0)
    {
      supplyMonitorPriv.rac_detected_cnt --;
    }
    if (supplyMonitorPriv.rac_detected_cnt > (suMoCoP->racDetectCnt * 10))      // Factor 10, because rac_detected_cnt is incremented by 10
    {
      supplyMonitorPriv.rac_detected_cnt = (suMoCoP->racDetectCnt * 10);
      Supply.flags.SM_POWERTYPE = RAC;
    }



    // 2nd: Voltage ok, no commutation: Detect DC operation mode
    // But be careful: In line sag test with one missing half wave, 2 complete commutations are missing
    if (supplyMonitorPriv.commTimeoutCnt > suMoCoP->ac_detect_timeout) {
      supplyMonitorPriv.commTimeoutCnt = suMoCoP->ac_detect_timeout;
      supplyMonitorPriv.rac_detected_cnt = 0;
      Supply.flags.SM_POWERTYPE = DC;
      /// \todo Set DC_RAC-flag: Might be solved with PowerFailCounter (but this is reset some lines above)
    }
  }
}
static void SupMonEvaOvervoltage (void){

  //uint32_t shutdownValidThreshold = suMoCoP->shutdownVoltSqrPosTh;
  // For DC, increase SD threshold by 25% as a balance between 1.41 crest factor and some ripple on the DC voltage
  // Needed for rectified AC input during sleep mode, when measured line input equals to RAC peak voltage
  // \todo: Double check whether overvoltage protection should be active with DC input voltage
  //    if (Supply.flags.DC) {
  Supply.flags.SM_OVERVOLTAGE = OV_NO;
//  if (DC==Supply.flags.SM_POWERTYPE) {
//    shutdownValidThreshold += shutdownValidThreshold >> 1;  // Adding 25% for voltage is adding 50% for V^2 (!)
//  }

   if (Supply.Ulinepk.mean >= (suMoCoP->shutdownVoltPeak)) {  // no more RMS OV but peak
 // if (supplyMonitorPriv.ulineRms.mean >= (suMoCoP->shutdownVoltSqrPosTh)) {
//  if (supplyMonitorPriv.ulineRms.mean >= shutdownValidThreshold) {
    // Send signal when we hadn't OV yet or when the ballast is still not / no longer (DALI!) in sleep mode
    Supply.flags.SM_OVERVOLTAGE = OV_YES;
  }
}

// ---------------------------------------------------------------------------------------------------------------------
/*! \brief Init Supply Monitor.
 * \ingroup SupplyMon2Api
 *
 * Init internal data and set operation mode = "run" for this module. <br>
 * \warning Call this function exactly 1 time per startup/since reset or know precisely what you do.
 *
 * \tparam None
 * \retval None
 */


void SupplyMonitorInit(void) {
  //memset(&supplyMonitorPriv, 0, sizeof(supplyMonitorPriv));
  //memset(&Supply, 0, sizeof(Supply));
  //Supply.flags.Powerfail=1;
  Supply.flags.SM_MODE=sm_mode_reset;
  Supply.flags.SM_OVERVOLTAGE=OV_NO;
  Supply.flags.SM_POWERTYPE=NA;
  Supply.comm_ts=0;
  Supply.comm_udiff=0;
  //Supply.period=0;
  Supply.phase=0;
  //Supply.uline=0;
  Supply.uline_diff_abs=0;
  //Supply.uline_peak=0;
  Supply.uline_diff_abs_max = 0;
  Supply.uline_diff_abs_min = 0xFFFFFFFF;
  no_commutation_detected_cnt = 0;

  supplyMonitorPriv.lastUline=0;          ///< For sleep modes: Store Uline of last run of SupplyMonitorEval()
  supplyMonitorPriv.commLastTimestamp=0;  ///< To detect new commutation events and to measure the period
  //supplyMonitorPriv.umaxTrack=0;          ///< Track Umax during a (unfinshed) half cycle
  supplyMonitorPriv.phaseCnt=0;           ///< Internal high resoultion line phase counter
  supplyMonitorPriv.polarityOld=0;        ///< Polarity at the end of last SupplyMonitorTimestamp()
  supplyMonitorPriv.commTimeoutCnt=0;     ///< Counter for DC detection
  supplyMonitorPriv.powerFailCnt=0;       ///< Counter for missing SUPPLYMON_ABS_MIN_V -> power fail
  supplyMonitorPriv.powerSagCnt=0;        ///< Counter for continuos voltage decrements during sleep mode
  //supplyMonitorPriv.ulineRms=0;         ///< RMS Value of Uline
  supplyMonitorPriv.diff_abs_max = 0;
  supplyMonitorPriv.diff_abs_min = 0xFFFFFFFF;
  supplyMonitorPriv.rac_detected_cnt = 0;
  Supply.uline_diff_abs_diff_max_min = 0;


  FilterInit(&Supply.period, 0x10000,              FILTERSPEED(  110, 1.000), PERIOD_50_60HZ_BORDER_us);  // Tau = 1.00 sec; Filter update is done at every supply commutation
  FilterInit(&supplyMonitorPriv.ulineRms, 0x10000, FILTERSPEED(10000, 0.155), 0 );  // Tau = 155 ms (bad resolution, resulting speed para = 10)
  evaluate_timer=0;
  //suMoCoP = &supplyMonConstantsStandard;
  suMoCoP = &supplyMonConstants15V;
  sm_state = sm_state_reset;

//  FilterInit(&Supply.Ulinepk, 1, FILTERSPEED(SYSTICK_INT_Hz, 2.000), 0);
//  FilterInit(&Supply.Ulineabs, 1, FILTERSPEED(SYSTICK_INT_Hz, 0.050), 0);
  supplymonitor_deviation_cnt = 0;

  check_cmph_prev = 0;
  check_cmph_active = false;
  check_cmph_event_cnt = 0;
  check_cmph_failure = false;
  check_cmph_fail_time_cnt = 0;
  check_cmph_max_fail_time_cnt = 0;
  check_cmph_total_fail_time_cnt = 0;

  check_loss_of_vin = 0;
  input_voltage_prev = 0;
  monitor_duration_cnt = 0;
  loss_of_vin_detected_cnt = 0;
  Supply.request_pfc_burst = false;

  pimp_ulinepk_cnt = SUPPLYMONITOR_PIMP_ULINEPK_DURATION;
}
static void ResetEva(void) {
  Supply.flags.SM_POWERTYPE=NA;
  supplyMonitorPriv.powerFailCnt = 0;
  supplyMonitorPriv.commTimeoutCnt = 0;
  supplyMonitorPriv.rac_detected_cnt = 0;
}
/*! Evaluate the supply voltage characteristics.
 * \ingroup SupplyMon2Api
 *
 * This routine is called by timebase in roughly 2 ms intervals to set different supplyMonConstants.
 * The constants are set automatically during startup to allow to enter Lab Mode(15V) or LowVolatgProg-Mode
 * or Calibration-Mode in addition there is a hysteresis for normal operation.
 * A 3 bit sm-mode variuable is provoided to signal the mode to the system.
 *
 * \tparam None
 * \retval None
 */
// OLAF_tmp: test setup
// OLAF_tmp: Supply.uline2 Summe der Last Werte
// OLAF_tmp: if (AC==Supply.flags.SM_POWERTYPE || DC==Supply.flags.SM_POWERTYPE)
// OLAF_tmp:


#ifdef DISABLE_POWER_MONITOR
volatile static bool powerIsOn=1;
#endif
extern uint8_t g_ac_flag;
static void SupplyMonitorFSM (void)
{

#ifdef DISABLE_POWER_MONITOR

#ifndef TEST_ONOFF
if(g_ac_flag)
{
powerIsOn=1;
//P1_1_set();
}
else
{
powerIsOn=0;
//P1_1_reset();
}
#endif
    if ( powerIsOn )
    {
        suMoCoP = &supplyMonConstants230VACwide;
        Supply.flags.SM_POWERTYPE = AC;
        Supply.flags.SM_MODE = sm_mode_230V;
        sm_state = sm_state_230VACwide;
    }
#endif

    switch (sm_state) {
  case sm_state_reset:
    fsm_cycle_cnt=0;
    sm_state=sm_state_15V;
    suMoCoP = &supplyMonConstants15V;
    ResetEva();
    break;
        case sm_state_15V:
            if (fsm_cycle_cnt < suMoCoP->state_timeout)
            {
                fsm_cycle_cnt++;
            }
            else
            {
                Supply.flags.SM_MODE = sm_mode_15V;
            }

            if (Supply.uline_diff_abs > suMoCoP->exit_voltage)
            {
                fsm_cycle_cnt = 0;
                ResetEva();
                suMoCoP = &supplyMonConstants48V;
                Supply.flags.SM_MODE = sm_mode_reset;
                sm_state = sm_state_48V;
            }
        break;
  case sm_state_48V:
    fsm_cycle_cnt++;;
//    if ((fsm_cycle_cnt>suMoCoP->state_timeout)||(Supply.uline_diff_abs>suMoCoP->exit_voltage)) {
    if (Supply.uline_diff_abs>suMoCoP->exit_voltage) {
      fsm_cycle_cnt=0;
      ResetEva();
      suMoCoP = &supplyMonConstants293V;
      Supply.flags.SM_MODE=sm_mode_reset;
      sm_state=sm_state_293V;
    }
    else if ((AC==Supply.flags.SM_POWERTYPE) || (DC==Supply.flags.SM_POWERTYPE)) {
      fsm_cycle_cnt=0;
      Supply.flags.SM_MODE=sm_mode_48V;
    }

    break;
  case sm_state_293V:
    fsm_cycle_cnt++;
    if ((PF==Supply.flags.SM_POWERTYPE)||(Supply.uline_diff_abs>suMoCoP->exit_voltage)||(fsm_cycle_cnt > suMoCoP->state_timeout)) {
      ResetEva();
      fsm_cycle_cnt=0;
      suMoCoP = &supplyMonConstants230VDCnarrow;
      Supply.flags.SM_MODE=sm_mode_reset;  // allays reset and no powerfail unless first time sm_mode_230 was proven
      sm_state=sm_state_230VDCnarrow;
    }
    else if ((AC==Supply.flags.SM_POWERTYPE) || (DC==Supply.flags.SM_POWERTYPE)) {
      Supply.flags.SM_MODE=sm_mode_voltage_calibration;
    }
    break;
  /* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
  case sm_state_230VDCnarrow:
    if ((DC==Supply.flags.SM_POWERTYPE)&&(OV_YES==Supply.flags.SM_OVERVOLTAGE)) {
      Supply.flags.SM_MODE=sm_mode_230V;
    }
    if (PF==Supply.flags.SM_POWERTYPE) {
      Supply.flags.SM_MODE=sm_mode_powerfail;
    }
    if ((DC==Supply.flags.SM_POWERTYPE)&&(OV_NO==Supply.flags.SM_OVERVOLTAGE)) {
      Supply.flags.SM_MODE=sm_mode_230V;
      suMoCoP = &supplyMonConstants230VDCwide;
      sm_state=sm_state_230VDCwide;
    }
    if ((AC==Supply.flags.SM_POWERTYPE) || (RAC == Supply.flags.SM_POWERTYPE)) {
      ResetEva();
      suMoCoP = &supplyMonConstants230VACnarrow;
      sm_state=sm_state_230VACnarrow;
    }
    break;
  case sm_state_230VDCwide:
    if (PF==Supply.flags.SM_POWERTYPE) {
      ResetEva();
      suMoCoP = &supplyMonConstants230VDCnarrow;
      sm_state=sm_state_230VDCnarrow;
      Supply.flags.SM_MODE=sm_mode_powerfail;
    }
    if (OV_YES==Supply.flags.SM_OVERVOLTAGE) {
      ResetEva();
      suMoCoP = &supplyMonConstants230VDCnarrow;
      sm_state=sm_state_230VDCnarrow;
    }
    if ((AC==Supply.flags.SM_POWERTYPE) || (RAC == Supply.flags.SM_POWERTYPE)) {
      suMoCoP = &supplyMonConstants230VACwide;
      sm_state=sm_state_230VACwide;
    }
    break;
  case sm_state_230VACnarrow:
    if ((DC==Supply.flags.SM_POWERTYPE)||(PF==Supply.flags.SM_POWERTYPE)) {
      ResetEva();
      Supply.flags.SM_MODE=sm_mode_powerfail;
      suMoCoP = &supplyMonConstants230VDCnarrow;
      sm_state=sm_state_230VDCnarrow;
    }
    if (((AC==Supply.flags.SM_POWERTYPE) || (RAC == Supply.flags.SM_POWERTYPE))&&(OV_YES==Supply.flags.SM_OVERVOLTAGE)) {
      Supply.flags.SM_MODE=sm_mode_230V;
    }
    if (((AC==Supply.flags.SM_POWERTYPE) || (RAC == Supply.flags.SM_POWERTYPE))&&(OV_NO==Supply.flags.SM_OVERVOLTAGE)) {
      Supply.flags.SM_MODE=sm_mode_230V;
      suMoCoP = &supplyMonConstants230VACwide;
      sm_state=sm_state_230VACwide;
    }
    break;
  case sm_state_230VACwide:
    if (PF==Supply.flags.SM_POWERTYPE) {
      ResetEva();
      Supply.flags.SM_MODE=sm_mode_powerfail;
      suMoCoP = &supplyMonConstants230VACnarrow;
      sm_state=sm_state_230VACnarrow;
    }
    if (OV_YES==Supply.flags.SM_OVERVOLTAGE) {
      ResetEva();
      suMoCoP = &supplyMonConstants230VACnarrow;
      sm_state=sm_state_230VACnarrow;
    }
    if (DC==Supply.flags.SM_POWERTYPE) {
      suMoCoP = &supplyMonConstants230VDCwide;
      sm_state=sm_state_230VDCwide;
    }
    break;
  default:
    break;
  }

}


static void supplymonitor_adjust_Ulinepk(void)
{
  if (pimp_ulinepk_cnt > 0)
  {
    pimp_ulinepk_cnt --;
  }

  if (Supply.Ulinepk.mean >= Supply.Ulinepk.last)                               // calculation: "abs(.mean - .last)"
  {
    supplymonitor_Ulinepk_delta = Supply.Ulinepk.mean - Supply.Ulinepk.last;
  }
  else
  {
    supplymonitor_Ulinepk_delta = Supply.Ulinepk.last - Supply.Ulinepk.mean;
  }

  if (supplymonitor_Ulinepk_delta > VOLTS(10))
  {
    supplymonitor_deviation_cnt ++;
  }
  else
  {
    supplymonitor_deviation_cnt = 0;
  }

  if ((supplymonitor_deviation_cnt >= (35000/395)) || (0 != pimp_ulinepk_cnt)) // 35ms --> min drei Halbwellen @AC50Hz
  {
    Supply.Ulinepk.mean = Supply.Ulinepk.last;                                  // manipulate .mean due to long filter time
    supplymonitor_deviation_cnt = 0;
  }
}


// ---------------------------------------------------------------------------------------------------------------------
/*! Watch the line voltage and extract "simple" key parameters, which are to be evaluated later on
 * \ingroup SupplyMon2Api
 *
 * Fast and as far as possible jitter free routine called every systick interrupt. Like described, it only detects and
 * saves the key parameters of the commutation and of the last line voltage half sine.
 *
 * \internal
 * The key parameters are:
 * - a timestamp of the commutation (in systick resolution)
 * - the voltage difference at commutation time (for a tbd line sync PLL to compensate the poor time resolution)
 * - the peak voltage of the last half cycle
 *
 * Evaluating these parameters (AC/DC detection, 50/60Hz detect, power fail detect, ...) is done in other routines
 *
 * \tparam None
 * \retval None
 */
static void SupplyMonitorTimestamp(void) {
  si32        uline_diff, phase_help;
  ui32        ulinesqare = 0;
  ui8         polarity_new;

  uint32_t    uline_diff_abs;

  //debug_daniel
  //copy to monitor a bitfield

//  if (evaluate_timer++>4)
//  {
//    SupplyMonitorEvaluate();
//    evaluate_timer=0;
//  }

  TimebaseSinceStart_us += 395;
  // Evaluate line peak voltage
//  Supply.uline = Uline1.mean + Uline2.mean;     // Line voltage at the ballast (unsigned)
//  if (supplyMonitorPriv.umaxTrack < Supply.uline) {              // Peak detect Umax
//    supplyMonitorPriv.umaxTrack = Supply.uline;
//  }



  FilterInvoke(&Supply.Ulinepk, Supply.uline_diff_abs_max);
  FilterInvoke(&Supply.Ulineabs, Supply.uline_diff_abs);
  supplymonitor_adjust_Ulinepk();



  // Measure rectified mean square of supply voltage
  // Use the diff of Uline1 and Uline2 instead of the sum: Indeed, due to the time constants, it's the better criteria
  // See the investigations 2013-08 of Max on the THD table implementation with uline.last instead of the phase
  // \todo Prio1 Clarify compliance of this with Innotec Rectified AC -> overvoltage protection -> Mail WL 2014-08-04
  //uline_diff = ((si32)((si32)Uline1.last) - ((si32)Uline2.last));

  //uline_diff_abs = (uint32_t) uline_diff;
  if (uline_diff_abs >= (uint32_t)(1 << 31)) {
    uline_diff_abs = 0xFFFFFFFF - uline_diff_abs + 1;
  }
  Supply.uline_diff_abs = uline_diff_abs;


  if (supplyMonitorPriv.diff_abs_max < uline_diff_abs)
  {
    supplyMonitorPriv.diff_abs_max = uline_diff_abs;
  }

  if (supplyMonitorPriv.diff_abs_min > uline_diff_abs)
  {
    supplyMonitorPriv.diff_abs_min = uline_diff_abs;
  }


  SupplyMonitorCheckLossOfInputVoltage();


  ulinesqare = (uline_diff >> 16)*(uline_diff >> 16);
  if( ulinesqare >= ((1 << 25) - 1) ){    //prevent overflow (happens at 536Vpk / 379Vrms)
    ulinesqare = ((1 << 25) - 1);
  }
  ulinesqare = ulinesqare >> 10;
  FilterInvoke(&supplyMonitorPriv.ulineRms, ulinesqare);

  // Detect and "time-stamp" a line voltage commutation and store some key parameters of last half wave
  // But do this only, if we have a sufficient voltage difference and we aren't in the "noise only" range.
//  uline_diff = Uline1.last - Uline2.last;       // Line voltage at the ballast (signed)
//  uline_diff = Uline1.mean - Uline2.mean;       // Line voltage at the ballast (signed)
  if (uline_diff > suMoCoP->polchangeMinDiffVolt || uline_diff < - suMoCoP->polchangeMinDiffVolt ){


    // Save minimum data when no commutation is detected: every 30ms
    no_commutation_detected_cnt ++;
    if (no_commutation_detected_cnt > (18e3/395))
    {
      Supply.uline_diff_abs_max       = supplyMonitorPriv.diff_abs_max;
      supplyMonitorPriv.diff_abs_max  = 0;
      Supply.uline_diff_abs_min       = supplyMonitorPriv.diff_abs_min;
      supplyMonitorPriv.diff_abs_min  = 0xFFFFFFFF;
      no_commutation_detected_cnt     = 0;
      if (Supply.uline_diff_abs_max >= Supply.uline_diff_abs_min)
      {
        Supply.uline_diff_abs_diff_max_min = Supply.uline_diff_abs_max - Supply.uline_diff_abs_min;
      }
      else
      {
        Supply.uline_diff_abs_diff_max_min = 0;
      }
    }


    polarity_new = 1 + (uline_diff < 0);        // results in 1 = "Uline1 < Uline2" or 2 = "Uline1 >= Uline2"
    if (supplyMonitorPriv.polarityOld == 0) {                     // This is the first polarity detection
      supplyMonitorPriv.polarityOld = polarity_new;               // then do nothing except to store the very first polarity
    }
    else {
      if (supplyMonitorPriv.polarityOld != polarity_new) {        // Polarity changed
        supplyMonitorPriv.polarityOld   = polarity_new;
        Supply.comm_ts                  = TimebaseSinceStart_us;
        Supply.comm_udiff               = uline_diff;
        //Supply.uline_peak               = supplyMonitorPriv.umaxTrack;
        //supplyMonitorPriv.umaxTrack     = 0;
        supplyMonitorPriv.phaseCnt      = 0;
        Supply.uline_diff_abs_max       = supplyMonitorPriv.diff_abs_max;
        supplyMonitorPriv.diff_abs_max  = 0;
        Supply.uline_diff_abs_min       = supplyMonitorPriv.diff_abs_min;
        supplyMonitorPriv.diff_abs_min  = 0xFFFFFFFF;
        no_commutation_detected_cnt     = 0;
        if (Supply.uline_diff_abs_max >= Supply.uline_diff_abs_min)
        {
          Supply.uline_diff_abs_diff_max_min = Supply.uline_diff_abs_max - Supply.uline_diff_abs_min;
        }
        else
        {
          Supply.uline_diff_abs_diff_max_min = 0;
        }
      }
    }
  }

  // Calculate and export line phase
  // Increment the phase counter, target is to deliver the binary range 0 to 127 for one half sine
  //if (Supply.flags.AC50Hz) {
  if (0) {
    phase_help        = (ui32) (65536.0 * (100e-6 / 10.000e-3) * 255 );
  }
  //else if (Supply.flags.AC60Hz) {
  else if (0) {
    phase_help        = (ui32) (65536.0 * (100e-6 / 8.3333e-3) * 255 );
  }
  else {
    supplyMonitorPriv.phaseCnt  = 0;
    phase_help                  = 0;
  }
  supplyMonitorPriv.phaseCnt   += phase_help;
  //phase_help      = ((supplyMonitorPriv.phaseCnt + (SUPPLYMON_PHASE_COMPENSATION * 65536)) >> 16);
  if (phase_help  > 255) {
    phase_help   -= 255;
  }
  Supply.phase    = phase_help;
}
void SupplyMonitorCyclicTask (void) {
  SupplyMonitorTimestamp();
  evaluate_timer++;
  if (evaluate_timer>=5)  // set to 5 -> 2ms
  {
    evaluate_timer=0;
    SupMonEvaFailACDC();
    SupMonEvaOvervoltage();
    SupplyMonitorFSM();
  }

  SupplyMonitorCheckSupply();

}


//------------------------------------------------------------------------------
uint32_t hw_P1_1_read (void)
{
  uint32_t input_level;
  //input_level = P1_1_read();
  return input_level;
}


void SupplyMonitorCheckSupply (void)
{
  uint32_t check_cmph;

  check_cmph = hw_P1_1_read();

  // falling edge
  if ((check_cmph_prev) && (!check_cmph))
  {
    check_cmph_event_cnt ++;
    check_cmph_failure = true;
    check_cmph_fail_time_cnt = 0;
  }

  // rising edge / active flag
  if ((!check_cmph_prev) && (check_cmph))
  {
    check_cmph_active = true;
    check_cmph_failure = false;
  }

  // sum up total number of timeslices
  if ((!check_cmph) && (true == check_cmph_active))
  {
    check_cmph_total_fail_time_cnt ++;
  }

  if (true == check_cmph_failure)
  {
    check_cmph_fail_time_cnt ++;
  }

  if (check_cmph_fail_time_cnt > check_cmph_max_fail_time_cnt)
  {
    check_cmph_max_fail_time_cnt = check_cmph_fail_time_cnt;
  }

  check_cmph_prev = check_cmph;
}



void SupplyMonitorCheckLossOfInputVoltage(void)
{
  uint32_t input_voltage_drop;

  switch (check_loss_of_vin)
  {
  case 0:
    input_voltage_drop = (Supply.Ulineabs.mean >> SUPPLYMONITOR_INPUT_VOLTAGE_DEC_SCALE ) * SUPPLYMONITOR_INPUT_VOLTAGE_DEC_FACTOR; // entspricht Grenzwert -2% @50mslimit_decrease;

    input_voltage_prev = Supply.Ulineabs.mean - input_voltage_drop;

    monitor_duration_cnt = 0;
    check_loss_of_vin = 1;
  break;

  case 1:
    if (monitor_duration_cnt >= ((50e3/395)-1))
    {
      if (input_voltage_prev > Supply.Ulineabs.mean)
      {
        // Spannung fällt schnell --> Netz entfernt
        if (loss_of_vin_detected_cnt <= 2)
        {
          loss_of_vin_detected_cnt ++;
        }

        if (2 == loss_of_vin_detected_cnt)
        {
          Supply.request_pfc_burst = true;
          loss_of_vin_detected_cnt = 0;
        }
      }
      else
      {
        // Spannung steigt oder ist stabil
        loss_of_vin_detected_cnt = 0;
      }
      check_loss_of_vin = 0;
    }
    monitor_duration_cnt ++;
  break;

  default:
  break;
  }

  // nicht bei AC, RAC oder "Eingangsspannung" kleiner 34V
  if ((AC==Supply.flags.SM_POWERTYPE) || (RAC == Supply.flags.SM_POWERTYPE) || (Supply.uline_diff_abs < VOLTS(34)))
  {
    Supply.request_pfc_burst = false;
  }
}



void SupplyMonitorPimpUlinepk(void)
{
  pimp_ulinepk_cnt = SUPPLYMONITOR_PIMP_ULINEPK_DURATION;

}
/// \}  //defgroup SupplyMon2
