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
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: xmc1300_utils.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/fileTank/xmc1300_utils.c $

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


#define PWM_CURRENT_INI  5760
#define PWM_VOLTAGE_INI  3200

void ControllerInit(void)
{
  //----CLOCK-SETUP---------------------------------------------------------------------------------
  SCU_GENERAL->PASSWD = 0x000000C0UL;                                                 // disable write protection to SCU registers
  WR_REG(SCU_CLK->CLKCR, SCU_CLK_CLKCR_CNTADJ_Msk, SCU_CLK_CLKCR_CNTADJ_Pos, 0x3FFU); // set delay for stabilizing VCC to 16us
  // set divider IDIV for main clock (reset value of CLKCR: 0x3FF00400) -> IDIV=bit8..15
  SCU_CLK->CLKCR = 0x3FF00000 | ((uint32_t)(32000000/SYSCLK_Hz) << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk);
  // peripheral clock = 2x main clock
  SCU_CLK->CLKCR |= (uint32_t)1 << (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Pos & (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Msk;
  SCU_CLK->CGATCLR0  |=	0X00000021D	;
  SCU_GENERAL->PASSWD = 0x000000C3UL;                                                 // enable write protection to SCU registers
  //------------------------------------------------------------------------------------------------
  
  //----SYSTICK-SETUP-------------------------------------------------------------------------------
  //SystemCoreClockUpdate();    //@todo: scheint sinnlos --> pruefen, entfernen
  SysTick_Config(SYSCLK_Hz / SYSTICK_INT_Hz);
  //------------------------------------------------------------------------------------------------
  
  
  
  P0_9_set_mode(OUTPUT_PP_GP);
  P0_9_reset();
  P0_10_set_mode(INPUT);
  P0_11_set_mode(OUTPUT_OD_GP);
  P0_11_set();
  P0_12_set_mode(INPUT_PU);
  
  
  P0_1_set_mode(OUTPUT_PP_GP);
  P0_1_reset();
  P0_2_set_mode(OUTPUT_PP_GP);
  P0_2_reset();
  P0_3_set_mode(OUTPUT_PP_GP);
  P0_3_reset();
  P0_4_set_mode(OUTPUT_PP_GP);
  P0_4_reset();
  
  P0_8_set_mode(OUTPUT_PP_GP);
  P0_8_reset();
  P2_1_set_mode(INPUT);
  P2_2_set_mode(INPUT);
  P2_6_set_mode(INPUT);
  P2_7_set_mode(INPUT);
  
  /* Configure ADC Pins*/
  P2_0_set_mode(INPUT);
  P2_3_set_mode(INPUT);
  P2_4_set_mode(INPUT);
  P2_5_set_mode(INPUT);
  P2_8_set_mode(INPUT);
  P2_9_set_mode(INPUT);
  P2_10_set_mode(INPUT);
  P2_11_set_mode(INPUT);
  
  /* Configure additional Pins of U100 */
  P0_0_set_mode(OUTPUT_PP_GP);
  P0_0_set();
  P0_6_set_mode(INPUT_PU);
  P0_7_set_mode(OUTPUT_PP_GP);
  P0_7_set();
  P0_14_set_mode(INPUT_PU);
  
  P1_0_set_mode(OUTPUT_PP_GP);
  P1_0_reset();
  P0_13_set_mode(OUTPUT_PP_GP);
  P0_13_reset();
  P1_1_set_mode(INPUT);
  
  /* Configure unused Pins*/
  P0_5_set_mode(OUTPUT_PP_GP);          // Debug pin
  P0_5_reset();
  
  P0_8_set_mode(INPUT_PU);              // unused (input, pull-up)
  P1_2_set_mode(INPUT_PU);              // unused (input, pull-up)
  P1_3_set_mode(INPUT_PU);              // unused (input, pull-up)
  P1_4_set_mode(INPUT_PU);              // unused (input, pull-up)
  P1_5_set_mode(INPUT_PU);              // unused (input, pull-up)
  
  
  /* Configure for CCU40 and CCU41*/
  //p0.1
  PORT0->IOCR0 &= ~0x000000f8UL;  //OUTPUT_PP_AF4
  PORT0->IOCR0 |= 0xA0U << 0;
  CCU40->GIDLC	|=		0X00000100;//dividor
  CCU40_CC40->PSC	&=	0XFFFFFFFA;//64MHz
  CCU40_CC40->PRS	=	0x00001900;//0X0000027F; ton 1us blanking time 5us//180
  CCU40_CC40->CRS	=	PWM_CURRENT_INI;//0X00000140;
  CCU40->GCSS	|=		0X00000001;//transfer
  CCU40->GIDLC	|=	0X00000001;//IDEL mode clear
  CCU40_CC40->TCSET	=	0X00000001; //start timer
  PORT0->IOCR0 &= ~0x0000f800UL;//OUTPUT_PP_AF4
  PORT0->IOCR0 |= 0xA0U << 8;
  //P0_1_set_mode(OUTPUT_PP_AF4);
  CCU40->GIDLC	|=		0X00000100;//dividor
  CCU40_CC41->PSC	&=	0XFFFFFFFA;//64MHz
  CCU40_CC41->PRS	=	0x00001900;//0X0000027F; ton 1us blanking time 5us//180
  CCU40_CC41->CRS	=	PWM_VOLTAGE_INI;//0X00000140;
  CCU40->GCSS	|=		0X00000010;//transfer
  CCU40->GIDLC	|=	0X00000002;//IDEL mode clear
  CCU40_CC41->TCSET	=	0X00000001; //start timer  
  /////////////// Below WDT Configure is generated automatically, please do not change ///////////////
  WDT->WLB	=	0X00000000;
  WDT->WUB	=	0X0000FA00;
  WDT->CTR	=	0X00000001;
  /////////////// Above WDT Configure is generated automatically, please do not change ///////////////
  
  
  /***** Enable WDT *****/
  WR_REG(WDT->SRV,WDT_SRV_SRV_Msk,WDT_SRV_SRV_Pos,0xABADCAFE);					//WDT_Service();
  WDT->CTR	=	0X00000001;
}

#endif


/** @}  End of group HW */
