//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   MemoryBank_DaliParams.h
///   \since  2017-07-19
///   \brief  Declaration of the interface to access MemoryBank "DaliParams". Definition of pseudo-address to access Dali parameters
///           (Dali parameters of Dali-standard IEC 62386-102, IEC 62386-207 and IEC 62386-209 (Tc only))
///           Note: MPC_DALI_LENGTH differs with DaliLib-configuration. It is determined based on #ifdefs needed to include/exclude parameters
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#define MPC_DALI_PARAMS_OFFSET    5                                             // Offset of MemoryBank-address related to Mpc-address. Valid for all MemoryBanks with DaliParams

#define MPC_DaliT6_VERSION        1                                             // new MemoryBank for Dali parameter 102 + 207
#define MPC_DaliT6_ID             40
// #define MPC_DALI_T6_LENGTH    (MPC_DALI_FastFadeTIME +1)                     // defined at the end of the enum

#define MPC_DaliT8_TW_VERSION     1                                             // "Version" of enum of Dali Params for TunableWhite  1: MemoryBank for TunableWhite added.
#define MPC_DaliT8_TW_ID          41
// #define MPC_DALI_TW_LENGTH    (MPC_DALI8_TC_WARMEST_L +1)                    // defined at the end of the enum

#define MPC_DaliT8_XY_VERSION     1                                             // "Version" of enum of Dali Params for xy-coordinate 1: MemoryBank for xy added.
#define MPC_DaliT8_XY_ID          42
// #define MPC_DALI_XY_LENGTH                                                   // defined at the end of the enum

// Interface to the Dali parameters, structured as MPC. No init or reset allowed!
mpc_rw_enum MemoryBank_DaliT6_GetValue( uint8_t address, uint8_t *pValue, uint8_t channel );
mpc_rw_enum MemoryBank_DaliT6_SetValue( uint8_t address, uint8_t value, uint8_t channel );

typedef enum {
  MPC_DALI_PowerOnLEVEL =   0,
  MPC_DALI_FailureLEVEL =   1,
  MPC_DALI_MinLEVEL =       2,
  MPC_DALI_MaxLEVEL =       3,
  MPC_DALI_FadeRATE =       4,
  MPC_DALI_FadeTIME =       5,
  MPC_DALI_EXTFadeTIME =    6,
  MPC_DALI_LowGROUP =       7,
  MPC_DALI_HighGROUP =      8,
  MPC_DALI_SCENE0 =         9,
  MPC_DALI_SCENE1 =        10,
  MPC_DALI_SCENE2 =        11,
  MPC_DALI_SCENE3 =        12,
  MPC_DALI_SCENE4 =        13,
  MPC_DALI_SCENE5 =        14,
  MPC_DALI_SCENE6 =        15,
  MPC_DALI_SCENE7 =        16,
  MPC_DALI_SCENE8 =        17,
  MPC_DALI_SCENE9 =        18,
  MPC_DALI_SCENE10 =       19,
  MPC_DALI_SCENE11 =       20,
  MPC_DALI_SCENE12 =       21,
  MPC_DALI_SCENE13 =       22,
  MPC_DALI_SCENE14 =       23,
  MPC_DALI_SCENE15 =       24,
  MPC_DALI_ADDRESS =       25,
  MPC_DALI_OPERATINGMODE = 26,
#if ( (DALI_CONFIG_DEVICE_TYPE == 6) || (DALI_CONFIG_DEVICE_1stTYPE == 6) )
  MPC_DALI_CURVE =         27,                                                  // only supported by DaliDeviceType 6
  MPC_DALI_FastFadeTIME =  28                                                   // only supported by DaliDeviceType 6
#endif
} mpc_dali_enum;
#define MPC_DALI_T6_LENGTH    (MPC_DALI_FastFadeTIME +1)


//#if (DALI_CONFIG_DEVICE_2ndTYPE == 8)                                           // LED-properties and default settings for PowerOn-Colour, SystemFailure-Colour
mpc_rw_enum MemoryBank_DaliT8_TW_GetValue( uint8_t address, uint8_t *pValue, uint8_t channel );
mpc_rw_enum MemoryBank_DaliT8_TW_SetValue( uint8_t address, uint8_t value, uint8_t channel );

typedef enum {
  MPC_DALI8_TYPE                = 0,                                            // ColourType, valid for all colours! Upon change of the ColourType, all colours are reset to their default values!

  MPC_DALI8_SCENE0_VALUE_H      = 1,                                            // Start of 16bit-values always with odd number!!! First common "addresses" for all types..
  MPC_DALI8_SCENE0_VALUE_L      = 2,                                            // .. In case of colourType "xy", these values (..__VALUE_H and .._VALUE_L)
  MPC_DALI8_SCENE1_VALUE_H      = 3,                                            // .. define the x-value "address", the "address of the y-value is defined below.
  MPC_DALI8_SCENE1_VALUE_L      = 4,
  MPC_DALI8_SCENE2_VALUE_H      = 5,
  MPC_DALI8_SCENE2_VALUE_L      = 5,
  MPC_DALI8_SCENE3_VALUE_H      = 7,
  MPC_DALI8_SCENE3_VALUE_L      = 8,
  MPC_DALI8_SCENE4_VALUE_H      = 9,
  MPC_DALI8_SCENE4_VALUE_L      = 10,
  MPC_DALI8_SCENE5_VALUE_H      = 11,
  MPC_DALI8_SCENE5_VALUE_L      = 12,
  MPC_DALI8_SCENE6_VALUE_H      = 13,
  MPC_DALI8_SCENE6_VALUE_L      = 14,
  MPC_DALI8_SCENE7_VALUE_H      = 15,
  MPC_DALI8_SCENE7_VALUE_L      = 16,
  MPC_DALI8_SCENE8_VALUE_H      = 17,
  MPC_DALI8_SCENE8_VALUE_L      = 18,
  MPC_DALI8_SCENE9_VALUE_H      = 19,
  MPC_DALI8_SCENE9_VALUE_L      = 20,
  MPC_DALI8_SCENE10_VALUE_H     = 21,
  MPC_DALI8_SCENE10_VALUE_L     = 22,
  MPC_DALI8_SCENE11_VALUE_H     = 23,
  MPC_DALI8_SCENE11_VALUE_L     = 24,
  MPC_DALI8_SCENE12_VALUE_H     = 25,
  MPC_DALI8_SCENE12_VALUE_L     = 26,
  MPC_DALI8_SCENE13_VALUE_H     = 27,
  MPC_DALI8_SCENE13_VALUE_L     = 28,
  MPC_DALI8_SCENE14_VALUE_H     = 29,
  MPC_DALI8_SCENE14_VALUE_L     = 30,
  MPC_DALI8_SCENE15_VALUE_H     = 31,
  MPC_DALI8_SCENE15_VALUE_L     = 32,

  MPC_DALI8_POWER_ON_VALUE_H    = 33,
  MPC_DALI8_POWER_ON_VALUE_L    = 34,

  MPC_DALI8_FAILURE_VALUE_H     = 35,
  MPC_DALI8_FAILURE_VALUE_L     = 36

, MPC_DALI8_TC_PHYS_COOLEST_H   = 37,
  MPC_DALI8_TC_PHYS_COOLEST_L   = 38,
  MPC_DALI8_TC_PHYS_WARMEST_H   = 39,
  MPC_DALI8_TC_PHYS_WARMEST_L   = 40,
  MPC_DALI8_TC_COOLEST_H        = 41,
  MPC_DALI8_TC_COOLEST_L        = 42,
  MPC_DALI8_TC_WARMEST_H        = 43,
  MPC_DALI8_TC_WARMEST_L        = 44
} mpc_daliT8_TW_enum;
#define MPC_DALI_TW_LENGTH    (MPC_DALI8_TC_WARMEST_L +1)

#ifdef nix  // best implementation not decided yet

//#if (DALI_CONFIG_T8_TYPES & (COLOUR_TYPE_XY || DALI_T8_TYPE_PRIMARY) )        // In case of xy-Coordinate , these may be updated
  MPC_DALI8_PRIMAYRY1_X_H       = (1 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY1_X_L       = (2 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY1_Y_H       = (3 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY1_Y_L       = (4 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY1_H      = (5 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY1_L      = (6 + MPC_DALI8_END1)

  MPC_DALI8_PRIMAYRY2_X_H       = (7 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY2_X_L       = (8 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY2_Y_H       = (9 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY2_Y_L       = (10 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY2_H      = (11 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY2_L      = (12 + MPC_DALI8_END1)
    #if (OUTPUT_CHANNELS >= 3)
  MPC_DALI8_PRIMAYRY3_X_H       = (13 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY3_X_L       = (14 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY3_Y_H       = (15 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY3_Y_L       = (16 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY3_H      = (17 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY3_L      = (18 + MPC_DALI8_END1)
    #else
      #define MPC_DALI8_END2      MPC_DALI8_TY_PRIMAYRY2_L
    #endif
    #if (OUTPUT_CHANNELS >= 4)
  MPC_DALI8_PRIMAYRY4_X_H       = (19 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY4_X_L       = (20 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY4_Y_H       = (21 + MPC_DALI8_END1)
  MPC_DALI8_PRIMAYRY4_Y_L       = (22 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY4_H      = (23 + MPC_DALI8_END1)
  MPC_DALI8_TY_PRIMAYRY4_L      = (24 + MPC_DALI8_END1)
       #define MPC_DALI8_END2     MPC_DALI8_TY_PRIMAYRY4_L
   #else
      #define MPC_DALI8_END2      MPC_DALI8_TY_PRIMAYRY3_L
    #endif
    #ifdef (DALI_CONFIG_T8_TYPES & COLOUR_TYPE_XY)
  MPC_DALI8_SCENE0_VALUE_Y_H      = (1 + MPC_DALI8_END2),                                           // Start of 16bit-values! First common "addresses" for all types..
  MPC_DALI8_SCENE0_VALUE_Y_L      = (2 + MPC_DALI8_END2),                                           // .. In case of colourType "xy", these values (..__VALUE_H and .._VALUE_L)
  MPC_DALI8_SCENE1_VALUE_Y_H      = (3 + MPC_DALI8_END2),                                           // .. define the x-value "address", the "address of the y-value is defined below.
  MPC_DALI8_SCENE1_VALUE_Y_L      = (4 + MPC_DALI8_END2),
  MPC_DALI8_SCENE2_VALUE_Y_H      = (5 + MPC_DALI8_END2),
  MPC_DALI8_SCENE2_VALUE_Y_L      = (6 + MPC_DALI8_END2),
  MPC_DALI8_SCENE3_VALUE_Y_H      = (7 + MPC_DALI8_END2),
  MPC_DALI8_SCENE3_VALUE_Y_L      = (8 + MPC_DALI8_END2),
  MPC_DALI8_SCENE4_VALUE_Y_H      = (9 + MPC_DALI8_END2),
  MPC_DALI8_SCENE4_VALUE_Y_L      = (10 + MPC_DALI8_END2),
  MPC_DALI8_SCENE5_VALUE_Y_H      = (11 + MPC_DALI8_END2),
  MPC_DALI8_SCENE5_VALUE_Y_L      = (12 + MPC_DALI8_END2),
  MPC_DALI8_SCENE6_VALUE_Y_H      = (13 + MPC_DALI8_END2),
  MPC_DALI8_SCENE6_VALUE_Y_L      = (14 + MPC_DALI8_END2),
  MPC_DALI8_SCENE7_VALUE_Y_H      = (15 + MPC_DALI8_END2),
  MPC_DALI8_SCENE7_VALUE_Y_L      = (16 + MPC_DALI8_END2),
  MPC_DALI8_SCENE8_VALUE_Y_H      = (17 + MPC_DALI8_END2),
  MPC_DALI8_SCENE8_VALUE_Y_L      = (18 + MPC_DALI8_END2),
  MPC_DALI8_SCENE9_VALUE_Y_H      = (19 + MPC_DALI8_END2),
  MPC_DALI8_SCENE9_VALUE_Y_L      = (20 + MPC_DALI8_END2),
  MPC_DALI8_SCENE10_VALUE_Y_H     = (21 + MPC_DALI8_END2),
  MPC_DALI8_SCENE10_VALUE_Y_L     = (22 + MPC_DALI8_END2),
  MPC_DALI8_SCENE11_VALUE_Y_H     = (23 + MPC_DALI8_END2),
  MPC_DALI8_SCENE11_VALUE_Y_L     = (24 + MPC_DALI8_END2),
  MPC_DALI8_SCENE12_VALUE_Y_H     = (25 + MPC_DALI8_END2),
  MPC_DALI8_SCENE12_VALUE_Y_L     = (26 + MPC_DALI8_END2),
  MPC_DALI8_SCENE13_VALUE_Y_H     = (27 + MPC_DALI8_END2),
  MPC_DALI8_SCENE13_VALUE_Y_L     = (28 + MPC_DALI8_END2),
  MPC_DALI8_SCENE14_VALUE_Y_H     = (29 + MPC_DALI8_END2),
  MPC_DALI8_SCENE14_VALUE_Y_L     = (30 + MPC_DALI8_END2),
  MPC_DALI8_SCENE15_VALUE_Y_H     = (31 + MPC_DALI8_END2),
  MPC_DALI8_SCENE15_VALUE_Y_L     = (32 + MPC_DALI8_END2)
} mpc_daliT8_XY_enum;

//#define MPC_DALI_LENGTH 199

//#if (DALI_CONFIG_DEVICE_TYPE == 6)
//  #define MPC_DALI_LENGTH   (int8_t)(MPC_DALI_FastFadeTIME+1)
//#elif ((DALI_CONFIG_DEVICE_TYPE == 255) && (DALI_CONFIG_DEVICE_1stTYPE == 6))
//  #if (DALI_CONFIG_DEVICE_2ndTYPE == 8)
//    #define MPC_DALI_LENGTH   ((int8_t)MPC_DALI8_TC_PHYS_WARMEST_L+1)
//    #warning "Not supported yet: DaliDevice_Mpc for DaliType 8, xy!"
 #endif
#endif

