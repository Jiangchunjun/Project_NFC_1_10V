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
// $Id: test_supplymonitor_basic.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_supplymonitor_basic.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "unity_fixture.h"
#include "fff.h"
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


DEFINE_FFF_GLOBALS;

//#define static

//------------------------------------------------------------------------------
// unit under test functions
//------------------------------------------------------------------------------
//uint8_t basic(uint8_t *buffer, uint8_t byte_cnt);

//------------------------------------------------------------------------------
// global module external declared variables
//------------------------------------------------------------------------------
void StdoutString(const char* str)
{
}
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
uint32_t iteration_count;
//extern supply_monitor_state_t Supply;
//------------------------------------------------------------------------------
// local test variables
//------------------------------------------------------------------------------
void SupplyMonitorInit(void);
void SupplyMonitorCyclicTask(void);

# define M_PI           3.14159265358979323846  /* pi */
# define MY_SQRT2        1.4142135623731

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------
void filter_si_unit_values(double uline1, double uline2, double urail)
{
    double uline1_last;
    double uline2_last;
    double urail_last;

    uline1_last=uline1/ULINE1_ADC_FACTOR*1.0E6;
    uline2_last=uline2/ULINE2_ADC_FACTOR*1.0E6;
    urail_last=urail/URAIL_ADC_FACTOR*1.0E6;

    //FilterInvoke(&Uline1,(int)(1000.0*uline1)); // uline1 is in V
    FilterInvoke(&Uline1,(uint32_t)uline1_last); // uline1 is in V
    FilterInvoke(&Uline2,(uint32_t)uline2_last);
    FilterInvoke(&Urail,(uint32_t)urail_last);
}

//------------------------------------------------------------------------------
void create_ac_ramp(double uline_max_rms, double ramp_up_time, double freq, double time)
{
    double uline1;
    double uline2;
    double urail;
    double ul;



    if (time<ramp_up_time)
    {
        ul=(sin(2*M_PI*freq*time))*(MY_SQRT2*uline_max_rms/ramp_up_time*time);
    }
    else
    {
        ul=(sin(2*M_PI*freq*time))*(MY_SQRT2*uline_max_rms);
    }
    if (ul>0)
    {
        uline1=ul;
        uline2=0;
    }
    else
    {
        uline1=0;
        uline2=-ul;
    }
    urail=0;

    filter_si_unit_values(uline1, uline2, urail);
}

//------------------------------------------------------------------------------
void create_dc_ramp(double uline_max_rms, double ramp_up_time, double time)
{
    double uline1;
    double uline2;
    double urail;
    double ul;



    if (time<ramp_up_time)
    {
        ul = MY_SQRT2*uline_max_rms/ramp_up_time*time;
    }
    else
    {
        ul = MY_SQRT2*uline_max_rms;
    }
    if (ul>0)
    {
        uline1=ul;
        uline2=0;
    }
    else
    {
        uline1=0;
        uline2=-ul;
    }
    urail=0;

    filter_si_unit_values(uline1, uline2, urail);
}

//------------------------------------------------------------------------------
// SupplymonitorCyclic
//------------------------------------------------------------------------------
TEST_GROUP(supplymonitor_basic);

TEST_SETUP(supplymonitor_basic)
{
    FilterInit(&Uline1,  ULINE1_ADC_FACTOR,  FILTERSPEED(SYSTICK_INT_Hz, 0.020),0); // tau = 20ms  @todo: set correct tau
    FilterInit(&Uline2,  ULINE2_ADC_FACTOR,  FILTERSPEED(SYSTICK_INT_Hz, 0.020),0); // tau = 20ms  @todo: set correct tau
    FilterInit(&Urail,   URAIL_ADC_FACTOR,   FILTERSPEED(SYSTICK_INT_Hz, 0.100),0); // tau = 100ms  @todo: set correct tau


}

TEST_TEAR_DOWN(supplymonitor_basic)
{

}

//------------------------------------------------------------------------------
/** test checksum calculation */
TEST(supplymonitor_basic, test_initialization)
{
    SupplyMonitorInit();
    ASSERT_VERB_EQUAL( sm_mode_reset,  Supply.flags.SM_MODE, "Supply.flags.SM_MODE should be sm_mode_reset");

}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_curve_display)
{
    double time;
    FILE *csv_file;
    char filename1[50]="voltage_test_curves.txt";

    time=0;

    csv_file = fopen(filename1, "w");
    if (csv_file == NULL)
    {
        printf("Error opening file %s!\n", filename1);
    }

    fprintf(csv_file,"Time;Uline1;Uline2;Urail;Uline1.mean \n");

    SupplyMonitorInit();
    for (time=0; time<1.010; time=time+(SUPPLYMON_STAMP_us/1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(230, 0.1, 50, time);
        SupplyMonitorCyclicTask();
        fprintf(csv_file,"%8.6f ; %3.0f ; %3.0f ; %3.0f ; %3.0f \n", time, (double)Uline1.last, (double)Uline2.last, (double)Urail.last, (double)Uline1.mean );
        //fprintf(csv_file,"%8.6f ; %3.0f ; %3.0f ; %3.0f ; %u \n", time, (double)Uline1.last, (double)Uline2.last, (double)Urail.last, Supply.Uline2 );
    }
    fclose(csv_file);
}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_0_volt)
{
    double time;
    double state_timeout;

    SupplyMonitorInit();

    time=0;
    state_timeout=(suMoCoP->state_timeout)*SUPPLYMON_EVAL_us/1.0E6;


    for (time=0; time<state_timeout; time=time+(SUPPLYMON_STAMP_us/1E6))
    {
        create_ac_ramp(0, 0, 50, time); //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        SupplyMonitorCyclicTask();

        ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE,
                          "Supply.flags.SM_MODE should be sm_mode_reset @ Uin=0 before state_timeout is reached!  TIME: %f  Uline1.mean: %u;",time, Uline1.mean);
    }

    for (; time<(state_timeout+(2*SUPPLYMON_EVAL_us/1.0E6)); time=time+(SUPPLYMON_STAMP_us/1E6))
    {
        create_ac_ramp(0, 0, 50, time); //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        SupplyMonitorCyclicTask();

    }

    ASSERT_VERB_EQUAL(sm_mode_15V, Supply.flags.SM_MODE,
                      "Supply.flags.SM_MODE should be sm_mode_15V @ Uin=0 latest after 2 additional Eval periodes!  TIME: %f  Uline1.mean: %u; %u",time, Uline1.mean, fsm_cycle_cnt);

}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_230_volt)
{
    double time;

    time=0;
    SupplyMonitorInit();
    iteration_count=0;
    for (time=0; time<1; time=time+(SUPPLYMON_STAMP_us/1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(230, 0.1, 50, time);

        SupplyMonitorCyclicTask();
        iteration_count++;

        //ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE,
        //                          "TESTXXXX Supply.flags.SM_MODE   TIME: %f  Supply.uline2: %u;",time, Supply.uline2);

//    if (sm_mode_voltage_calibration==Supply.flags.SM_MODE) {
//        printf("Supply.uline2: %u   \n ", Supply.uline2 );
//        printf("Time: %f ", time);
//    }

        if (time<0.022)
        {
            ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "Supply.flags.SM_MODE is sm_mode_reset at time<0.022s! TIME: %f",time);
        }
        if (time>0.1)
        {
            ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "Supply.flags.SM_MODE is sm_mode_230V at time>0.11s!");
            ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE, "Supply.flags.SM_POWERTYPE is AC at time>0.11s!");
        }
        if (sm_mode_230V==Supply.flags.SM_MODE)
        {
            ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE, "Supply.flags.SM_POWERTYPE is AC if SM_MODE==sm_mode_230V!");
        }
    }
    //ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE,
    //                            "Supply.flags.SM_MODE should be not sm_mode_reset when Uin is 230V TEST TO BE REWORKED!  TIME: %f  Uline1.mean: %u;",time, Uline1.mean);
    printf("Output  \n ");
    printf("Uline1.mean: %u   \n ", Uline1.mean );
    printf("sm_state of FSM %u  \n  ", sm_state );

    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE,
                      "Supply.flags.SM_MODE should be not sm_mode_230V when Uin is 230V!  TIME: %f  Uline1.mean: %u;",time, Uline1.mean);


}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_15_volt)
{
    double time;
    double state_timeout;
    uint32_t ramp_voltage;

    SupplyMonitorInit();

    time = 0;
    iteration_count = 0;
    ramp_voltage = (uint32_t) (
                       ((supplyMonConstants15V.exit_voltage / 1E6) - 1) / MY_SQRT2);
    state_timeout = (suMoCoP->state_timeout * SUPPLYMON_EVAL_us / 1.0E6);

    for (time = 0; time < state_timeout;
            time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        create_ac_ramp(ramp_voltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
        iteration_count++;
        ASSERT_VERB_EQUAL(
            sm_mode_reset, Supply.flags.SM_MODE,
            "Supply.flags.SM_MODE is sm_mode_reset at the beginning!"
            "   TIME: %f  Supply.uline_diff_abs: %u;",
            time, Supply.uline_diff_abs);
    }

    for (; time < state_timeout + (2 * SUPPLYMON_EVAL_us / 1.0E6);
            time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        create_ac_ramp(ramp_voltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
        iteration_count++;
    }

    ASSERT_VERB_EQUAL(
        sm_mode_15V,
        Supply.flags.SM_MODE,
        "Supply.flags.SM_MODE should be sm_mode_15V at less exit voltage input after 1.004s"
        " -> TIME: %f  Supply.uline_diff_abs: %u;",
        time, Supply.uline_diff_abs);

    ASSERT_VERB_EQUAL(sm_state_15V, sm_state,
                      "sm_state should be still sm_state_15 after 1.004s");
}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_20_volt)
{
    double i1;
    uint32_t ramp_voltage;

    ramp_voltage = (uint32_t) (
                       ((supplyMonConstants15V.exit_voltage / 1E6) + 1) / MY_SQRT2);

    SupplyMonitorInit();
    iteration_count = 0;
    for (i1 = 0; i1 < (2 * SUPPLYMON_EVAL_FACTOR); i1++)
    {
        create_dc_ramp(ramp_voltage, 0.0, i1);
        SupplyMonitorCyclicTask();
        iteration_count++;
    }
    ASSERT_VERB_EQUAL(
        sm_state_48V, sm_state,
        "sm_state should switch to sm_state_48V after 2.0s at 20Vac");
}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_293_volt)
{
    double time;

    time = 0;
    iteration_count = 0;

    SupplyMonitorInit();

    for (time = 0; time < 1.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(293, 0.0, 50, time);
        SupplyMonitorCyclicTask();
        iteration_count++;

        if (time > 0.04)
        {
            ASSERT_VERB_EQUAL(
                sm_mode_voltage_calibration,
                Supply.flags.SM_MODE,
                "Supply.flags.SM_MODE is sm_mode_voltage_calibration at time>0.04s!");
            ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                              "Supply.flags.SM_POWERTYPE is AC at time>0.04s!");
        }
        if (sm_mode_voltage_calibration == Supply.flags.SM_MODE)
        {
            ASSERT_VERB_EQUAL(
                AC, Supply.flags.SM_POWERTYPE,
                "Supply.flags.SM_POWERTYPE is AC if SM_MODE==sm_mode_230V!");
        }

    }
    ASSERT_VERB_EQUAL(
        sm_mode_voltage_calibration, Supply.flags.SM_MODE,
        "Supply.flags.SM_MODE should be sm_mode_voltage_calibration!");

    ASSERT_VERB_EQUAL(
        sm_state_293V,
        sm_state,
        "sm_state should stay at sm_state_293V for calibration after 2.0s at 293Vac");
}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_293_volt_and_go_down_to_230_volt)
{
    double time;
    time = 0;
    SupplyMonitorInit();
    for (time = 0; time < 1.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(293, 0.0, 50, time);
        SupplyMonitorCyclicTask();
        if (time > 0.04)
        {
            ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                              "Supply.flags.SM_POWERTYPE is AC at time>0.04s!");
            ASSERT_VERB_EQUAL(
                sm_mode_voltage_calibration,
                Supply.flags.SM_MODE,
                "Supply.flags.SM_MODE is sm_mode_voltage_calibration at time>0.04s!");
        }
        if (sm_mode_voltage_calibration == Supply.flags.SM_MODE)
        {
            ASSERT_VERB_EQUAL(
                AC, Supply.flags.SM_POWERTYPE,
                "Supply.flags.SM_POWERTYPE is AC if SM_MODE==sm_mode_293V!");
        }
    }
    ASSERT_VERB_EQUAL(
        sm_mode_voltage_calibration, Supply.flags.SM_MODE,
        "Supply.flags.SM_MODE should be sm_mode_voltage_calibration!");

    for (time = 1.0; time < 2.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(230, 0.0, 50, time);
        SupplyMonitorCyclicTask();
        //ASSERT_VERB_EQUAL(sm_mode_voltage_calibration, Supply.flags.SM_MODE,
        //                          "Supply.flags.SM_MODE should be not sm_mode_voltage_calibration when Uin is 230V!  TIME: %f  Uline1.mean: %u;",time, Supply.uline2);
        if (time > 1.1)
        {
            ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                              "Supply.flags.SM_POWERTYPE is AC at time>1.1s!");
            ASSERT_VERB_EQUAL(
                sm_mode_230V, Supply.flags.SM_MODE,
                "Supply.flags.SM_MODE is sm_mode_230V at time>1.1s!");
        }
    }
}

//------------------------------------------------------------------------------
TEST(supplymonitor_basic, test_ramp_up_AC_check_lower_level_and_hysteresis)
{
    // stay 1s and 1V below turn on threshold voltage (no start)
    // stay 1s and 1V above turn on threshold voltage (start)
    // stay 1s and 1V above turn off threshold voltage (keep on running)
    // stay 1s and 1V below turn off threshold voltage (stop with PF)

    double linevoltage;
    double time;

    time = 0;

    SupplyMonitorInit();

    linevoltage = (((supplyMonConstants230VACnarrow.absMinVolt) / 1E6))
                  / (sqrt(2)) - 1.0;

    for (time = 0; time < 1.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(linevoltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
    }
    //ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE, "Supply.flags.SM_POWERTYPE is AC after 1s!");
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE,
                      "Supply.flags.SM_MODE is sm_mode_reset ");

    ASSERT_VERB_EQUAL(
        sm_state_230VACnarrow,
        sm_state,
        "sm_state should be sm_state_230VACnarrow after 1s for less voltage"
        " than 230VACnarrow.absMinVolt ");


    linevoltage = linevoltage + 2.0;
    for (time = 1.0; time < 2.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(linevoltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
    }
    ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                      "Supply.flags.SM_POWERTYPE is AC after 2s!");
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE,
                      "Supply.flags.SM_MODE is sm_mode_230V after 2s!");

    ASSERT_VERB_EQUAL(
        sm_state_230VACwide,
        sm_state,
        "sm_state should be sm_state_230VACwide after 1s for more voltage"
        " than 230VACnarrow.absMinVolt ");

    // .shutdownVoltSqrPosTh = SQUARE( VOLTS(286) >> 16 ) << 6,          // means 286V @ AC 10 V Hysteresis
    linevoltage = (double) supplyMonConstants230VACwide.shutdownVoltSqrPosTh;
    linevoltage = linevoltage / 64;
    linevoltage = sqrt(linevoltage);
    linevoltage = linevoltage * 65536;
    linevoltage = linevoltage / 1000000;
    linevoltage = linevoltage - 1;

    for (time = 2.0; time < 3.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(linevoltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
    }
    ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                      "Supply.flags.SM_POWERTYPE is AC after 3s!");
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE,
                      "Supply.flags.SM_MODE is sm_mode_230V after 3s!");

    linevoltage = linevoltage + 12;
    for (time = 3.0; time < 5.0; time = time + (SUPPLYMON_STAMP_us / 1E6))
    {
        //create_ac_ramp(uline_max_rms, ramp_up_time, freq, time)
        create_ac_ramp(linevoltage, 0.0, 50, time);
        SupplyMonitorCyclicTask();
    }

    ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE,
                      "Supply.flags.SM_POWERTYPE is AC after 5s!");
    ASSERT_VERB_EQUAL(OV_YES, Supply.flags.SM_OVERVOLTAGE,
                      "Supply.flags.SM_OVERVOLTAGE is OV_YES after 5s!");

    ASSERT_VERB_EQUAL(
        0,
        time,
        "WARNING SM-Tests are not sufficient, due to inaccuracy within OVERVOLTAGE RMS CALCULATION!!");

}





//----------------------------------------------------------------------------------------------------------------------
// Test Runner
//----------------------------------------------------------------------------------------------------------------------
// SupplymonitorCyclic
TEST_GROUP_RUNNER(supplymonitor_basic)
{
//    RUN_TEST_CASE(supplymonitor_basic, test_curve_display);
//    RUN_TEST_CASE(supplymonitor_basic, test_initialization);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_0_volt);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_230_volt);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_15_volt);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_20_volt);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_293_volt);
//    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_293_volt_and_go_down_to_230_volt);
    RUN_TEST_CASE(supplymonitor_basic, test_ramp_up_AC_check_lower_level_and_hysteresis);

}

