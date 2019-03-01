/******************** (C) COPYRIGHT 2007 - 2016 OSRAM  ***************************************************************************
* File Name         : DaliDeviceType_FEATURES.h
* Description       : Feature definitions of the different devices types
* Author(s)         : Jutta Avril, Matthias Schneider
* Created           : 28.04.2010
**********************************************************************************************************************************/


//  By " #define Tn_Fx_TEXT 0xNN" a mask (i.e. loacation of the bit) is defined for a feature with following notation
//    Tn_Fx_TEXT:
//    Tn:             Tn defines the DaliDeviceType n, for which this definition is valid,
//       Fx:          Fx defines the FeatureByte x, in which the feature is defined (if the selected DaliDeviceType has only 1 FeatureByte, only x=1 is )
//          TEXT:     TEXT describes the (single) feature, for which which the mask is defined by the value 0xNN
//
//    Tn_Text_TEXT:   Same as above, but ..
//       Text:        Text defines a featured byte with special naming
//
//

//------------------------------------------------------------------------------
//
//                     DaliDeviceType 0
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 0)                                                     // .. if DaliDeviceType = 0

#endif



//------------------------------------------------------------------------------
//
//                     DaliDeviceType 2
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 2)                                                     // .. if DaliDeviceType = 2
  //ToDo
#endif



//------------------------------------------------------------------------------
//
//           DaliDeviceType 3 - Low Voltage Halogen Lamps
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 3)                                                     // .. if DaliDeviceType = 3
  //ToDo
#endif



//------------------------------------------------------------------------------
//
//              DaliDeviceType 4 - Incandescent Lamps
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 4)                                                     // .. if DaliDeviceType = 4

  // List of all possible Feature Byte 1, selected by definition of configDeviceType4_FEATURES1:
  #define T4_F1_OVER_CURRENT_DETECTION                  0x01                    //
  #define T4_F1_OPEN_CIRCUIT_DETECTION                  0x02
  #define T4_F1_LOAD_DECREASE_DETECTION                 0x04
  #define T4_F1_LOAD_INCREASE_DETECTION                 0x08
  #define T4_F1_Reserved                                0x10
  #define T4_F1_THERMAL_SHUTDOWN                        0x20
  #define T4_F1_THERMAL_OVERLOAD_RED                    0x40
  #define T4_F1_PHYSICAL_SELECTION                      0x80

  // List of all possible Feature Byte 2, selected by definition of configDeviceType4_FEATURES2:
  #define T4_F2_ANSWER_TEMPERATURE                      0x01
  #define T4_F2_ANSWER_SUPPLY_VOLTAGE                   0x02
  #define T4_F2_ANSWER_SUPPLY_FREQUENCY                 0x04
  #define T4_F2_ANSWER_LOAD_VOLTAGE                     0x08
  #define T4_F2_ANSWER_LOAD_CURRENT                     0x10
  #define T4_F2_ANSWER_LOAD_POWER                       0x20
  #define T4_F2_ANSWER_LOAD_RATING                      0x40
  #define T4_F2_ANSWER_LOAD_OVERCURRENT_RED             0x80

  // List of all possible Feature Byte 3, selected by definition of configDeviceType4_FEATURES3:
  #define T4_F3_DIM_MODE_LEADING                        0x01                    // also used for Cmd 239: Query Dimmer Status"
  #define T4_F3_DIM_MODE_TRAILING                       0x02                    // also used for Cmd 239: Query Dimmer Status"
  #define T4_F3_NON_LOG_CURVE_SUPPORT                   0x08
  #define T4_F3_ANSWER_LOAD_UNSUITABLE                  0x80
  #define T4_F3_Reserved  0x70

  #define T4_F1_ENABLE_REFERENCE_MEASURE_MASK   (T4_F1_LOAD_DECREASE_DETECTION |T4_F1_LOAD_INCREASE_DETECTION)    //

#endif



//------------------------------------------------------------------------------
//
//                     DaliDeviceType 5
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 5)                                                     // .. if DaliDeviceType = 5
  //ToDo
#endif



//------------------------------------------------------------------------------
//
//                     DaliDeviceType 6 -- LED modules
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 6) || (DALI_CONFIG_DEVICE_1stTYPE == 6)         // .. if DaliDeviceType = 6
  #define T6_ExtendedVersionNUMBER 1
  // Complete list of masks to define the gear type. Define configDeviceType6_GEAR_TYPE by ORing the corresponding masks.
  #define T6_GearType_LED_POWER_SUPPLY_INTEGRATED       0x01                    ///< \brief Definition of the bit positions in the gear type byte
  #define T6_GearType_LED_MODULE_INTEGRATED             0x02
  #define T6_GearType_AC_SUPPLY_POSSIBLE                0x04
  #define T6_GearType_DC_SUPPLY_POSSIBLE                0x08
  #define T6_GearType_Reserved1                         0x10
  #define T6_GearType_Reserved2                         0x20
  #define T6_GearType_Reserved3                         0x40
  #define T6_GearType_Reserved4                         0x80

  // Complete list of masks to define the operation mode. Define configDeviceType6_OPERATION_MODE by ORing the corresponding masks.
  #define T6_OperatingMode_PWM_POSSIBLE                 0x01                    ///< \brief Definition of the bit positions in the operation mode byte
  #define T6_OperatingMode_AM_POSSIBLE                  0x02
  #define T6_OperatingMode_CONTROL_CURRENT_POSSIBLE     0x04
  #define T6_OperatingMode_HIGH_CURRENT_PULSE_POSSIBLE  0x08
  #define T6_OperatingMode_Reserved1                    0x10
  #define T6_OperatingMode_Reserved2                    0x20
  #define T6_OperatingMode_Reserved3                    0x40
  #define T6_OperatingMode_Reserved4                    0x80

  // Complete list of masks to define the Features. Define configDeviceType6_FEATURES by ORing the corresponding masks.
  #define T6_Feature_OVER_CURRENT_DETECTION             0x01                    ///< \brief Definition of the bit positions in the feature byte
  #define T6_Feature_OPEN_CIRCUIT_DETECTION             0x02
  #define T6_Feature_LOAD_DECREASE_DETECTION            0x04
  #define T6_Feature_LOAD_INCREASE_DETECTION            0x08
  #define T6_Feature_CURRENT_PROTECTOR_AVAILABLE        0x10
  #define T6_Feature_THERMAL_SHUTDOWN                   0x20
  #define T6_Feature_THERMAL_OVERLOAD_RED               0x40
  #define T6_Feature_PHYSICAL_SELECTION                 0x80

  // List of all possible feedback bits (FAILURE STATUS, OPERATING MODE) of DALI_CONFIG_DEVICE_TYPE = 6
  // Bits to define the Failure Status (answer to command 241)
  #define T6_Failure_SHORT_CIRCUIT                      0x01                    ///< \brief Definition of the bit positions in the failure byte.
  #define T6_Failure_OPEN_CIRCUIT                       0x02
  #define T6_Failure_LOAD_DECREASE                      0x04
  #define T6_Failure_LOAD_INCREASE                      0x08
  #define T6_Failure_CURRENT_PROTECTOR_ACTIVE           0x10
  #define T6_Failure_THERMAL_SHUTDOWN                   0x20
  #define T6_Failure_THERMAL_OVERLOAD_RED               0x40
  #define T6_Failure_REFERENCE_FAILED                   0x80

  // Bits to define the active state of Operating Mode (answer to command 252)
  #define T6_OperatingMode_PWM_ACTIVE                   0x01                    ///< \brief Definition of the bit positions in the active operatingMode byte.
  #define T6_OperatingMode_AM_ACTIVE                    0x02
  #define T6_OperatingMode_OUT_CURRENT_CONTROLLED       0x04
  #define T6_OperatingMode_HIGH_CURRENT_PULSE_ACTIVE    0x08
  #define T6_OperatingMode_NON_LOG_DIMMING_ACTIVE       0x10
  #define T6_OperatingMode__Reserved1                   0x20
  #define T6_OperatingMode__Reserved2                   0x40
  #define T6_OperatingMode__Reserved3                   0x80

  #define T6_DIMMING_CURVE_LOG 0
  #define T6_DIMMING_CURVE_LIN 1

  // derived masks
  #define T6_ReferenceEnable_MASK (T6_Failure_LOAD_DECREASE|T6_Failure_LOAD_INCREASE|T6_Failure_CURRENT_PROTECTOR_ACTIVE)

  #define DALI_GEAR_FAILURE_MASK    (T6_Failure_THERMAL_SHUTDOWN | T6_Failure_THERMAL_OVERLOAD_RED)
  #define DALI_LAMP_FAILURE_MASK    (T6_Failure_SHORT_CIRCUIT | T6_Failure_OPEN_CIRCUIT | T6_Failure_LOAD_DECREASE | T6_Failure_LOAD_INCREASE | T6_Failure_CURRENT_PROTECTOR_ACTIVE)
  #define DALI_LAMP_ON_OPERATING_MASK   (T6_OperatingMode_PWM_ACTIVE | T6_OperatingMode_AM_ACTIVE | T6_OperatingMode_OUT_CURRENT_CONTROLLED | T6_OperatingMode_HIGH_CURRENT_PULSE_ACTIVE)

#endif

//------------------------------------------------------------------------------
//
//                     DaliDeviceType 7
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_TYPE == 7)                                              // .. if DaliDeviceType = 7
  //ToDo
#endif



//------------------------------------------------------------------------------
//
//                     DaliDeviceType 8
//
//------------------------------------------------------------------------------
#if (DALI_CONFIG_DEVICE_2ndTYPE == 8)                                           // .. if DaliDeviceType = 8 (needs type of lamps as DaliDeviceType)
  #define T8_ExtendedVersionNUMBER 2
// definition of types supported by DALI
  #define DALI_T8_TYPE_XY               0x10                                    // defined in Config_T6T8_...h
  #define DALI_T8_TYPE_TC               0x20
  #define DALI_T8_TYPE_PRIMARY          0x40
  #define DALI_T8_TYPE_RGBWAF           0x80
  #define DALI_T8_TYPE_MASK             UINT8_MAX

// defines for QUERY Gear Features/Status
  #define DALI_T8_AUTO_ACTIVATE_F       0x01
  #define DALI_T8_AUTOCALIB_SUPPORTED_F 0x40
  #define DALI_T8_AUTOCALIB_RECOVER_F   0x80

  #define COLOUR_FEATURES_DEFAULT       DALI_T8_AUTO_ACTIVATE_F

  // defines for QUERY Colour Status
  #define COLOUR_STATUS_XY_OOR_F        0x01                                    // OOR: OutOfRange
  #define COLOUR_STATUS_TC_OOR_F        0x02                                    // OOR: OutOfRange
  #define COLOUR_AUTOCALIB_RUNNING_F    0x04
  #define COLOUR_AUTOCALIB_SUCCESS_F    0x08
  #define COLOUR_TYPE_XY_ACTIVE_F       DALI_T8_TYPE_XY
  #define COLOUR_TYPE_TC_ACTIVE_F       DALI_T8_TYPE_TC
  #define COLOUR_TYPE_PRIMARY_ACTIVE_F  DALI_T8_TYPE_PRIMARY
  #define COLOUR_TYPE_RGBWAF_ACTIVE_F   DALI_T8_TYPE_RGBWAF
#endif
