// ---------------------------------------------------------------------------------------------------------------------
// Firmware for SSL Full Digital M2 SELV Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     LLS D EC EM - GCC
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
// Initial version:
// $Author: F.Branchetti $
// $Revision: 4738 $
// $Date: 2016-07-08 17:31:17 +0800 (Fri, 08 Jul 2016) $
// $Id: test_supplymonitor_integration.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_supplymonitor_integration.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "unity_fixture.h"
#include "unit_testing_utilities.h"

//#include "global.h"

#include "filter.h"
//#include "analog.h"
#include "analog_userinterface.h"
//#include "parameters.h"
#include "parameter.h"
#include "analog_parameters.h"
#include "analog_userinterface.h"


#include "supplymonitor_userinterface.h"
#include "supplymonitor_local.h"
#include "SupplyMonitorDefaults.h"
#include "test_supplymonitor_overwrites.h"


#ifdef NDEBUG
// if called from IAR EWARM
char test_data_path[] = "../test_data";
#else
// if called from Makefile
char test_data_path[] = "tests/test_data";
#endif


//#define static

//------------------------------------------------------------------------------
// unit under test functions
//------------------------------------------------------------------------------
void SupplyMonitorInit(void);
void SupplyMonitorCyclicTask(void);

//------------------------------------------------------------------------------
// global module external declared variables
//------------------------------------------------------------------------------
extern Filter_t Uline1;
extern Filter_t Uline2;
extern Filter_t Urail;
extern uint32_t evaluate_timer;
extern const supply_monitor_constants_t supplyMonConstants15V;
extern const supply_monitor_constants_t supplyMonConstants48V;
extern const supply_monitor_constants_t supplyMonConstants293V;
extern const supply_monitor_constants_t supplyMonConstants230VDCnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VDCwide;
extern const supply_monitor_constants_t supplyMonConstants230VACnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VACwide;
//extern supply_monitor_state_t Supply;

//------------------------------------------------------------------------------
// global test variables
//------------------------------------------------------------------------------
uint32_t integration_iteration_count;

//------------------------------------------------------------------------------
// local test variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------
uint32_t set_next_values_from_testfile(FILE *file)
{
    uint32_t            uline1;
    uint32_t            uline2;
    uint32_t            return_value;


    if(process_test_file_uint32_2(file, &uline1, &uline2))
    {
        FilterInvoke(&Uline1,(uint32_t)(uline1/ULINE1_ADC_FACTOR*1.0E6));
        FilterInvoke(&Uline2,(uint32_t)(uline2/ULINE1_ADC_FACTOR*1.0E6));
        return -1;
    }

    return 0;
}


//------------------------------------------------------------------------------
// SupplymonitorCyclic
//------------------------------------------------------------------------------
TEST_GROUP(supplymonitor_integration);

TEST_SETUP(supplymonitor_integration)
{
    FilterInit(&Uline1,  ULINE1_ADC_FACTOR,  FILTERSPEED(SYSTICK_INT_Hz, 0.020),0); // tau = 20ms  @todo: set correct tau
    FilterInit(&Uline2,  ULINE2_ADC_FACTOR,  FILTERSPEED(SYSTICK_INT_Hz, 0.020),0); // tau = 20ms  @todo: set correct tau
    FilterInit(&Urail,   URAIL_ADC_FACTOR,   FILTERSPEED(SYSTICK_INT_Hz, 0.100),0); // tau = 100ms  @todo: set correct tau


}

TEST_TEAR_DOWN(supplymonitor_integration)
{

}

//------------------------------------------------------------------------------

TEST(supplymonitor_integration, test_ramp_up_20_volt)
{
    double    time;
    char      filename[255];
    FILE      *file;

    // open test data file
    sprintf(filename, "%s/%s",test_data_path, "2016_01_13_OTi_DALI_35_UF_BSample_No_PE_No_Probe_293VAC_to_230VAC_6811B.csv");
    file = open_test_file(filename);

    SupplyMonitorInit();
    integration_iteration_count=0;

    while (set_next_values_from_testfile(file))
    {
        SupplyMonitorCyclicTask();
        integration_iteration_count++;
        ASSERT_VERB_EQUAL(
            sm_mode_reset, Supply.flags.SM_MODE,
            "Supply.flags.SM_MODE should stay sm_mode_reset at 20V input.");
    }


    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state,
                      "sm_state should switch to sm_state_230VDCnarrow after 2.0s at 20Vac");

    // close test data file
    fclose(file);
}


//------------------------------------------------------------------------------
// Test Runner
//------------------------------------------------------------------------------
// SupplymonitorCyclic
TEST_GROUP_RUNNER(supplymonitor_integration)
{
    RUN_TEST_CASE(supplymonitor_integration, test_ramp_up_20_volt);

}

