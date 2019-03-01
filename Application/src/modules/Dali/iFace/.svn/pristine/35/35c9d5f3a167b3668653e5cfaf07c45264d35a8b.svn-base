//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file BallastServices.h
///   \since 2017-06-22
///   \brief Declaration of the Interface-functions which need to / may be defined in the device specific code.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

// General functions to provide information about the ECG (for non-dali devices declare these function protypes in BallastServices.h)
uint16_t BallastServices_GetFirmwareVersion( void );                            // mandatory: Return the firmware version in format (Major<<8)+Minor
uint8_t BallastServices_GetEAN(uint8_t index);                                  // mandatory: Get a byte of the EAN, selected by index: index=0 read HighByte, index=5 read LowByte, index >5 (invalid) returns 0
uint8_t BallastServices_GetMaxMpcNumber(void);                                  // mandatory: Get the highest number of all MemoryBanks supported by this device

bool BallastServices_GearOk( uint8_t channel );                                 // mandatory: Return "true", if the device is fully operational or no device-failures can be detected. Return "false", if the device is not fully operational
uint16_t BallastServices_GetDefaultHardwareVersion( void );                     // mandatory: Get the default settings for hardware-version, may be overwritten in productionMode / via NFC

typedef enum {
  NO_SUPPLY = 0,
  AC_SUPPLY = 1,
  DC_SUPPLY = 2
} TypeSupply;
TypeSupply BallastServices_GetSupply(void);

void BallastServices_NotificationProductionmode (bool new_state);
