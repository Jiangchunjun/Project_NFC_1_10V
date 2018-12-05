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
// $Id: kernel_parameters.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/fileTank/kernel_parameters.h $
//
/// \file
/// \brief Parameters of the ballast independ from the used Microcontroller
/// For all parameters related to the used micro, goto/see at ./ARCH/arch_resources.h


/**---------------------------------------------------------------------------------------------------------------------
 * \addtogroup HW
 * \{
 **/

//@todo: braucht man das hier (gibt gcc-Fehlermeldung)? -> verschoben in global.h
//#ifndef ARCH                                                                                        // Set architecture if not already given by debugging/simulation mode
//  #define ARCH                  XMC1300                                                             ///< Microcontroller architecture, used to include the hardware drivers
//  #define ARCH_STM32            1
//  #define ARCH_XMC1300          1 // 1: AA-Step
//#endif

#define SYSCLK_Hz               32000000    //   32000000                                                     ///< [Hz] Cortex M system clock
#define ADC_RESOLUTION          12                                                                  ///< [bits] Resolution of the ADC


// (Independent) Watchdog settings
// watchdog reset after 1/LSI_CLK * WDG_PRESCALER * WDG_RELOAD
#define WDG_ENABLE
#define WDG_PRESCALER           IWDG_Prescaler_4                                                                        // allowed prescaler values -> see stm32_watchdog.h
#define WDG_RELOAD              500              // 200: 20 ms                                                          // Watchdog reload value -> is at every Watchdog_CounterReload() function call



//----------------------------------------------------------------------------------------------------------------------
// Infos for the build system
#define HARDWARE                "GIA"           ///< Build system info: STM32, 55W, Version A
#define HW_OPTION               "_A"            ///< Build system info: No special hardware option
#ifndef RELEASE
 #define RELEASE                "0.0"           ///< Build system info: Major/minor release number
#endif
#ifndef RELEASE_BRANCH
 #define RELEASE_BRANCH         "00"           ///< Build system info: Sub release number (no in branch/tag)
#endif
#define COMPILE_OPT             "000"           ///< Build system info: Compile options


//----------------------------------------------------------------------------------------------------------------------
// Voltage divider measurements

// Voltage divider is 182.200 : 1.000 @ 3,45AVCC -> Resolution = 153.50mV / lsb, Full scale = 3,45V * 182,2 = 628.590V
#define HW_VOLTREF_uV           3450000         ///< [uV]   Voltage of the ADC reference
#define HW_VOLTDIV_TOP          1800            ///< [kOhm] Resistance of the upper string of the voltage dividers
#define HW_VOLTDIV_BOT          10              ///< [kOhm] Resistance of the lower part of the voltage dividers
/** An equivalent simple voltage divider Rtop,Rbottom has to be calculated for the splitted ADC_RAIL/ORC_RAIL voltage divider.
*   The current AND voltage of Rbottom are the same as with R174.
*   @image html parameters_v_rail_divider.jpg "equivalent voltage divider for ADC_RAIL" */
#define HW_VOLTDIV_RAIL_TOP     3716.5          ///< [kOhm] resistance of the equivalent upper resistor of the v_rail voltage divider
#define HW_VOLTDIV_RAIL_BOT     20              ///< [kOhm] resistance of the lowest resistor of the v_rail voltage divider

#define HW_VOLTDIV_LED_TOP     906          ///< [kOhm] resistance of the equivalent upper resistor of the v_rail voltage divider
//ACHTUNG! HW_VOLTDIV_LED_BOT wurde korrigiert vom theoretichen Wert 6.8k auf 7.13k
#define HW_VOLTDIV_LED_BOT     7.0           ///< [kOhm] resistance of the lowest resistor of the v_rail voltage divider

#define HW_VOLTREF_SELV_uV      2560000         ///< [uV]   Voltage of the ADC reference on SELV side

#define HW_VCC_SELV_uV          3454000         ///< [uV]   Vcc on SELV side (for temp sense NTC voltage divider)
                                                ///< 2013-11-26: Released voltage references for SELV side:
                                                ///< - TI  TL431AIDBZR: Vref = 2495 [2470 .. 2520] mV, I ref = 2 uA typ
                                                ///< - NXP TL431AIDBZR: Vref = 2495 [2470 .. 2520] mV, I ref = 2 uA typ
                                                ///< V KA = Vref * (1 + R1/R2) + Iref * R1
                                                ///< R1 = 1k8, R2 = 4k7
                                                ///< Vref = 3.4505 V + 3.6 mV = 3454 mV

#define HW_NTC_RES_VCC          33000           ///< [Ohm]  Resistance of pull-up for NTC on SELV side

//#define DISCOVERY
///\}


/**---------------------------------------------------------------------------------------------------------------------
 * \addtogroup OS
 * \{
 **/
#define SYSTICK_INT_us          400   //Max 141217  10000->400          ///< [us] Interval and resolution of systick interrupt (<1024us, so that no autocalibration of ADC is done --> otherwise wrong ADC offset because of bug ADC_AI.004
#define SYSTICK_INT_Hz          (1000000/SYSTICK_INT_us)  ///< [Hz] Frequency of timebase scheduler

#define MONITOR_STRING_ENCRYPT  1               ///< Encrypt strings output by monitor functions/MONITOR_LED
#define MONITOR_SENDBUFSIZE     1024            ///< Size of output ring buffer for monitor functions/MONITOR_LED

// Limits for output current/power/...
#define CURRENT_LIMIT_A         2.68            ///< Upper limit of all reasonable led currents (measured, setval, ...)
                                                ///< Used for some asserts to ensure overflow prevention. Not to be
                                                ///< confused with OUTPUT_CURRENT_MAX_A, which is considerable lower

//#define DALI                    1               ///< Define if DALI should be integrated. 0: not integrated, 1: integrated
#define DALI                    0               ///< Define if DALI should be integrated. 0: not integrated, 1: integrated

///\} // OS




//#define I_LED_100_PERCENT_mA       1000        //20150317mg: changed from uA to mA
#define I_LED_100_PERCENT_mA       500        //20150317mg: changed from uA to mA



/**---------------------------------------------------------------------------------------------------------------------
 * \addtogroup CORE
 * \{
 **/
#define RESTART_INTERVAL_ms     4000            ///< [s] Interval for the ballast to retry startups in non-
                                                      ///< critical failure condition (open circuit, short circuit, ...)

#define CURRENT_OUTPUT_MAX_A    1.400           ///< [A] Max output current at the LED string
#define CURRENT_OUTPUT_MIN_A    0.006           ///< [A] Min output current at the LED string
#define POWER_OUTPUT_MAX_W      GLOBAL_POWER_OUTPUT_MAX_W     ///< [W] Max output power at the LED string

#define POWER_OFFSET_W          2.2             ///< [W] Average power for self supply of the ballast
#define POWER_STANDBY_W         0.400           ///< [W] Standby power consumption (for smart grid membank)

#define TEMPERATURE_PWR_RED_C   NTC_115C        ///< [°C] Temperature of NTC on SELV side to start power reduction
#define TEMPERATURE_SHUTDOWN_C  NTC_130C        ///< [°C] Temperature of NTC on SELV side for shutdown
#define TEMPERATURE_CUTOFF_s    40              ///< [s]  Time to switchoff if max. power reduction is not
                                                ///<      sufficient to lower the temperature - also used if
                                                ///<      TEMPERATURE_SHUTDOWN_C is exceeded
#define THERMAL_SLEEP_s         (15*60)         ///< [s]  Sleep time after thermal shutdown. <b> Please be aware:</b>
                                                ///<      This time can be circumvented by DALI off/on.

// Constants for Ki calculation
#define THERMAL_KI_TDIFF_HI     NTC_129C        ///< [°C] To calc temperature difference for power reduction.
                                                ///<   Ki (THERMAL_KI_TDIFF_LO) is derived from this. <br>
                                                ///<   Value taken from PTi50 - 22% in 22minutes at delta t = 18°C
                                                ///<   the difference "THERMAL_KI_TDIFF_HI - THERMAL_KI_TDIFF_LO"
                                                ///<   power reduction slope is "PWR_RED_SLOPE" <br>
                                                ///<   !!! \b Nonlinear because temp-diff is dependent on NTC-values.
																								///<   For example diff of NTC_100C and NTC_118C is about twice the diff
   																						  ///<   of NTC_115C and NTC_129C thus the dimming speed will differ by
   																						  ///<   factor 2 also with same diff to TEMPERATURE_PWR_RED_C!
#define THERMAL_KI_TDIFF_LO     NTC_115C        ///< Lower temperature to calc Ki. See \ref THERMAL_KI_TDIFF_HI
#define THERMAL_MIN             50              ///< [%] percentage of nominal power to regulate to by thermal pwr red.
#define PWR_RED_SLOPE           1.01            ///< [%/min] Slope of power reduction at TEMPERATURE_PWR_RED_C + (THERMAL_KI_TDIFF_HI - THERMAL_KI_TDIFF_LO)- 1% will be set to 0,8% (24min for 20%) because of integer conversion - see "Interne_Berechnungen_SSL_Compact - NTC_Stuff" for this

#define PWR_RED_PIC_CALL_PERIOD 320             ///< [ms] calling period of temperature control task - needed for all the timings

// Constants for Ki calculation
#define THERMAL_KP_TDIFF        (NTC_100C-NTC_118C) ///< [°C] Temperature difference to TEMPERATURE_PWR_RED_C for power reduction
#define PWR_RED_SLOPE_KP        0               ///< [%] Step caused by Kp at a temp difference of THERMAL_KP_TDIFF




#define SUPPLYMON_DEFAULTS_230V_50Hz_60Hz_DC
//#include "SupplyMonitorDefaults.h"


#define SLEEP_PFC_BURST_ms      15              ///< [ms] Time for a PFC burst in sleep mode (-> opmodes.c)

///\} // CORE



//Analog Setings
//#define ULINE1_ADC_FACTOR       120804      // (3.45V/4096) * 1e6 * ((4700kR + 33kR) / 33kR) = 120804
//#define ULINE2_ADC_FACTOR       120804      // (3.45V/4096) * 1e6 * ((4700kR + 33kR) / 33kR) = 120804
//#define URAIL_ADC_FACTOR        157500      //
//#define ULED_ADC_FACTOR         113064      // (3.45V/4096) * 1e6 * ((453k + 453k + 6k8) / 6k8) = 113064
//#define IBUCK_ADC_FACTOR        351         // (3.45V/4096) * 1e6 * (1/12)* (1/0.20R) = 351
//#define ULEDSET_ADC_FACTOR      3071        //








//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
// For debugging on the PC, DALI has to be omited ()DALI library doesn't allow this)
#ifdef _X86_
  #undef DALI
  #define DALI 0
#endif

