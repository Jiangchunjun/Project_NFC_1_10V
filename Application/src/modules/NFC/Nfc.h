/** ----------------------------------------------------------------------------
// Nfc.h
// -----------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EC - MCC
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Change history, major milestones and credits:
//   2016-08, S. el Barradie, J. Eisenberg
//
// $Author: g.salvador $
// $Revision: 16419 $
// $Date: 2019-01-10 21:00:12 +0800 (Thu, 10 Jan 2019) $
// $Id: Nfc.h 16419 2019-01-10 13:00:12Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nfc/tags/v2.7/Src/Nfc.h $

*   \file
*   \brief User header file for the whole NFC Module
*/

#ifndef __NFC__H__
#define __NFC__H__

//------------------------------------------------------------------------------
// defines
//------------------------------------------------------------------------------
#define NFC_VERSION_MAJOR   												   2
/** NFC TAG Status Register ECG_ON value when ECG is switched on
 * \note This value (4 bit) should be less than 16 */
#define NFC_STATUS_REGISTER_ECG_ON_VALUE                                    0x00
/** NFC TAG Status Register ECG_ON value when ECG is switched off
 * \note This value (4 bit) should be less than 16 */
#define NFC_STATUS_REGISTER_ECG_OFF_VALUE                                   0x00

/** NFC TAG Status Register ECG_ON value when ECG is entering standby state
 * \note This value (4 bit) should be less than 16 */
#define NFC_STATUS_REGISTER_ECG_STDBY_VALUE                                 0x05

//------------------------------------------------------------------------------
// enums
//------------------------------------------------------------------------------
// NFC initialization status
typedef enum
{
    UNDEFINED   = 0,
    IN_PROGRESS = 1,
    FINISHED    = 2
} nfc_init_status_t;


// NFC module mode
typedef enum
{
    NFC_OFF     = 0,
    NFC_ON      = 1,
    NFC_STANDBY = 2
} nfc_mode_t;

typedef enum
{
    nfc_run_time,
    nfc_power_down,
    nfc_sleep_mode,
    nfc_what_ever
} nfc_power_down_use_cases;

//------------------------------------------------------------------------------
// service functions
//------------------------------------------------------------------------------
void pwrDownSleep(void);


//------------------------------------------------------------------------------
// global functions
//------------------------------------------------------------------------------
void   NfcInit(void);
void   NfcRuntimeInit(void);
void   NfcCyclic(void);
void   NfcOnPowerDown(nfc_power_down_use_cases useCase);
void   NfcSetSleep(void);
void   NfcSetWakeup(void);
void   NfcWatchdogResetRequest(void);
void   NfcResetTagAccessRights(void);
int8_t NfcIsNewDataPresentAtPowerOn(void);
bool   NfcIsPowerOnTagDataAvailable(void);
nfc_init_status_t NfcInitStatus(void);
bool   NfcInitialPowerUp(void);
void   NfcSwitchOff(void);
void   NfcSwitchOn(void);
uint8_t NfcServices_GetEcgOnValue(nfc_mode_t ecg_state);
#endif // __NFC__H__
