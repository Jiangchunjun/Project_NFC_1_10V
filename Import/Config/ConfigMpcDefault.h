/******************** (C) COPYRIGHT 2015 OSRAM AG *********************************************************************************
* File Name         : ConfigMpcDefault.h
* Description       : Memory Bank default settings
* Author(s)         : Template: Jutta Avril (JA), PL-LMS TRT
* Created           : 12.12.2012 (JA)
* Modified by       : 16.01.2015 (JE)
**********************************************************************************************************************************/


//=============================================================================
//
//                     M e m o r y   B a n k
//
//=============================================================================
#ifdef DALI_CONFIG_MPC_CLM
  #define MPC_CLM_NUMBER                          DALI_CONFIG_MPC_CLM
#else
  // #define MPC_CLM_NUMBER           // if not a DaliDevice, define the number here
#endif
#ifdef MPC_CLM_NUMBER
  #define MPC_CLM_TABLE_ENTRIES                   4                             // number of table entries
  #define MPC_CLM_TABLE_MaxLEVEL                  100                           // max light level in percent
  #define MPC_CLM_TABLE_MinLEVEL                  70                            // min light level in percent
// #define MPC_CLM_TABLE_ENABLE                                                 // if defined, CLM is enabled per default!
  #define MPC_CLM_TABLE_0_kHOURS                  0
  #define MPC_CLM_TABLE_0_FACTOR                  70
  #define MPC_CLM_TABLE_1_kHOURS                  50
  #define MPC_CLM_TABLE_1_FACTOR                  100
  #define MPC_CLM_TABLE_2_kHOURS                  255
  #define MPC_CLM_TABLE_2_FACTOR                  255
  #define MPC_CLM_TABLE_3_kHOURS                  255
  #define MPC_CLM_TABLE_3_FACTOR                  255
#endif


///-----------------------------------------------------------------------------
#ifdef DALI_CONFIG_MPC_EMERGENCY
  #define MPC_EMERGENCY_NUMBER                    DALI_CONFIG_MPC_EMERGENCY
#else
  // #define MPC_EMERGENCY_NUMBER                                               /// Define the bank number and enable the "emergency page" Multi Purpose Center
#endif
#ifdef MPC_EMERGENCY_NUMBER
  #define MPC_EMERGENCY_LIGHT_DC                  185                           // DALI level at DC operation. If 255, light level equals light level at AC supply
  #define MPC_EMERGENCY_CONFIGURATION             0
#endif

///-----------------------------------------------------------------------------
#ifdef DALI_CONFIG_MPC_INFO
  #define MPC_INFO_NUMBER                         DALI_CONFIG_MPC_INFO
#else
  // #define MPC_INFO_NUMBER                                                    /// \brief Define the memory page number to enable the "info page" memory bank
#endif
#ifdef MPC_INFO_NUMBER
  #define configInfoLampOperationCounter                                        // optional parts of MemoryBank_Info
  // #define configInfoTempMonitoring                                           // optional parts of MemoryBank_Info
  // #define configInfoLineOvervoltage                                          // optional parts of MemoryBank_Info
#endif

///-----------------------------------------------------------------------------
#ifdef DALI_CONFIG_MPC_OT
  #define MPC_OT_NUMBER                           DALI_CONFIG_MPC_OT
#else
  // #define MPC_OT_NUMBER                                                      /// Define the page number and enable the "OT Configuration" Multi Purpose Center
#endif
#ifdef MPC_OT_NUMBER
  #define configMemoryBank_Ot_enable                            1                 // 0: LedSet off, 1: LedSet on
  #define configMemoryBank_Ot_default_current_high              0                 // minimal led current high byte
  #define configMemoryBank_Ot_default_current_low               250               // minimal led current low byte
  #define configMemoryBank_Ot_max_output_current                40                // value * 25 mA -> 1000 mA max
  #define configMemoryBank_Ot_min_output_current                10                // value * 25mA -> 250 mA min
  #define configMemoryBank_Ot_min_output_dim_current            1
#endif

///---------------------------------------------------------------------------------------------------------------------
#ifdef DALI_CONFIG_MPC_PASSWORD1
  #define MPC_PASSWORD1_NUMBER                         DALI_CONFIG_MPC_PASSWORD1                                        // no selectable defaults for MPC_PASSWORD1
#endif

#ifdef DALI_CONFIG_MPC_PASSWORD2
  #define MPC_PASSWORD2_NUMBER                         DALI_CONFIG_MPC_PASSWORD2                                        // no selectable defaults for MPC_PASSWORD2
#endif

///---------------------------------------------------------------------------------------------------------------------
#ifdef  DALI_CONFIG_MPC_POWER
  #define MPC_POWER_NUMBER                                 DALI_CONFIG_MPC_POWER
  /// Define how the Application generates the power consumption value (Power Info Byte of "Power Consumption MPC")
  /// bit 1: 1 - Operation power is measured, 0 - Operation power is estimated
  /// bit 0: 1 - Stand by power is measured, 0 - Stand by power is estimated
  #define MPC_POWER_INFO_BYTE                                                  2
#endif

///---------------------------------------------------------------------------------------------------------------------
/// Define the page number and enable the "TDI and Corridor" Multi Purpose Center
#ifdef DALI_CONFIG_MPC_TDC
  #define MPC_TDC_NUMBER                                     DALI_CONFIG_MPC_TDC
#endif
