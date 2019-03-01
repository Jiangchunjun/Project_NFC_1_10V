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
// $Author: M.Gerber $
// $Revision: 3332 $
// $Date: 2016-03-23 16:06:48 +0800 (Wed, 23 Mar 2016) $
// $Id: SupplyMonitorDefaults.h 3332 2016-03-23 08:06:48Z M.Gerber $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/SupplyMonitorDefaults.h $
//

/**
 * \addtogroup SupplyMon2Parameter
 * \{
 *
 *  \file
 *  \brief Default values for SupplyMonitor parameters
 */


//----------------------------------------------------------------------------------------------------------------------
// Defaults for 220 - 240V 50/60Hz nets including corresponding DC supplies

#ifdef SUPPLYMON_DEFAULTS_230V_50Hz_60Hz_DC


/**
 * Call interval for \ref SupplyMonitorEvaluate() in us.
 */
#define SUPPLYMON_STAMP_us                         SYSTICK_INT_us  // Fine for all line frequencies below 200Hz
#define SUPPLYMON_EVAL_FACTOR                        5  // necessary for FSM
#define SUPPLYMON_EVAL_us       (SUPPLYMON_STAMP_us * SUPPLYMON_EVAL_FACTOR)  // Fine for all line frequencies below 200Hz


/** \brief Absolute min. input voltage for power fail detect.
 *
 * Valid for all operation modes (run/sleep) and all supply characteristics (AC, DC, RAC) <br>
 * This value has to be exceeded at least for a short time within a interval of \ref SUPPLYMON_POWERFAIL_ms to
 * avoid a power fail detection.
 */
#define SUPPLYMON_ABS_MIN_V                        160  // 200VDC - 20% resp. 180VAC - 30% line sag - 10% tolerance

/// Same as \ref SUPPLYMON_ABS_MIN_V, but for low voltage programming mode
#define SUPPLYMON_ABS_MIN_LV_V                      40  // 42V - 5% and with 5 V margin to LNK302


/** \brief Interval for DC detect.
 *
 * Within this time constant a communtation should happen regularily to detect AC: If line voltage is present and this
 * interval is exceeded without a commutation, DC supply is detected and signaled.
 */
#define SUPPLYMON_DC_DETECT_TIMEOUT_ms              34  // Be careful with full half wave sags + DC detection issues!


/** \brief Interval for power fail detect.
 *
 * Time for subsequent line voltage samples below \ref SUPPLYMON_ABS_MIN_V until a power fail is detected and signaled.
 */
#define SUPPLYMON_POWERFAIL_ms                      30  // A bit more than 2 half periods (full half wave sags + phase)

/** \brief Time to detect an <b>indication</b> for powerfail during sleep mode.
 *
 * Interval for subsequent falling line voltage samples (= X-Cap voltage), after which a power fail is asumed if
 * the ballast is in sleep mode (when no power is drawn out of the Xcaps).
 *
 * As said, active only during sleep mode: During this mode, this indication then starts the PFC for a short time
 * to double check the situation (do the Xcaps discharge or not?)
 *
 * A "real" and signaled powerfail itself is detected by a "real" low voltage at the X-Caps.
 */
#define SUPPLYMON_SLEEP_POWERSAG_ms                 26  // A bit more than 2 half periods of 50 Hz


/** \brief Max. frequency accepted as normal AC
 *
 * Above this frequency, commutations are considered as noise and are filtered to get rid of ring effects during
 * switch on/off with high SupplyMonitorEval() call rates<br>
 * If there is really a high frequency as supply (above this limit), the following happens:
 * The high frequency commuations will be filtered until one of the many upcoming commutations fits into this limit.
 * In consequence, AC supply is flagged nevertheless, but neither 50Hz- nor 60Hz-indicator will be set. <br>
 * But but realistic: In this case, the half period in struct \ref Supply won't reflect the real supply frequency
 * but would provide a frequency around the double of this limit (period of half wave). Moreover: You can't beat
 * the Nyquist condition related to \ref SUPPLYMON_EVAL_us
 */
#define SUPPLYMON_AC_MAXFREQ_Hz                     99  // Highest value possible for SUPPLYMON_EVAL_us = 2ms


/** \brief Voltage to be achieved at the input terminals after power fail within 0.5 sec
 *
 * Above this voltage at the input terminals, the PFC tries to discharge the Xcaps with a short burst if a
 * falling slope of 50% per sec. is detected in <b>sleep mode</b> <br>
 * This is needed for 2 reasons: <br>
 * 1 st, <b>"Saftey"</b>: To ensure a voltage below this limit at the line terminals within a short time
 * 2 nd: To determine if line is really disconnected or whether there was only a small voltage dip during sleep mode
 */
#define SUPPLYMON_XCAP_DISCHARGE_V                  30  // 34V - 10%

/// Same as \ref SUPPLYMON_XCAP_DISCHARGE_V, but for low voltage programming mode
#define SUPPLYMON_XCAP_DISCHARGE_LV_V               15  // Discharge significantly in low voltage mode


/** \brief Voltage difference to detect a AC commutation.
 *
 * This voltage is the threshold for the difference between both line voltage measurement channels (L resp. N) to
 * signal that a polarity change has happend. <br>
 * A value to low increases the risk for spurious event and jitter, a value to high introduces a significant
 * time lag between real commutation and it's recognition. <br>
 * Nevertheless the exact value of this level is not too important, but shifts - as said - the phase of the
 * commutation timestamps to be compensated by \ref SUPPLYMON_PHASE_COMPENSATION
 */
#define SUPPLYMON_POL_MINDIFF_V                     30  // No spuriuous issues observed, reasonable phase shifts
#define SUPPLYMON_POL_MINDIFF_V_DC_230              15  // Tweak to allow AC recognition within low DC voltages OLAF!!
/// Same as \ref SUPPLYMON_POL_MINDIFF_V, but for low voltage programming mode
#define SUPPLYMON_POL_MINDIFF_LV_V                  10  // Fine for low voltage programming


/** \brief Phase compensation for AC supply.
 *
 * Compensate the phase shift of \ref supply_monitor_state_t.phase to the real AC input voltages caused by hardware,
 * filters and \ref SUPPLYMON_POL_MINDIFF_V.
 */
#define SUPPLYMON_PHASE_COMPENSATION                22  // Perfect for SUPPLYMON_POL_MINDIFF_V = 30V with 50 Hz


/** \brief Line overvoltage shutdown above this voltage (rms)
 */
#define SUPPLYMON_OVERVOLT_SHUTDOWN_V              280  // According to spec OTi DALI 50


/** \brief After overvoltage shutdown, restart again below this voltage
 */
#define SUPPLYMON_OVERVOLT_RESTART_V               270  // Gives 10V hysteresis, maybe a bit to narrow at 240VAC +10%


#endif // SUPPLYMON_DEFAULTS_230V_50Hz_60Hz_DC

/** \} */ // SupplyMonitor
