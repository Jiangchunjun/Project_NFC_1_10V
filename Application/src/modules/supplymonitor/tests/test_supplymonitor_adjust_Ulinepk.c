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
// $Id: test_supplymonitor_adjust_Ulinepk.c 4738 2016-07-08 09:31:17Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/modules/supplymonitor/tests/test_supplymonitor_adjust_Ulinepk.c $
//
/// \file
/// \brief Unit tests for supplymonitor module

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


#include "parameter.h"
#include "supplymonitor_userinterface.h"
#include "supplymonitor_local.h"
#include "SupplyMonitorDefaults.h"
#include "test_supplymonitor_overwrites.h"

#include "unity_fixture.h"
#include "unit_testing_utilities.h"


//------------------------------------------------------------------------------
// unit under test functions
//------------------------------------------------------------------------------
void supplymonitor_adjust_Ulinepk(void);

//------------------------------------------------------------------------------
// global module external declared variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// local test variables
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Utility functions
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// SupplymonitorAdjustUlinepk
//------------------------------------------------------------------------------
TEST_GROUP(supplymonitor_adjust_Ulinepk);

TEST_SETUP(supplymonitor_adjust_Ulinepk)
{

}
TEST_TEAR_DOWN(supplymonitor_adjust_Ulinepk)
{
}

//------------------------------------------------------------------------------
/** test thermal_power_red_internal when temperature is equal to limit value and no initial power reduction
 * - force initial power reduction to 0
 * - force temperatur vlaue to limit value
 * test:
 * - integral value is 0
 * - integral_help value is 0
 * - power reduction is 0
 */
TEST(supplymonitor_adjust_Ulinepk, first_test)
{

    Supply.Ulinepk.mean = VOLTS(325);
    Supply.Ulinepk.last = VOLTS(326);
    supplymonitor_deviation_cnt = 10;
    Filter_t Ulinepk_expected;
    Ulinepk_expected.mean = VOLTS(325);
    uint32_t Ulinepk_delta_expected = Supply.Ulinepk.last - Supply.Ulinepk.mean;


    // function under test
    supplymonitor_adjust_Ulinepk();


    ASSERT_VERB_EQUAL(supplymonitor_Ulinepk_delta, Ulinepk_delta_expected, "TEST failed");

    ASSERT_VERB_EQUAL(Supply.Ulinepk.mean, Ulinepk_expected.mean, "TEST failed");

    ASSERT_VERB_EQUAL(supplymonitor_deviation_cnt, 0, "TEST failed");

}

//----------------------------------------------------------------------------------------------------------------------
// Test Runner
//----------------------------------------------------------------------------------------------------------------------
// SupplymonitorAdjustUlinepk
TEST_GROUP_RUNNER(supplymonitor_adjust_Ulinepk)
{
    RUN_TEST_CASE(supplymonitor_adjust_Ulinepk, first_test);
}
