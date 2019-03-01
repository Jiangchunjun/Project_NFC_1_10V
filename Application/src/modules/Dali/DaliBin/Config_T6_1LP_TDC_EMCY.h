///-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file Config_T6_1LP_TDC_EMCY.h
///   \since 2016-06-20
///   \brief Definitions to configure the DaliLib.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

// Definition of fixed properties of this DaliLib-Choice:
#define GLOBAL_FastTIME_TICK_ms             (2.500)                             // Basic system time slice in [ms] for all modules, integrated in DaliLib
#define GLOBAL_SlowTIME_TICK_ms             (10.0 * GLOBAL_FastTIME_TICK_ms)    // Slow system time slice in [ms] for all modules, integrated in DaliLib

#define DEVICE_CHANNELS                     1                                   // Number of logical control gear units in the bus unit (optional: default = 1). Value canbe queried by MemoryBank 0, address 0x19.
#define OUTPUT_CHANNELS                     DEVICE_CHANNELS                     // Number of lamps (identical to logical control gear units in case of  DaliDeviceType 6)

#define DALI_CONFIG_DEVICE_TYPE             6                                   // Define the DaliDevice type by its number
  #define DALI_CONFIG_TYPE6_MIN_FastFADE_TIME_DEFAULT     27                    // set as default, if DaliServices_GetMinFastFadeTime() is not defined

#define DEVICE_TOUCHDIM                                                         // TouchDim_MPC is initialised by default values as defined in the spec.

#define DALI_CONFIG_MEMORY_BANK1_LastADDRESS  60
#define MPC_EMERGENCY_LIGHT_DC              185                                 // Sets the Dali Level in case of DC-supply and by this enables the emergency function in the code!
#define MPC_EMERGENCY_CONFIGURATION         0

