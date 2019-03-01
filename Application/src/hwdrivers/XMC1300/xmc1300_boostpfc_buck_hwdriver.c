// ---------------------------------------------------------------------------------------------------------------------
// XMC1300 hardware driver for BoostPFC and Buck for OTi NONSELV Generation 2 ballasts
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
// Initial version: O. Busse, M. Gerber, 2014-06
// $Author: c.schwarzfischer $
// $Revision: 10381 $
// $Date: 2017-10-19 19:21:16 +0800 (Thu, 19 Oct 2017) $
// $Id: xmc1300_boostpfc_buck_hwdriver.c 10381 2017-10-19 11:21:16Z c.schwarzfischer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_boostpfc_buck_hwdriver.c $

/*******************************************************************************************************************//**
* @file
* @brief XMC1300 hardware driver for BoostPFC and Buck module (for PCB since ZNN-3030986-000-02)
  @image html xmc1300_boostpfc_hwdriver_PFC_timer_configuration.jpg "BoostPFC timer configuration of XMC1300"
***********************************************************************************************************************/

#ifdef __ICCARM__

#define HWDRIVER_BOOSTPFC
#define HWDRIVER_BUCK

#include <stdbool.h>

#include "gpio_xmc1300_tssop38.h" // to set ports, etc.
#include "xmc1300.h"              // for macros SET_BIT, WR_REG, etc.
#include "boostpfc_hal.h"
//#include "buck_hal.h"
#include "boostpfc_parameters.h"
#include "boostpfc_userinterface.h"
//#include "buck_parameters.h"
//#include "buck_userinterface.h"
//#include "intrinsics.h"

//----------------------------------------------------------------------------------------------------------------------
// INTERNAL DEFINES
#define PFC_ON_MIN      2           // 31,25ns@64MHz -> no gate drive, only VCCO is supplied by VCCH/VCCL
#define PFC_DEMAG_MAX   6400        // 100us@64MHz -> to keep monostable flip flop of FAN3182 active, so that FAN3182 stays in steady-state
#define PFC_DEMAG_MAX_INTERMEDIATE      128
#define PFC_DEMAG_MAX_SURGE  640

#define P0_3_SET_MODE(mode)         \
    PORT0->IOCR0 &= ~0xf8000000UL;  \
    PORT0->IOCR0 |= (mode) << 24;

#define P0_4_SET_MODE(mode)         \
    PORT0->IOCR4 &= ~0x000000f8UL;  \
    PORT0->IOCR4 |= (mode) << 0;

#define CC80_ST_MASK (CCU8_GCST_CC80ST1_Msk | CCU8_GCST_CC80ST2_Msk)
#define CC81_ST_MASK (CCU8_GCST_CC81ST1_Msk | CCU8_GCST_CC81ST2_Msk)

//----------------------------------------------------------------------------------------------------------------------
static volatile uint32_t comparator_events;          // counter for number of overcurrent comparator events  // don't delete volatile here!
static uint32_t boostpfc_deepsleep_requested;
static uint32_t buck_deepsleep_requested;
//extern bool buck_module_adc_active;
bool m_CCU8_power_status;

//----------------------------------------------------------------------------------------------------------------------
// INTERNAL FUNCTIONS
static void BoostPFC_Buck_HwCAPCOM8Init (void);  // initialize CAPCOM 8 timer unit for boostPFC and buck
static void BoostPFC_Buck_HwCAPCOM8Start (void); // start CAPCOM 8 timer unit for boostPFC and buck
static void BoostPFC_Buck_HwCAPCOM8Stop (void);  // stop CAPCOM 8 timer unit for boostPFC and buck
static void BoostPFC_Buck_HwCAPCOM4Init (void);  // initialize CAPCOM 4 timer unit for buck
static void BoostPFC_Buck_HwInit (void);
static void BoostPFC_Buck_HwDeepsleep2Ready (void);
static void BoostPFC_Buck_HwReady2Deepsleep (void);
static void BoostPFC_HwCC82Init (void);          // initialize CC82 timer slice for boostPFC
static void Buck_HwCC42Init (void);              // initialize CC42 timer slice for aux timer (anti-glow MOSFET gate drive)
#if REMOVED_CODE
static void Buck_HwERUInit (void);               // initialize Event Request Unit for buck
static void BoostPFC_Buck_HwComparatorInit (void);        // initialize analog comparators for buck
static void BoostPFC_Buck_HwComparatorEnable (void);      // enable analog comparators for buck
static void BoostPFC_Buck_HwComparatorDisable (void);     // disable analog comparators for buck
static void Buck_HwPOSIFInit (void);             // initialize Position Interface for synchronized timer update of buck
static void Buck_HwPOSIFEnable (void);           // start Position Interface for synchronized timer update of buck
static void Buck_HwPOSIFDisable (void);          // stop Position Interface for synchronized timer update of buck
static void Buck_HwCC43Init (void);              // initialize CC43 timer slice for buck (PWM)
static void Buck_HwCC80Init (void);              // initialize CC80 timer slice for buck (low side switch)
static void Buck_HwCC81Init (void);              // initialize CC81 timer slice for buck (high side swtich)
static void Buck_HwCC83Init (void);              // initialize CC83 timer slice for buck (Capturing)
void SCU_2_IRQHandler(void);                    // Interrupt Service Routine for buck overcurrent
void CCU40_3_IRQHandler(void);
void CCU80_0_IRQHandler(void);
void POSIF0_0_IRQHandler(void);
#endif

//----------------------------------------------------------------------------------------------------------------------

/* internal function */
/* global configuration of CAPCOM8 timer unit */
static void BoostPFC_Buck_HwCAPCOM8Init (void)
{
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                              // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_CCU80_Pos);                    // Disable clock gating of CCU80
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2LOW_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284 & PWMSP001 [1.0.34] -> CCU4Global_lInit())
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                              // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(CCU80->GIDLC,CCU8_GIDLC_SPRB_Pos);                                 // Set the Pre-scalar run bit

    // global CAPCOM8 initialization
    CCU80->GCTRL  = ( 1 << CCU8_GCTRL_MSE0_Pos) |    // enable shadow transfer request via the CCU8x.MCSS input for CC80 (low side timer)
                    ( 1 << CCU8_GCTRL_MSE1_Pos) |    // enable shadow transfer request via the CCU8x.MCSS input for CC81 (high side timer)
                    ( 3 << CCU8_GCTRL_MSDE_Pos) ;    // also request shadow transfer for dither compare value via the MCSS input
//                  ( 4 << CCU8_GCTRL_PRBC_Pos) ;    // GSTAT.PRB and prescaler registers are cleared when Run Bit of CC83 is cleared
}


/* internal function */
/* start CAPCOM8 timer unit */
static void BoostPFC_Buck_HwCAPCOM8Start (void)
{
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                              // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_CCU80_Pos);                    // Disable clock gating of CCU80
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2LOW_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284 & PWMSP001 [1.0.34] -> CCU4Global_lInit())
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                              // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(CCU80->GIDLC,CCU8_GIDLC_SPRB_Pos);                                 // Set the Pre-scalar run bit

    m_CCU8_power_status = true;
}


/* internal function */
/* stop CAPCOM8 timer unit */
static void BoostPFC_Buck_HwCAPCOM8Stop (void)
{
    SET_BIT(CCU80->GIDLS,CCU8_GIDLS_CPRB_Pos);                                  // clear the Pre-scalar run bit
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                               // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATSET0, SCU_CLK_CGATSET0_CCU80_Pos);                     // Enable clock gating of CCU80
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2HIGH_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284)
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                               // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1

    m_CCU8_power_status = false;
}


/* internal function */
/* global configuration of CAPCOM4 timer unit for aux timer (attention: DALI also uses CAPCOM4) */
static void BoostPFC_Buck_HwCAPCOM4Init (void)
{
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                              // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_CCU40_Pos);                    // Disable clock gating of CCU80
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2LOW_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284 & PWMSP001 [1.0.34] -> CCU4Global_lInit())
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                              // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(CCU40->GIDLC,CCU4_GIDLC_SPRB_Pos);                                 // Set the Pre-scalar run bit
}


void BoostPFC_Buck_HwInit (void)
{
    static uint32_t init_flag = 0;
    if (init_flag == 0)
    {
        comparator_events = 0;          // counter for number of overcurrent comparator events
        boostpfc_deepsleep_requested = 1;
        buck_deepsleep_requested = 1;

        BoostPFC_Buck_HwCAPCOM8Init(); // start and configure CAPCOM8
#if REMOVED_CODE
        Buck_HwCC80Init();             // configure timer slice for low side switch
        Buck_HwCC81Init();             // configure timer slice for high side switch
        Buck_HwCC83Init();             // configure timer slice for capturing buck
#endif

        BoostPFC_HwCC82Init();         // configure timer slice for boost
        BoostPFC_Buck_HwCAPCOM4Init(); // start and configure CAPCOM4
        Buck_HwCC42Init();             // configure timer slice for Anti-Glow switch

#if REMOVED_CODE
        Buck_HwCC43Init();             // configure timer slice for PWM
#endif

        BoostPFC_Buck_HwCAPCOM8Stop();   // stop CAPCOM8

#if REMOVED_CODE
        BoostPFC_Buck_HwComparatorInit();       // configure comparators (for ZCD, overcurrent)
        Buck_HwERUInit();              // configure event request unit (for ZCD, overcurrent, timers)
        Buck_HwPOSIFInit();            // configure position interface (for correctly timed timer update)
#endif
        init_flag = 1;
    }
}


//attention! while loop until VDDC is stabilized --> Buck_HwDeepsleep2Ready shall not be called during run mode of the ballast!
void BoostPFC_Buck_HwDeepsleep2Ready (void)
{
    BoostPFC_Buck_HwCAPCOM8Start();                               // start CAPCOM8

#if REMOVED_CODE
    BoostPFC_Buck_HwComparatorEnable();                           // enable comparators (for overcurrent and ZCD)
    Buck_HwPOSIFEnable();                                         // enable POSIF for synchronized timer update
#endif

    CCU80_CC81->SWR = 0x00000F3FU;                                // CC81: clear all interrupts
    SET_BIT(CCU80->GIDLC, CCU8_GIDLC_CS1I_Pos);                   // CC81: clear IDLE state
    CCU80_CC80->SWR = 0x00000F3FU;                                // CC80: clear all interrupts
    SET_BIT(CCU80->GIDLC, CCU8_GIDLC_CS0I_Pos);                   // CC80: clear IDLE state
    CCU80_CC83->SWR = 0x00000F3FU;                                // CC83: clear all interrupts
    SET_BIT(CCU80->GIDLC, CCU8_GIDLC_CS3I_Pos);                   // CC83: clear IDLE state
    CCU40_CC43->SWR = 0x00000F3FU;                                // CC43: clear all interrupts
    SET_BIT(CCU40->GIDLC, CCU4_GIDLC_CS3I_Pos);                   // CC43: clear IDLE state
    CCU80_CC82->SWR = 0x00000F3FU;                                // CC82: clear all interrupts of CC82
    SET_BIT(CCU80->GIDLC, CCU8_GIDLC_CS2I_Pos);                   // CC82: clear IDLE state of CC82
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX);        // GDI pulses to keep FAN3182 in run mode, no gate drive
    P0_9_set_mode(OUTPUT_PP_AF5);                                 // set P0.9  (PFC_GATE) to CCU80.OUT21
    P0_11_set_mode(OUTPUT_OD_AF5);                                // set P0.11 (PFC_RETRIGGER_GATING) to CCU80.OUT23
    SET_BIT(CCU80_CC82->TCSET, CCU8_CC8_TCSET_TRBS_Pos);          // set timer run bit for CC82 -> start the boost timer
}


void BoostPFC_Buck_HwReady2Deepsleep (void)
{
    if ((boostpfc_deepsleep_requested) &&  (buck_deepsleep_requested))
    {
        P0_9_set_mode(OUTPUT_PP_GP);                                 // set P0.9  (PFC_GATE) to GPIO
        P0_9_reset();                                                // set P0.9  (PFC_GATE) to low
        P0_11_set_mode(OUTPUT_OD_GP);                                // set P0.11 (PFC_RETRIGGER_GATING) to GPIO
        P0_11_reset();                                               // set P0.11 (PFC_RETRIGGER_GATING) to low
        SET_BIT(CCU80_CC82->TCCLR, CCU8_CC8_TCCLR_TRBC_Pos);         // clear timer run bit -> stops the timer
        SET_BIT(CCU80->GIDLS, CCU8_GIDLS_SS2I_Pos);                  // set IDLE state of CC82
        SET_BIT(CCU80->GIDLS, CCU8_GIDLS_SS3I_Pos); // CC83: set IDLE state

        // NOTE: is CCU42 still used? Probably not...
        SET_BIT(CCU40->GIDLS, CCU4_GIDLS_SS2I_Pos); // CC42: set IDLE state

        SCU_INTERRUPT->SRMSK &= ~(uint32_t)SCU_INTERRUPT_SRMSK_ORC1I_Msk; // disable the interrupt trigger source
        SCU_INTERRUPT->SRMSK &= ~(uint32_t)SCU_INTERRUPT_SRMSK_ORC2I_Msk; // disable the interrupt trigger source
        SCU_INTERRUPT->SRMSK &= ~(uint32_t)SCU_INTERRUPT_SRMSK_ORC3I_Msk; // disable the interrupt trigger source

#if REMOVED_CODE
        BoostPFC_Buck_HwComparatorDisable();                 // disable comparators
        Buck_HwPOSIFDisable();                      // disable position interface
#endif
    }
}

void BoostPFC_Buck_Hw_turn_off_CCU8( void )
{
    BoostPFC_Buck_HwCAPCOM8Stop();              // shut-down CCU8, if not required by buck
    m_CCU8_power_status = false;

}

bool BoostPFC_Buck_Hw_is_CCU8_active( void )
{
    return m_CCU8_power_status;
}

/**********************************************************************************************************************/
/******************************************************B O O S T*******************************************************/
/**********************************************************************************************************************/

void BoostPFC_HwInit (void)
{
    BoostPFC_Buck_HwInit();
}


/* internal function */
/* configure CC82 timer slice for generating gate-drive signal of BoostPFC transistor */
static void BoostPFC_HwCC82Init (void)
{
    //----CCU8 Slice2 (CC82) SETUP----------------------------------------------------------------------------------------
    // TRAP function used for limiting on-time when "overcurrent" event at pfc shunt occurs
    // dead time is required, to prevent false ZCD detection when period is reached and PFC-MOSFET is switched on again (ZCD-gating not active at the beginning of the period)

    CCU80_CC82->TC   |= ( 1 << CCU8_CC8_TC_CLST_Pos)   | // set the CLST-Bit to activate shadow transfer on timer clear, otherwise new period and compare values are never updated without a period match
                        // should not be needed, because there will always be a period match because ZCD loads compare2 with period value in boundary mode instead of resetting the timer
                        ( 1 << CCU8_CC8_TC_TLS_Pos)    | // Set Timer Load with compare 2 value (pfc_max_period - t_off_adder)
                        ( 1 << CCU8_CC8_TC_TRAPE1_Pos) | // enable TRAP for CCU80.OUT21 (PFC_GATE)
                        ( 1 << CCU8_CC8_TC_TRPSE_Pos)  ; // exit TRAP state synchronized to PWM signal (on-time is limited)

    CCU80_CC82->INS  |= ( 1 << CCU8_CC8_INS_EV0IS_Pos) | // Configure Event 0 to ZCD P0.10 (CCU80.IN2B)
                        ( 2 << CCU8_CC8_INS_EV0EM_Pos) | // Configure external load feature falling edge (ZCD)
                        ( 3 << CCU8_CC8_INS_LPF0M_Pos) ; // enable low pass filtering of 7 clock cycles for Event 0 – t_filter = 105ns

    CCU80_CC82->CMC  |= ( 1 << CCU8_CC8_CMC_LDS_Pos)   | // Set Timer Load when event 0 (ZCD) occurs
                        ( 1  << CCU8_CC8_CMC_TS_Pos)   ; // enable TRAP (triggered by event 2)

    CCU80_CC82->DTC  |= ( 1 << CCU8_CC8_DTC_DTE2_Pos)  | // enable dead time channel 2
                        ( 1 << CCU8_CC8_DTC_DCEN4_Pos) ; // enable dead time for inverted CC8yST2

    CCU80_CC82->DC2R |=   64 << CCU8_CC8_DC2R_DT2F_Pos ; // set dead time for CC8yST2 fall value: 1000ns@64MHz

// Max: funktioniert nicht, weil man nur mit CCU80.SR0 oder SR1 eine Verbindung zum NVIC herstellen kann
//  CCU80_CC82->INTE = ( 1 << CCU8_CC8_INTE_CM1UE_Pos);
//  CCU80_CC82->SRS = ( 2 << CCU8_CC8_SRS_CM1SR_Pos);
//
//  // enable IRQ for CCU80.SR0 (period match CC81)
//  // CC81: clear all interrupts
//  CCU80_CC82->SWR = 0x00000F3FU;
//  NVIC_ClearPendingIRQ(CCU80_1_IRQn);
//  NVIC_EnableIRQ(CCU80_1_IRQn);
}


void BoostPFC_HwUpdateTimer (uint32_t pfc_on, uint32_t pfc_off_adder, uint32_t pfc_demag_max)
{
    CCU80_CC82->PRS  = (pfc_on + pfc_demag_max);                 // set period for CC82
    CCU80_CC82->CR1S = (pfc_on);                                 // set compare1 for CC82
    CCU80_CC82->CR2S = (pfc_on + pfc_demag_max - pfc_off_adder); // set compare2 for CC82
    CCU80->GCSS = CCU8_GCSS_S2SE_Msk;                            // request shadow transfer for CC82
}


void BoostPFC_HwReset2Off (void)
{
    // Port change not necessary, because they are already set to safe state inside ControllerInit()
    P0_9_set_mode(OUTPUT_PP_GP);                                 // set P0.9 (PFC_GATE) to GPIO
    P0_9_reset();                                                // set P0.9 (PFC_GATE) to low
    P0_10_set_mode(INPUT);		                                   // PFC_RETRIGGER
    P0_12_set_mode(INPUT_PU);                                    // PFC_SHUNT_CMP_OUT (CCU80.IN1A)
}


void BoostPFC_HwDeepsleep2Ready (void)
{
    boostpfc_deepsleep_requested = 0;
    BoostPFC_Buck_HwDeepsleep2Ready();
}


void BoostPFC_HwReady2Run (void)
{
}


void BoostPFC_HwRun2Ready (void)
{
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX);       // GDI pulses to keep FAN3182 in run mode, no gate drive
}


void BoostPFC_HwRun2ReadyWithoutZCD (void)
{
    P0_9_set_mode(OUTPUT_PP_GP);                                                  // set P0.9  (PFC_GATE) to GPIO
    WR_REG(CCU80_CC82->INS, CCU8_CC8_INS_EV0EM_Msk, CCU8_CC8_INS_EV0EM_Pos, 0);   // disable ZCD event
    CCU80_CC82->SWR = 0x00000F3FU;                                                // clear event flags
    CCU80_CC82->TCCLR = (1 << CCU8_CC8_TCCLR_TCC_Pos) |                           // Stop and Clear Timer
                        (1 << CCU8_CC8_TCCLR_TRBC_Pos);
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX_INTERMEDIATE);           // GDI pulses to keep FAN3182 in run mode, no gate drive

    CCU80_CC82->TCSET = (1 << CCU8_CC8_TCSET_TRBS_Pos);                           // Start Timer
    P0_9_set_mode(OUTPUT_PP_AF5);
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX_SURGE);                  // GDI pulses to keep FAN3182 in run mode, no gate drive
}


void BoostPFC_HwEnableZCD (void)
{
    WR_REG(CCU80_CC82->INS, CCU8_CC8_INS_EV0EM_Msk, CCU8_CC8_INS_EV0EM_Pos, 2);   // enable ZCD event
}


void BoostPFC_HwGateSwitchOff (void)
{
    P0_9_set_mode(OUTPUT_PP_GP);                                                  // set P0.9  (PFC_GATE) to GPIO
    P0_9_reset();                                                                 // set P0.9  (PFC_GATE) to low
}


void BoostPFC_HwGateSwitchOn (void)
{
    P0_9_set_mode(OUTPUT_PP_GP);
    WR_REG(CCU80_CC82->INS, CCU8_CC8_INS_EV0EM_Msk, CCU8_CC8_INS_EV0EM_Pos, 0);   // disable ZCD event
    CCU80_CC82->SWR = 0x00000F3FU;                                                // clear event flags
    CCU80_CC82->TCCLR = (1 << CCU8_CC8_TCCLR_TCC_Pos) |                           // Stop and Clear Timer
                        (1 << CCU8_CC8_TCCLR_TRBC_Pos);
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX_INTERMEDIATE);           // GDI pulses to keep FAN3182 in run mode, no gate drive

    CCU80_CC82->TCSET = (1 << CCU8_CC8_TCSET_TRBS_Pos);                           // Start Timer
    P0_9_set_mode(OUTPUT_PP_AF5);
    BoostPFC_HwUpdateTimer (PFC_ON_MIN, 0, PFC_DEMAG_MAX_SURGE);                  // GDI pulses to keep FAN3182 in run mode, no gate drive
}


void BoostPFC_HwReady2Deepsleep (void)
{
    boostpfc_deepsleep_requested = 1;
    BoostPFC_Buck_HwReady2Deepsleep();
}


uint32_t BoostPFC_HwQueryCmpEvent (void)
{
    return 0;
}

uint32_t BoostPFC_HwGetDemagTime (void)
{
    return 0;
}

void BoostPFC_HwActivateCapture (void)
{
//  CCU80_CC83->INS  |= (15 << CCU8_CC8_INS_EV0IS_Pos) |   // event 0 (external start): (CCU80.IN3P) (CCU80.ST2 - at compare 1 match of boost timer)
//                      ( 1 << CCU8_CC8_INS_EV0EM_Pos) |   // event 0 (external start): rising edge (CCU80.IN3P) (CCU80.ST2 - at compare 1 match of boost timer)
//                      (15 << CCU8_CC8_INS_EV1IS_Pos) |   // event 1 (CCU80.IN1F) (CCU80.IN3P) (CCU80.ST2 - at compare 1 match of boost timer)
//                      ( 2 << CCU8_CC8_INS_EV1EM_Pos) ;   // event 1 (capt0): falling edge (CCU80.IN3P) (CCU80.ST2 - at ZCD of boost)
}


void BoostPFC_HwTriggerCapture (void)
{
}

/**********************************************************************************************************************/
/********************************************************B U C K ******************************************************/
/**********************************************************************************************************************/

/* internal function */
static void Buck_HwCC42Init (void)
{
    // set prescaler to 2 --> 32 MHz
    CCU40_CC42->PSC  |= ( 1 << CCU4_CC4_PSC_PSIV_Pos);

    // set edge aligned mode with single shot enabled
    CCU40_CC42->TC   |= ( 1 << CCU4_CC4_TC_TSSM_Pos);

    CCU40_CC42->CRS = 0;
    CCU40_CC42->PRS = 0xFFFF;
    // set shadow transfer request for CC42
    CCU40->GCSS = CCU4_GCSS_S2SE_Msk;

    // EV0IS = 10: event 0 (external start + external stop) CCU80.SR3->ERU.IOUT2->CCU40.IN2K
    // EV0EM = 1 : event 0 (external start + external stop): rising edge
    CCU40_CC42->INS  |= (10 << CCU4_CC4_INS_EV0IS_Pos) |
                        ( 1 << CCU4_CC4_INS_EV0EM_Pos) ;

    CCU40_CC42->CMC  |= ( 1 << CCU4_CC4_CMC_STRTS_Pos) ;
//                        ( 1 << CCU4_CC4_CMC_ENDS_Pos)  ;
}


uint32_t SCU_2_INTERRUPT_BOOSTPFC_ORC_REC_flag = 0;
uint32_t SCU_2_INTERRUPT_BOOSTPFC_ORC_RAIL_flag = 0;
uint32_t SCU_2_INTERRUPT_BUCK_ORC_RAIL_flag = 0;


uint32_t BoostPFC_HwRead_ORC_REC_flag(void)
{
    return SCU_2_INTERRUPT_BOOSTPFC_ORC_REC_flag;
}

uint32_t BoostPFC_HwRead_ORC_RAIL_flag(void)
{
    return SCU_2_INTERRUPT_BOOSTPFC_ORC_RAIL_flag;
}

void BoostPFC_HwClear_ORC_REC_flag(void)
{
    SCU_2_INTERRUPT_BOOSTPFC_ORC_REC_flag = 0;
}

void BoostPFC_HwClear_ORC_RAIL_flag(void)
{
    SCU_2_INTERRUPT_BOOSTPFC_ORC_RAIL_flag = 0;
}

void BoostPFC_HwEnable_ORC_REC_Interrupt(void)
{
    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC2I_Pos);                 // clear the event status flag
    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC2I_Pos);                 // enable the interrupt trigger source
    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC3I_Pos);                 // clear the event status flag
    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC3I_Pos);                 // enable the interrupt trigger source
}

void BoostPFC_HwEnable_ORC_RAIL_Interrupt(void)
{
    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC1I_Pos);                 // clear the event status flag
    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC1I_Pos);                 // enable the interrupt trigger source
}




#endif
/** @}  End of group BUCK */
