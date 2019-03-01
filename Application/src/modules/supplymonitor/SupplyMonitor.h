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
// $Author: O.Busse $
// $Revision: 2513 $
// $Date: 2016-01-15 18:00:57 +0800 (Fri, 15 Jan 2016) $
// $Id: SupplyMonitor.h 2513 2016-01-15 10:00:57Z O.Busse $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/SupplyMonitor.h $
//

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
//typedef union {                         // supply_flags_t
//  ui32        all;                      ///< All flags together
//
//  struct {
//    /***  edge_trig_flags (lower 16 bit!) ***/
//    unsigned  AC:1;                     ///< Indicates AC operation
//    unsigned  AC50Hz:1;                 ///< Indicates 50Hz operation
//    unsigned  AC60Hz:1;                 ///< Indicates 60Hz operation
//    unsigned  fill_1:1;                 ///< Separate the bits for debug reasons
//    unsigned  DC:1;                     ///< Indicates DC operation (always set if DC, either "pure" or "RAC")
//    unsigned  DC_RAC:1;                 ///< Indicates DC operation with rectified AC
//    unsigned  fill_2:1;                 ///< Separate the bits for debug reasons
//    unsigned  Powerfail:1;              ///< Indicates a power fail situation
//    unsigned  fill_x:8;                 ///< Filler to separate low word (16bit) and high word if flags are added, adapt this
//
//    /***  state_trig_flags (higher 16 bit!) ***/
//    unsigned  Overvoltage:1;            ///< Indicates a line overvoltage
//  };
//
//  struct {
//    unsigned  edge_trig_flags:  16;     ///< Separate the 32bit flags var in two 16bit ranges, that can be accessed separately
//    unsigned  state_trig_flags: 16;     ///< Like masking the 32 bit - if searching for bit change operations via
//                                        ///< .all --> ALSO SEARCH for .edge_trig_flags and .state_trig_flags
//  };
//
//} supply_flags_t;


/// A struct to access all information provided by the supply monitor.
//typedef struct {                          // supply_state_t
//  supply_flags_t  flags;                  ///< See seperate supply_flags_t typedef doc
//  //ui32            uline;                  ///< Momentary line voltage (Uline1.mean + Uline2.mean)
//  //ui32            uline_peak;             ///< Line peak voltage during last line voltage half wave
//  uint32_t        uline2;                 ///< Momentary line voltage (Uline1.last + Uline2.last)
//  uint32_t        uline_peak2;            ///< Line peak voltage during last line voltage half wave
//  uint32_t        uline_diff_abs;         ///< ABS(Uline1.last - Uline2.last)
//  ui32            comm_ts;                ///< Timestamp of last line voltage commmutation event
//  ui32            comm_udiff;             ///< Voltage difference at last line voltage commmutation event
//  Filter_t        period;                 ///< Period of half wave of the line voltage in AC operation mode (in DC operation: 0)
//  ui8             phase;                  ///< Exports the phase of the line voltage for THD enhancement or such;
//                                          ///< The range of this indicator is from 0 to 255 and for sure there are
//                                          ///< missing values and a small offset around the zero crossing.
//                                          ///< <br> In the end, 4 or 5 MSB can be used
//                                          ///< \b Preliminary feature without a PLL to sync on the line voltage;
//                                          ///< And, also don't rely on the phase too much, there are 2 or 3 ticks shift
//} supply_monitor_state_t;

#ifndef MODULE_SUPPLYMONITOR
const                                     // Read only for the rest of the world
#endif
extern supply_monitor_state_t Supply;     //  ... but writable for the module itself




extern void SupplyMonitorInit(void);
extern void SupplyMonitorSetState(generic_state_t new_state);
extern void SupplyMonitorSetConstants(char newconstants);
extern void SupplyMonitorTimestamp(void);
extern void SupplyMonitorEvaluate(void);

extern uint32_t TimebaseSinceStart_us;
void print_supply_info(void);


/// \}  //defgroup SupplyMon2
