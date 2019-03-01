/******************** (C) COPYRIGHT 2013 - 2014 OSRAM AG *********************************************************************************
* File Name         : ProductionTest.c
* Description       : Functions to provide device-specific test steps in production.
* Author(s)         : Jutta Avril (JA)
* Created           : 14.02.2014 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**********************************************************************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include <intrinsics.h>
#include <global.h>
//#include <build_version.h>
//#include <calibration.h>
//#include <miscStuff.h>
//#include <analog_userinterface.h>
//#include <currentset_userinterface.h>
//#include <god_userinterface.h>
//#include <module_helper.h>
#include <XMC1000_RomFunctionTable.h>
#include <config.h>
#include <ProductionTest.h>
//#include "BMI_module.h"


#define BUILD_BUG_ON(condition)         ((void)sizeof(char[1 - 2*!!(condition)]))   // If no static assertion is available

static_assert( sizeof(GLOBAL_BASIC_CODE_STRING)==8+1, "Basic code string has the wrong length!!" );

static bool m_calibration_enable = false;


void ProductionTest_Init( uint8_t channel )
{
    // add your code, if needed
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Production test functions - Activate Test Step.
///
/// \par Sequence
/// - Go to factory mode with the Osram Command "Set Factory Mode"
/// - Set with DALI Command 257 "Data Transfer Register" the test Id in the DTR register
/// - Start test function with the twice Osram Command 1032 "Activate Test Step"
///
///
/// \param .
///
/// \retval			nothing
//---------------------------------------------------------------------------------------------------------------------------
TEST_RESPONSE_t ProductionTest_Step( uint8_t step, uint8_t dtr1, uint8_t dtr2 )
{
//    TEST_RESPONSE_t queryAnswered = TEST_NOT_SUPPORTED;                                                                                            // set true as default, set to false, if not supported
//
//    uint16_t word_value;
//
//    word_value = dtr2 << 8;
//    word_value |= dtr1;
//
//    switch( step )
//    {
//
//    case 1:         // add your test steps here ..
//        break;
//
//    case 20:
//        if ( word_value == 0xB15A )
//        {
//            GodStopDelayedStart();
//            m_calibration_enable = true;
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 40:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_start, 0 );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 41:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_calculate_vin, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 42:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_set_current_1, 0 );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 43:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_store_current_1, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 44:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_store_vout_1, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 45:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_set_current_2, 0 );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 46:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_store_current_2_and_calculation, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 47:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_store_vout_2_and_calculation, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 48:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_ledset_calculation, word_value );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 49:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_step_end, 0 );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//    case 50:
//        if ( m_calibration_enable )
//        {
//            calibration_command( calibration_clear_data, 0 );
//            queryAnswered = TEST_DONE;
//        }
//        break;
//
//
//    case 60:
//        if ( word_value == 0x4CE9 )
//        {
//            XMC1000_BmiInstallationReq(0xf9c3);
//        }
//        break;
//
//    case 199:       // .. valid numbers are in the range 0 <= test step <= 199
//        break;
//
//    default:
//        queryAnswered = TEST_NOT_SUPPORTED;
//        break;
//    }
//    return queryAnswered;
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief Production test functions - Query Test Value.
///
/// \par Sequence
/// - Go to factory mode with the Osram Command "Set Factory Mode"
/// - Set with DALI Command 257 "Data Transfer Register" the value Id in the DTR register
/// - Query value with the Osram Command 1016 "Query Test Value"
///
/// \param daliDevice	 	Pointer to the TypDaliDevice data structure.
///
/// \retval			nothing
//---------------------------------------------------------------------------------------------------------------------------
TEST_RESPONSE_t ProductionTest_Query( uint8_t query, uint8_t* pDtr1, uint8_t* pDtr2, uint8_t channel )
{
//    TEST_RESPONSE_t queryAnswered = TEST_DONE;                                                    // set true as default, set to false, if not supported
//
//    uint16_t tmp = 0;
//
//    switch( query )
//    {
//    case 10:         // Uline1 mean
//        tmp = scale_value_to_16( Uline1.mean, 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 11:         // Uline2 mean
//        tmp = scale_value_to_16( Uline2.mean, 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 12:         // Urail mean
//        tmp = scale_value_to_16( Urail.mean, 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 13:         // Uled mean
//        tmp = scale_value_to_16( Uled.mean, 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 14:         // Uledset mean
//        tmp = scale_value_to_16( Uledset.mean, 1, 1000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 15:         // Uline1 raw
//        tmp = scale_value_to_16( Uline1.raw, 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 16:         // Uline2 raw
//        tmp = scale_value_to_16( Uline2.raw, 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 17:         // Urail raw
//        tmp = scale_value_to_16( Urail.raw, 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 18:         // Uled raw
//        tmp = scale_value_to_16( Uled.raw, 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 19:         // Uledset raw
//        tmp = scale_value_to_16( Uledset.raw, 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 20:         // uC temperature
//        tmp = *currentset_feedback_struct.temp_sensor_actual;
//
//        if ( tmp > 273 )
//        {
//            tmp = tmp - 273;
//        }
//        else
//        {
//            tmp = 0;
//        }
//
//        *pDtr2 = (uint8_t)(tmp);
//        *pDtr1 = 0;
//        break;
//
//    case 21:        // calibration result
//        *pDtr2 = calibration_get_result();
//        *pDtr1 = 0;
//        break;
//
//    case 23:    // buck output current
////        tmp = scale_value_to_16( hBuck_get_output_current(), 1, 1000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 24:    // buck output voltage
////        tmp = scale_value_to_16( hBuck_get_output_voltage(), 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 25:    // buck low delay
////        tmp = scale_value_to_16( hbuck_get_low_delay(), 1, 1 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 26:    // Analog compensated Vout
//        tmp = scale_value_to_16( analog_get_compensated_vout(), 1, 100000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 27:
//        tmp = scale_value_to_16( Umaxref.mean, 1, 1000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 28:    // Currentset nominal current
//        tmp = scale_value_to_16( *currentset_feedback_struct.nominal_current, 1, 1000 );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 29:    // GOD error
//        *pDtr2 = (uint8_t)god_feedback_struct.error_message;
//        *pDtr1 = 0;
//        break;
//
//    case 30:    // Buck error
////        *pDtr2 = (uint8_t)buck_feedback_struct.error;
////        *pDtr1 = 0;
//        break;
//
//    case 31:    // Firmware version
//        tmp = (DEVICE_FW_VERSION_MAJOR << 8) | DEVICE_FW_VERSION_MINOR;
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 32:    // SVN revision
//       // tmp = SVN_REV_ROOT;
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 33:    // SVN Timestamp lo
//       // tmp = (uint16_t)SVN_CHECK_TIMESTAMP;
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 34:    // SVN Timestamp hi
//       // tmp = (uint16_t)(SVN_CHECK_TIMESTAMP >> 16);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 35:    // Global DMAT lo
//        tmp = (uint16_t)GLOBAL_DMAT_NO;
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 36:    // Global DMAT hi
//        tmp = (uint16_t)(GLOBAL_DMAT_NO >> 16);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 38:    // buck m
//        tmp = (uint16_t)calibration_get_data( calibration_idx_buck_m );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 39:    // Buck q
//        tmp = (uint16_t)calibration_get_data( calibration_idx_buck_q );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 40:    // Buck lt
//        tmp = (uint16_t)calibration_get_data( calibration_idx_buck_lt );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 41:    // vled
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_vled ) >> 1);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 42:    // vled2
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_vled2 ) >> 1);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 43:    // ledset
//        tmp = (uint16_t)calibration_get_data( calibration_idx_ledset );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 44:    // temperature
//        tmp = (uint16_t)calibration_get_data( calibration_idx_temperature );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 45:    // vline1
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_vline1 ) >> 2);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 46:    // vline2
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_vline2 ) >> 2);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 47:    // vrail
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_vrail ) >> 2);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 48:    // calibrated bitfield
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_calibrated ) );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 49:    // RAW filtered vout value
//        tmp = calibration_get_filtered_vout_raw();
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 50:    // Uledset scale value
//        tmp = (uint16_t)(calibration_get_data( calibration_idx_uledset ) );
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 51:    // Global Basic code string 7-6
//        tmp = ((uint8_t)GLOBAL_BASIC_CODE_STRING[7])<<8+((uint8_t)GLOBAL_BASIC_CODE_STRING[6]);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 52:    // Global Basic code string 5-4
//        tmp = ((uint8_t)GLOBAL_BASIC_CODE_STRING[5])<<8+((uint8_t)GLOBAL_BASIC_CODE_STRING[4]);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 53:    // Global Basic code string 3-2
//        tmp = ((uint8_t)GLOBAL_BASIC_CODE_STRING[3])<<8+((uint8_t)GLOBAL_BASIC_CODE_STRING[2]);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//    case 54:    // Global Basic code string 1-0
//        tmp = ((uint8_t)GLOBAL_BASIC_CODE_STRING[1])<<8+((uint8_t)GLOBAL_BASIC_CODE_STRING[0]);
//        separate_bytes( tmp, pDtr1, pDtr2 );
//        break;
//
//
//    case 199:       // .. valid numbers are in the range 0 <= test step <= 199
//        break;
//
//    default:
//        queryAnswered = TEST_DONE;
//        break;
//    }

 //   return queryAnswered;
}


