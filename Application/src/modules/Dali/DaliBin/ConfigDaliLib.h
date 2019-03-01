/******************** (C) COPYRIGHT 2016 OSRAM AG **********************************************************************************
* File Name         : ConfigDaliLib.h for T6_1LP_OUTD_EMCY
* Description       : Individual #defines to provide a choice of DaliLibraries
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 20.06.2016 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
******************************************************************************************************************************************/

// Definition of fixed properties of this DaliLib-Choice:
#define DEVICE_CHANNELS                           1                             // Max number of logical control gear units in the bus unit (optional: default = 1). Value canbe queried by MemoryBank 0, address 0x19.
#define DEVICE_OUTDOOR
  #define	OUTDOOR_POWER_OFF_TIME_ms               (600)
  #define DALI_CONFIG_DEFAULT_OPERATINGMODE_OUTDOOR                             // If defined: The Outdoor-device starts in Outdoor-Mode after flashing

#define DALI_CONFIG_DEVICE_TYPE                   6                             // Define the DaliDevice type by its number
  #define DALI_CONFIG_TYPE6_MIN_FastFADE_TIME_DEFAULT     27                    // set as default, if DaliServices_GetMinFastFadeTime() is not defined
  
#define MPC_EMERGENCY_LIGHT_DC                                             255
#define MPC_EMERGENCY_CONFIGURATION                                       0x00
  
// Definition of optional properties:
// Code is only included into the hex-file, if any function in MemoryBank_Password.h is called from device-specific code

#define MPC_CLM_TABLE_ENTRIES                                                8                                        // number of table entries
#define MPC_CLM_TABLE_MaxLEVEL                                             150                                        // max Constant lumen factor in [%]
#define MPC_CLM_TABLE_MinLEVEL                                              50                                        // min Constant lumen factor in [%]
//  #define MPC_CLM_TABLE_ENABLE                                                                                      // if #define is enabled, CLM is enabled per default
#define MPC_CLM_TABLE_0_kHOURS                                               0                                        // lifetime n[0] in [khours]
#define MPC_CLM_TABLE_0_FACTOR                                             100                                        // CLM factor n[0] in [%]
#define MPC_CLM_TABLE_1_kHOURS                                             255                                        // lifetime n[1] in [khours]
#define MPC_CLM_TABLE_1_FACTOR                                             255                                        // CLM factor n[1] in [%]
#define MPC_CLM_TABLE_2_kHOURS                                             255                                        // lifetime n[2] in [khours], 255 = end of table
#define MPC_CLM_TABLE_2_FACTOR                                             255                                        // CLM factor n[2] in [%], 255 = end of table
#define MPC_CLM_TABLE_3_kHOURS                                             255                                        // lifetime n[3] in [khours], 255 = end of table
#define MPC_CLM_TABLE_3_FACTOR                                             255                                        // CLM factor n[3] in [%], 255 = end of table
#define MPC_CLM_TABLE_4_kHOURS                                             255
#define MPC_CLM_TABLE_4_FACTOR                                             255
#define MPC_CLM_TABLE_5_kHOURS                                             255
#define MPC_CLM_TABLE_5_FACTOR                                             255
#define MPC_CLM_TABLE_6_kHOURS                                             255
#define MPC_CLM_TABLE_6_FACTOR                                             255
#define MPC_CLM_TABLE_7_kHOURS                                             255
#define MPC_CLM_TABLE_7_FACTOR                                             255

// Code is only included into the hex-file, if any function in MemoryBank_Password.h is called from device-specific code
#define PASSWORD_BANKS                            2                             // always 2 Password-Mpc are available.
