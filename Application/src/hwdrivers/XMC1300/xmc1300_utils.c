// ---------------------------------------------------------------------------------------------------------------------
// Firmware for OTi NONSELV Generation 2 Ballasts
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
// Initial version: M. Gerber, 2014-04
// $Author: Chunjun.Jiang $
// $Revision: 14328 $
// $Date: 2018-07-25 09:42:20 +0800 (Wed, 25 Jul 2018) $
// $Id: xmc1300_utils.c 14328 2018-07-25 01:42:20Z Chunjun.Jiang $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_utils.c $

/*******************************************************************************************************************//**
* @file
* @brief General setup of XMC1300 microcontroller
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup HW HW - Hardware drivers and hardware dependent parameters
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

#include "gpio_xmc1300_tssop38.h"
#include "kernel_parameters.h"
#include "xmc1300_utils.h"
#include "pse_handle.h"
// INTERRUPT PRIORITY LEVEL
// highest priority is IRQ_PRIORITY_LEVEL_0
#define  IRQ_PRIORITY_LEVEL_0       0
#define  IRQ_PRIORITY_LEVEL_1       1
#define  IRQ_PRIORITY_LEVEL_2       2
#define  IRQ_PRIORITY_LEVEL_3       3

void ControllerInit(void)
{
    //----CLOCK-SETUP---------------------------------------------------------------------------------
    SCU_GENERAL->PASSWD = 0x000000C0UL;                                                 // disable write protection to SCU registers
    WR_REG(SCU_CLK->CLKCR, SCU_CLK_CLKCR_CNTADJ_Msk, SCU_CLK_CLKCR_CNTADJ_Pos, 0x3FFU); // set delay for stabilizing VCC to 16us
    // set divider IDIV for main clock (reset value of CLKCR: 0x3FF00400) -> IDIV=bit8..15
    SCU_CLK->CLKCR = 0x3FF00000 | ((uint32_t)(32000000/SYSCLK_Hz) << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk);
    // peripheral clock = 2x main clock
    SCU_CLK->CLKCR |= (uint32_t)1 << (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Pos & (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Msk;
    SCU_GENERAL->PASSWD = 0x000000C3UL;                                                 // enable write protection to SCU registers
    //------------------------------------------------------------------------------------------------

    //----INTERRUPT PRIORITIES----------------------------------------------------
    // BUCK - ADC measurement trigger at low PWM dutycycle
    NVIC_SetPriority(POSIF0_0_IRQn, IRQ_PRIORITY_LEVEL_0);
    // BUCK - ZCD activation
    NVIC_SetPriority(CCU80_0_IRQn, IRQ_PRIORITY_LEVEL_1);
    // BUCK and BOOST - Error Detection
    NVIC_SetPriority(SCU_2_IRQn, IRQ_PRIORITY_LEVEL_2);
    // DALI - RX
    NVIC_SetPriority(CCU40_0_IRQn, IRQ_PRIORITY_LEVEL_3);
    // DALI - TX
    NVIC_SetPriority(CCU40_1_IRQn, IRQ_PRIORITY_LEVEL_3);
    
    //NVIC_SetPriority(CCU40_2_IRQn, IRQ_PRIORITY_LEVEL_3);
    
   // NVIC_SetPriority(CCU40_3_IRQn, IRQ_PRIORITY_LEVEL_3);

    // SYSTEM - Systick (configured by SysTick_Config to IRQ_PRIORITY_LEVEL_3)
    // priority is set in SFR SHPR3
    //  NVIC_SetPriority(SysTick_IRQn, IRQ_PRIORITY_LEVEL_3);

    //----SYSTICK-SETUP (including IRQ priority to 3) & START Timer --------------
    //SystemCoreClockUpdate();    //@todo: scheint sinnlos --> pruefen, entfernen
    SysTick_Config(SYSCLK_Hz / SYSTICK_INT_Hz);
    //------------------------------------------------------------------------------------------------

    //----U100: Pinout 8c for ZNN-3030986-000-02------------------------------------------------------

    /* Configure Boost */
    // P0_3_set_mode(OUTPUT_OD_AF5);         // PFC_RETRIGGER_GATING (CCU80.OUT03)
    P0_9_set_mode(OUTPUT_PP_GP);          // PFC_GATE
    P0_9_reset();                         // keep PFC_GATE low
    P0_10_set_mode(INPUT);                 // PFC_RETRIGGER (CCU80.IN0B)
    P0_11_set_mode(OUTPUT_OD_GP);          // PFC_RETRIGGER_GATING
    P0_11_set();                           // disable gating of PFC_RETRIGGER
    P0_12_set_mode(INPUT_PU);             // PFC_SHUNT_CMP_OUT (CCU80.IN0A)

    /* Configure Hysteretic Buck */
    P0_1_set_mode(OUTPUT_PP_GP);          // SET_PWM
    P0_1_reset();                         // 0
    P0_2_set_mode(OUTPUT_PP_GP);          // HB_L
    P0_2_reset();                         // keep HB_L low
    P0_3_set_mode(OUTPUT_PP_GP);          // HB_H
    P0_3_reset();                         // keep HB_H low
    P0_4_set_mode(OUTPUT_PP_GP);          // SET_START
    P0_4_reset();                         // keep everything off

    P2_1_set_mode(INPUT);                 // Buck shunt
    P2_2_set_mode(INPUT);                 // Buck Max Ref
    P2_6_set_mode(INPUT);                 // Buck shunt
    P2_7_set_mode(INPUT);                 // Buck Min Ref


    /* Configure ADC Pins*/
    P2_0_set_mode(INPUT);                 // ADC_RAIL
    P2_3_set_mode(INPUT);                 // ORC_RAIL
    P2_4_set_mode(INPUT);                 // ADC_REC_N
    P2_5_set_mode(INPUT);                 // ADC_REC_L
    P2_8_set_mode(INPUT);                 // ADC_LED (rerouted to use the ORC on pin 2.8)
    P2_9_set_mode(INPUT);
    P2_10_set_mode(INPUT);                // LEDSET_SET
    P2_11_set_mode(INPUT);                // ADC_LED

    /* Configure additional Pins of U100 */
    P0_6_set_mode(INPUT_PU);              // DALI Rx (Input with Pull-Up, because external comparator has open-collector output)
    P0_7_set_mode(OUTPUT_PP_GP);          // DALI Tx      GPIO-Output Push-Pull
    P0_7_set();                           // keep Tx off

    P0_14_set_mode(INPUT_PU);             // SPD0
    P0_15_set_mode(OUTPUT_PP_GP);         // DEBUG_LED   Used for Dataout & BMI Communication!!
    P0_15_reset();                        // used for Dataout & BMI !!

    P1_1_set_mode(INPUT);                 // CMPH voltage on fairchild

    P1_5_set_mode(OUTPUT_PP_GP);          // Switch for the power of LEDset2+
    P1_5_set();                           // LEDset2+ enabled

    P1_4_set_mode(OUTPUT_PP_GP);          // unused (input, pull-up)
    P1_4_set();                           // CMP References enabled

    P0_8_set_mode(OUTPUT_PP_GP);          // Switch for voltage divider reference (PFC Shunt Comparator)
    P0_8_set();                           // enabled

    /* Configure unused Pins*/
    P0_0_set_mode(INPUT);                 // unused (input, NO pull-down until schematic 21mm is fixed)
    //P0_5_set_mode(INPUT_PD);              // unused (input, pull-down)
    P1_2_set_mode(INPUT_PD);              // unused (input, pull-down)
    P1_3_set_mode(INPUT_PD);              // unused (input, pull-down)
    
    P1_1_set_mode(OUTPUT_PP_GP); 
    P1_1_reset();
    P0_10_set_mode(INPUT_PD);
    P2_0_enable_digital();
    P2_0_set_mode(INPUT_PD);
    P0_9_set_mode(OUTPUT_PP_GP);          // PFC_GATE
    P0_9_set(); 
    pwm_hw_ini();
    capature_hw_ini();
    P0_13_set_mode(INPUT_PU); 
}

#endif


/** @}  End of group HW */
