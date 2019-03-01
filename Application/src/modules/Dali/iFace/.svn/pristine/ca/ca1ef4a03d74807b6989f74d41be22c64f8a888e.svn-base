//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file   ColourServicesTc.h
///   \since  2017-02-03
///   \brief  Interface-functions to the services of MemoryBank "TunableWhite" and "Dim2Dark". Based the actual dimLevel and on the actual colour-command,
///           the service functions calculate the dimLevels for the cool and the warm lamp in format [0.16] with 0xFFFF = 100 %.
///           Note: The service functions return 0 for both output-channels, if the MemoryBank is not suppoerted or if the function is disabled.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

typedef struct {
  uint16_t cool;
  uint16_t warm;
} TypeDimLamps;
TypeDimLamps ColourServicesTc_CalcOutputs( uint32_t colourTemperature, uint16_t level, uint8_t channel);  // forward both actual colourTemperature in format [8+16.8] in mirek and dimLevel in format [0.16]
TypeDimLamps ColourServicesTc_CalcDim2Warm( uint16_t level, uint8_t channel);                        // forward only dimLevel in format [0.16]

bool ColourServicesTc_SwapOutputs ( uint8_t channel );                          // returns true, if outputs should be swapped