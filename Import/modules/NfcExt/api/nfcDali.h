// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram spa
//     Via Castagnole 65/a
//     31100 Treviso (I)
//
//
// The content of this file is intellectual property of OSRAM spa. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: 2014-12, g.marcolin@osram.it
//
// Change History:
//
// $Author: G.Marcolin $
// $Revision: 2911 $
// $Date: 2016-02-11 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: nfcDali.h 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/NfcExt/api/nfcDali.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NFC_DALI_H
#define __NFC_DALI_H

#ifdef MODULE_NFC_DALI

#include <stdint.h>

/**********************************************************************************************************************/

#define DEVICE_CHANNELS_MAX				(4)

#if ( DEVICE_CHANNELS > DEVICE_CHANNELS_MAX )
#error	!!! Too many DALI channels !!!
#endif

/**********************************************************************************************************************/

#define ERR_HANDLER_NFC_DALI_TOT_SIZE           "Total size exceeded. Increase MB_TOT_SIZE_BYTE"
#define ERR_HANDLER_NFC_DALI_MAX_MB_SIZE        "MB size exceeded. Increase MB_MAX_SIZE_BYTE"

/**********************************************************************************************************************/

//#ifdef __nfcDali
typedef struct
{
	uint8_t number;
	uint8_t sizeByte;
	uint8_t *ptr;
    
}mem_bank_t;

typedef struct
{
    uint8_t GTIN[6];
    uint8_t FW_VER[2];
    uint8_t SE_NUM[8];
    uint8_t HW_VER[2];
    uint8_t DALI_VER[3];
    uint8_t Num_Dev;
    uint8_t Num_Con_Gear;
    uint8_t Channel_Index;
}mem_bank0_t;

typedef struct
{
    uint8_t Config_Id[14];
}mem_bank1_t;

typedef struct 
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t LEDset2_status;
    uint8_t Mode_Setting;
    uint8_t Norm_current_0;
    uint8_t Norm_current_1;
    uint8_t Tuning_factor;
    uint8_t Pro_Current_0;
    uint8_t Pro_Current_1;
    uint8_t Thermal_index;
    uint8_t Ledset_current_0;
    uint8_t Ledset_current_1;
    uint8_t Actual_out_current_0;
    uint8_t Actual_our_current_1;
    uint8_t Out_Voltage_0;
    uint8_t Out_Voltage_1;
}mem_bank_csm_t;


typedef struct 
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Command;
    uint8_t Table_time_1;
    uint8_t Table_level_1;
    uint8_t Table_time_2;
    uint8_t Table_level_2;
    uint8_t Table_time_3;
    uint8_t Table_level_3;
    uint8_t Table_time_4;
    uint8_t Table_level_4;
}mem_bank_clm_t;

typedef struct
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Satus_infor;
    uint8_t Unlock_con;
    uint8_t Error_con;
    uint8_t PassWord_Msb;
    uint8_t Password_Lsb;
    uint8_t Password_Confirm;
    uint8_t New_passw_Msb;
    uint8_t New_passw_Lsb;
}mem_bank_psw1_t; 

typedef struct
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Oper_con_3;
    uint8_t Oper_con_2;
    uint8_t Oper_con_1;
    uint8_t Inter_oper_tem;
    uint8_t Max_tem;
    uint8_t Tem_integrator_3;
    uint8_t Tem_integrator_2;
    uint8_t Tem_integrator_1;
    uint8_t Num_shut_con;
    uint8_t Max_mes_s_vol;
    uint8_t Total_OV_time_2;
    uint8_t Total_OV_time_1;
}mem_bank_inf_t;

typedef struct
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Satus_infor;
    uint8_t Unlock_con;
    uint8_t Error_con;
    uint8_t PassWord_Msb;
    uint8_t Password_Lsb;
    uint8_t Password_Confirm;
    uint8_t New_passw_Msb;
    uint8_t New_passw_Lsb;
}mem_bank_psw2_t; 

typedef struct

{
    uint8_t Psw1_copy_status;
    uint8_t Psw1_encrypted_msb;
    uint8_t Psw1_encrypted_lsb;
}mem_bank_psw1_copy_t;

typedef struct

{
    uint8_t Psw2_copy_status;
    uint8_t Psw2_encrypted_msb;
    uint8_t Psw2_encrypted_lsb;
}mem_bank_psw2_copy_t;


typedef struct
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Eol_status;
    uint8_t Eol_enable;
    uint8_t Eol_time;
}mem_bank_eol_t;
/******************************************************************************/

typedef struct
{
    uint8_t MPC_ID;
    uint8_t MPC_VER;
    uint8_t Astro_status;
    uint8_t Astro_control;
    uint8_t Astro_nom_level;
    //uint8_t Astro_startup_fade_h;//add two bytes for startup fade time 2017.1.22 remove this to astro V01 2017.3.2
    uint8_t Astro_startup_fade_l;
    uint8_t Astro_astro_fade;
    uint8_t Astro_switchoff_fade;
    uint8_t Astro_dim_starttime_h;
    uint8_t Astro_dim_starttime_l;
    uint8_t Astro_dim_level1;
    uint8_t Astro_dim_duration1_h;
    uint8_t Astro_dim_duration1_l;
    uint8_t Astro_dim_level2;
    uint8_t Astro_dim_duration2_h;
    uint8_t Astro_dim_duration2_l; 
    uint8_t Astro_dim_level3;
    uint8_t Astro_dim_duration3_h;
    uint8_t Astro_dim_duration3_l;   
    uint8_t Astro_dim_level4;    
    uint8_t Astro_latitude_h;
    uint8_t Astro_latitude_l;
    uint8_t Astro_longtitude_h;
    uint8_t Astro_longtitude_l;
    uint8_t Astro_time_shift;
    uint8_t Astro_ed_time_h;
    uint8_t Astro_ed_time_l;
}
mem_bank_astro_t;

/******************************************************************************/

typedef struct 
{
    mem_bank0_t mem_bank0;            //0  24  256
    mem_bank1_t mem_bank_1;           //1  14  40
    mem_bank_clm_t mem_bank_clm;      //4  11  120
    mem_bank_csm_t mem_bank_csm;      //3  16  56
    mem_bank_psw1_t mem_bank_psw1;    //16 10  192
    mem_bank_psw2_t mem_bank_psw2;    //28 10  204
    mem_bank_inf_t mem_bank_inf;      //29 14  80  
    mem_bank_psw1_copy_t mem_bank_psw1_copy;//xx 3 284
    mem_bank_psw2_copy_t mem_bank_psw2_copy;//xx 3 288
#ifdef ENASTRO
    mem_bank_astro_t mem_bank_astro;      //05 27 12 /* num change to 27 and address change to 12 
#endif
    mem_bank_eol_t mem_bank_eol;          //15 5 72
}mem_bank_nfc_t;

typedef struct
{
    mem_bank_nfc_t mem_bank_nfc_temp;
    uint32_t nfc_data_nvm_crc;
}mem_bank_nfc_nvm;

//mem_bank_nfc_t mem_q={{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23},{0,1,2,3,4,5,6,7,8,9,10,11,12,13},{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15}};


//extern mem_bank_nfc_t mem_q;

extern mem_bank_nfc_t mem_bank_nfc;
extern mem_bank_nfc_nvm mem_bank_nfc_nvm_t;
/**********************************************************************************************************************/

void nfcDali_Init(void);

void nfcDali_IncrSyncLocalData(void);

void nfcDali_SyncLocalData(void);

void nfcDali_SyncDaliLibrary(void);

/**********************************************************************************************************************/
#endif /* MODULE_NFC_DALI */

#endif


