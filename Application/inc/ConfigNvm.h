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

#define NVM_DATA_SIZE_BYTE                        (362)                         // increased to 360 to include calibration parameters
#define NVM_DATA_STRUCTS_CNT_MAX                  (18)                          // nvmData.idxMax  15

#define NVM_CALIBRATION_SIZE_BYTES                (0)

#define NVM_WRITE_CYCLES                          (200000)
#define NVM_SIZE_KBYTE                            (GLOBAL_NVM_SIZE_KBYTE)
