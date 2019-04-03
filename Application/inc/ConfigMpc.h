/******************** (C) COPYRIGHT 2015 OSRAM AG *********************************************************************************
* File Name         : ConfigMpcDefault.h
* Description       : Memory Bank default settings
* Author(s)         : Template: Jutta Avril (JA), PL-LMS TRT
* Created           : 12.12.2012 (JA)
* Modified by       : 05.10.2015 (JE)
**********************************************************************************************************************************/
#include "parameter.h"

///---------------------------------------------------------------------------------------------------------------------
#define MPC_CLM_TABLE_ENTRIES                         4                         // number of table entries
#define MPC_CLM_TABLE_MaxLEVEL                        100                       // max Constant lumen factor in [%]
#define MPC_CLM_TABLE_MinLEVEL                        70                        // min Constant lumen factor in [%]
//  #define MPC_CLM_TABLE_ENABLE                                                // if #define is enabled, CLM is enabled per default
#define MPC_CLM_TABLE_0_kHOURS                        0                         // lifetime n[0] in [khours]
#define MPC_CLM_TABLE_0_FACTOR                        70                        // CLM factor n[0] in [%]
#define MPC_CLM_TABLE_1_kHOURS                        50                        // lifetime n[1] in [khours]
#define MPC_CLM_TABLE_1_FACTOR                        100                       // CLM factor n[1] in [%]
#define MPC_CLM_TABLE_2_kHOURS                        255                       // lifetime n[2] in [khours], 255 = end of table
#define MPC_CLM_TABLE_2_FACTOR                        255                       // CLM factor n[2] in [%], 255 = end of table
#define MPC_CLM_TABLE_3_kHOURS                        255                       // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_3_FACTOR                        255                       // CLM factor n[3] in [%], 255 = end of table
#define MPC_CLM_TABLE_4_kHOURS                        255                       // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_4_FACTOR                        255                       // CLM factor n[3] in [%], 255 = end of table
#define MPC_CLM_TABLE_5_kHOURS                        255                       // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_5_FACTOR                        255                       // CLM factor n[3] in [%], 255 = end of table
#define MPC_CLM_TABLE_6_kHOURS                        255                       // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_6_FACTOR                        255                       // CLM factor n[3] in [%], 255 = end of table
#define MPC_CLM_TABLE_7_kHOURS                        255                       // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_7_FACTOR                        255                       // CLM factor n[3] in [%], 255 = end of table

///---------------------------------------------------------------------------------------------------------------------
#define MPC_EMERGENCY_LIGHT_DC                        185                       // 0 ... 254: light level at DC operation, 255: light level at DC = light level AC
#define MPC_EMERGENCY_CONFIGURATION                   0

///---------------------------------------------------------------------------------------------------------------------
#define MPC_POWER_INFO_BYTE                           2

#define MPC_CSM_LEDset2                                                         // if defined, LEDset2 interface is provided
#define MPC_CSM_MODE                            1                               // if defined, LEDset2 interface is enabled per default
#define MPC_CSM_DEFAULT_CURRENT                 0X578                           //1500mA

//// number of MPC instances in NFC tag
//#define MPC_NFC_INST                                 (MPC_MSK_NFC_INSTANCES + \
//                                                      MPC_BIO_NFC_INSTANCES + \
//                                                      MPC_CLM_NFC_INSTANCES + \
//                                                      MPC_EL_NFC_INSTANCES + \
//                                                      MPC_INFO_NFC_INSTANCES + \
//                                                      MPC_POWER_INFO_NFC_INSTANCES + \
//                                                      MPC_TD_NFC_INSTANCES + \
//                                                      MPC_DGUARD_NFC_INSTANCES + \
//                                                      MPC_MAC_NFC_INSTANCES + \
//                                                      MPC_DALI_MB0_NFC_INSTANCES + \
//                                                      MPC_DALI_MB1_NFC_INSTANCES + \
//                                                      MPC_DALI_T6_NFC_INSTANCES + \
//                                                      MPC_DALI_T8_TW_NFC_INSTANCES + \
//                                                      MPC_TW_NFC_INSTANCES )
//
//
//#if DEVICE_CHANNELS == 1
//  STATIC_ASSERT(( MPC_NFC_INST == MPC_COUNT ),    "MPC_NFC_INST not equal to MPC_COUNT");
//#endif

#define MPC_MAX_NUMBER                         50   // Highest membank number in the DALI address room
#define MPC_COUNT                              10   // Number of Mpcs allocated
#define MPC_NFC_INST                           10

#define MPCs_DALI   // Numbers fix, only to create and include the DALI membanks 0 and 1 and DALI parameter page(s)
#define MPC_MSK_NUMBER                          2
#define MPC_CSM_NUMBER                          3
#define MPC_CLM_NUMBER                          4
#define MPC_BIO_NUMBER                          6
//#define MPC_EMERGENCY_NUMBER                   17
#define MPC_EOL_NUMBER                         21   // read-only in 1DIM
#define MPC_ASTRO_NUMBER                       28
#define MPC_INFO_NUMBER                        29
#define MPC_O2T_NUMBER                         34

//#define MPC_POWER_NUMBER                       30
//#define MPC_TDC_NUMBER                         31
//#define MPC_DALI_T6_NUMBER                     40

#define configMemoryBank_Astro_opMode                                     0x00    // OFF

#define configMemoryBank_Astro_startUp_fade                                  0 // start up
#define configMemoryBank_Astro_astro_fade                              (180/2)
#define configMemoryBank_Astro_switchOff_fade                              255 // disable s.down
#define configMemoryBank_Astro_start_time                                  120
#define configMemoryBank_Astro_time1                                       120
#define configMemoryBank_Astro_time2                                       120
#define configMemoryBank_Astro_time3                                       120
#define configMemoryBank_Astro_nominal_level                               100 //start up
#define configMemoryBank_Astro_level1                                       50
#define configMemoryBank_Astro_level2                                       50
#define configMemoryBank_Astro_level3                                       50
#define configMemoryBank_Astro_level4                                      100

#define configMemoryBank_Eol_enable                                          1  // off
#define configMemoryBank_Eol_eol_time                                       50


#define DALI_CONFIG_MEMORY_BANK1_LastADDRESS    60


