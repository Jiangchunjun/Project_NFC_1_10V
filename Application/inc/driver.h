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
// $Id: xmc1300_utils.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/fileTank/xmc1300_utils.h $

/*******************************************************************************************************************//**
* @file
* @brief Header file for general setup of XMC1300 microcontroller and helper functions
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup HW HW - Hardware drivers and hardware dependent parameters
* @{
***********************************************************************************************************************/

#ifndef __NFC_DRIVER_
#define __NFC_DRIVER_

#ifdef MODULE_NFC

#include <string.h>
#include "gpio_xmc1300_tssop38.h"
#include "i2c_driver.h"

#include "power_task.h"
#include "nfc_local.h"
#include "i2c_userinterface.h"

/**tag address from 900 t0 1503 are used for OTA*/ 


#define NFC_ED_TIME_ADRESS       1504//1000
#define NFC_ED_REG_ADRESS        1520//1016
#define NFC_RUNTIME_PRE          1524//1020




#define		PHASE_DISABLE_VALUE_U8		255
#define		PHASE_DISABLE_VALUE_U16		65535 		// 0xFFFF
#define		PHASE_DISABLE_VALUE_S16		(-1)
#define		PHASE_DISABLE_VALUE_U32		0x3FFFF

#define		ASTRO_ENABLE_MASK			0x01
#define		ASTRO_TIME_BASE_MASK		0x04
#define     ASTRO_RESET_TIME_MASK       0X40       //bit 6 for reset ontime


#ifdef 		FAST_TIME_ASTRO
    #define     ASTRO_FAST_RATE             60  
	#define		TIME_MULTIPLY				60//1									// 1 min -> 1 sec
	#define		H24_CYCLE_MINUTES			((uint32_t) 24*60/ASTRO_FAST_RATE ) 	// Cycle 24h
    #define		ONTIME_MAX_VALUE_MINUTES	(24*60/ASTRO_FAST_RATE) 								// 24 hours    
	#define		ONTIME_MIN_VALUE_MINUTES	( (4*60)/ASTRO_FAST_RATE)								// minutes x misura
	#define		CONST_ED_70					24//4									// sec.

#else
    #define		ONTIME_MAX_VALUE_MINUTES	(24*60) 								// 24 hours    
    #define		TIME_MULTIPLY				60
    #define		H24_CYCLE_MINUTES			((uint32_t)24*60) 						// minutes;  24 hours
    #define		ONTIME_MIN_VALUE_MINUTES	( 240 )								// 235 minutes
#endif



#define NFC_TIME_ADRRESS            0x740   //0X 7C0
#define NFC_TIME_COUNT              0xC0   // C0 for 48 pcs data;// 0x40 only for 16 pcs saving
#define NVM_TIME_SAVE_PERIOD (36000)//10h
#define NFC_1_10_ENABLE             292

typedef enum
{
    NFC_DATA_TAG,
    
    NFC_DATA_BACKUP
}
nfc_data_address;


typedef enum
{
    NFC_DATA_DEFAULT,
    
    NFC_DATA_NEW_TAG,
    
    NFC_DATA_BACK_TAG,
    
    NFC_DATA_NVM
}
nfc_data_read_type;

extern uint8_t g_nfc_current_set;
extern uint8_t g_nfc_update_flag;
extern uint8_t g_pwm_update_flag;
extern uint8_t g_nfc_start_flag;
extern uint32_t g_nfc_time,g_ed_time;
extern uint8_t  g_nfc_time_id;

extern nfc_local_state_t nfc_local_state;
extern uint8_t  g_nfc_flag_pwm;
extern uint8_t  g_nfc_dim_percent;
extern uint8_t  g_nfc_current_percent;
extern uint8_t  g_psw_update_flag,g_astro_flag;



void nfc_bank_pwm_control(void);
void nfc_time_record(void);
uint8_t nfc_bank_clm_handle(void);
uint8_t nfc_time_id(void);
uint8_t nfc_bank_counter(void);
void nfc_pwm_update(void);
void nfc_nvm_data_write(void);
uint8_t nfc_nvm_data_read(void);
bool nfc_ed_record(void);
uint16_t AstroInit(void);
uint16_t AstroTimer(void);
void Astro_reset(void);
uint16_t getMainsTimerMinutes();
void nfc_nvm_time_write(void);
void nfc_nvm_time_read(void);
void nfc_flash_write(void);
void nfc_eol_handle(void);
void nfc_time_handle(void);
#else
#define g_nfc_time  (0)
#define g_ed_time   (0)

#endif /* MODULE_NFC */

#endif /* __NFC_DRIVER_ */

/** @}  End of group HW */