/******************** (C) COPYRIGHT 2009 - 2012 OSRAM AG ******************************************************************************************************
* File Name       : DaliConfig.h
* Author(s)       : Matthias Schneider (MS), PL-LMS TRT . Jutta Avril (JA), PL-LMS TRT. J. Eisenberg (JE), MUC
* Created by      : 01.01.2009   MS
* Description     : Configuration file for a DaliSlaveDevice (defining the specific hardware+features of the project)
* Modified        : DATE         JA: Defining the settings for the project "DaliLibraryTest_AVR"
* Modified by     : 23.01.2013   JE: adapted to library G3
**************************************************************************************************************************************************************/

//======================================================================================================================
//
//                               T i m i n g
//
//======================================================================================================================

// time after mains-on until cyclic DALI task is called (an AC/DC supply is returned to DALI)
#define DALI_CONFIG_TIMER_CLK_MHz                          DEVICE_PERIPHCLK_MHz

//======================================================================================================================
//
//    D A L I - I n t e r f a c e   H a r d w a r e   A d a p t i o n
//
//======================================================================================================================

//-----   config Rx timer   ---------------------------------------------------
#define DALI_CONFIG_Rx_TIMER				  0	      // CCU40 timer (value has to be between 0 - 3)
#define DALI_CONFIG_RxPIN_P0_6				        // input of timer 0 (TSSOP38)

//-----   config Tx timer   ---------------------------------------------------
#define DALI_CONFIG_Tx_TIMER 			    1	      // CCU40 timer (value has to be between 0 - 3, but different from DALI_CONFIG_Rx_TIMER)
#define DALI_CONFIG_TxPIN_P0_7				        // output of timer 1 (TSSOP38)

//-----   config interrupt service requests   ---------------------------------
// use one of four CCU40 service requests, the lower one has higher priority
#define DALI_CONFIG_CPTR_SRQ				  1	      // (value has to be between 0 - 3)
#define DALI_CONFIG_COMP_SRQ				  0	      // (value has to be between 0 - 3)

//#define DALI_CONFIG_TX_LOWminusHIGH_us                                     (120)  // optional correction of Tx-pattern: Set to 0, then measure Te-time at low level and high level. Change define to the difference

//======================================================================================================================
//
//             D A L I   D e v i c e   +   D e v i c e   T y p e
//
//======================================================================================================================

///<-  Common definitions for all DaliSlaves
#define DALI_CONFIG_DEVICE_TYPE                                                6
#define DALI_CONFIG_PHYSminLEVEL_log                                          (1200)//185//85

///<-  Special definitions for the selected DaliDevice Type (s. DaliDeviceType_FEATURES.h for details)
#define DALI_CONFIG_TYPE6_GEAR_TYPE               (T6_GearType_LED_POWER_SUPPLY_INTEGRATED|T6_GearType_AC_SUPPLY_POSSIBLE)
#define DALI_CONFIG_TYPE6_OPERATING_MODES         (T6_OperatingMode_AM_POSSIBLE|T6_OperatingMode_CONTROL_CURRENT_POSSIBLE)         // PWM & output current controlled                                                                            // .. and select the Features supported. Here: short circuit & thermal shut down (s. DaliDeviceType_FEATURES.h)
#define DALI_CONFIG_TYPE6_FEATURES		            (T6_Feature_OVER_CURRENT_DETECTION|T6_Feature_OPEN_CIRCUIT_DETECTION|T6_Feature_THERMAL_OVERLOAD_RED) // configDeviceTypeX_Features: Replace X by the DeviceType defined ..

// define to activate FastFadeTime capability (only for device type 6 - LED)
#define DALI_CONFIG_TYPE6_MIN_FastFADE_TIME                                   20                                        // Define a number in the range 1..27. If not defined: automatically set to 1. If no number defined: compiler error.

#define DALI_FastTIME_TICK_ms                   2.5
#ifdef DALI_INVERT
#define DALI_CONFIG_TX_INVERT 
#define DALI_CONFIG_RX_INVERT
#endif