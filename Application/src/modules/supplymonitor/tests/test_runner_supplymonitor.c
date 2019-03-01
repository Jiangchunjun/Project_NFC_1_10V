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
// $Id: test_runner_supplymonitor.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_runner_supplymonitor.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unity_fixture.h"

#include "filter.h"

#include "supplymonitor_parameters.h"
#include "supplymonitor_userinterface.h"
#include "SupplyMonitorDefaults.h"
#include "supplymonitor_local.h"

// declaration should be only once added to testfiles
// required to declare local variable overwrites only once
#define DECLARE_LOCAL_OVERWRITES
#include "test_supplymonitor_overwrites.h"

//------------------------------------------------------------------------------
// external variable test declarations
//------------------------------------------------------------------------------
Filter_t Uline1;
Filter_t Uline2;
Filter_t Urail;
uint32_t evaluate_timer;
//supply_state_t sm_state;

//------------------------------------------------------------------------------
static void RunAllTests(void)
{
    printf("\n\n---------- Run Tests -----------\n");
    RUN_TEST_GROUP(supplymonitor_level_fsm);
    RUN_TEST_GROUP(supplymonitor_evaluate);
    RUN_TEST_GROUP(supplymonitor_basic);
    RUN_TEST_GROUP(supplymonitor_adjust_Ulinepk);
    //RUN_TEST_GROUP(supplymonitor_integration);
}

//------------------------------------------------------------------------------
int main(int argc, char * argv[])
{
    return UnityMain(argc, argv, RunAllTests);
}
