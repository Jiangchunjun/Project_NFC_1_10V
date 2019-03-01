/******************** (C) COPYRIGHT OSRAM GmbH ****************************************************************************************************************
* File Name         : DaliServices.c (Template)
* Description       : Definition of the Interface-functions providing device specific code, which is needed for the Dali protocol.
*                     These functions are declared in DaliServices.h and called by the DALI Ballast library.
*                     For functions marked as mandatory, please write the function body according to the demands of your your device.
*                     For functions marked as optional, write the function body, IF your device needs this function. ELSE: delete this function.
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 04.08.2014 (JA)         Providing all (empty) functions, which are declared in DaliServices.h
* Modified by       : DATE       (AUTHOR)     Writing the device-specific function bodies
**************************************************************************************************************************************************************/
/// \file
///

#include <stdint.h>
#include <stdbool.h>
#include <parameter.h>

#define MODULE_DALI
#include <Config.h>
#include <DaliDeviceType_FEATURES.h>                                                                                    // all definitions needed for the selected configDeviceType
#include <DaliServices.h>
#include <MpcDefs.h>
#include <ConfigNvm.h>
#include <nvm.h>
#include <nfc.h>

////#include <god_userinterface.h>
//#include <analog_userinterface.h>
//#include <os3_userinterface.h>
//#include <currentset_userinterface.h>
//#include <SupplyMonitor_userinterface.h>
//#include <currentset_userinterface.h>

#include <MemoryBank_Clm.h>                                                                                             // use the memory bank "CLM"
//#include <module_helper.h>
#include <MemoryBank_CSM.h>

#include <lightControlServices.h>
#include <ballastServices.h>
#include <MpcInfo_Services.h>

#include <gpio_xmc1300_tssop38.h>


//----------------------------------------------------------------------------------------------------------------------
// locally used function prototypes
bool MpcInfoServices_GetLightLevelState( void );


//----------------------------------------------------------------------------------------------------------------------
/// \brief Function is called after firmware start to initalise the functions which control lamp power.
/// \param                    channels: Ignore, IF your device only has 1 channel. ELSE: Provide an init-loop.
/// \retval                   Nothing.
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_Init( uint8_t channels )
{
}

#ifdef DEVICE_CHANNELS_FLEX
//----------------------------------------------------------------------------------------------------------------------
/// \brief Function returns the actual number of logical control gear units selected by e.g. DIP switch.
///        Called once while init-phase.
/// \param                      Nothing.
/// \retval                     channel size.
//----------------------------------------------------------------------------------------------------------------------
uint8_t DaliServices_GetChannelSize(void)
{
    return 1;                                                                                                             // only 1 channel used
}
#endif


//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function is called each background loop. Time between calls depends on the scheduler used!
/// \param  channel             The channel to be updated
/// \retval                     Nothing.
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_Action( uint8_t channel )
{
    // optional: add code here, if your application needs function calls in background loop
}



//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function is called each DALI_FastTIME_TICK_ms (2.5 ms).
/// \param  channel             The channel to be updated.
/// \retval                     None.
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_FastTimer( uint8_t channel )
{
    // optional: add code here, if your application needs function calls at fixed time ticks
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief  This function is called each DALI_SlowTIME_TICK_ms (25.0 ms).
/// \param  channel             The channel to be updated
/// \retval                     Nothing.
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_SlowTimer( uint8_t channel )
{
    
}

#ifdef DALI_CONFIG_OutputLEVEL
//----------------------------------------------------------------------------------------------------------------------
/// \brief  The function forwards the demanded daliLevel for logarithmic curve according to IEC62386-102 to an external unit.
///         Use this function only, if control of the lamp power is done externally!
/// \param  daliLevel           The daliLevel in resolution 8.8
/// \param  channel             The channel to be updated
/// \retval                     Nothing.
//----------------------------------------------------------------------------------------------------------------------
//
void DaliServices_LogDaliLevel( uint16_t daliLevel, uint8_t channel )
{
    // mandatory, if DALI_CONFIG_OutputLEVEL is defined: add your code here
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  The function forwards the demanded daliLevel for linear curve according to IEC62386-102 to an external unit.
///         Use this function only, if control of the lamp power is done externally!
/// \param  daliLevel           The lampPower in resolution 8.8 (for linear curve)
/// \param  channel             The channel to be updated
/// \retval                     Nothing.
//----------------------------------------------------------------------------------------------------------------------

void DaliServices_LinDaliLevel( uint16_t daliLevel, uint8_t channel )
{
    // mandatory, if DALI_CONFIG_OutputLEVEL is defined and linear dimming curve is provided by the selected DALI_CONFIG_DEVICE_TYPE: add your code here
}
#else
//----------------------------------------------------------------------------------------------------------------------
/// \brief  The function forwards the demanded lampPower by the DaliLib to the unit, which controls the lamp power.
///         Use this function, if control of the lamp power is done internally!
/// \param  arcPowerLevel       The lampPower in resolution 8.8 (linear range, 0xFFFF = 100 %)
/// \param  channel             The channel to be updated
/// \retval                     Nothing.
//----------------------------------------------------------------------------------------------------------------------

static uint16_t m_current_dali_level;
//extern uint16_t g_dali_level;
extern uint8_t g_ovp_judge,g_ocp_judge;
void LightControlServices_SetArcPower( uint16_t arcPowerLevel, uint8_t channel )
{
    //CurrentSet_SetDaliArcPowerLevel((uint32_t)arcPowerLevel);

    m_current_dali_level = arcPowerLevel;
    //g_dali_level = arcPowerLevel;
//    MpcInfo_SetDaliLevel(arcPowerLevel);
}
#endif

bool MpcInfoServices_GetLightLevelState( void )
{
    return ( m_current_dali_level > 0 );
}


//----------------------------------------------------------------------------------------------------------------------
/** Get the actual lamp / ECG failure state and forward it to the DALI library
 *
 * \ failure states for device type 6 (LED applications) as defined in DaliDeviceType_FEATURES.h:
 * \
 * \ #define T6_Failure_SHORT_CIRCUIT                 0x01
 * \ #define T6_Failure_OPEN_CIRCUIT                  0x02
 * \ #define T6_Failure_LOAD_DECREASE                 0x04
 * \ #define T6_Failure_LOAD_INCREASE                 0x08
 * \ #define T6_Failure_CURRENT_PROTECTOR_ACTIVE      0x10
 * \ #define T6_Failure_THERMAL_SHUTDOWN              0x20
 * \ #define T6_Failure_THERMAL_OVERLOAD_RED          0x40
 * \ #define T6_Failure_REFERENCE_FAILED              0x80
 *
 * \ param  ui8  channel
 * \ retval ui8   failure status
 **/
//----------------------------------------------------------------------------------------------------------------------
uint8_t DaliServices_GetFailure( uint8_t channel )
{
    uint8_t failure= 0;

//    if ( g_ocp_judge==0 )
//    {
//        failure |= T6_Failure_SHORT_CIRCUIT;
//    }
//
//    if ( g_ovp_judge==0 )  //(god_feedback_struct.error_message == led_overvoltage)
//    {
//        failure |= T6_Failure_OPEN_CIRCUIT;
//    }
//
////    // Report an active power reduction in case of thermal overload
//    if ( 1==currentset_feedback_struct.thermal_powerred_activ )
//    {
//        failure |= T6_Failure_THERMAL_OVERLOAD_RED;
//    }

    return failure;
}



//----------------------------------------------------------------------------------------------------------------------
/** Get the actual operating mode from the secondary side and forward it to the DALI library
 * The operating mode is only != 0 if there is no lamp failure (open or short circuit) and if the buck is running
 *
 * \ operating modes for device type 6 (LED applications) as defined in DaliDeviceType_FEATURES.h:
 * \
 * \ #define T6_OperatingMode_PWM_ACTIVE                 0x01                                                           ///< \brief Definition of the bit positions in the active operatingMode byte.
 * \ #define T6_OperatingMode_AM_ACTIVE                  0x02
 * \ #define T6_OperatingMode_OUT_CURRENT_CONTROLLED     0x04
 * \ #define T6_OperatingMode_HIGH_CURRENT_PULSE_ACTIVE  0x08
 *
 * \param  uint8_t  channel
 * \retval uint8_t  Operating Mode
 **/
//----------------------------------------------------------------------------------------------------------------------
uint8_t DaliServices_GetType6_OperatingModes( void )
{
    return T6_OperatingMode_AM_ACTIVE | T6_OperatingMode_OUT_CURRENT_CONTROLLED;
}

uint8_t DaliServices_GetOperatingMode( uint8_t channel )
{
    uint8_t operatingMode = 0;
    uint8_t failure_state = DaliServices_GetFailure(channel) & (~T6_Failure_THERMAL_OVERLOAD_RED);                        // consider failures except thermal overload reduction

    //if ( ( currentset_dali_arc_power_level != 0 ) )                                                                     // evaluate operating mode only if DALI level > 0
    {
        operatingMode |= T6_OperatingMode_AM_ACTIVE;
        operatingMode |= T6_OperatingMode_OUT_CURRENT_CONTROLLED;
    }

    return operatingMode;
}


//---------------------------------------------------------------------------------------------------------------------
/// \brief    Returns the actual type of power supply (coded as enum-type)
/// \param    None
/// \retval   Actual power supply, i.e. NO_SUPPLY, AC_SUPPLY or DC_SUPPLY (if DC-mode supported)
//---------------------------------------------------------------------------------------------------------------------

TypeSupply BallastServices_GetSupply(void)
{
static    TypeSupply  supplytmp=AC_SUPPLY,test_supply=1;

    // right now DALI does not check whether one has 230V or 48V
    // At the moment this does not matter, because Dali is only called by module god
    // if the power is ok and Dali shall operate OLAF
//    if (sm_mode_230V == Supply.flags.SM_MODE)
//    {
//        if (AC == Supply.flags.SM_POWERTYPE)
//        {
            supplytmp = AC_SUPPLY;
//        }
//        else
//        {
//            supplytmp = DC_SUPPLY;
//        }
//    }
//    else
//    {
//        supplytmp = NO_SUPPLY;
//    }

    return test_supply;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///<- brief Interface to nvm

//----------------------------------------------------------------------------------------------------------------------
/** function executes necessary instructions before power down
 *  Stops all application processes
 * \param  none
 * \retval none
 **/
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_PowerDown( void )                                                                                     // Stops all application processes and sets the ?-parts used by the application into power saving state
{
    //StdoutString("writeall executed...\n");
    //nvmWriteAll();
}


//----------------------------------------------------------------------------------------------------------------------
/** function restores the uC settings at restart after standby
 * \param  uint8_t  channels (neglected, used for multistring devices)
 * \retval none
**/
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_Restart( uint8_t channels )                                                                           // Restores the ? settings after power-down as needed by the application
{
    //nvmResume(); //
}


//----------------------------------------------------------------------------------------------------------------------
/** function forces reset of the device after power loss of longer than DALI_POWER_OFF_TIME_FTT
 * \param  none
 * \retval none
**/
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_ResetDevice(void)                                                                                     // Forces Watchdog-Timer. If not already done in DaliServices_WriteStepNvm(): Store non-volatile memory
{
    nvmWriteAll();
    NfcOnPowerDown(nfc_power_down);
    //P1_1_set();
    // Fix for NFC. Maybe not required in the future anymore
    for ( int i = 0; i < 525000; i++ )
    {
        __no_operation();
    }
    
     WDT->SRV = 0xAFFEAFFEUL;   //os3_wdt_force_sys_reset();
}


//----------------------------------------------------------------------------------------------------------------------
/** function is called after DALI 'SAVE PERSISTENT VARIABLES' command and forwards the request to the application
 *  The max number of write commands is limited according to DALI ED 2.0 recommendation (set to 1000)
 * \param  none
 * \retval none
**/
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_StoreNvm(void)
{
    if (nvmGetWriteCycles() < NVM_WRITE_CYCLES)
    {
        GOD_Request_nvmWriteAll();                                                                                        // send NVM write request to application
    }
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Function provides the firmware version number of the device to the DaliLibrary.
/// \param                    Nothing.
/// \retval                   FirmwareVersion major + Minor as 8.8 value.
//----------------------------------------------------------------------------------------------------------------------
uint16_t DaliServices_GetFirmwareVersion( void )
{
    return (DEVICE_FW_VERSION_MAJOR<<8) + DEVICE_FW_VERSION_MINOR;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///<- brief Optional functions

#ifndef DEVICE_EAN
//----------------------------------------------------------------------------------------------------------------------
/// \brief    Returns a byte of the EAN, selected by index.
///           This function is only called, if DEVICE_EAN is not defined in Config.h (e.g. in case of device-families)!
/// \param    index: selects 1 byte of the EAN. Range of index is 0 ..5.
/// \retval   Selected byte of the EAN. If index=0 read HighByte, if index=5 read LowByte, index >5 (invalid) returns 0
//----------------------------------------------------------------------------------------------------------------------
uint8_t DaliServices_GetEAN(uint8_t index)
{
    if (index <= 5)
    {
        return (uint64_t)(GLOBAL_DEVICE_EAN) >> (index*8);
    }
    else
    {
        return 0;
    }
}
#endif

#ifdef POWERon_DELAY_TIME_ms
//----------------------------------------------------------------------------------------------------------------------
/// \brief    Returns the delay between switching on power supply and reaching the endless loop in main in device-internal unit.
///           This function is only called, if POWERon_DELAY_TIME_ms is not defined in Config.h!
/// \param    None
/// \retval   Time delay in FTT (FastTimeTick = 2.5 ms)
//----------------------------------------------------------------------------------------------------------------------
uint8_t DaliServices_GetPowerOnDelay_FTT( void )
{
    uint8_t answer = (uint8_t)( POWERon_DELAY_TIME_ms/DALI_FastTIME_TICK_ms );
    return answer;
}
#endif

#ifdef DALI_CONFIG_CORRECT_FADE
//----------------------------------------------------------------------------------------------------------------------
/// \brief    This functions allows to correct fast fades to level=0, which the device cannot follow, if commanded from DaliLibrary.
///           All fadeTimes lower or equal to DALI_CONFIG_CORRECT_FADE are controlled by DaliServices(), when triggered by this function.
/// \param    fadeRate: The fadeRate demanded (coding according to IEC62386-102).
///           actLevel: The actual level from which the fade should start.
///           channel: The device channel selected
/// \retval   Time delay in FTT (FastTimeTick = 2.5 ms)
//----------------------------------------------------------------------------------------------------------------------
void DaliServices_FadeOff( int32_t fadeRate, uint16_t actLevel, uint8_t channel )
{
    // trigger to fade down to MinLevel until arcPowerLevel = 0 received by DaliServices_SetArcPower()
}
#endif

uint16_t LightControlServices_GetPhysMinLinear( void)
{
    return  (650);//(DALI_CONFIG_PHYSminLEVEL_log);
}

uint8_t DaliServices_GetType6_GearType( void)
{
    return DALI_CONFIG_TYPE6_GEAR_TYPE;
}

uint8_t DaliServices_GetType6_Features( void)
{
    return DALI_CONFIG_TYPE6_FEATURES;
}

uint8_t DaliServices_GetMinFastFadeTime( void)
{
    return DALI_CONFIG_TYPE6_MIN_FastFADE_TIME;
}

bool LightControlServices_IsLampFailure( uint8_t channel )
{
    if ( (DaliServices_GetFailure(channel) & DALI_LAMP_FAILURE_MASK) == 0 )
    {
        return false;
    }
    else
    {
        return true;
    }
}


bool LightControlServices_IsLampOn( uint8_t channel )
{
    if ( (DaliServices_GetOperatingMode(channel) & DALI_LAMP_ON_OPERATING_MASK) == 0 )
    {
        return true;//
    }
    else
    {
        return true;
    }
}


bool BallastServices_GearOk( uint8_t channel )
{
    return ( DaliServices_GetFailure( channel ) & DALI_GEAR_FAILURE_MASK ) == 0;
}


uint32_t MpcPowerServices_GetPowerConsumption( uint8_t channel )
{
    return 64;
}


uint16_t BallastServices_GetDefaultHardwareVersion( void )
{
    return 0x100;
}


uint8_t BallastServices_GetMaxMpcNumber( void )
{
    return MPC_MAX_NUMBER;
}


uint8_t BallastServices_GetEAN( uint8_t index )
{
    return (uint8_t)(((uint64_t)GLOBAL_DEVICE_EAN >> index * 8) & 0xFF);
}


uint16_t BallastServices_GetFirmwareVersion( void )
{
    return (DEVICE_FW_VERSION_MAJOR<<8) + DEVICE_FW_VERSION_MINOR;
}


void ErrorShutdown(char* str)
{
    while (1) {};
}

uint32_t  ControllerUniqueID (void)

{
  static uint32_t ueser_id ;
  
    ueser_id=SCU_GENERAL->DBGROMID;
    return SCU_GENERAL->IDCHIP;
}

uint32_t getFWRevRoot(void)
{
    //return SVN_REV_ROOT;
}

uint8_t NfcServices_GetEcgOnValue(nfc_mode_t ecg_state)
{
    if(true == ecg_state)
    {
            return NFC_STATUS_REGISTER_ECG_ON_VALUE;
    }
    
    return NFC_STATUS_REGISTER_ECG_OFF_VALUE;
}
void NfcTagPowerOn(void)
{
    
}
void NfcPowerDownPrepare( void ) 
{
    
}

void NfcWatchdogResetRequest( void )
{
    // Add a watchdog reset here...
    WDT->SRV = 0xABADCAFEUL;                                                      // write "magic word" to WDT service register
}

void NfcPowerDownSleep( void ) 
{
}

void NfcPowerDownResume( void ) 
{
    
}

void GOD_Request_nvmWriteAll(void)
{
    //gi.store_request=please_store;
}
void pwrDownSleep(void)
{

}