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
// $Id: test_supplymonitor_level_fsm.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_supplymonitor_level_fsm.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//#include "global.h"

#include "filter.h"
//#include "analog.h"
#include "analog_userinterface.h"
#include "parameter.h"
//#include "parameters.h"

#include "supplymonitor_userinterface.h"
#include "supplymonitor_local.h"
#include "SupplyMonitorDefaults.h"
#include "test_supplymonitor_overwrites.h"

#include "unity_fixture.h"
#include "unit_testing_utilities.h"

//#define static

//------------------------------------------------------------------------------
// unit under test functions
//------------------------------------------------------------------------------
void SupplyMonitorInit(void);
void SupplyMonitorFSM(void);
void ResetEva(void);
// hier local funktionen bekannt machen OLAF
//------------------------------------------------------------------------------
// global module external declared variables
//------------------------------------------------------------------------------
//extern const supply_monitor_constants_t supplyMonConstantsStandard;
//extern const supply_monitor_constants_t supplyMonConstantsLowVoltPrg;
extern const supply_monitor_constants_t supplyMonConstants15V;
extern const supply_monitor_constants_t supplyMonConstants48V;
extern const supply_monitor_constants_t supplyMonConstants293V;
extern const supply_monitor_constants_t supplyMonConstants230VDCnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VDCwide;
extern const supply_monitor_constants_t supplyMonConstants230VACnarrow;
extern const supply_monitor_constants_t supplyMonConstants230VACwide;





//extern supply_monitor_state_t Supply;
//------------------------------------------------------------------------------
// local test variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SupplymonitorCyclic
//------------------------------------------------------------------------------
TEST_GROUP(supplymonitor_level_fsm);

TEST_SETUP(supplymonitor_level_fsm)
{

}

TEST_TEAR_DOWN(supplymonitor_level_fsm)
{

}

//------------------------------------------------------------------------------
/** test checksum calculation */
TEST(supplymonitor_level_fsm, test_init)
{
    sm_state=sm_state_48V;
    SupplyMonitorInit();
    ASSERT_VERB_EQUAL(sm_state_reset, sm_state, "Initial State is State Reset");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants15V, suMoCoP, "should point to 15V after init instead of nowhere");
}
TEST(supplymonitor_level_fsm, test_state_change_from_reset_to_15V)
{
    SupplyMonitorInit();
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_15V, sm_state, "state should be: sm_state_15V after first call");
    ASSERT_VERB_EQUAL(0, fsm_cycle_cnt, "cnt should be initialized");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants15V, suMoCoP, "should point to supplyMonConstants15V");
}
/**
test:
- sm_state_15V
- sm_mode switched from sm_mode_reset to sm_mode_15V after fsm_cycle_cnt exceeds suMoCoP.modus_timeout
*/
TEST(supplymonitor_level_fsm, test_sm_mode_15V_is_set_after_timeout)
{
    Supply.flags.SM_MODE=sm_mode_reset;
    sm_state=sm_state_15V;
    suMoCoP = &supplyMonConstants15V;
    //Supply.uline2=supplyMonConstants15V.exit_voltage+1;

    fsm_cycle_cnt=0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if time is not reached");

    fsm_cycle_cnt=supplyMonConstants15V.state_timeout+1;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_15V, Supply.flags.SM_MODE, "mode should be: sm_mode_15 after cnt exceeds time");
}

TEST(supplymonitor_level_fsm, test_state_change_from_15V_to_48V_voltage_exceeded)
{
    sm_state=sm_state_15V;
    suMoCoP = &supplyMonConstants15V;
    Supply.uline_diff_abs=supplyMonConstants15V.exit_voltage+1;
    Supply.flags.SM_POWERTYPE = NA;
    fsm_cycle_cnt = 0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_48V, sm_state, "state should be: sm_state_48V after uline2 exceeds level");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants48V, suMoCoP, "should point to supplyMonConstants48V");
}


TEST(supplymonitor_level_fsm, test_state_change_from_48V_to_293V_voltage_exceeded)  // Test state change voltage exceed
{
    sm_state=sm_state_48V;
    suMoCoP = &supplyMonConstants48V;
    Supply.uline2=supplyMonConstants48V.absMinVolt+1;
    Supply.flags.SM_POWERTYPE = AC;
    fsm_cycle_cnt = 0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_48V, Supply.flags.SM_MODE, "mode should be: sm_mode_48 when AC flag is set");

    Supply.uline_diff_abs=supplyMonConstants48V.exit_voltage+1;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_293V, sm_state, "state should be: sm_state_293V after uline2 exceeds level");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants293V, suMoCoP, "should point to supplyMonConstants293V");
}
TEST(supplymonitor_level_fsm, test_state_change_from_48V_to_293V_after_cnt_exceeds_time)  // Test time exceed
{
    sm_state=sm_state_48V;
    suMoCoP = &supplyMonConstants48V;
    //Supply.uline2=supplyMonConstants48V.absMinVolt+1;
    Supply.uline_diff_abs=supplyMonConstants48V.exit_voltage;
    Supply.flags.SM_POWERTYPE = DC;
    fsm_cycle_cnt = 0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_48V, Supply.flags.SM_MODE, "mode should be: sm_mode_48 when DC flag is set");
    fsm_cycle_cnt=supplyMonConstants48V.state_timeout+1;
    //Supply.flags.DC = 0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_293V, sm_state, "state should be: sm_state_293V after cnt exceeds time and no DC or AC flag");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants293V, suMoCoP, "should point to supplyMonConstants293V");
}

TEST(supplymonitor_level_fsm, test_state_change_from_293V_to_230Vnarrow_when_overvoltage)
{
    sm_state=sm_state_293V;
    suMoCoP = &supplyMonConstants293V;
    Supply.uline2=supplyMonConstants293V.absMinVolt+1;
    Supply.flags.SM_POWERTYPE = AC;
    fsm_cycle_cnt = 0;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_voltage_calibration, Supply.flags.SM_MODE, "mode should be: sm_mode_voltage_calibration when AC flag is set");

    Supply.uline_diff_abs=supplyMonConstants293V.exit_voltage+1;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should be: sm_state_230VDCnarrow after overvoltage is detected");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCnarrow, suMoCoP, "should point to supplyMonConstants293V");
}
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
/* XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX*/
TEST(supplymonitor_level_fsm, test_state_change_from_230VDCnarrow_to_230VDCwide_when_DC_and_OV_NO)
{
    sm_state=sm_state_230VDCnarrow;
    Supply.flags.SM_MODE = sm_mode_reset;
    suMoCoP = &supplyMonConstants230VDCnarrow;
    Supply.flags.SM_POWERTYPE = DC;
    Supply.flags.SM_OVERVOLTAGE = OV_YES;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if start is not possible");
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should stay: sm_state_230VDCnarrow if start is not possible");

    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V when DC and OV_NO is set");
    ASSERT_VERB_EQUAL(sm_state_230VDCwide, sm_state, "state should be: sm_state_230VDCwide after first detecting DC and OV_NO");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCwide, suMoCoP, "should point to supplyMonConstants230VDCwide");
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VACwide_when_AC_and_OV_NO)
{
    sm_state=sm_state_230VACnarrow;
    Supply.flags.SM_MODE = sm_mode_reset;
    suMoCoP = &supplyMonConstants230VACnarrow;
    Supply.flags.SM_POWERTYPE = AC;
    Supply.flags.SM_OVERVOLTAGE = OV_YES;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if start is not possible");
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should stay: sm_state_230VACnarrow if start is not possible");

    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V when AC and OV_NO is set");
    ASSERT_VERB_EQUAL(sm_state_230VACwide, sm_state, "state should be: sm_state_230VACwide after first detecting AC and OV_NO");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VACwide, suMoCoP, "should point to supplyMonConstants230VDCwide");
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VDCnarrow_to_230VACnarrow_when_AC)
{
    sm_state=sm_state_230VDCnarrow;
    Supply.flags.SM_MODE = sm_mode_reset;
    suMoCoP = &supplyMonConstants230VDCnarrow;
    Supply.flags.SM_POWERTYPE = NA;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should stay: sm_state_230VDCnarrow if nothing happend");
    Supply.flags.SM_POWERTYPE = AC;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should stay: sm_mode_reset when switch to ACnarrow");
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should be: sm_state_230VACnarrow after switching to ACnarrow");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VACnarrow, suMoCoP, "should point to supplyMonConstants230VACnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called");
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VDCnarrow_when_DC)
{
    sm_state=sm_state_230VACnarrow;
    Supply.flags.SM_MODE = sm_mode_reset;
    suMoCoP = &supplyMonConstants230VACnarrow;
    Supply.flags.SM_POWERTYPE = NA;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should stay: sm_state_230VACnarrow if nothing happend");
    Supply.flags.SM_POWERTYPE = DC;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should stay: sm_mode_reset when switch to DCnarrow");
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should be: sm_state_230VDCnarrow after switching to DCnarrow");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCnarrow, suMoCoP, "should point to supplyMonConstants230VDCnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VDCnarrow_when_PF)
{
    sm_state=sm_state_230VACnarrow;
    Supply.flags.SM_MODE = sm_mode_reset;
    suMoCoP = &supplyMonConstants230VACnarrow;
    Supply.flags.SM_POWERTYPE = NA;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should be: sm_mode_reset if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should stay: sm_state_230VACnarrow if nothing happend");
    Supply.flags.SM_POWERTYPE = PF;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_reset, Supply.flags.SM_MODE, "mode should stay: sm_mode_reset when switch to DCnarrow");
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should be: sm_state_230VDCnarrow after switching to DCnarrow");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCnarrow, suMoCoP, "should point to supplyMonConstants230VDCnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
/* XXXXXXXXXXXXXX Tests from wide to narrow and wide to wide XXXXXXXXXXXX */
TEST(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VDCnarrow_when_PF)
{
    sm_state=sm_state_230VDCwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VDCwide;
    Supply.flags.SM_POWERTYPE = DC;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VDCwide, sm_state, "state should stay: sm_state_230VDCwide if nothing happend");
    Supply.flags.SM_POWERTYPE = PF;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_powerfail, Supply.flags.SM_MODE, "mode should goto: sm_mode_powerfail if PF");
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should be: sm_state_230VDCnarrow after PF");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCnarrow, suMoCoP, "should point to supplyMonConstants230VDCnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VDCnarrow_when_OV_YES)
{
    sm_state=sm_state_230VDCwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VDCwide;
    Supply.flags.SM_POWERTYPE = DC;
    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VDCwide, sm_state, "state should stay: sm_state_230VDCwide if nothing happend");
    Supply.flags.SM_OVERVOLTAGE = OV_YES;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_230VDCnarrow, sm_state, "state should be: sm_state_230VDCnarrow after OV_YES");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCnarrow, suMoCoP, "should point to supplyMonConstants230VDCnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VACwide_when_AC)
{
    sm_state=sm_state_230VDCwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VDCwide;
    Supply.flags.SM_POWERTYPE = DC;
    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VDCwide, sm_state, "state should stay: sm_state_230VDCwide if nothing happend");
    Supply.flags.SM_POWERTYPE = AC;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should stay: sm_mode_230V if only switching from DC to AC");
    ASSERT_VERB_EQUAL(sm_state_230VACwide, sm_state, "state should be: sm_state_230VACwide after AC");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VACwide, suMoCoP, "should point to supplyMonConstants230VACwide");
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VACnarrow_when_PF)
{
    sm_state=sm_state_230VACwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VACwide;
    Supply.flags.SM_POWERTYPE = AC;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VACwide, sm_state, "state should stay: sm_state_230VACwide if nothing happend");
    Supply.flags.SM_POWERTYPE = PF;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_powerfail, Supply.flags.SM_MODE, "mode should goto: sm_mode_powerfail if PF");
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should be: sm_state_230VACnarrow after PF");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VACnarrow, suMoCoP, "should point to supplyMonConstants230VACnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VACnarrow_when_OV_YES)
{
    sm_state=sm_state_230VACwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VACwide;
    Supply.flags.SM_POWERTYPE = AC;
    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VACwide, sm_state, "state should stay: sm_state_230VACwide if nothing happend");
    Supply.flags.SM_OVERVOLTAGE = OV_YES;
    supplyMonitorPriv.powerFailCnt = 1; // just to check call on ResetEva()
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_state_230VACnarrow, sm_state, "state should be: sm_state_230VACnarrow after OV_YES");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VACnarrow, suMoCoP, "should point to supplyMonConstants230VDCnarrow");
    ASSERT_VERB_EQUAL(0,supplyMonitorPriv.powerFailCnt, "ResetEva has to be called"); // check one function at least
}
TEST(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VDCwide_when_DC)
{
    sm_state=sm_state_230VACwide;
    Supply.flags.SM_MODE = sm_mode_230V;
    suMoCoP = &supplyMonConstants230VACwide;
    Supply.flags.SM_POWERTYPE = AC;
    Supply.flags.SM_OVERVOLTAGE = OV_NO;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should be: sm_mode_230V if nothing happend");
    ASSERT_VERB_EQUAL(sm_state_230VACwide, sm_state, "state should stay: sm_state_230VACwide if nothing happend");
    Supply.flags.SM_POWERTYPE = DC;
    SupplyMonitorFSM();
    ASSERT_VERB_EQUAL(sm_mode_230V, Supply.flags.SM_MODE, "mode should stay: sm_mode_230V if only switching from AC to DC");
    ASSERT_VERB_EQUAL(sm_state_230VDCwide, sm_state, "state should be: sm_state_230VACwide after DC");
    TEST_ASSERT_EQUAL_PTR_MESSAGE(&supplyMonConstants230VDCwide, suMoCoP, "should point to supplyMonConstants230VDCwide");
}



//----------------------------------------------------------------------------------------------------------------------
// Test Runner
//----------------------------------------------------------------------------------------------------------------------
// SupplymonitorCyclic
TEST_GROUP_RUNNER(supplymonitor_level_fsm)
{
    RUN_TEST_CASE(supplymonitor_level_fsm, test_init);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_reset_to_15V);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_sm_mode_15V_is_set_after_timeout);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_15V_to_48V_voltage_exceeded);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_48V_to_293V_voltage_exceeded);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_48V_to_293V_after_cnt_exceeds_time);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_293V_to_230Vnarrow_when_overvoltage);
    /* XXXXX*/
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VDCnarrow_to_230VDCwide_when_DC_and_OV_NO);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VACwide_when_AC_and_OV_NO);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VDCnarrow_to_230VACnarrow_when_AC);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VDCnarrow_when_DC);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACnarrow_to_230VDCnarrow_when_PF);
    /* XXXXXXXXXXXXXX Tests from wide to narrow and wide to wide XXXXXXXXXXXX */
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VDCnarrow_when_PF);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VDCnarrow_when_OV_YES);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VDCwide_to_230VACwide_when_AC);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VACnarrow_when_PF);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VACnarrow_when_OV_YES);
    RUN_TEST_CASE(supplymonitor_level_fsm, test_state_change_from_230VACwide_to_230VDCwide_when_DC);

}

