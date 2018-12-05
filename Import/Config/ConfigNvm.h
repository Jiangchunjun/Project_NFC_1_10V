/******************** (C) COPYRIGHT 2015 OSRAM AG *********************************************************************************
* File Name         : ConfigNvm.h
* Description       : Definitions of device-specific properties of a "Dali Control Gear" Device (defining the specific hardware+features of the device)
*                   : device <write device name here>.
* Author(s)         : Template: Jutta Avril (JA)
* Created           : 16.04.2015 (JA)
* Modified by       :
**********************************************************************************************************************************/

//=============================================================================
//     N o n - v o l a t i l e   M e m o r y
//=============================================================================

//#define NVM_DATA_SIZE_BYTE                        (230*DEVICE_CHANNELS)
//#define NVM_DATA_STRUCTS_CNT_MAX                  (100*DEVICE_CHANNELS)

#define NVM_DATA_SIZE_BYTE                        (300)   // nvmData.totSizeByte 284
#define NVM_DATA_STRUCTS_CNT_MAX                  (16)    // nvmData.idxMax  14

#define NVM_CALIBRATION_SIZE_BYTES                (0)

#define NVM_WRITE_CYCLES                          (200000)
//#define NVM_FLASH_SIZE_BYTES                      (uint16)255
#define NVM_SIZE_KBYTE                            (64)