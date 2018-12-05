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
// Indent style: Replace tabs by spaces, 2 spaces per indentation level, max. 120 chars per line
//
// Initial version: O. Busse, M. Gerber, 2014-04
// $Author: G.Marcolin $
// $Revision: 2921 $
// $Date: 2016-02-12 11:14:57 +0100 (ven, 12 feb 2016) $
// $Id: main.c 2921 2016-02-12 10:14:57Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/main.c $


/*******************************************************************************************************************//**
* @file
* @brief Main program for OTi NONSELV Generation 2 ballasts.
* Hardware independent part, but optimized and tested only for a 32bit Cortex-M0 machine,
* currently an Infineon XMC1301.
* This program relies on:
*  - Atomar 32-bit accesses to ram (no INT_DISABLE() or such is used to read/write a uint32_t)
*  - Fast, 1-cycle barrel shifter (extensively used also for 32bit variables)
*  - Fast, 1-cycle multiplier (extensively used also for 32bit variables) <br>
*
* It is also intended to be merged with DaliSlave Software Library Gen4 (c) Osram GmbH, LE D LMS, Traunreut
***********************************************************************************************************************/


/*******************************************************************************************************************//**
* @mainpage Doxygen Documentation for the OTi NONSELV Generation 2 Ballasts Firmware.
*
* Please refer to "Files" and "Modules" above to switch to the documentation itself.
*
* <br> <br> To improve this doc, please refer to:
* - http://www.stack.nl/~dimitri/doxygen/
* - http://www.stack.nl/~dimitri/doxygen/manual/docblocks.html
* - http://www.stack.nl/~dimitri/doxygen/manual/markdown.html
*
***********************************************************************************************************************/

#include "xmc1300_utils.h"            // declaration of ControllerInit() --> move somewhere else?
#include "gpio_xmc1300_tssop38.h"     // to toggle pins... (usually not needed)
#include "nfcDali.h"
#include "nfc.h"
#include "MpcDefs.h"
#include "driver.h"
#define MODULE_I2C
#define MODULE_NFC
#include "i2c_driver.h"
#include "M24LRxx.h"

//extern mem_bank_nfc_t mem_bank_nfc;
//extern nfc_data_t nfcData;
/**********************************************************************************************************************/

void nfcUpdateStart(void)
{

}

/**********************************************************************************************************************/

void nfcUpdateStop(void)
{

}

/**********************************************************************************************************************
* Prints out a string on standard output
*
*/

void StdoutString(const char* str)
{

}

/**********************************************************************************************************************
* Puts out a char on standard output
*
*/

void StdoutChar(const char c)
{

}



/**********************************************************************************************************************
* This function reads the MPC with an offset of 3 applied to the address.
*
*/

//mpc_rw_enum Mpc_Read(uint16_t mpc_number, uint8_t address, uint8_t *value)
//{
//    address += 3;
//
//    // Read MPC byte
//
//    return (mpc_rw_enum)0;
//}

/**********************************************************************************************************************
* This function returns the lenght in bytes of the specified memory bank minus 3
*
*/

uint8_t Mpc_GetLength(uint16_t bank)
{
    //uint8_t lenght;

    // lenght = get mem bank lenght
    //lenght -= 3;

    return 0;
}

/**********************************************************************************************************************
* This function makes a WDT refresh
*
*/

void nfcRefreshWdt(void)
{
     WDT->SRV = 0xABADCAFEUL;        // write "magic word" to WDT service register // Wdt = 250 ms
}

/**********************************************************************************************************************
* System tick interuupt service routine
*
*/

void SysTick_Handler(void) 
{
    static uint32_t cnt = 0;
    static uint32_t s_time_count=0;
    static uint32_t s_time_record_count=0;

    if(++cnt == 3)                  // 2500 Hz
    {
        cnt = 0;
        nfcTimer();               // Update every 1,200 ms        
    }
    
    if(s_time_count++>=2499)
    {
      nfcRefreshWdt();
      s_time_count=0;
      g_nfc_time++;
      P0_4_toggle();
      if(s_time_record_count++>=29)
      {
        s_time_record_count=0;
        g_nfc_flag_record=1;
      }
      
    }
}

/**********************************************************************************************************************/

int main(void)
{  
  ControllerInit();
  nfcInit();
  nfcDali_Init();
  __enable_interrupt();                                                       // turn on all interrupts (needed by DALI)
  nfc_bank_ini();  
  g_nfc_time_id=nfc_time_id(); 
  while (1)
  {                                                                             // infinite loop
    nfcManager();      
    nfc_bank_update(); 
    nfc_bank_clm_handle();
    nfc_bank_pwm_control();  
    nfc_pwm_update();
    nfc_time_record();
  }
}

/** @}  End of group OS */
