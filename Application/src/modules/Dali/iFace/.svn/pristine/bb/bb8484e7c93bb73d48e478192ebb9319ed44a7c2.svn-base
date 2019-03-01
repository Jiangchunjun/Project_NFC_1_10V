//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   BallastDefs.h
///   \since  2017-10-30
///   \brief  Definitions needed from outside DaliLib or from outsideECG. Check handling, if choices are enhanced!
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is confidential and not intended for public release.
///   All rights reserved.  Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

#ifndef BALLAST_DEFS
#define BALLAST_DEFS
typedef enum {
  Ballast_CONF_UNKNOWN =  0,
  Ballast1CHANNEL =       1,
  Ballast2CHANNEL =       2,
  Ballast3CHANNEL =       3,
  Ballast4CHANNEL =       4,
  Ballast5CHANNEL =       5,
  Ballast6CHANNEL =       6,
  Ballast1SYNC =          0x08,                                                 // only 1 Dali-Instance, all hardware-outputs are driven synchronously
  Ballast1DIM2WARM =      0x09,
  Ballast2DIM2WARM =      0x0A,
  Ballast1CHANNEL_XY =    0x10,                                                 // acc. to Dali-209
  Ballast1CHANNEL_TW =    0x20,                                                 // acc. to Dali-209
  Ballast2CHANNEL_TW =    0x21,
  Ballast1PrimaryN =      0x40,                                                 // acc. to Dali-209
  Ballast1RGBWAF =        0x80                                                  // acc. to Dali-209
} TypeBallastCONFIGURATION;
#endif

TypeBallastCONFIGURATION DaliBallast_ConfigureDali(TypeBallastCONFIGURATION configuration);
