/******************** (C) COPYRIGHT 2015 OSRAM AG *********************************************************************************
* File Name         : ConfigMpcDefault.h
* Description       : Memory Bank default settings
* Author(s)         : Template: Jutta Avril (JA), PL-LMS TRT
* Created           : 12.12.2012 (JA)
* Modified by       : 05.10.2015 (JE)
**********************************************************************************************************************************/

#define MPC_MAX_NUMBER                                                        31

//======================================================================================================================
//
//                     M e m o r y   B a n k
//
//======================================================================================================================
#define MPC_CSM_NUMBER                                                         3
  #define MPC_CSM_LEDset2                                                                                               // if defined, LEDset2 interface is provided
  #define MPC_CSM_MODE                                                      0x01                                        // LEDset enabled as factory default, no other mode is used
  #define MPC_CSM_DEFAULT_CURRENT                                           I_LED_MIN                                        // LED current default value if LEDset is disabled [in mA]

#define  MPC_ASTRO_NUMBER                                                      30

#define MPC_CLM_NUMBER                                                         4                                        // Memory Bank Number for Constant Lumen Module
  #define MPC_CLM_TABLE_ENTRIES                                                4                                        // number of table entries
  #define MPC_CLM_TABLE_MaxLEVEL                                             100                                        // max Constant lumen factor in [%]
  #define MPC_CLM_TABLE_MinLEVEL                                              70                                        // min Constant lumen factor in [%]
//  #define MPC_CLM_TABLE_ENABLE                                                                                        // if #define is enabled, CLM is enabled per default
  #define MPC_CLM_TABLE_0_kHOURS                                               0                                        // lifetime n[0] in [khours]
  #define MPC_CLM_TABLE_0_FACTOR                                              70                                        // CLM factor n[0] in [%]
  #define MPC_CLM_TABLE_1_kHOURS                                              50                                        // lifetime n[1] in [khours]
  #define MPC_CLM_TABLE_1_FACTOR                                             100                                        // CLM factor n[1] in [%]
  #define MPC_CLM_TABLE_2_kHOURS                                             255                                        // lifetime n[2] in [khours], 255 = end of table
  #define MPC_CLM_TABLE_2_FACTOR                                             255                                        // CLM factor n[2] in [%], 255 = end of table
  #define MPC_CLM_TABLE_3_kHOURS                                             255                                        // lifetime n[3] in [khours], 255 = end of table
  #define MPC_CLM_TABLE_3_FACTOR                                             255                                        // CLM factor n[3] in [%], 255 = end of table

///---------------------------------------------------------------------------------------------------------------------
#define MPC_PASSWORD1_NUMBER                                                  16                                        // Memory Bank Number for MPC_PASSWORD1
#define MPC_PASSWORD1_COPY_NUMBER                                             6                                         //Only for the version use

///---------------------------------------------------------------------------------------------------------------------
#define MPC_EMERGENCY_NUMBER                                                  17                                        // Memory Bank Number for Emergency Page
  #define MPC_EMERGENCY_LIGHT_DC                                             185                                        // 0 ... 254: light level at DC operation, 255: light level at DC = light level AC
  #define MPC_EMERGENCY_CONFIGURATION                                          0                                        //

///---------------------------------------------------------------------------------------------------------------------
#define MPC_PASSWORD2_NUMBER                                                  28                                        // Memory Bank Number for MPC_PASSWORD2
#define MPC_PASSWORD2_COPY_NUMBER                                             7                                         //Only for the version use

///---------------------------------------------------------------------------------------------------------------------
#define MPC_INFO_NUMBER                                                       29                                        // Memory Bank Number for Info Page
  // always enabled: #define MPC_INFO_OPERATION_COUNTER                                                                 // optional parts of MemoryBank_Info
  // not supported yet:
    // #define configInfoTempMonitoring                                                                                 // optional parts of MemoryBank_Info
    // #define configInfoLineOvervoltage                                                                                // optional parts of MemoryBank_Info

///---------------------------------------------------------------------------------------------------------------------
#define MPC_POWER_NUMBER                                                      30                                        // Memory Bank Number for Power Info Page
  /// Define how the Application generates the power consumption value (Power Info Byte of "Power Consumption MPC")
  /// bit 1: 1 - Operation power is measured, 0 - Operation power is estimated
  /// bit 0: 1 - Stand by power is measured, 0 - Stand by power is estimated
  #define MPC_POWER_INFO_BYTE                                                  2

#define MPC_EOL_NUMBER                                                        15
///---------------------------------------------------------------------------------------------------------------------
#define MPC_TDC_NUMBER                                                        31                                        // Memory Bank Number for TouchDIM / Corridor


///---------------------------------------------------------------------------------------------------------------------
#ifndef NDEBUG
#define DALI_CONFIG_MPC_DEBUG                                                255                                        // Memory bank number for Debug MPC -> always 255
#endif
