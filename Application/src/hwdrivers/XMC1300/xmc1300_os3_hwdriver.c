// ---------------------------------------------------------------------------------------------------------------------
// OS3 module for OTi NONSELV Generation 2 ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     LE D - M
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
// Initial version: C. Schwarzfischer, 2014-10
// $Author: F.Branchetti $
// $Revision: 5700 $
// $Date: 2016-10-13 15:39:00 +0800 (Thu, 13 Oct 2016) $
// $Id: xmc1300_os3_hwdriver.c 5700 2016-10-13 07:39:00Z F.Branchetti $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_os3_hwdriver.c $

/*******************************************************************************************************************//**
* @file
* @brief XMC1300 hardware driver for OS3 module
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup OS3 OS3 - 'O'perating 'S'ystem for 'OS'RAM 'O'ptotronic 'S'ings Module
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

#include "os3_hal.h"
#include "xmc1300.h"
#include "gpio_xmc1300_tssop38.h"



uint32_t os3_hw_read_systick_value (void)
{
    uint32_t systick_value;                                                       // create local variable
    systick_value = PPB->SYST_CVR;                                                // read SysTick Current Value Register
    return systick_value;                                                         // return read value
}

void os3_hw_stop_systick (void)
{
    PPB->SYST_CSR &= ~PPB_SYST_CSR_ENABLE_Msk;                                    // reset SysTick ENABLE Bit
}

void os3_hw_start_systick (void)
{
    PPB->SYST_CSR |= PPB_SYST_CSR_ENABLE_Msk;                                     // set SysTick ENABLE Bit
}

void os3_hw_reload_systick (void)
{
    PPB->SYST_CVR = 0;                                                            // write SysTick Current Value Register
}

void os3_hw_change_systick_value (uint32_t systick_value)
{
    PPB->SYST_RVR = (systick_value - 1);                                          // change SysTick Value Reload Register
    PPB->SYST_CVR = 0;                                                            // write SysTick Current Value Register
}


void os3_hw_wdt_init (uint32_t wlb, uint32_t wub)
{
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                                 // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_WDT_Pos);                         // Disable clock gating of WDT
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2LOW_Msk) {}    // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284 & PWMSP001 [1.0.34] -> CCU4Global_lInit())
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                                 // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1

    WDT->WLB = wlb;                                                               // write window lower bound
    WDT->WUB = wub;                                                               // write window upper bound
}

void os3_hw_wdt_reset (void)
{
    WDT->SRV = 0xABADCAFEUL;                                                      // write "magic word" to WDT service register
}

uint32_t os3_hw_read_cmph (void)
{
    uint32_t read_hw_cmph;
    read_hw_cmph = P1_1_read();
    return read_hw_cmph;
}

void os3_hw_wdt_force_sys_reset (void)
{
    WDT->SRV = 0xAFFEAFFEUL;                                                      // DON'T write "magic word" to WDT service register --> force WDT reset
}

void os3_hw_wdt_start (void)
{
    WDT->CTR |= WDT_CTR_ENB_Msk;                                                  // set WDT ENABLE Bit
}

void os3_hw_wdt_stop (void)
{
    WDT->CTR &= ~WDT_CTR_ENB_Msk;                                                 // reset WDT ENABLE Bit
}

void os3_hw_wdt_check_reset (void)
{
    // checken, ob der letzte Systemreset ein WDT Reset war!
}

#endif


/** @}  End of group OS3 */
