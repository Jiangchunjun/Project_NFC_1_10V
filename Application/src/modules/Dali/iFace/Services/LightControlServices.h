//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file LightControlServices.h
///   \since 2017-02-03
///   \brief Declaration of functions called from module LightControl. Must be defined in device-specific code.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

void LightControlServices_SetArcPower( uint16_t arcPowerLevel, uint8_t lamp );
uint16_t LightControlServices_GetPhysMinLinear( void );                         // returns the (actual) PhysicalMinLevel of the device as fraction of the maximum output level. Scaling is linear. I.e. e.g. 1 % = 655.
bool LightControlServices_IsLampFailure( uint8_t lamp );
bool LightControlServices_IsLampOn( uint8_t lamp );
