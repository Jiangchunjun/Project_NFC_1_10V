#ifndef __SUPPLYMONITOR_USERINTERFACE_H__
#define __SUPPLYMONITOR_USERINTERFACE_H__

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
// $Author: F.Branchetti $
// $Revision: 6164 $
// $Date: 2016-11-21 23:53:04 +0800 (Mon, 21 Nov 2016) $
// $Id: SupplyMonitor_userinterface.h 6164 2016-11-21 15:53:04Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/SupplyMonitor_userinterface.h $
//

#include <stdbool.h>
#include "global.h"
#include "filter.h"
/** \ingroup SupplyMon2Intern
 * \file
 * \brief Header file for input power monitoring.
 *
 * \addtogroup SupplyMon2Api
 * \{
 */

//----------------------------------------------------------------------------------------------------------------------
// Static data types, variables and functions

/// A bitfield/union used as a part of \ref supply_monitor_state_t to hold the status bits of the supply voltage.
///
/// \warning All bits can be accessed in 3 modes/names
///  - via all
///  - via edge_trig_flags/state_trig_flags
///  - via the bit's names
typedef enum
{
  /** \brief Reset state, never entered again when this mode is left.
  */
  sm_mode_reset,

  /** \brief Powerfail mode;  Entered when voltage is too low to stay in the level already entered: There is now way back from e.g. 230 V to 48 or 15 V
  */
  sm_mode_powerfail,

  /* \brief Operation mode, when no other voltage level is recognized after a tbd \b timeout.
   * Will never be entered, when LV or 230V mode was active since last hardware reset.
  \warning Be careful: During this mode neither AC nor DC flags are valid, but might be set.
  */
  sm_mode_15V,

  /* \brief Low voltage operation mode: Entered when there is sufficient voltage at line input to ensure DALI Low Voltage programming
   * mode and a timeout that noirmal line voltage isn't reached, is passed
   *
   * Will never be entered, when 230V mode was active since last hardware reset.
   *
   * \warning But be careful: During line dip tests it might happen, that the ballast starts within this mode after a real brown out event, but will proceed to normal operation later on.
   *
   */
  sm_mode_48V,

  /* \brief Normal operation mode.
   *
   * With some hysteresis, this mode can proceed to powerfail or overvoltage only.
  */
  sm_mode_230V,

  /* \brief Overvoltage condition, PFC is not allowed to run.
   *
   * A surge might trigger this state, but is not guaranteed to do.
   *
   * With some hysteresis, this mode can proceed to powerfail or normal mode only.
   */

  //sm_mode_over_voltage,

  /* \brief calibration condition, PFC is not allowed to run.
   *
   * This state can be reached only once during start-up. If it is not reached within ~20ms
   *
   * it cannot be entered later on. But this mode can mask overvoltage event.
   *
   * Mode is used to calibrate voltage divider Uline1/2 and Urail within production.
   */

  sm_mode_voltage_calibration


} supply_mode_t;
typedef enum // used as a mutual for powerfail, AC, DC and unknown flag
{
  NA,  // not applicable = unknown
  PF,  // powerfail
  AC,  // AC
  DC,  // DC (AC and DC should be an extra enum)
  RAC  // rectified AC
} supply_powertype_t;

typedef enum // used as a mutual for overvoltage flag
{
  OV_NO,
  OV_YES
} supply_overvoltage_flag_t;

typedef struct {                         // supply_flags_t

  supply_overvoltage_flag_t SM_OVERVOLTAGE;
  supply_powertype_t SM_POWERTYPE;       ///< used as a mutual for powerfail, AC, DC and unknown flag
  supply_mode_t SM_MODE;

} supply_monitor_intern_flags_t;


/** \brief Structure for a consistent, hardware independent access to analog values with a defined, fixed scaling which
 *  are moreover already calibrated and filtered.
 *
 * For example, rail voltage can be accessed by:
 *
 *  - Urail.raw   Raw ADC read-out value dependend on the hardware as there is voltage reference, voltage dividers,
 *                tolerances, ...
 *  - Urail.last  Last Urail processed as a 32-bit value scaled in VOLT_uV_LSB (normally ~ 1 ... 8 uV / LSB)
 *                independent of hardware implementation
 *  - Urail.mean  RC-filtered Urail as a 32-bit value scaled in VOLT_uV_LSB (normally ~ 1 ... 8 uV / LSB)
 *                independent of hardware implementation
 *
 *  See also: \ref Analog.c, \ref FilterInit() and \ref FilterInvoke()
 */
typedef struct
{
    /// \brief RC-filtered, 32-bit value suited to access analog values in a hardware independent way
    /// with a defined, fixed scaling (for voltages e.g. in VOLT_uV_LSB, normally 1 uV / LSB).
    /// \n This is the main output of the filter() infrastructure. It normally includes all needed calculations and
    /// calibration factors and is also, if possible, temperature compensated. See \ref scale for further reference.
    uint32_t      mean;

    /// \brief Like \ref mean, a 32-bit value suited to access analog values in a hardware independent way, but which is
    /// \b not filtered:
    /// \n In the end, this is the scaled last sample of the Filter() input.
    uint32_t      last;

    /// \brief Scaling factor for the values to be filtered.
    /// This factor usually is used to get normalized values (e.g. 1 uV / LSB) from raw analog values.
    /// \n In a perfect setup, it includes the \b nominal, \b calculated factor dependent on reference voltage,
    /// voltage dividers, compensation for systematic errors as well as a \b calibration factor if available.
    /// \n \n It also can be set to 1 to use the Filter() setup for a plain filtering only (for other than analog values
    /// as slowly adapting thresholds, already otherwise scaled values or similar).
    uint32_t      scale;

    /// Last value fed into the filter \b before scaling, quite often a raw ADC read-out value
    uint32_t      raw;

    /// \brief Max. raw value the filter can process without overflow; Set by FilterInit() according to \ref scale,
    /// a parameter ot this function.
    uint32_t      rawmax;

    /// \brief Speed of the software RC-filter in 1 / 65536.
    /// Tau of the filter can be calculated as follows:
    /// tau = 65536 * [ Interval of calling FilterInvoke() ] / speed \n
    /// I.e. that (65536 / speed) calls to FilterInvoke() are needed to reach 1/exp(1) = 63% of a value to be filtered.
    /// \n \n A value of 0 suppresses the filtering: All values passed to FilterInvoke() will - after the scaling - go
    /// directly to the data fields \ref last and \ref mean of this struct.
    uint16_t      speed;

    /// Internal data of Filter()-functions to accumulate post decimal point values
    uint16_t      frac;

} Filter_t;



/// A struct to access all information provided by the supply monitor.
typedef struct {                          // supply_state_t
  //supply_flags_t  flags;                  ///< See seperate supply_flags_t typedef doc
  supply_monitor_intern_flags_t  flags;   ///< See seperate supply_flags_t typedef doc
  //uint32_t        uline;                  ///< Momentary line voltage (Uline1.mean + Uline2.mean)
  //uint32_t        uline_peak;             ///< Line peak voltage during last line voltage half wave
  uint32_t        uline2;                 ///< Momentary line voltage (Uline1.last + Uline2.last)
  uint32_t        uline_diff_abs;         ///< ABS(Uline1.last - Uline2.last)
  uint32_t        uline_diff_abs_max;
  uint32_t        uline_diff_abs_min;
  uint32_t        uline_diff_abs_diff_max_min;
  uint32_t        comm_ts;                ///< Timestamp of last line voltage commmutation event
  uint32_t        comm_udiff;             ///< Voltage difference at last line voltage commmutation event
  Filter_t        period;                 ///< Period of half wave of the line voltage in AC operation mode (in DC operation: 0)
  uint8_t         phase;                  ///< Exports the phase of the line voltage for THD enhancement or such;
                                          ///< The range of this indicator is from 0 to 255 and for sure there are
                                          ///< missing values and a small offset around the zero crossing.
                                          ///< <br> In the end, 4 or 5 MSB can be used
                                          ///< \b Preliminary feature without a PLL to sync on the line voltage;
                                          ///< And, also don't rely on the phase too much, there are 2 or 3 ticks shift
  Filter_t        Ulinepk;
  Filter_t        Ulineabs;
  bool            request_pfc_burst;
} supply_monitor_state_t;

extern uint32_t check_cmph_event_cnt;
extern uint32_t check_cmph_max_fail_time_cnt;
extern uint32_t check_cmph_total_fail_time_cnt;

extern supply_monitor_state_t Supply;     //  ... but writable for the module itself


extern void SupplyMonitorInit(void);
extern void SupplyMonitorCyclicTask(void);

extern void SupplyMonitorCheckSupply(void);

extern void SupplyMonitorCheckLossOfInputVoltage(void);

extern void SupplyMonitorPimpUlinepk(void);

/// \}  //defgroup SupplyMon2
#endif /* __SUPPLYMONITOR_USERINTERFACE_H__ */