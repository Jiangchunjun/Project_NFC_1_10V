/******************** (C) COPYRIGHT OSRAM GmbH ****************************************************************************************************************
* File Name         : MpcServices.c (Template)
* Description       : Definition of Interface-functions to provide device specific code, needed for the Osram-specific MemoryBanks.
*                     For functions marked as mandatory, please write function body according to the demands of your your device
*                     For functions marked as optional, write the function body, IF your device needs this function. ELSE: delete this function.
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2.
* Created           : 25.02.2016 (JA)         Providing all (empty) functions, which are declared in MpcServices.h
* Modified by       : DATE       (AUTHOR)     Writing the device-specific function bodies
**************************************************************************************************************************************************************/
/// \file
///
#include <stdint.h>
#include <stdbool.h>

#define MODULE_MPC                                                              // to #include "MpcConfig.h" in config.h
#include "Config.h"
#include "ConfigMpc.h"
#include "MpcServices.h"
#include "MpcInfo_userinterface.h"

#include "currentset_userinterface.h"
//#include "buck_userinterface.h"
#include "SupplyMonitor_userinterface.h"
#include "analog_userinterface.h"
#include <hbuck.h>
#include <module_helper.h>

#include <hBuck.h>

uint32_t MpcPowerServices_GetPowerConsumption( uint8_t channel );
uint8_t MpcInfoServices_GetActualTemperature( void );
uint8_t MpcInfoServices_GetSupplyVoltage( void );
bool MpcInfoServices_GetOvervoltageShutDownState( void );

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief Function is called after firmware start to initalise the variables needed to support the Memorybanks.
/// \param                    Nothing.
/// \retval                   Nothing.
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
void MpcServices_Init( uint8_t channels )
{
    // optional: add code here, if your application needs to be initialized
}


#ifdef MPC_POWER_NUMBER
//----------------------------------------------------------------------------------------------------------------------
/// \brief Returns the actual input power
///
///
/// \param channel      The channel to be queried
/// \param *data         pointer to data array for input power
///
/// \retval
//----------------------------------------------------------------------------------------------------------------------
uint32_t MpcPowerServices_GetPowerConsumption( uint8_t channel )
{
    uint32_t input_power;
    uint32_t v_out;
    uint32_t i_out;
    
    v_out = analog_get_compensated_vout();
    i_out = hBuck_get_output_current();
    
    if ( i_out == 0 )
    {
        input_power = GPC_P_STANDBY;
    }
    else
    {
        v_out >>= 10;
        i_out >>= 10;
        
        input_power = i_out * v_out;
        input_power >>= 11;
        
        input_power *= GPC_M;
        input_power >>= 16;

        input_power += GPC_Q;
    }
    
    return input_power;
}
#endif



//----------------------------------------------------------------------------------------------------------------------
/// \brief  Returns the actual device temperature to MemoryBank_Info.c (MPC2: General Info Page)
///
/// \param  None
///
/// \retval actual temperature = T[°C] + 50 for temperature range -50 ... 204
//----------------------------------------------------------------------------------------------------------------------
uint8_t MpcInfoServices_GetActualTemperature( void )
{
    return ( *(currentset_feedback_struct.temp_sensor_actual) - 223);
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief    Returns the actual supply voltage to MemoryBank_Info.c (MPC2: General Info Page)
///
/// \param    None
///
/// \retval   supplyVoltage= ( actual voltage in [V] - 120) / 2
//----------------------------------------------------------------------------------------------------------------------
uint8_t MpcInfoServices_GetSupplyVoltage( void )
{
    uint8_t supplyVoltage = 0xFF; // 0xFF: not available
    return supplyVoltage;
}


//----------------------------------------------------------------------------------------------------------------------
/// \brief Returns a flag to MemoryBank_Info.c (MPC2: General Info Page) in case of a overvoltage shut down event
///
/// \param  None
///
/// \retval true in case of a overvoltage shut down event
//----------------------------------------------------------------------------------------------------------------------
bool MpcInfoServices_GetOvervoltageShutDownState( void )
{
    bool state = false; // no overvoltage
    
    if ( OV_YES == Supply.flags.SM_OVERVOLTAGE )    // OV_NO
    {
        state = true;
    }

    return state;
}


