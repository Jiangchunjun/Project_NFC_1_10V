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
#include "nfcDali.h"
#include "i2c_driver.h"
#include "M24LRxx.h"
#include "nfc.h"
#include "crc.h"
#include "power_task.h"
#include "mcu_config.h"

/**tag address from 900 t0 1503 are used for OTA*/ 

#define NFC_T4T_UPDATE_REGISTER     3  // Register flag for mobile and T4T check
#define NFC_MOBILE_REGISTER         413// Register flag for mobile mode setting
#define NFC_CURRENT_ADDRESS         60 // csm current setting address
#define NFC_MOBILE_CURRENT          0  // Mobile mode current setting address
#define NFC_MOBLIE_DATA             0X123456 //mobile mode flag
#define NFC_SECOND_ADRESS           416 //back up data start address
#define NFC_GTIN_ADRESS             256 // GTIN data address
#define NFC_ED_TIME_ADRESS          1504//1000
#define NFC_ED_REG_ADRESS           1520//1016
#define NFC_RUNTIME_PRE             1524//1020

#define MEM_BANK0_GTIN_0            g_gtin_num[0];//0x03
#define MEM_BANK0_GTIN_1            g_gtin_num[1];//0xAF
#define MEM_BANK0_GTIN_2            g_gtin_num[2];//0xA3
#define MEM_BANK0_GTIN_3            g_gtin_num[3];//0xA9
#define MEM_BANK0_GTIN_4            g_gtin_num[4];//0x89
#define MEM_BANK0_GTIN_5            g_gtin_num[5];//0x36
#define MEM_BANK0_FW_V_0            FW_VERSION_MAJOR
#define MEM_BANK0_FW_V_1            FW_VERSION_MINOR
#define MEM_BANK0_S_N_0             0xFF
#define MEM_BANK0_S_N_1             0xFF //FC
#define MEM_BANK0_S_N_2             0xFF
#define MEM_BANK0_S_N_3             0xFF
#define MEM_BANK0_S_N_4             0xFF
#define MEM_BANK0_S_N_5             0xFF
#define MEM_BANK0_S_N_6             0xFF
#define MEM_BANK0_S_N_7             0xFF
#define MEM_BANK0_HW_V_0            HW_VERSION_MAJOR
#define MEM_BANK0_HW_V_1            HW_VERSION_MINOR //update to V1.0
#define MEM_BANK0_DALI_101_V        0x08
#define MEM_BANK0_DALI_102_V        0x08
#define MEM_BANK0_DALI_103_V        0xFF
#define MEM_BANK0_N_CON_DEV         0x00
#define MEM_BANK0_N_CON_GEAR        0x01
#define MEM_BANK0_CH_INDEX          0x00

#define MEM_BANK1_CONF_ID_0         0xFF
#define MEM_BANK1_CONF_ID_1         0xFF
#define MEM_BANK1_CONF_ID_2         0xFF
#define MEM_BANK1_CONF_ID_3         0xFF
#define MEM_BANK1_CONF_ID_4         0xFF
#define MEM_BANK1_CONF_ID_5         0xFF
#define MEM_BANK1_CONF_ID_6         0xFF
#define MEM_BANK1_CONF_ID_7         0xFF
#define MEM_BANK1_CONF_ID_8         0xFF
#define MEM_BANK1_CONF_ID_9         0xFF
#define MEM_BANK1_CONF_ID_10        0xFF
#define MEM_BANK1_CONF_ID_11        0xFF
#define MEM_BANK1_CONF_ID_12        0xFF
#define MEM_BANK1_CONF_ID_13        0xFF

#define MEM_BANK2_MPC_ID            0x0C
#define MEM_BANK2_MPC_VER           0x01
#define MEM_BANK2_LEDSET_STA        0x00 //Change from 0X80 to 0X00
#define MEM_BANK2_MODE_SET          0x00
#define MEM_BANK2_NOR_CURRENT_0     ((uint8_t)((POWER_DEFALUT_CURRENT >> 8) & 0xFF)) //0x02
#define MEM_BANK2_NOR_CURRENT_1     ((uint8_t)(POWER_DEFALUT_CURRENT & 0xFF))        //0x58
#define MEM_BNAK2_TUNING_FAC        0xFF//0xFF
#define MEM_BNAK2_PRO_CURRENT_0     0xFF//0xFF
#define MEM_BNAK2_PRO_CURRENT_1     0xFF//0xFF
#define MEM_BNAK2_THERMAL_INDEX     0xFF
#define MEM_BANK2_LEDSET_CURRENT_0  0xFF
#define MEM_BANK2_LEDSET_CURRENT_1  0xFF
#define MEM_BANK2_ACT_CURRENT_0     0xFF//0xFF
#define MEM_BANK2_ACT_CURRENT_1     0xFF//0xFF
#define MEM_BANK2_ACT_VO_0          0xFF//0xFF
#define MEM_BANK2_ACT_VO_1          0xFF//0xFF

#define MEM_BANK_CLM_MPC_ID         0x06
#define MEM_BANK_CLM_MPC_VER        0x00
#define MEM_BANK_CLM_COM_EN         0x00
#define MEM_BANK_CLM_TABLE_T_1      0x00
#define MEM_BANK_CLM_AD_LEVEL_1     0x64
#define MEM_BANK_CLM_TABLE_T_2      0xFF
#define MEM_BANK_CLM_AD_LEVEL_2     0x64
#define MEM_BANK_CLM_TABLE_T_3      0xFF
#define MEM_BANK_CLM_AD_LEVEL_3     0x64
#define MEM_BANK_CLM_TABLE_T_4      0xFF
#define MEM_BANK_CLM_AD_LEVEL_4     0x64


#define MEM_BANK_PSW1_MPC_ID        0x0E
#define MEM_BANK_PSW1_MPC_VER       0x01
#define MEM_BANK_PSW1_STA_INF       0x00
#define MEM_BANK_PSW1_UNLOCK_CON    0xFF
#define MEM_BANK_PSW1_ERROR_CON     0xFF
#define MEM_BANK_PSW1_PSW_MSB       0x00
#define MEM_BANK_PSW1_PSW_LSB       0x00
#define MEM_BANK_PSW1_PSW_CONFIRM   0x00
#define MEM_BANK_PSW1_NEW_PSW_MSB   0x00
#define MEM_BANK_PSW1_NWE_PSW_LSB   0x00

#define MEM_BANK_PSW2_MPC_ID        0x0E
#define MEM_BANK_PSW2_MPC_VER       0x01
#define MEM_BANK_PSW2_STA_INF       0x00
#define MEM_BANK_PSW2_UNLOCK_CON    0xFF
#define MEM_BANK_PSW2_ERROR_CON     0xFF
#define MEM_BANK_PSW2_PSW_MSB       0x00
#define MEM_BANK_PSW2_PSW_LSB       0x00
#define MEM_BANK_PSW2_PSW_CONFIRM   0x00
#define MEM_BANK_PSW2_NEW_PSW_MSB   0x00
#define MEM_BANK_PSW2_NWE_PSW_LSB   0x00

#define MEM_BANK_INF_MPC_ID         0x02
#define MEM_BANK_INF_MPC_VER        0x00
#define MEM_BANK_INF_OPER_CON_3     0x00
#define MEM_BANK_INF_OPER_CON_2     0x00
#define MEM_BANK_INF_OPER_CON_1     0x00
#define MEM_BANK_INF_OPER_TEM       0xFF
#define MEM_BANK_INF_MAX_TEM        0xFF
#define MEM_BANK_INF_TEM_INTE_3     0xFF
#define MEM_BANK_INF_TEM_INTE_2     0xFF
#define MEM_BANK_INF_TEM_INTE_1     0xFF
#define MEM_BANK_INF_NUM_SHUT_CON   0xFF
#define MEM_BANK_INF_MAX_M_VOL      0xFF//0XFF
#define MEM_BANK_INF_TOTAL_OV_TIME_2 0xFF//0XFF
#define MEM_BANK_INF_TOTAL_OV_TIME_1 0xFF//0XFF

#define MEM_BANK_ASTRO_MPC_ID         0X05
#define MEM_BANK_MPC_VER              0X01


#define MEM_BANK_1_10_ENABLE         0X01
#define MEM_BANK_1_10_LEVEL_H        0X0C
#define MEM_BANK_1_10_LEVEL_L        0XCC


#define		PHASE_DISABLE_VALUE_U8		255
#define		PHASE_DISABLE_VALUE_U16		65535 		// 0xFFFF
#define		PHASE_DISABLE_VALUE_S16		(-1)
#define		PHASE_DISABLE_VALUE_U32		0x3FFFF

#define		ASTRO_ENABLE_MASK			0x01
#define		ASTRO_TIME_BASE_MASK		0x02
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

extern uint8_t g_nfc_ini_flag;
extern uint8_t g_nfc_current_set;
extern uint8_t g_nfc_update_flag;
extern uint8_t g_pwm_update_flag;
extern uint8_t g_nfc_start_flag;
extern uint32_t g_nfc_time,g_ed_time;
extern uint8_t  g_nfc_time_id;
extern uint8_t  g_nfc_flag_record;
extern uint8_t  g_nfc_flag_pwm;
extern uint8_t  g_nfc_dim_percent;
extern uint8_t  g_nfc_current_percent;
extern uint8_t  g_psw_update_flag,g_astro_flag;
extern uint8_t  g_astro_arr[20];

void nfc_bank_ini(void);
void nfc_bank_update(void);
void nfc_mobile_handle(void);
uint8_t nfc_bank_handle(void);
void nfc_bank_pwm_control(void);
void nfc_time_record(void);
uint8_t nfc_mpc_addr_abs(uint8_t mpc_num);
uint8_t nfc_time_id(void);
uint8_t nfc_bank_clm_handle(void);
uint8_t nfc_bank_counter(void);
void nfc_bank_ini_write(void);
void nfc_bank_ini_read(void);
void nfc_bank_tag_read(void);
void nfc_pwm_update(void);
void nfc_data_copy(void);
void nfc_nvm_data_write(void);
uint8_t nfc_nvm_data_read(void);
void nfc_psw_update(void);
uint16_t nfc_ed_record(void);
uint16_t AstroInit(void);
uint16_t AstroTimer(void);
void Astro_reset(void);
uint16_t getMainsTimerMinutes();
uint8_t nfc_crc_check(nfc_data_address nfc_address);
void nfc_nvm_time_write(void);
void nfc_nvm_time_read(void);
void nfc_flash_write(void);
void nfc_eol_handle(void);
#else
#define g_nfc_time  (0)
#define g_ed_time   (0)

#endif /* MODULE_NFC */

#endif /* __NFC_DRIVER_ */

/** @}  End of group HW */