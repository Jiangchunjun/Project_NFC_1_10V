/******************** (C) COPYRIGHT 2012 - 2016 OSRAM AG ******************************************************************************************************
* File Name         : MemoryBanks_Debug.c (Template)
* Description       : Providing functions of device-specific MemoryBank Debug. Only content of address 0 is fixed: MPC_Debug_LastADDRESS
*                     Values which need several bytes start with high-byte at lowest address and end with the low-byte at the highest address of this variable
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 12.01.2016 (JA)         Creating Template with sample-code
* Modified by       : DATE       (AUTHOR)     DESCRIPTION OF CHANGE: Writing device-specific code
**************************************************************************************************************************************************************/

// address 0: MPC_Debug_LastADDRESS
// address 1 .. MPC_Debug_LastADDRESS: Device specific data (read / write access device-specific)

#define MPC_Debug_LastADDRESS 32

#include <stdint.h>
#include <stdbool.h>
#include "MemoryBank_Debug.h"
#include "hBuck.h"
#include "currentset_userinterface.h"
#include "nvm.h"


void MemoryBank_Debug_Init(void)
{

}

#define DEBUG_FACTOR_CURRENT        131
#define DEBUG_BITSHIFT_CURRENT      17
#define DEBUG_MILLIAMPS_16BIT(x)    ((uint16_t) ((( (uint32_t) (x * DEBUG_FACTOR_CURRENT)) >> DEBUG_BITSHIFT_CURRENT) + 1 ))


bool MemoryBank_Debug_GetValue(uint8_t address, uint8_t *pValue, uint8_t channel )    // defined weak in MemoryBank.c, overwritten by including this file
{

    uint16_t nom_current_debug = DEBUG_MILLIAMPS_16BIT(*(currentset_feedback_struct.nominal_current));
    uint16_t buck_i_led_nom_debug = 0;

    switch (address)
    {
    case 0:
        *pValue = MPC_Debug_LastADDRESS;
        return true;
    case 1:
        *pValue = 0;
        return true;
    case 2:
        *pValue = 0;
        return true;
    case 3:
        *pValue = (uint8_t) (nom_current_debug >> 8);                               // nom current high byte (considering LED set current, CLM factor, ...)
        return true;
    case 4:                                                                       // nom current low byte (considering LED set current, CLM factor, ...)
        *pValue = (uint8_t) nom_current_debug;
        return true;
    case 5:
        *pValue = (uint8_t) (buck_i_led_nom_debug >> 8);                            // I LED actual high byte (considering dimming level, temperature, VLED, VIN)
        return true;
    case 6:
        *pValue = (uint8_t) buck_i_led_nom_debug;                                   // I LED actual low byte (considering dimming level, temperature, VLED, VIN)
        return true;
    case 7:
        *pValue = (uint8_t) buck_feedback_struct.error;                             // buck feedback struct error 8 bit
        return true;
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        *pValue = 0;
        return true;
    case 13:
        *pValue = (uint8_t) ( *(currentset_feedback_struct.temp_sensor_actual) - 223); // *pValue = (T in [°C] + 50) for current range -50 ... 204°C
        return true;
    case 14:
        return true;
    case 15:
        *pValue = 0; // reserved for: vbusmin_target (V rail set) (high byte)
        return true;
    case 16:
        *pValue = 0; // reserved for: vbusmin_target (V rail set) (low byte)
        return true;
    case 17:
        *pValue = 0; // reserved for: Surge counter (high byte)
        return true;
    case 18:
        *pValue = 0; // reserved for: Surge counter (low byte)
        return true;
    case 19:
        *pValue = 0; // reserved for: Flag power reduction active;
        return true;
    case 20:
        *pValue = 0; // reserved
        return true;
    case 21:
        *pValue = 0; // reserved
        return true;
    case 22:
        *pValue = 0; // reserved
        return true;
    case 23:
        *pValue = 0; // reserved
        return true;
    case 24:
        *pValue = 0; // reserved
        return true;
    case 25:
        *pValue = 0; // reserved
        return true;
    case 26:
        *pValue = 0; // reserved
        return true;
    case 27:
        *pValue = 0; // reserved
        return true;
    case 28:
        *pValue = 0; // reserved
        return true;
    case 29:
        *pValue = 0; // reserved
        return true;
    case 30:
        *pValue = 0; // reserved
        return true;
    case 31:
        *pValue = 0; // reserved
        return true;
    case 32:
        *pValue = 0; // reserved
        return true;

    default:                                                                                                              // if address is not supported
        return false;
    }
}

bool MemoryBank_Debug_SetValue(uint8_t address, uint8_t value, uint8_t channel )                                        // defined weak in MemoryBank.c, overwritten by including this file
{
    switch (address)
    {
    default:                                                                                                              // debug memory bank is read-only!
        return false;
    }
}
