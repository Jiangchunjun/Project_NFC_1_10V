// ---------------------------------------------------------------------------------------------------------------------
// XMC1300 hardware driver for analog subsystem for OTi NONSELV Generation 2 ballasts
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
// Initial version: O. Busse, M. Gerber, 2014-05
// $Author: Chunjun.Jiang $
// $Revision: 13806 $
// $Date: 2018-06-18 17:31:32 +0800 (Mon, 18 Jun 2018) $
// $Id: xmc1300_adc_hwdriver.c 13806 2018-06-18 09:31:32Z Chunjun.Jiang $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Platforms/10%20Isolated/OT%20PSE%20DALI/branches/20180612_DALI_Update/src/hwdrivers/XMC1300/xmc1300_adc_hwdriver.c $

/*******************************************************************************************************************//**
* @file
* @brief XMC1300 hardware driver for analog subsystem
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup XMC1300_HWDRIVER_ADC HWDRIVER - ADC - XMC1300 Hardware Driver for analog subsystem
* @{
***********************************************************************************************************************/

#ifdef __ICCARM__

#include "analog_hal.h"
//#include "kernel_parameters.h" // for definition of ARCH_XMC1300
#include "xmc1300.h"
//#include "buck_userinterface.h"

#define ADC_NO_OF_CHANNELS_G0  2
#define ADC_NO_OF_CHANNELS_G1  2
#define CALIB00 0x480340E0  // for ADC calibration routines (code from Infineon)
#define CALIB01 0x480340E4  // for ADC calibration routines (code from Infineon)

uint32_t * SHS0_CALOC0 = (uint32_t *) CALIB00; // for ADC calibration routines (code from Infineon)
uint32_t * SHS0_CALOC1 = (uint32_t *) CALIB01; // for ADC calibration routines (code from Infineon)


void ADC_HwInit(void)
{
    // Disable Clock Gating of VADC
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                              // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_VADC_Pos);                     // Disable clock gating of VADC
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2LOW_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284 & PWMSP001 [1.0.34] -> CCU4Global_lInit())
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                              // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1

    // Enable ADC Module
    //if (ARCH_XMC1300==1) { SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC0_Pos); } // Enable ORC; required due to erratum ADC_AI.003 in AA_Step

//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC1_Pos);                  // enable ORC P2.3 (ORC Rail)
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_CNF1_Pos);                    // rising edge: OCR @Overvoltage
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC2_Pos);                  // enable ORC P2.4 (REC_N)
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_CNF2_Pos);                    // rising edge: OCR @Overvoltage
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC3_Pos);                  // enable ORC P2.5 (REC_L)
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_CNF3_Pos);                    // rising edge: OCR @Overvoltage
//
//    // NOTE: My added code to check the overvoltage
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC6_Pos);                  // enable ORC6 -> P2.8 (ORC V_LED)
//    SET_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_CNF6_Pos);                    // rising edge: OCR
//
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC6I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC6I_Pos);                 // enable the interrupt trigger source
//
//    NVIC_ClearPendingIRQ(SCU_2_IRQn);                                    // clear node 2 pending status
//    NVIC_EnableIRQ(SCU_2_IRQn);                                          // enable node 2 for interrupt request generation
//
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC1I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC2I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC3I_Pos);                 // clear the event status flag
//
//    CLR_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC1I_Pos);                 // do not enable here the interrupt trigger source
//    CLR_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC2I_Pos);                 // do not enable here the interrupt trigger source
//    CLR_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC3I_Pos);                 // do not enable here the interrupt trigger source
    //disable ORC and SCU interrupt.
    // End of my code

//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC1I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC1I_Pos);                 // enable the interrupt trigger source
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC2I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC2I_Pos);                 // enable the interrupt trigger source
//    SET_BIT(SCU_INTERRUPT->SRCLR, SCU_INTERRUPT_SRCLR_ORC3I_Pos);                 // clear the event status flag
//    SET_BIT(SCU_INTERRUPT->SRMSK, SCU_INTERRUPT_SRMSK_ORC3I_Pos);                 // enable the interrupt trigger source

    CLR_BIT(VADC->CLC, VADC_CLC_DISR_Pos);                                // Request to enable ADC Module clock
    while((RD_REG(VADC->CLC, VADC_CLC_DISS_Msk, VADC_CLC_DISS_Pos))!= 0); // Wait till ADC Module clock is enabled


    WR_REG(VADC_G0->ARBCFG, VADC_G_ARBCFG_ANONC_Msk, VADC_G_ARBCFG_ANONC_Pos, 3);   // set normal operation (ANONS is set to 11b)
    WR_REG(VADC_G1->ARBCFG, VADC_G_ARBCFG_ANONC_Msk, VADC_G_ARBCFG_ANONC_Pos, 3);   // set normal operation (ANONS is set to 11b)

    // Startup Calibration

    // SUCAL in GLOBCFG
    // CALS and CAL abfragen
    // separate f�r G0 und G1
    // STATE in SHSCFG abfragen

    // Execute start up calibration
    SET_BIT(VADC->GLOBCFG, VADC_GLOBCFG_SUCAL_Pos);                                     // Initiate start up calibration
    while((RD_REG(VADC_G0->ARBCFG, VADC_G_ARBCFG_CALS_Msk, VADC_G_ARBCFG_CALS_Pos))!= 1)
        while((RD_REG(VADC_G0->ARBCFG, VADC_G_ARBCFG_CAL_Msk, VADC_G_ARBCFG_CAL_Pos))!= 0); // Wait till calibration is completed

    while((RD_REG(VADC_G1->ARBCFG, VADC_G_ARBCFG_CALS_Msk, VADC_G_ARBCFG_CALS_Pos))!= 1);
    while((RD_REG(VADC_G1->ARBCFG, VADC_G_ARBCFG_CAL_Msk, VADC_G_ARBCFG_CAL_Pos))!= 0); // Wait till calibration is completed

    //while((RD_REG(SHS0->SHSCFG, SHS_SHSCFG_STATE_Msk, SHS_SHSCFG_STATE_Pos)) == 3);  // n�tig? oder bereits durch die beiden Befehle vorher abgedeckt?

//  // Configure arbitration for Group 0 & 1 - operating mode
//  CLR_BIT(VADC_G0->ARBCFG, VADC_G_ARBCFG_ARBM_Pos);                               //###########// The arbiter runs permanently - needed for parallel sampling
//  CLR_BIT(VADC_G1->ARBCFG, VADC_G_ARBCFG_ARBM_Pos);                               //###########// The arbiter runs permanently - needed for parallel sampling
//
//  // Configure arbitration for Group 0 & 1 - scan source
//  SET_BIT(VADC_G0->ARBPR, VADC_G_ARBPR_ASEN1_Pos);                                // Enable arbitration slot 1 (group scan source)
//  SET_BIT(VADC_G1->ARBPR, VADC_G_ARBPR_ASEN1_Pos);                                // Enable arbitration slot 1 (group scan source)


    // Configure Scan sources Group 0
    // Output current add by moon
    SET_BIT(VADC_G0->ASSEL, VADC_G_ASSEL_CHSEL6_Pos);                               // Select G0 CH6 on P2.1 as input channel (Output current)
    WR_REG(VADC_G0->CHCTR[6], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 0); // Select G0 Result Register 0 for G0 CH6 (Output current)
    // Output voltage
    SET_BIT(VADC_G0->ASSEL, VADC_G_ASSEL_CHSEL7_Pos);                               // Select G0 CH7 on P2.2 as input channel (ouput voltage)
    WR_REG(VADC_G0->CHCTR[7], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 1); // Select G0 Result Register 1 for G0 Ch7 (ouput voltage)


    // Configure Scan sources Group 1
    // Temperature 
    SET_BIT(VADC_G1->ASSEL, VADC_G_ASSEL_CHSEL5_Pos);                               // Select G1 CH5 on P2.3 as input channel (Temperature)
    WR_REG(VADC_G1->CHCTR[5], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 0); // Select G1 Result Register 0 for G1 CH5 (Temperature)
    // VCC voltage detection
    SET_BIT(VADC_G1->ASSEL, VADC_G_ASSEL_CHSEL6_Pos);                               // Select G1 CH6 on P2.4 as input channel (VCC voltage)
    WR_REG(VADC_G1->CHCTR[6], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 1); // Select G1 Result Register 1 for G1 CH6 (VCC voltage)
//    // ADC_LED
//    SET_BIT(VADC_G1->ASSEL, VADC_G_ASSEL_CHSEL0_Pos);                               // Select G1 CH0 on P2.8 as input channel (ADC_LED)
//    WR_REG(VADC_G1->CHCTR[0], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 2); // Select G1 Result Register 3 for G1 CH0 (ADC_LED)
//    // LEDSET_SET
//    SET_BIT(VADC_G1->ASSEL, VADC_G_ASSEL_CHSEL2_Pos);                               // Select G1 CH2 on P2.10 as input channel(LEDSET_SET)
//    WR_REG(VADC_G1->CHCTR[2], VADC_G_CHCTR_RESREG_Msk, VADC_G_CHCTR_RESREG_Pos, 3); // Select G1 Result Register 4 for G1 CH2 (LEDSET_SET)


    // Configure arbitration for Group 0 & 1 - operating mode
    CLR_BIT(VADC_G0->ARBCFG, VADC_G_ARBCFG_ARBM_Pos);                               //###########// The arbiter runs permanently - needed for parallel sampling
    CLR_BIT(VADC_G1->ARBCFG, VADC_G_ARBCFG_ARBM_Pos);                               //###########// The arbiter runs permanently - needed for parallel sampling

    // Configure arbitration for Group 0 & 1 - scan source
    SET_BIT(VADC_G0->ARBPR, VADC_G_ARBPR_ASEN1_Pos);                                // Enable arbitration slot 1 (group scan source)
    SET_BIT(VADC_G1->ARBPR, VADC_G_ARBPR_ASEN1_Pos);                                // Enable arbitration slot 1 (group scan source)


    // Configure Group 0
    SET_BIT(VADC_G0->ASMR, VADC_G_ASMR_ENGT_Pos);	// No gating: Conversion requests are issued if at least one pending bit is set
//    SET_BIT(VADC_G0->ASMR, VADC_G_ASMR_SCAN_Pos);	// Enable autoscan: a request source event automatically generates a load event
//    SET_BIT(VADC_G0->ASMR, VADC_G_ASMR_LDEV_Pos);	// Generate a load event: NUR bei continuous operation!!!

    // Configure Group 1
    SET_BIT(VADC_G1->ASMR, VADC_G_ASMR_ENGT_Pos);	// No gating: Conversion requests are issued if at least one pending bit is set
//    SET_BIT(VADC_G1->ASMR, VADC_G_ASMR_SCAN_Pos);	// Enable autoscan: a request source event automatically generates a load event
//    SET_BIT(VADC_G1->ASMR, VADC_G_ASMR_LDEV_Pos);	// Generate a load event: NUR bei continuous operation!!!
}


void ADC_HwDeInit(void)
{
    // Disable ADC Module
    SET_BIT(VADC->CLC, VADC_CLC_DISR_Pos);                                // Request to disable ADC Module clock
    while((RD_REG(VADC->CLC, VADC_CLC_DISS_Msk, VADC_CLC_DISS_Pos))!= 1); // Wait till ADC Module clock is disabled
    CLR_BIT(COMPARATOR->ORCCTRL, COMPARATOR_ORCCTRL_ENORC0_Pos);          // Disable ORC; required due to erratum ADC_AI.003

    // Enable Clock Gating of VADC
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C0UL;                         // disable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
    SET_BIT(SCU_CLK->CGATSET0, SCU_CLK_CGATSET0_VADC_Pos);                // Enable clock gating of VADC
    while ((uint32_t)SCU_CLK->CLKCR & (uint32_t)SCU_CLK_CLKCR_VDDC2HIGH_Msk) {} // Wait until VDDC is stabilized (16us, because CLKCR.CNTADJ=0x3FF: ref. to XMC1300 AA-Step Reference Manual V1.1 2014-04 page 284)
    SCU_GENERAL->PASSWD = (uint32_t)0x000000C3UL;                         // enable bit protection scheme for SCU_CLKCR, SCU_CGATSET0, SCU_CGATCLR0, SCU_ANAOFFSET, VADC0_ACCPROT0, VADC0_ACCPROT1
}



// copy raw values from ADC group result registers into global array adc_readout_values_n
// ADC_CH_xxx from analog_hal.h are chosen in a way that they match the order from of ADC group result registers
void ADC_HwUpdateRawValues(uint16_t* adc_readout_values_n)
{
    uint32_t adc_result_register_no;


    // start conversion
    SET_BIT(VADC_G0->ASMR, VADC_G_ASMR_LDEV_Pos); // Generate a load event
    SET_BIT(VADC_G1->ASMR, VADC_G_ASMR_LDEV_Pos); // Generate a load event

    // wait until all channels are finished
    while ((RD_REG(VADC_G0->VFR, 0x00000003, 0)) != 0x00000003)                 // G0
    {
    };
    while ((RD_REG(VADC_G1->VFR, 0x0000000F, 0)) != 0x00000003)                 // G1
    {
    };


    // copy content of ADC result registers of group 0 into global array
    for ( adc_result_register_no = 0; adc_result_register_no < ADC_NO_OF_CHANNELS_G0; adc_result_register_no++ )
    {
        adc_readout_values_n[adc_result_register_no] =
            RD_REG( VADC_G0->RES[adc_result_register_no], VADC_G_RES_RESULT_Msk, VADC_G_RES_RESULT_Pos );
    }

    // copy content of ADC result registers of group 1 into global array
    for (adc_result_register_no = 0; adc_result_register_no < ADC_NO_OF_CHANNELS_G1; adc_result_register_no++)
    {
        adc_readout_values_n[ADC_NO_OF_CHANNELS_G0 + adc_result_register_no] =
            RD_REG(VADC_G1->RES[adc_result_register_no], VADC_G_RES_RESULT_Msk, VADC_G_RES_RESULT_Pos );
    }
}


#endif
/** @}  End of group XMC1300_HWDRIVER_ADC */
