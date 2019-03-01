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
// $Id: test_supplymonitor_evaluate.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_supplymonitor_evaluate.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


//#include "global.h"

//#include "filter.h"
//#include "analog.h"
//#include "parameters.h"
#include "parameter.h"
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
void SupMonEvaFailACDC(void);
void SupMonEvaOvervoltage(void);
//------------------------------------------------------------------------------
// global module external declared variables
//------------------------------------------------------------------------------
//extern const supply_monitor_constants_t supplyMonConstantsStandard;
//extern const supply_monitor_constants_t supplyMonConstantsLowVoltPrg;
//extern const supply_monitor_constants_t supplyMonConstants15V;
//extern const supply_monitor_constants_t supplyMonConstants48V;
//extern const supply_monitor_constants_t supplyMonConstants293V;
extern const supply_monitor_constants_t supplyMonConstants230VDCnarrow; // only used as an example here
//extern const supply_monitor_constants_t supplyMonConstants230VDCwide;

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
TEST_GROUP(supplymonitor_evaluate);

TEST_SETUP(supplymonitor_evaluate)
{
    suMoCoP = &supplyMonConstants230VDCnarrow;
    Supply.flags.SM_POWERTYPE=NA;
    supplyMonitorPriv.powerFailCnt=0;
}
TEST_TEAR_DOWN(supplymonitor_evaluate)
{

}

//------------------------------------------------------------------------------
/** test checksum calculation */
/**
test:
- sm_state_15V
- sm_mode switched from sm_mode_reset to sm_mode_15V after fsm_cycle_cnt exceeds suMoCoP.modus_timeout
*/

TEST(supplymonitor_evaluate, SupMonEvaFailACDC_SetFail_AfterTime)
{
    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(NA, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be NA");

    Supply.uline_diff_abs = (suMoCoP->absMinVolt)-1;
    supplyMonitorPriv.powerFailCnt=suMoCoP->powerFailCount;

    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(PF, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be PF");
}


TEST(supplymonitor_evaluate, SupMonEvaFailACDC_SetAC_after_Commutation)
{
    Supply.uline_diff_abs = (suMoCoP->absMinVolt); // should be no powerfail
    supplyMonitorPriv.commLastTimestamp=1000000;  // just a timestamp = 1s;
    Supply.comm_ts=supplyMonitorPriv.commLastTimestamp+PERIOD_AC_NOISE_BORDER_us;  // too short

    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(NA, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be NA");

    supplyMonitorPriv.commLastTimestamp=1000000;  // just a timestamp = 1s;
    Supply.comm_ts=supplyMonitorPriv.commLastTimestamp+10000;  // 10ms should be reasonable

    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(AC, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be AC");
}
TEST(supplymonitor_evaluate, SupMonEvaFailACDC_SetDC_after_Commutation)
{
    Supply.uline_diff_abs = (suMoCoP->absMinVolt); // should be no powerfail
    supplyMonitorPriv.commLastTimestamp=1000000;  // just a timestamp = 1s;
    Supply.comm_ts=supplyMonitorPriv.commLastTimestamp+PERIOD_AC_NOISE_BORDER_us;  // too short
    supplyMonitorPriv.commTimeoutCnt = suMoCoP->ac_detect_timeout-1;

    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(NA, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be NA");

    supplyMonitorPriv.commTimeoutCnt = suMoCoP->ac_detect_timeout;
    SupMonEvaFailACDC();
    ASSERT_VERB_EQUAL(DC, Supply.flags.SM_POWERTYPE, "SM_POWERTYPE should be DC");
}




TEST_GROUP_RUNNER(supplymonitor_evaluate)
{
    RUN_TEST_CASE(supplymonitor_evaluate, SupMonEvaFailACDC_SetFail_AfterTime);
    RUN_TEST_CASE(supplymonitor_evaluate, SupMonEvaFailACDC_SetAC_after_Commutation);
    RUN_TEST_CASE(supplymonitor_evaluate, SupMonEvaFailACDC_SetDC_after_Commutation);
}
