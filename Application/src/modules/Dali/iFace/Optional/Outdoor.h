/*******************************************************************************************************************************
* File Name         : Outdoor.h
* Description       : Definitions of device-specific OUTDOOR service routines
* Author(s)         : Fighera Diego
* Created           : 28.05.2015
* Modified by       : DATE       (AUTHOR)
**********************************************************************************************************************************/

typedef enum {
  OUTDOOR_CONFIG_NOT_OK = 0,
  OUTDOOR_CONFIG_OK
} OUTDOOR_CONFIG_t;
OUTDOOR_CONFIG_t Outdoor_Init(void);
void Outdoor_FastTimer(uint8_t channels);
void Outdoor_Action(uint8_t channels);
void Outdoor_Stop(void);
