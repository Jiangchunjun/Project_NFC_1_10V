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
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: global.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/fileTank/global.h $
//
/// \file
/// \brief Parameters of the ballast independ from the used Microcontroller
/// For all parameters related to the used micro, goto/see at ./ARCH/arch_resources.h


#ifndef __GLOBAL_H__
#define __GLOBAL_H__


// add global unit testing definitions
#ifdef UNIT_TESTING
    #include "unit_testing.h"
#endif

//----------------------------------------------------------------------------------------------------------------------
/// \addtogroup OS
///\{

//----------------------------------------------------------------------------------------------------------------------
// Compiler specific things

#ifdef __ICCARM__               // IAR compiler for ARM
  #define STATIC_ASSERT(...)    static_assert(__VA_ARGS__) ///< Static assert
  #define INT_DISABLE()         __disable_interrupt() ///< Also include <intrinsics.h> or <core_cm0.h> to make this work
  #define INT_ENABLE()          __enable_interrupt()  ///< Also include <intrinsics.h> or <core_cm0.h> to make this work
  #define NOP()                 __no_operation()      ///< Also include <intrinsics.h> or <core_cm0.h> to make this work
                                                      ///< \note NOP() on Cortex is NOT guaranteed to be time consuming !
  /** \brief Storage class for Test Driven Development (TDD) purposes.
   * - Translates to `const` on ARM Cortex-M targets: <b> Place the according data in flash memory </b> and restrict
   *   access to read-only at compile time (when no typecasts are used) as well as by hardware!
   * - On PC/X86 target, this define translates to nothing, which places the according data in ram with full r/w access
   *   (.bss section). Then, unit test routines can modify "normally" constant data for limit tests or such.
   *
   * \warning There is a certain taste of a dirty hack with this define: If no TDD tricks are needed,
   * use keyword `const` to make things clear and to have the same code in the target as well as in TDD environment.
   * With `const` you get read-only access also on X86 as the data is placed in .rdata with the according memory
   * protection flags set and even a typecast to "non-const" doesn't help (the root cause for this define).
   */
  #define FLASH                 const
#endif

#ifdef _X86_                    // Simulation/debug on the PC
  #define ARCH                  X86
  #define ARCH_X86              1                 ///< Microcontroller architecture
  #define STATIC_ASSERT(...)    _Static_assert(__VA_ARGS__) ///< Static assert for gcc
  #define INT_DISABLE()         ///< Currently there's no useful counterpart for this on PC simulation
  #define INT_ENABLE()          ///< Currently there's no useful counterpart for this on PC simulation
  #define NOP()                 ///< There's no useful counterpart for NOP() on PC simulation
  #define FLASH                 ///< Place a struct in r/w memory on PC instead of ro section for TDD purposes
#endif

#ifndef _X86_
  #define ARCH                  XMC1300                                                             ///< Microcontroller architecture, used to include the hardware drivers
  #define ARCH_STM32            1
  #define ARCH_XMC1300          1 // 1: AA-Step
#endif

// #define POLPO_TRANSMIT_TEST

//----------------------------------------------------------------------------------------------------------------------
// Voltage variables (high/mid voltage measurements)

#include <stdint.h>
#include "kernel_parameters.h" //MaxG20140722
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


typedef enum  {         //   generic_state_t
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

#endif
