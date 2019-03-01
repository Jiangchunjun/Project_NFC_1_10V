//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   MemoryBank_CLM.h
///   \since  2012-11-12
///   \brief  Default definitions and interface to MultiPurposeCenter ConstantLumenOutput (CLM). MPC-ID = 6. Specified by 2D1 2786696-03
///           The default definitions are only used, if MPC_CLM_USE_DEFAULT_SETTINGS is defined in Config.h (or sub-header)
///           Note: The code supports MPC_CLM_TABLE_ENTRIES of 4 or 8 only.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is confidential and not intended for public release.
///   All rights reserved. Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

///-> Default settings, used if MPC_CLM_DEFAULT_SETTINGS is defined by the user. Otherwise the user has to define these values himself.
#ifdef MPC_CLM_USE_DEFAULT_SETTINGS
  #define MPC_CLM_TABLE_ENTRIES                  8                              // number of table entries (4 or 8)
  #define MPC_CLM_TABLE_MaxLEVEL                 100                            // max Constant lumen factor in [%]
  #define MPC_CLM_TABLE_MinLEVEL                 70                             // min Constant lumen factor in [%]
  //#define MPC_CLM_TABLE_ENABLE                                                // if defined, CLM is enabled after flashing
  #define MPC_CLM_TABLE_0_kHOURS                 0                              // lifetime n[0] in [khours]
  #define MPC_CLM_TABLE_0_FACTOR                 70                             // CLM factor n[0] in [%]
  #define MPC_CLM_TABLE_1_kHOURS                 50                             // lifetime n[1] in [khours]
  #define MPC_CLM_TABLE_1_FACTOR                 100                            // CLM factor n[1] in [%]
  #define MPC_CLM_TABLE_2_kHOURS                 255                            // lifetime n[2] in [khours], 255 = end of table
  #define MPC_CLM_TABLE_2_FACTOR                 255                            // CLM factor n[2] in [%], 255 = end of table
  #define MPC_CLM_TABLE_3_kHOURS                 255                            // lifetime n[3] in [khours], 255 = end of table
  #define MPC_CLM_TABLE_3_FACTOR                 255                            // CLM factor n[3] in [%], 255 = end of table
  #define MPC_CLM_TABLE_4_kHOURS                 255                            // this and following definitions: only used, if MPC_CLM_TABLE_ENTRIES > 4
  #define MPC_CLM_TABLE_4_FACTOR                 255                            // ...
  #define MPC_CLM_TABLE_5_kHOURS                 255
  #define MPC_CLM_TABLE_5_FACTOR                 255
  #define MPC_CLM_TABLE_6_kHOURS                 255
  #define MPC_CLM_TABLE_6_FACTOR                 255
  #define MPC_CLM_TABLE_7_kHOURS                 255
  #define MPC_CLM_TABLE_7_FACTOR                 255
#endif

///-> Fixed definitions, which must not be changed
#define MPC_ConstantLumen_ID                    6
#define MPC_ConstantLumen_VERSION               3                               // Rev. 3 since 2016-06
#define MPC_ConstantLumen_LastADDRESS           (5+2*MPC_CLM_TABLE_ENTRIES)

// ---   M e m o r y B a n k   I n t e r f a c e
void MemoryBank_Clm_Init( uint32_t runTime_mins, uint8_t channel );
void MemoryBank_Clm_Reset( uint8_t channel );
mpc_rw_enum MemoryBank_Clm_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Clm_SetValue( uint8_t address, uint8_t value, uint8_t channel );

void MemoryBank_Clm_FactoryReset( void );

void MemoryBank_Clm_UpdateAdjustFactor(uint32_t runTime_mins, uint8_t channel); // called from Info-Page

// ---   U s e r   I n t e r f a c e
#define MPC_CLM_100PERCENT_FACTOR     0x4000                                    // Output range up to 150 %!
uint16_t ConstantLumen_GetFactor(uint8_t channel);                              // Returns the pre-calculated CLM-factor, if CLM is enabled. Else returns MPC_CLM_100PERCENT_FACTOR = 100 %.
