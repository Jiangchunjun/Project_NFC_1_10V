// ---------------------------------------------------------------------------------------------------------------------
// Firmware for OTi NONSELV Generation 2 Ballasts
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
// Initial version: W. Limmer, 2013-01
// $Author: F.Branchetti $
// $Revision: 13757 $
// $Date: 2018-06-14 20:07:08 +0800 (Thu, 14 Jun 2018) $
// $Id: global.h 13757 2018-06-14 12:07:08Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/kernel/global.h $
//
/// \file
/// \brief Parameters of the ballast independ from the used Microcontroller
/// For all parameters related to the used micro, goto/see at ./ARCH/arch_resources.h
#include <stdint.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <stddef.h>

#ifndef __GLOBAL_H__
#define __GLOBAL_H__


//----------------------------------------------------------------------------------------------------------------------
/// \addtogroup OS
///\{

//----------------------------------------------------------------------------------------------------------------------
// Compiler specific things


#define FLASH                 const

#define STATIC_ASSERT(...)    static_assert(__VA_ARGS__)
#define WEAK                  __weak
#define DLL_EXPORT
#define DEPRECATED _Pragma ("deprecated") // _Pragma is a C99-only keyword!
#define FLASH_ON_CORTEX_M     const
#define REV_BYTE_ORDER16(x)     __REV16(x)
#define REV_BYTE_ORDER32(x)     __REV(x)
#define COMPILER_WARN_PUSH
#define COMPILER_WARN_POP _Pragma("diag_warning=Pe061,Pe063,Pe068")
#define COMPILER_WARN_IGNORE_OVERFLOW _Pragma ("diag_suppress=Pe061")
#define COMPILER_WARN_IGNORE_INT_SIGN _Pragma ("diag_suppress=Pe068")
#define COMPILER_WARN_IGNORE_SHIFT_CNT_OVERFLOW _Pragma ("diag_suppress=Pe063")
#define INT_DISABLE()         __disable_interrupt() ///< Also include <intrinsics.h> or <core_cm0.h> to make this work
#define INT_ENABLE()          __enable_interrupt()  ///< Also include <intrinsics.h> or <core_cm0.h> to make this work
#define NOP()                 __no_operation()      ///< Also include <intrinsics.h> or <core_cm0.h> to make this work

#define STATIC static



#define ARCH                  XMC1300                                                             ///< Microcontroller architecture, used to include the hardware drivers
#define ARCH_STM32            1
#define ARCH_XMC1300          1 // 1: AA-Step


// #define POLPO_TRANSMIT_TEST

//----------------------------------------------------------------------------------------------------------------------
// Voltage variables (high/mid voltage measurements)

#include <stdint.h>

//#include "kernel_parameters.h" //MaxG20140722
typedef uint32_t  ui32;       ///< A shorter name for an unsigned integer with 32 bits
typedef uint16_t  ui16;       ///< A shorter name for an unsigned integer with 16 bits
typedef uint8_t   ui8;        ///< A shorter name for an unsigned integer with  8 bits
typedef int32_t   si32;       ///< A shorter name for a    signed integer with 32 bits
typedef int16_t   si16;       ///< A shorter name for a    signed integer with 16 bits
typedef int8_t    si8;        ///< A shorter name for a    signed integer with  8 bits

// In reference to DALI lib conventions; Throws some compiler warnings, which should remind us to establish
// a common global.h soon

#ifndef __VARTYPES
#define __VARTYPES
typedef uint8_t               byte;
typedef int16_t               int16;
typedef uint16_t              uint16;
typedef uint32_t              uint32;
typedef int32_t               int32;
typedef uint64_t              uint64;
#endif

// MIN and MAX macros
#define MAX(a,b) ((a) > (b) ? a : b)
#define MIN(a,b) ((a) < (b) ? a : b)


typedef enum            //   generic_state_t
{
    state_reset = 0,      ///< Hardware/module never touched since hardware reset, see also table above
    state_deepsleep,      ///< Hardware/module disabled by software, see also table above
    state_ready,          ///< Hardware/module initalized and ready to run, see also table above
    state_run,            ///< Hardware/module fully active, see also table above
} generic_state_t;


extern uint32_t pc_simulation;


#define CLR_BITS(lvalue, mask)    lvalue &= (~mask)
#define SET_BITS(lvalue, mask)    lvalue |= (mask)
#define BITMASK(bitcount)         (ui32)((1ULL<<bitcount)-1)    ///< Provides a number of right adjusted bits

//----------------------------------------------------------------------------------------------------------------------
// Other Constants
#define THERMAL_100_PERC				0x1000	// this is the internal 100% equivalent for thermal power reduction

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// Scaling of physical parameters

// Time ----------------------------------------------------------------------------------------------------------------
/// Use this macro to convert nanoseconds to timer ticks; result is float (!), use typecast if needed.
/// A tick is the shortest time present i the system (1 / timer's clock)
#define NANOSECS_TO_TICKS_f(ns)   ((ns) * 1.0e-9 * SYSCLK_Hz)
/// Use this macro to convert microseconds to timer ticks; result is float (!), use typecast if needed.
/// A tick is the shortest time present i the system (1 / timer's clock)
#define MICROSECS_TO_TICKS_f(us)  ((us) * 1.0e-6 * SYSCLK_Hz)
/// Use this macro to convert a constant, float time in seconds (~ > 1 ms) to a integer scaled in systick interrupt
/// counts.
#define SECS_TO_SYS_TICKS(s)    ((ui32)(1e6 * s / SYSTICK_INT_us))

// Voltage -------------------------------------------------------------------------------------------------------------
#define MICROVOLT_PER_LSB       1         ///< uV per LSB of all scaled and filtered voltage values -> FS (32-Bit) = +/-4294 V

/// Use this macro to convert a voltage (int or float) to the global voltage scaling
#define VOLTS(v)                ((si32) (1000000.0 * (v) / MICROVOLT_PER_LSB))

/// Use this macro to convert a voltage in mV (int or float) to the global voltage scaling
#define MILLI_VOLTS(v)          ((si32) (1000.0 * (v) / MICROVOLT_PER_LSB))

/// Use this macro to convert a voltage from internal format back to mV
#define BACK_TO_MILLI_VOLTS(v)  ((si32) ((v) * MICROVOLT_PER_LSB/ 1000.0))

/// Use this macro to convert a voltage from internal format back to V; Works up to 8 uV_per_LSB
#define BACK_TO_VOLT(v)         ((((v) >> 16) * (ui32)( (65536LL * 65536 * MICROVOLT_PER_LSB) / 1000000 )) >> 16)

/// Resolution of the "raw" ADC (without a voltage divider at the input) in nV
#define ADC_nV_PER_LSB_RAW      ((ui32)(1000.0 * HW_VOLTREF_uV / (1 << ADC_RESOLUTION)))  // 842.285@[3,45V/12Bit]

/// Resolution of the ADC with the "system" voltage divider at the input
#define ADC_uV_PER_LSB_DIV      ((ui32)(0.0010 * ADC_nV_PER_LSB_RAW * (HW_VOLTDIV_TOP+HW_VOLTDIV_BOT)/HW_VOLTDIV_BOT))  //152.453@[3,45V/12Bit]

/// Resolution of the ADC with the rail voltage divider
#define ADC_uV_PER_LSB_RAIL      ((ui32)(0.0010 * ADC_nV_PER_LSB_RAW * (HW_VOLTDIV_RAIL_TOP+HW_VOLTDIV_RAIL_BOT)/HW_VOLTDIV_RAIL_BOT))  //152.453@[3,45V/12Bit]
//#define ADC_uV_PER_LSB_RAIL      ((ui32)(0.0010 * ADC_nV_PER_LSB_RAW * 187))

/// Resolution of the ADC with the led voltage divider
#define ADC_uV_PER_LSB_LED      ((ui32)(0.0010 * ADC_nV_PER_LSB_RAW * (HW_VOLTDIV_LED_TOP+HW_VOLTDIV_LED_BOT)/HW_VOLTDIV_LED_BOT))


// Current -------------------------------------------------------------------------------------------------------------
#define NANOAMPS_PER_LSB       10         ///< nA per LSB of all scaled and filtered current values -> FS (32-Bit) = +/- 21.5 A

/// Use this macro to convert a current (int or float) to the global scaling
#define AMPS(a)                ((si32) (1.0e9 * (a) / NANOAMPS_PER_LSB))

/// Use this macro to convert a current in mA (int or float) to the global scaling
#define MILLI_AMPS(a)          ((si32) (1.0e6 * (a) / NANOAMPS_PER_LSB))

/// Use this macro to convert a current from internal format back to mA
#define BACK_TO_MILLI_AMPS(a)  ((si32) ((a) * NANOAMPS_PER_LSB/ 1.0e6))

///\}

#define DISABLE_POWER_MONITOR         // DEBUG: flag to disable power monitor check. ECG can be turned on and off
                                        // using m_debug_enable_AC boolean variable
//#define DISABLE_THERMAL_MANAGEMENT    // Switch to disable thermal protection
//#define HW_VERSION_FIX_ENABLE         // Switch to enable HW version fix
#define DISABLE_8S_DELAY              // Disables the 8s delay in case the firmware is not calibrated
//#define DISABLE_BMI_CHECK             // Disables the BMI check at the beginning of the firmware


#ifdef DISABLE_THERMAL_MANAGEMENT
#warning ********************* Thermal management is disabled!!! *********************
#endif

#endif
