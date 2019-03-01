//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   DaliBallast.h
///   \since  2013-09-30
///   \brief  Interface to the functions according to DALI standard IEC62386-102.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is confidential and not intended for public release.
///   All rights reserved. Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

void DaliBallast_Init(void);                                                    // function to be called by kernel: at init
void DaliBallast_CyclicTask(void);                                              // function to be called by kernel: must be called at least 3 times within 2.5 ms!

uint32_t DaliBallast_GetPowerOnMinutes(void);                                   // returns the powerOntime of the device in minutes

typedef enum {
  BALLAST_MODE_DALI=0,          // strictly conform to IEC 62386-101, IEC 62386-102 and, if supported, IEC 62386-20x
  BALLAST_MODE_ENHANCED,        // manufacturer specific modes acc. to IEC 62386-102, 9.9.4 Operating mode 0x80 to 0xFF: manufacturer specific modes
  BALLAST_MODE_PRODUCTION,      // device is in production mode
  BALLAST_MODE_SPECIAL,         // TOUCHDIM or CORRIDOR or OUTDOOR
  // BALLAST_MODE_XY               // not supported yet: e.g. XY=ZIGBEE
  BALLAST_MODE_UNKNOWN
} TypeBallastOperatingMODE;
TypeBallastOperatingMODE DaliBallast_GetOperatingMode( uint8_t channel );
