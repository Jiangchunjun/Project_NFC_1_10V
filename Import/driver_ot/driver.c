// ---------------------------------------------------------------------------------------------------------------------
// Firmware for OT haybay 
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH

//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version:  2016-04-25
// $Author: Moon Jiang $
// $Revision: 00 $
// $Date: 2016-04 25 16:19:38 +0100 (gio, 11 feb 2016) $
// $Id: driver.c 2911 2016-04-25 15:19:38Z Moon Jiang $
/*******************************************************************************************************************//**
* @file
* @brief Handle RF request from RF and generate PWM singal to control OT
***********************************************************************************************************************/

/*******************************************************************************************************************//**
* @defgroup HW HW - Hardware drivers and hardware dependent parameters
* @{
***********************************************************************************************************************/

#ifdef MODULE_NFC

#include "driver.h"
#include "Password.h"
#include "xmc_flash.h"
#include "kernel_parameters.h"

#define ASTRO_FADE_START_H        0X00
#define ASTRO_FADE_START_L        0XAA
#define ASTRO_FADE_OFF          0X0A
#define ASTRO_FADE_ASTRO        0X4A  


uint8_t  g_nfc_ini_flag         = 0;
uint8_t  g_moble_flag           = 0;
uint8_t  g_nfc_current_set      = 0;
uint8_t  g_nfc_update_flag      = 0;
uint8_t  g_pwm_update_flag      = 1;
uint8_t  g_nfc_start_flag       = 0;
uint8_t  g_psw_update_flag      = 0;
uint32_t g_nfc_time             = 0;
uint32_t g_ed_time              = 0;
uint32_t g_last_ontime          =0;
uint8_t  g_nfc_time_id          = 0;
uint8_t  g_nfc_flag_record      = 0;
uint8_t  g_nfc_flag_pwm         = 0;
uint8_t  g_nfc_dim_percent      = 100;
uint8_t  g_nfc_current_percent  = 50;
uint8_t  g_astro_arr[20]={0X02,0X01,0X64,ASTRO_FADE_START_H,ASTRO_FADE_START_L,ASTRO_FADE_ASTRO,ASTRO_FADE_OFF,0X00,0XB4,0X50,0X00,0XF0,\
                            0X3C,0X01,0X2c,0X28,0X01,0XE0,0X50,0X00};//only for test use
uint8_t  g_astro_flag=0;//flag for Astros start purpose
uint8_t  g_inf_time_update=0;
uint8_t  g_flag_flash_write=0;
uint8_t  flag_t4t_update=0;
uint32_t g_time_nvm_pre=0;
uint16_t g_min_dim_level=0;
uint8_t last_dim_level=4;
const uint8_t g_gtin_num[6]=GTIN_NUM; 

extern nfc_mirror_state_t nfcMirrorState;

typedef struct
{
	uint16_t  mode ;
  	int16_t  duration[8];
	uint16_t  level[8];
	uint16_t  fade[8];

	int32_t  stepcounter[8];
	uint32_t  ontimeED;
	uint16_t  presentLevel ;

}
typeAstro_t;
typeAstro_t  dimmer;

uint8_t dimEnable;
uint8_t dimStep;
uint8_t dimFlag;
bool dimOverride;



/*******************************************************************************
* @Brief   NFC tag data ini
First check if it is new tag (check GTIN and new_flag)
then check if has new data update, finally check backup data. 
IF all the tag and back data are broken, will start with defaul data
* @Param   
* @Note    ini structure for nfc_bank
* @Return  
*******************************************************************************/
void nfc_bank_ini(void)
{ 
    uint8_t s_nf_resigter;
#ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,137,61,87,92};
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    uint8_t s_count=0;
    
    uint16_t current_set=0;
    
    //uint8_t temp_data[6];
    
    uint8_t read_type_flag=0;// read type: 1 default data; 2 new data ; 3 back up data
    
    
    if(!g_nfc_ini_flag)
    {
        
        //        M24LRxx_ReadByte(NFC_INI_REGISTER,&s_nf_resigter);
        //        
        //        M24LRxx_ReadBuffer(NFC_GTIN_ADRESS,6,temp_data);
        //        
        //        for(s_count=0;s_count<6;s_count++)
        //        {
        //            
        //            if(temp_data[s_count]!=g_gtin_num[s_count])
        //            {
        //                
        //                s_nf_resigter=(NFC_INI_REGISTER_OK&0XF0);
        //                
        //                break;
        //                
        //            }
        //            
        //            s_nf_resigter=NFC_INI_REGISTER_OK; // GTIN is right, force INI register to set no
        //        }
        
        //        if(s_nf_resigter!=NFC_INI_REGISTER_OK)
        //        {
        //            
        //            read_type_flag=NFC_DATA_DEFAULT;  // it is new tag, write with default value
        //        
        //        }
        //        else
        //        {
        M24LRxx_ReadByte(NFC_T4T_UPDATE_REGISTER,&s_nf_resigter);
        
        if((s_nf_resigter&0xF0)==0X50)// check new data update
        {
            
            if(nfc_crc_check(NFC_DATA_TAG)==1) // new data crc check is right
            {
                /*judge if GTIN and FW version is right with default value*/
                uint8_t i=0,j=0;
                for(i=0;i<6;i++)
                {
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                    {
                        j=1;
                    }
                }
                if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                {
                    j=1;
                }
                
                if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                {
                    j=1;
                }
                if(j==0)
                {
                    read_type_flag=NFC_DATA_NEW_TAG;
                    
                    flag_t4t_update=1; // set flag for T4T update
                }
                else
                {
                    read_type_flag=NFC_DATA_DEFAULT;
                }
                /*judge if GTIN and FW version is right with default value*/ 
            }
            
            else //new data crc is wrong
            {
                
                if(nfc_crc_check(NFC_DATA_BACKUP)==1)     // check backup data crc   
                {
                    
                    /*judge if GTIN and FW version is right with default value*/
                    uint8_t i=0,j=0;
                    for(i=0;i<6;i++)
                    {
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                        {
                            j=1;
                        }
                    }
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                    {
                        j=1;
                    }
                    
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                    {
                        j=1;
                    }
                    if(j==0)
                    {
                        read_type_flag=NFC_DATA_BACK_TAG;
                    }
                    else
                    {
                        read_type_flag=NFC_DATA_DEFAULT;
                    }
                    
                }
                else                                    
                {
                    if(nfc_nvm_data_read()==1)
                    {
                        /*judge if GTIN and FW version is right with default value*/
                        uint8_t i=0,j=0;
                        for(i=0;i<6;i++)
                        {
                            if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                            {
                                j=1;
                            }
                        }
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                        {
                            j=1;
                        }
                        
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                        {
                            j=1;
                        }
                        if(j==0)
                            read_type_flag=NFC_DATA_NVM;    // all NFC tag data are wrong, read NVC data
                        else
                            read_type_flag=NFC_DATA_DEFAULT;
                    }
                    else
                        read_type_flag=NFC_DATA_DEFAULT; //all data is wrong use default data.
                    
                }
                
            }
            
        }
        else
        {
            if(nfc_crc_check(NFC_DATA_BACKUP)==1)     // check backup data crc   
            {
                
                
                /*judge if GTIN and FW version is right with default value*/
                uint8_t i=0,j=0;
                for(i=0;i<6;i++)
                {
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                    {
                        j=1;
                    }
                }
                if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                {
                    j=1;
                }
                
                if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                {
                    j=1;
                }
                if(j==0)
                    read_type_flag=NFC_DATA_BACK_TAG;
                else
                    read_type_flag=NFC_DATA_DEFAULT; //all data is wrong use default data.
                
            }
            else
            {
                
                if(nfc_crc_check(NFC_DATA_TAG)==1)
                {
                    uint8_t i=0,j=0;
                    for(i=0;i<6;i++)
                    {
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                        {
                            j=1;
                        }
                    }
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                    {
                        j=1;
                    }
                    
                    if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                    {
                        j=1;
                    }
                    if(j==0)
                        read_type_flag=NFC_DATA_NEW_TAG; // backup crc is wrong the check tag crc
                    else
                        read_type_flag=NFC_DATA_DEFAULT;
                }
                else
                {
                    
                    if(nfc_nvm_data_read()==1)
                    {
                        uint8_t i=0,j=0;
                        for(i=0;i<6;i++)
                        {
                            if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.GTIN[i]!=g_gtin_num[i])
                            {
                                j=1;
                            }
                        }
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[0]!=FW_VERSION_MAJOR)
                        {
                            j=1;
                        }
                        
                        if(mem_bank_nfc_nvm_t.mem_bank_nfc_temp.mem_bank0.FW_VER[1]!=FW_VERSION_MINOR)
                        {
                            j=1;
                        }
                        if(j==0)
                            read_type_flag=NFC_DATA_NVM;    // all NFC tag data are wrong, read NVC data
                        else
                            read_type_flag=NFC_DATA_DEFAULT;
                    }
                    else
                        read_type_flag=NFC_DATA_DEFAULT; //all data is wrong use default data.
                }
                
            }
            //            }
        }
        
        switch (read_type_flag) // opearte 3 options , default, tag, backup data.
        {
            
        case  NFC_DATA_DEFAULT:
            /*ini default value*/
            mem_bank_nfc.mem_bank0.GTIN[0]=MEM_BANK0_GTIN_0;
            mem_bank_nfc.mem_bank0.GTIN[1]=MEM_BANK0_GTIN_1;
            mem_bank_nfc.mem_bank0.GTIN[2]=MEM_BANK0_GTIN_2;
            mem_bank_nfc.mem_bank0.GTIN[3]=MEM_BANK0_GTIN_3;
            mem_bank_nfc.mem_bank0.GTIN[4]=MEM_BANK0_GTIN_4;
            mem_bank_nfc.mem_bank0.GTIN[5]=MEM_BANK0_GTIN_5;
            mem_bank_nfc.mem_bank0.FW_VER[0]=MEM_BANK0_FW_V_0;
            mem_bank_nfc.mem_bank0.FW_VER[1]=MEM_BANK0_FW_V_1;
            mem_bank_nfc.mem_bank0.SE_NUM[0]=MEM_BANK0_S_N_0;
            mem_bank_nfc.mem_bank0.SE_NUM[1]=MEM_BANK0_S_N_1;
            mem_bank_nfc.mem_bank0.SE_NUM[2]=MEM_BANK0_S_N_2;
            mem_bank_nfc.mem_bank0.SE_NUM[3]=MEM_BANK0_S_N_3;
            mem_bank_nfc.mem_bank0.SE_NUM[4]=MEM_BANK0_S_N_4;
            mem_bank_nfc.mem_bank0.SE_NUM[5]=MEM_BANK0_S_N_5;
            mem_bank_nfc.mem_bank0.SE_NUM[6]=MEM_BANK0_S_N_6;
            mem_bank_nfc.mem_bank0.SE_NUM[7]=MEM_BANK0_S_N_7;
            mem_bank_nfc.mem_bank0.HW_VER[0]=MEM_BANK0_HW_V_0;
            mem_bank_nfc.mem_bank0.HW_VER[1]=MEM_BANK0_HW_V_1;
            mem_bank_nfc.mem_bank0.DALI_VER[0]=MEM_BANK0_DALI_101_V;
            mem_bank_nfc.mem_bank0.DALI_VER[1]=MEM_BANK0_DALI_102_V;
            mem_bank_nfc.mem_bank0.DALI_VER[2]=MEM_BANK0_DALI_103_V;
            mem_bank_nfc.mem_bank0.Num_Dev=MEM_BANK0_N_CON_DEV;
            mem_bank_nfc.mem_bank0.Num_Con_Gear=MEM_BANK0_N_CON_GEAR;
            mem_bank_nfc.mem_bank0.Channel_Index=MEM_BANK0_CH_INDEX;
            mem_bank_nfc.mem_bank_1.Config_Id[0]=MEM_BANK1_CONF_ID_0;
            mem_bank_nfc.mem_bank_1.Config_Id[1]=MEM_BANK1_CONF_ID_1;
            mem_bank_nfc.mem_bank_1.Config_Id[2]=MEM_BANK1_CONF_ID_2;
            mem_bank_nfc.mem_bank_1.Config_Id[3]=MEM_BANK1_CONF_ID_3;
            mem_bank_nfc.mem_bank_1.Config_Id[4]=MEM_BANK1_CONF_ID_4;
            mem_bank_nfc.mem_bank_1.Config_Id[5]=MEM_BANK1_CONF_ID_5;
            mem_bank_nfc.mem_bank_1.Config_Id[6]=MEM_BANK1_CONF_ID_6;
            mem_bank_nfc.mem_bank_1.Config_Id[7]=MEM_BANK1_CONF_ID_7;
            mem_bank_nfc.mem_bank_1.Config_Id[8]=MEM_BANK1_CONF_ID_8;
            mem_bank_nfc.mem_bank_1.Config_Id[9]=MEM_BANK1_CONF_ID_9;
            mem_bank_nfc.mem_bank_1.Config_Id[10]=MEM_BANK1_CONF_ID_10;
            mem_bank_nfc.mem_bank_1.Config_Id[11]=MEM_BANK1_CONF_ID_11;
            mem_bank_nfc.mem_bank_1.Config_Id[12]=MEM_BANK1_CONF_ID_12;
            mem_bank_nfc.mem_bank_1.Config_Id[13]=MEM_BANK1_CONF_ID_13;   
            mem_bank_nfc.mem_bank_csm.MPC_ID=MEM_BANK2_MPC_ID;
            mem_bank_nfc.mem_bank_csm.MPC_VER=MEM_BANK2_MPC_VER;
            mem_bank_nfc.mem_bank_csm.LEDset2_status=MEM_BANK2_LEDSET_STA;
            mem_bank_nfc.mem_bank_csm.Mode_Setting=MEM_BANK2_MODE_SET;
            mem_bank_nfc.mem_bank_csm.Norm_current_0=MEM_BANK2_NOR_CURRENT_0;
            mem_bank_nfc.mem_bank_csm.Norm_current_1=MEM_BANK2_NOR_CURRENT_1;
            mem_bank_nfc.mem_bank_csm.Tuning_factor=MEM_BNAK2_TUNING_FAC;
            mem_bank_nfc.mem_bank_csm.Pro_Current_0=MEM_BNAK2_PRO_CURRENT_0;
            mem_bank_nfc.mem_bank_csm.Pro_Current_1=MEM_BNAK2_PRO_CURRENT_1;
            mem_bank_nfc.mem_bank_csm.Thermal_index=MEM_BNAK2_THERMAL_INDEX;
            mem_bank_nfc.mem_bank_csm.Ledset_current_0=MEM_BANK2_LEDSET_CURRENT_0;
            mem_bank_nfc.mem_bank_csm.Ledset_current_1=MEM_BANK2_LEDSET_CURRENT_1;
            mem_bank_nfc.mem_bank_csm.Actual_out_current_0=MEM_BANK2_ACT_CURRENT_0;
            mem_bank_nfc.mem_bank_csm.Actual_our_current_1=MEM_BANK2_ACT_CURRENT_1;
            mem_bank_nfc.mem_bank_csm.Out_Voltage_0=MEM_BANK2_ACT_VO_0;
            mem_bank_nfc.mem_bank_csm.Out_Voltage_1=MEM_BANK2_ACT_VO_1;
            mem_bank_nfc.mem_bank_clm.MPC_ID=MEM_BANK_CLM_MPC_ID;
            mem_bank_nfc.mem_bank_clm.MPC_VER=MEM_BANK_CLM_MPC_VER;
            mem_bank_nfc.mem_bank_clm.Command=MEM_BANK_CLM_COM_EN;
            mem_bank_nfc.mem_bank_clm.Table_time_1=MEM_BANK_CLM_TABLE_T_1;
            mem_bank_nfc.mem_bank_clm.Table_level_1=MEM_BANK_CLM_AD_LEVEL_1;
            mem_bank_nfc.mem_bank_clm.Table_time_2=MEM_BANK_CLM_TABLE_T_2;
            mem_bank_nfc.mem_bank_clm.Table_level_2=MEM_BANK_CLM_AD_LEVEL_2;
            mem_bank_nfc.mem_bank_clm.Table_time_3=MEM_BANK_CLM_TABLE_T_3;
            mem_bank_nfc.mem_bank_clm.Table_level_3=MEM_BANK_CLM_AD_LEVEL_3;
            mem_bank_nfc.mem_bank_clm.Table_time_4=MEM_BANK_CLM_TABLE_T_4;
            mem_bank_nfc.mem_bank_clm.Table_level_4=MEM_BANK_CLM_AD_LEVEL_4;
            mem_bank_nfc.mem_bank_psw1.MPC_ID=MEM_BANK_PSW1_MPC_ID;
            mem_bank_nfc.mem_bank_psw1.MPC_VER=MEM_BANK_PSW1_MPC_VER;
            mem_bank_nfc.mem_bank_psw1.Satus_infor=MEM_BANK_PSW1_STA_INF;
            mem_bank_nfc.mem_bank_psw1.Unlock_con=MEM_BANK_PSW1_UNLOCK_CON;
            mem_bank_nfc.mem_bank_psw1.Error_con=MEM_BANK_PSW1_ERROR_CON;
            mem_bank_nfc.mem_bank_psw1.New_passw_Msb=MEM_BANK_PSW1_PSW_MSB;
            mem_bank_nfc.mem_bank_psw1.Password_Lsb=MEM_BANK_PSW1_PSW_LSB ;
            mem_bank_nfc.mem_bank_psw1.Password_Confirm=MEM_BANK_PSW1_PSW_CONFIRM;
            mem_bank_nfc.mem_bank_psw1.New_passw_Msb=MEM_BANK_PSW1_NEW_PSW_MSB;
            mem_bank_nfc.mem_bank_psw1.New_passw_Lsb=MEM_BANK_PSW1_NWE_PSW_LSB;
            mem_bank_nfc.mem_bank_psw2.MPC_ID=MEM_BANK_PSW2_MPC_ID;
            mem_bank_nfc.mem_bank_psw2.MPC_VER=MEM_BANK_PSW2_MPC_VER;
            mem_bank_nfc.mem_bank_psw2.Satus_infor=MEM_BANK_PSW2_STA_INF;
            mem_bank_nfc.mem_bank_psw2.Unlock_con=MEM_BANK_PSW2_UNLOCK_CON;
            mem_bank_nfc.mem_bank_psw2.Error_con=MEM_BANK_PSW2_ERROR_CON;
            mem_bank_nfc.mem_bank_psw2.New_passw_Msb=MEM_BANK_PSW2_PSW_MSB;
            mem_bank_nfc.mem_bank_psw2.Password_Lsb=MEM_BANK_PSW2_PSW_LSB ;
            mem_bank_nfc.mem_bank_psw2.Password_Confirm=MEM_BANK_PSW2_PSW_CONFIRM;
            mem_bank_nfc.mem_bank_psw2.New_passw_Msb=MEM_BANK_PSW2_NEW_PSW_MSB;
            mem_bank_nfc.mem_bank_psw2.New_passw_Lsb=MEM_BANK_PSW2_NWE_PSW_LSB;
            mem_bank_nfc.mem_bank_inf.MPC_ID=MEM_BANK_INF_MPC_ID;
            mem_bank_nfc.mem_bank_inf.MPC_VER=MEM_BANK_INF_MPC_VER;
            mem_bank_nfc.mem_bank_inf.Oper_con_3=MEM_BANK_INF_OPER_CON_3;
            mem_bank_nfc.mem_bank_inf.Oper_con_2=MEM_BANK_INF_OPER_CON_2;
            mem_bank_nfc.mem_bank_inf.Oper_con_1=MEM_BANK_INF_OPER_CON_1;
            mem_bank_nfc.mem_bank_inf.Inter_oper_tem=MEM_BANK_INF_OPER_TEM;
            mem_bank_nfc.mem_bank_inf.Max_tem=MEM_BANK_INF_MAX_TEM;
            mem_bank_nfc.mem_bank_inf.Tem_integrator_3=MEM_BANK_INF_TEM_INTE_3;
            mem_bank_nfc.mem_bank_inf.Tem_integrator_2=MEM_BANK_INF_TEM_INTE_2;
            mem_bank_nfc.mem_bank_inf.Tem_integrator_1=MEM_BANK_INF_TEM_INTE_1;
            mem_bank_nfc.mem_bank_inf.Num_shut_con=MEM_BANK_INF_NUM_SHUT_CON;
            mem_bank_nfc.mem_bank_inf.Max_mes_s_vol=MEM_BANK_INF_MAX_M_VOL;
            mem_bank_nfc.mem_bank_inf.Total_OV_time_2=MEM_BANK_INF_TOTAL_OV_TIME_2;
            mem_bank_nfc.mem_bank_inf.Total_OV_time_1=MEM_BANK_INF_TOTAL_OV_TIME_1;
            mem_bank_nfc.mem_bank_astro.MPC_ID=MEM_BANK_ASTRO_MPC_ID;
            mem_bank_nfc.mem_bank_astro.MPC_VER=MEM_BANK_MPC_VER;
            mem_bank_nfc.mem_bank_eol.MPC_ID=13;
            mem_bank_nfc.mem_bank_eol.MPC_VER=0;
            mem_bank_nfc.mem_bank_1_10.Enable_1_10=MEM_BANK_1_10_ENABLE;
            mem_bank_nfc.mem_bank_1_10.Level_h_1_10=MEM_BANK_1_10_LEVEL_H;
            mem_bank_nfc.mem_bank_1_10.Level_l_1_10=MEM_BANK_1_10_LEVEL_L; 
            /*copy data to nfaData.item*/
            for(s_count=0;s_count<nfcData.totItemNum;s_count++)
            {
                memcpy(nfcData.item[s_count].ptr,(uint8_t*)&mem_bank_nfc+s_arr[s_count],nfcData.item[s_count].size);
            }
            /*write data in backup area*/
            nfc_bank_ini_write();
            /*Set no default flag*/
            //M24LRxx_WriteByte(NFC_INI_REGISTER,NFC_INI_REGISTER_OK);
            break;
            
        case NFC_DATA_NEW_TAG :  // data area at normal Tag
            
            
            //nfc_bank_tag_read(); //read data in the tag
            
            nfc_data_copy();
            
            if(flag_t4t_update==1)
            {
                //flag_t4t_update=0;
                
                nfc_nvm_data_write();// save new data in flash
                
                nfc_bank_ini_write(); //save new data
                
            }
            else
            {
                if(nfc_nvm_data_read()==0)// check if nvm data is wrong, update with new data
                {
                    nfc_nvm_data_write();// save new data in flash
                }
            }
            
            g_astro_flag=1;
            
            break;
            
        case NFC_DATA_BACK_TAG : 
            
            //nfc_bank_ini_read();  //read backupdata
            
            nfc_data_copy();
            
            if(nfc_nvm_data_read()==0)// nvm data is wrong, update with new data
            {
                nfc_nvm_data_write();// save new data in flash  
            }
            
            break;
            
        case NFC_DATA_NVM:  //read NVC data
            
            nfc_data_copy();
            
            g_psw_update_flag=1;
            /*set flag to gernerate encrption password*/
            
            break;
            
        default :
            
            break;
        }
    }
    current_set = mem_bank_nfc.mem_bank_csm.Norm_current_1 | (mem_bank_nfc.mem_bank_csm.Norm_current_0 << 8);   
    /* Set current setting to control loop */
    Power_SetCurrent(current_set, SET_MODE_NORMAL); //set the current
    
    g_nfc_ini_flag=1;
}

/*******************************************************************************
* @Brief   nfc_bank_update
check data update falg, if yes then save tag data in the backup area
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_bank_update(void)
{
    if(g_nfc_update_flag==1)
    { 
        
        if(nfcMirrorState==NFC_MIRROR_STATE_IDLE)
        {
            
            nfc_bank_ini_write(); //save the data in the second bank
           
            g_nfc_update_flag=0;  //close the update flag
        
        }
    }
}
/*******************************************************************************
* @Brief   nfc_bank_handle
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
uint8_t nfc_bank_handle(void)
{
    return 0;
}
/*******************************************************************************
* @Brief   nfc_bank_pwm_control
check nfc tag data update flag, if yes then update current and set Astro check flag.
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_bank_pwm_control(void)
{
    static uint16_t current_set=0;
    
    if((g_nfc_start_flag == 1) && (g_pwm_update_flag == 1))
    {
        
        g_pwm_update_flag = 0;
        
        current_set = mem_bank_nfc.mem_bank_csm.Norm_current_1 | (mem_bank_nfc.mem_bank_csm.Norm_current_0 << 8);
        /*get update current*/
        /* Set current setting to control loop */
        Power_SetCurrent(current_set, SET_MODE_NORMAL);
        
        g_astro_flag=1;
        
    }
}
/*******************************************************************************
* @Brief   nfc_bank_clm_handle
Constant lumen handle, first check if clm is enable. 
Then based on the current run time do the clm duty setting.
The const duty is change linearly based on the time counting.
* @Param   null
* @Note    
* @Return  0
*******************************************************************************/
uint8_t nfc_bank_clm_handle(void)
{ 
#ifdef  EN_CLM_TIME_FAST 
#define CLM_FAST_RATE (1000*60)
#else
#define CLM_FAST_RATE (1)    
#endif
#define CLM_TIME1 0X00
#define CLM_TIME2 0X01
#define CLM_TIME3 0X02
#define CLM_TIME4 0X03
    
    static uint32_t s_count=200;
    uint8_t s_clm_status=0X00;
    uint32_t clm_level=10000;
    //uint8_t  tx_buff[9];
    
    
    if(g_nfc_start_flag==1&&s_count++>200) //run in every 1s
    {
        s_count=0;
        
        if(mem_bank_nfc.mem_bank_clm.Command==0X01) //CLM enable judge
        { 
            if(g_nfc_time>((mem_bank_nfc.mem_bank_clm.Table_time_4)*1000*3600/CLM_FAST_RATE)) //period 4
            {
                
                s_clm_status=0X03;
                /*set clm PWM hanlde flag 03*/
                
            }
            else if(g_nfc_time>((mem_bank_nfc.mem_bank_clm.Table_time_3)*1000*3600/CLM_FAST_RATE)) //period 3
            {
               
                s_clm_status=0X02;
                /*set clm PWM hanlde flag 02*/
                
            }
            else if(g_nfc_time>((mem_bank_nfc.mem_bank_clm.Table_time_2)*1000*3600/CLM_FAST_RATE)) //period 2
            {
                
                s_clm_status=0X01;
                /*set clm PWM hanlde flag 01*/
                
            }
            else
            {
                
                s_clm_status=0X00;                                                                   //period 1
                /*set clm PWM hanlde flag 00*/
                
            }
            
            switch(s_clm_status) /*clm PWM hanlde*/
            {
                
            case CLM_TIME4:
                
                Power_SetConstantLumenValue(mem_bank_nfc.mem_bank_clm.Table_level_4*100);            // level 4, max tiem
                
                break;
                
            case CLM_TIME1:                                                                         // level 1 to level 2
                

                clm_level=(mem_bank_nfc.mem_bank_clm.Table_level_2-mem_bank_nfc.mem_bank_clm.Table_level_1)*100 \
                        *(g_nfc_time/(3600)-(mem_bank_nfc.mem_bank_clm.Table_time_1*1000/CLM_FAST_RATE))/ \
                        (((mem_bank_nfc.mem_bank_clm.Table_time_2-mem_bank_nfc.mem_bank_clm.Table_time_1)*1000)/CLM_FAST_RATE);
                /*linearly change rate between the two setting time points*/

                clm_level+=(mem_bank_nfc.mem_bank_clm.Table_level_1*100);
               
                Power_SetConstantLumenValue(clm_level);
               
                break;
                
            case CLM_TIME2:                                                                         // level 2 to level 3
                
                clm_level=(mem_bank_nfc.mem_bank_clm.Table_level_3-mem_bank_nfc.mem_bank_clm.Table_level_2)*100 \
                        *((g_nfc_time/3600)-(mem_bank_nfc.mem_bank_clm.Table_time_2*1000/CLM_FAST_RATE))/ \
                        (((mem_bank_nfc.mem_bank_clm.Table_time_3-mem_bank_nfc.mem_bank_clm.Table_time_2)*1000)/CLM_FAST_RATE);
                /*linearly change rate between the two setting time points*/

                clm_level+=(mem_bank_nfc.mem_bank_clm.Table_level_2*100);
                
                Power_SetConstantLumenValue(clm_level);
                
                break;
                
            case CLM_TIME3:                                                                        // level 3 to level 4
                
                clm_level=(mem_bank_nfc.mem_bank_clm.Table_level_4-mem_bank_nfc.mem_bank_clm.Table_level_3)*100 \
                        *((g_nfc_time/3600)-(mem_bank_nfc.mem_bank_clm.Table_time_3*1000/CLM_FAST_RATE))/ \
                        (((mem_bank_nfc.mem_bank_clm.Table_time_4-mem_bank_nfc.mem_bank_clm.Table_time_3)*1000)/CLM_FAST_RATE);
                /*linearly change rate between the two setting time points*/

                clm_level+=(mem_bank_nfc.mem_bank_clm.Table_level_3*100);
               
                Power_SetConstantLumenValue(clm_level);
               
                break;
                
            default:
                
                Power_SetConstantLumenValue(mem_bank_nfc.mem_bank_clm.Table_level_1*100);
                
                break;
            }
        }
        else 
        {
            
            Power_SetConstantLumenValue(10000);    //CLM is not enable
        
        }           
    }
    
    return 0;
    
}
/*******************************************************************************
* @Brief   nfc_bank_counter
* @Param   
* @Note    Config  moudle
* @Return  
*******************************************************************************/
uint8_t nfc_bank_counter(void)
{
    return 0;
}
/*******************************************************************************
* @Brief   nfc_time_record
record the driver run time in the nfc tag MCU using part
update g_timer in very 30s. Total 16 blocks are using.
every 16*30 s update time in the nfc_bank stucture.
16 blocks are running in cycle.
* @Note    
* @Return  
*******************************************************************************/
void nfc_time_record(void)
{
    uint32_t s_time=0;
    
    if(g_nfc_flag_record) // 60s flag check
    { 
        
        if(nfcMirrorState==NFC_MIRROR_STATE_IDLE) //no nfc mirror task
        {
            
            while(M24LRxx_WriteBuffer(NFC_TIME_ADRRESS+g_nfc_time_id*4,4,g_nfc_time)!=I2C_CODE_OK)
            {
                /*write g_nfc_time in user nfc tag area*/
            }
            
            g_nfc_flag_record=0;  //close 60s flag
            
            if(g_nfc_time_id++>=47)// block++;if it is up to number 47 , need to set to num 0
            {
                
                g_nfc_time_id=0;  //set block num to 0
                
                s_time=g_nfc_time/60; // g_time set from second to minute.
                
                mem_bank_nfc.mem_bank_inf.Oper_con_1= (s_time&0XFF);     //record running time with hours scale
               
                mem_bank_nfc.mem_bank_inf.Oper_con_2= ((s_time>>8)&0XFF); 
                
                mem_bank_nfc.mem_bank_inf.Oper_con_3= ((s_time>>16)&0XFF);
                
                g_nfc_update_flag=1;  //set flag to store time in backup area
            
            }
        }   
    }
}
/*******************************************************************************
* @Brief   nfc_time_id
read total 16 array time data, and set g_nfc_time with maximum data. 
and return time record current index id.
* @Param   null
* @Note    
* @Return  time record id
*******************************************************************************/
uint8_t nfc_time_id(void)
{
    uint8_t s_arr[NFC_TIME_COUNT];
    uint32_t s_arr_time[(NFC_TIME_COUNT>>2)];
    uint32_t s_run_time=0;
    uint8_t i=0,j=0;
    static uint32_t s_data[4],s_crc_data;
    
    M24LRxx_ReadBuffer(NFC_TIME_ADRRESS,64,&s_arr[0]); //read first 60 byte time data //all the time data
    
    M24LRxx_ReadBuffer(NFC_TIME_ADRRESS+64,64,&s_arr[0+64]); //read second 60 byte time data //all the time data
    
    M24LRxx_ReadBuffer(NFC_TIME_ADRRESS+128,64,&s_arr[0+128]); //read third 60 byte time data //all the time data
    
    for(i=0;i<NFC_TIME_COUNT;i+=4)
    {
        
        s_arr_time[j++]=(s_arr[i]<<0)+(s_arr[i+1]<<8)+(s_arr[i+2]<<16)+(s_arr[i+3]<<24);
        /* get all 16 record time in s_arr_time*/
        if(s_arr_time[(j-1)]==0XFFFFFFFF)
        {
            /*if value is FFFFFFFF then set it to zero*/
            s_arr_time[j-1]=0;
        }
    }
    
    j=0;
    
    for(i=1;i<(NFC_TIME_COUNT>>2);i++)
    {
        if(s_arr_time[0]<s_arr_time[i])
        {
            
            s_arr_time[0]=s_arr_time[i];// get the max time data
            
            j=i;
        
        }
    }
    
    g_nfc_time=s_arr_time[0]; 
    /*set g_nfc_time with max record time*/
    
    s_run_time=mem_bank_nfc.mem_bank_inf.Oper_con_1+(mem_bank_nfc.mem_bank_inf.Oper_con_2<<8)+(mem_bank_nfc.mem_bank_inf.Oper_con_3<<16);
    /*get setting running time fromm T4T */
    s_run_time*=60;
    /* transfor from sencond to minute*/
    if(flag_t4t_update==1)
        /* if inf_time is update by T4T then update the data */   
    {
        
        flag_t4t_update=0;
        
        g_nfc_time=s_run_time;
        for(i=0;i<48;i++) 
        {
            while(M24LRxx_WriteBuffer(NFC_TIME_ADRRESS+i*4,4,g_nfc_time)!=I2C_CODE_OK)//reset all the time data with update value
            {
                /*write g_nfc_time in user nfc tag area*/
            }
        }
        
        g_inf_time_update=1;//set flag by inf update
        
    }
    XMC_FLASH_ReadBlocks((uint32_t*)FLASH_TIME_ADDRESS,s_data,1);
    
    /*read time in nvm*/
    g_time_nvm_pre=s_data[0];
    
    crcReset();
    /*reset crc value*/
    for(i=0;i<4;i++)
    {
        
        crcLoadByte(*((uint8_t*)s_data+i));
        /*calculate crc value*/
    }
    s_crc_data=crcGet();//read crc data
    
    if(s_crc_data!=s_data[1])//crc data is not right
    {
        g_time_nvm_pre=g_nfc_time;//
        
        g_flag_flash_write=1;//set write flag
    }
    /*save the time in rom*/
    if(g_time_nvm_pre==0XFFFFFFFF)//if oxffffffff then it is wrong data
    {
        g_time_nvm_pre=0;
        /*set to default value 0*/
    }
    if(g_time_nvm_pre>g_nfc_time&&(g_inf_time_update==0))// judge if nvm_pretime is more than nfc_time
    {
        g_nfc_time=g_time_nvm_pre;
        /*if yes, then set back to g_time_nvm_pre*/
    }
    else
    {
        if((g_nfc_time-g_time_nvm_pre)>(NVM_TIME_SAVE_PERIOD*2))//judge if time is more than 20h and no update from inf
        {
            if(g_inf_time_update==0)
            {
                
                g_nfc_time=g_time_nvm_pre;
                
            }
            /*g_nfc_time is not in right range ,reset to g_time_nvm_pre*/  
            else
            {
                
                g_time_nvm_pre=g_nfc_time;
                
                g_flag_flash_write=1;
                
            }
        }
        
    }
    
    return j;  // return maximum time record index id.
}
/*******************************************************************************
* @Brief   nfc_bank_ini_write
write nfc tag data in MCU backup area
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_bank_ini_write(void)
{
    uint32_t i, j;
    uint32_t itemTypeNum = 0;
    uint32_t byteCnt;
    uint32_t byteOffset;
    uint8_t *p;
    nfc_data_type_t type = (nfc_data_type_t)0;
    uint32_t value;
    crcReset();
    uint8_t ramBuf[64];
    uint8_t nfcBuf[64];
    #ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,137,61,87,92};// this is for nfcData.item to mem_bank_nfc
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    
    for(i = 0; i < nfcData.totItemNum; i++)// cycle all the nfcDate items.
    {
        byteOffset = 0;	
        
        memcpy(ramBuf, s_arr[i]+(uint8_t*)&mem_bank_nfc, nfcData.item[i].size);	 // Get RAM data from mem_bank_nfc
        
        M24LRxx_ReadBuffer(NFC_REG_ADDR_MAX + nfcData.item[i].tagAddr+NFC_SECOND_ADRESS, nfcData.item[i].size, nfcBuf);	        // Get NFC data
        /*get the nfc backup data at the item number*/
        do
        {
            byteCnt = cmpGetSizeAligned(i, byteOffset);
            if(byteCnt)
            {         
                p = &ramBuf[byteOffset];
               
                memcpy(&value, p, byteCnt) ;  // compare ram and Backup data
                
                if(memcmp(&nfcBuf[byteOffset], p, byteCnt) != 0)	
                {
                    
                    M24LRxx_WriteBuffer(NFC_REG_ADDR_MAX+nfcData.item[i].tagAddr+NFC_SECOND_ADRESS+ byteOffset, byteCnt, value);  
                    /*if it is not equal, then write ram in the backup area.*/
                }
               
                byteOffset += byteCnt;
                /*move to next address*/
               
                for(j = 0; j < byteCnt; j++)
                {
                   
                    crcLoadByte(*p); // Calculate crc
                    
                    p++;
                
                }
            }
#ifdef ENABLT_WDT
            /* Feed watchdog */
            WDT_Feeds();
#endif
        }
        
        while(byteCnt);   //current itme check is done.
        
        if(++itemTypeNum >= nfcData.typeItemNum[type])//all the type items is check done
        {
           
            M24LRxx_WriteBuffer(nfcData.typeCrcAddr[type]+NFC_SECOND_ADRESS, 4, crcGet());										        // Write crc type
            /*record crc in back up area.*/
            crcReset();
            
            itemTypeNum = 0;
            
            type = (nfc_data_type_t)((uint32_t)type + 1);// QC and FC type moving.
        
        }
    }
    
}

/*******************************************************************************
* @Brief   nfc_bank_ini_read
read data from backup area to nfc_bank_data.
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_bank_ini_read(void)
{  
    uint8_t i;
    #ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,61,87,92};
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    
    for(i = 0; i < nfcData.totItemNum; i++)// read all the nfcData in backup area
    {
        
        M24LRxx_ReadBuffer(NFC_REG_ADDR_MAX+nfcData.item[i].tagAddr+NFC_SECOND_ADRESS, nfcData.item[i].size, nfcData.item[i].ptr); 
        /*read data*/
        memcpy((uint8_t*)&mem_bank_nfc+s_arr[i],nfcData.item[i].ptr,nfcData.item[i].size);
        /*copy data from ram to mem_bank_nfc*/
    }
    
}
/*******************************************************************************
* @Brief   nfc_bank_tag_read
read tag data to mem_bank_nfc
* @Param  null 
* @Note    
* @Return null
*******************************************************************************/
void nfc_bank_tag_read(void)
{  

    uint8_t i;
#ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,61,87,92};// this is for nfcData.item to mem_bank_nfc
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    for(i = 0; i < nfcData.totItemNum; i++)//read all the nfcData items in tag
    {
        
        M24LRxx_ReadBuffer(NFC_REG_ADDR_MAX+nfcData.item[i].tagAddr, nfcData.item[i].size, nfcData.item[i].ptr); 
        /*read data in tag*/
        memcpy((uint8_t*)&mem_bank_nfc+s_arr[i],nfcData.item[i].ptr,nfcData.item[i].size);
        /*copy data to mem_bank_nfc*/
    }
    
}
/*******************************************************************************
* @Brief   nfc_data_copy
Copy mem_bank_nfc_temp to mem_bank_nfc
* @Param  null 
* @Note    
* @Return null
*******************************************************************************/
void nfc_data_copy(void)
{  
 //uint8_t i; 
#ifdef ENASTRO  
    
#define MEM_BANK_SIZE  140  // change from 132 to 133, astro add one byte back to 132. @2017 3.12 update from 137 to 140 add 1-10 3 bytes
    
#else
    
#define MEM_BANK_SIZE  105
    
#endif
 
    memcpy((uint8_t*)&mem_bank_nfc,(uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp,MEM_BANK_SIZE);
    /*copy data to mem_bank_nfc*/
    
}
/******************************************************************************
* @Brief   nfc_psw_update
psw handle. psw1 and psw2 \, if new password is comming, then check and update
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_psw_update(void)
{
    uint16_t s_psw_data;
    //static uint8_t update_flag=0;
#ifdef ENASTRO   
    static uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,137,61,87,92};// this is for nfcData.item to mem_bank_nfc
#else    
    static uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif    
    uint8_t i;
    
    if(g_psw_update_flag)//new data flag check
    {
        if(mem_bank_nfc.mem_bank_psw1.New_passw_Lsb!=0X00||mem_bank_nfc.mem_bank_psw1.New_passw_Msb!=0X00)//not empty pasw
        {
            /*new psw is not empty*/
            if((uint8_t)(mem_bank_nfc.mem_bank_psw1.New_passw_Lsb+mem_bank_nfc.mem_bank_psw1.New_passw_Msb)== \
                mem_bank_nfc.mem_bank_psw1.Password_Confirm)// new psw sum check is right
            {
                s_psw_data=PasswordUpdate((mem_bank_nfc.mem_bank_psw1.New_passw_Msb<<8)| \
                    mem_bank_nfc.mem_bank_psw1.New_passw_Lsb);  
                /*get encrption psw*/
                if(mem_bank_nfc.mem_bank_psw1.Password_Lsb!=(s_psw_data&0XFF)|| \
                    mem_bank_nfc.mem_bank_psw1.PassWord_Msb!=(s_psw_data>>8))//encrption pasw is not equal
                    /*new psw is not euqal to psw*/               
                {                    
                    
                    mem_bank_nfc.mem_bank_psw1_copy.Psw1_copy_status=0X03;
                    /*set psw flag*/
                    mem_bank_nfc.mem_bank_psw1.Satus_infor=0X03;
                    /*set psw flag*/
                    mem_bank_nfc.mem_bank_psw1_copy.Psw1_encrypted_lsb=s_psw_data;
                    /*record encrption psw*/
                    mem_bank_nfc.mem_bank_psw1_copy.Psw1_encrypted_msb=(s_psw_data>>8);
                    /*record encrption psw*/
                    mem_bank_nfc.mem_bank_psw1.Password_Lsb=s_psw_data;
                    /* psw equal to new psw*/
                    mem_bank_nfc.mem_bank_psw1.PassWord_Msb=(s_psw_data>>8);
                    /* psw equal to new psw*/
                    mem_bank_nfc.mem_bank_psw1.New_passw_Lsb=0XFF;
                    
                    mem_bank_nfc.mem_bank_psw1.New_passw_Msb= mem_bank_nfc.mem_bank_psw1.Password_Confirm;                    
                }
            }
        }
        else
        {
            if(mem_bank_nfc.mem_bank_psw1_copy.Psw1_copy_status!=0X00)
            {
                /*remove psw flag*/
                mem_bank_nfc.mem_bank_psw1_copy.Psw1_copy_status=0X00;
                
                mem_bank_nfc.mem_bank_psw1.Satus_infor=0X00;
                
                mem_bank_nfc.mem_bank_psw1_copy.Psw1_encrypted_lsb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw1_copy.Psw1_encrypted_msb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw1.Password_Lsb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw1.PassWord_Msb=0X00;
                /*clear psw*/
            }
        }
        
        if(mem_bank_nfc.mem_bank_psw2.New_passw_Lsb!=0X00||mem_bank_nfc.mem_bank_psw2.New_passw_Msb!=0X00)
        {
            /*new psw is not empty*/
            if((uint8_t)(mem_bank_nfc.mem_bank_psw2.New_passw_Lsb+mem_bank_nfc.mem_bank_psw2.New_passw_Msb)== \
                mem_bank_nfc.mem_bank_psw2.Password_Confirm)// new psw sum check is right
            {
                
                s_psw_data=PasswordUpdate((mem_bank_nfc.mem_bank_psw2.New_passw_Msb<<8)| \
                    mem_bank_nfc.mem_bank_psw2.New_passw_Lsb);  
                /*get encrption psw*/
                if(mem_bank_nfc.mem_bank_psw2.Password_Lsb!=(s_psw_data&0XFF) || \
                    mem_bank_nfc.mem_bank_psw2.PassWord_Msb!=(s_psw_data>>8))
                {
                    /*new psw is not euqal to psw*/
                    
                    mem_bank_nfc.mem_bank_psw2_copy.Psw2_copy_status=0X03;
                    
                    mem_bank_nfc.mem_bank_psw2.Satus_infor=0X03;
                    /*set psw flag*/
                    mem_bank_nfc.mem_bank_psw2_copy.Psw2_encrypted_lsb=s_psw_data;
                    /*record encrption psw*/
                    mem_bank_nfc.mem_bank_psw2_copy.Psw2_encrypted_msb=(s_psw_data>>8);
                    /*record encrption psw*/
                    mem_bank_nfc.mem_bank_psw2.Password_Lsb=s_psw_data;
                    /* psw equal to new psw*/
                    mem_bank_nfc.mem_bank_psw2.PassWord_Msb=(s_psw_data>>8);
                    /* psw equal to new psw*/
                    
                    mem_bank_nfc.mem_bank_psw2.New_passw_Lsb=0XFF;
                    
                    mem_bank_nfc.mem_bank_psw2.New_passw_Msb=mem_bank_nfc.mem_bank_psw2.Password_Confirm;                   
 
                }
            }
        }
        else
        {
            if(mem_bank_nfc.mem_bank_psw2_copy.Psw2_copy_status!=0X00)
            {
                /*remove psw flag*/
                mem_bank_nfc.mem_bank_psw2_copy.Psw2_copy_status=0X00;
                
                mem_bank_nfc.mem_bank_psw2.Satus_infor=0X00;
                
                mem_bank_nfc.mem_bank_psw2_copy.Psw2_encrypted_lsb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw2_copy.Psw2_encrypted_msb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw2.Password_Lsb=0X00;
                /*clear psw*/
                mem_bank_nfc.mem_bank_psw2.PassWord_Msb=0X00;
                /*clear psw*/
            }
        }
        for(i = 0; i < nfcData.totItemNum; i++)
        {
            
            memcpy(nfcData.item[i].ptr,(uint8_t*)&mem_bank_nfc+s_arr[i],nfcData.item[i].size);
            /*copy data from mem_bank_nfc to nfcData.item*/
        }
        
        g_nfc_update_flag=1;
        /*set nfc back up flag*/
        
        g_psw_update_flag=0;
        /*clear psw update flag*/
    }
}
/*******************************************************************************
* @Brief   nfc_ed_record
calculate ed time and record ed time.
* @Param   
* @Note    
* @Return  ed time
*******************************************************************************/
uint16_t nfc_ed_record(void)
{
    uint8_t ed_index, ed_count,i;
    uint8_t time_arr[4];
    uint8_t ed_arr[16];
    uint32_t runtime_pre;
    uint32_t ed_time;
    
    /*need to add reset on-time history function*/
    while(M24LRxx_ReadByte(NFC_ED_REG_ADRESS,&ed_index)!=I2C_CODE_OK)
    { /*read ed index*/
    }
    if(ed_index==0XFF)
    {
        /*ed_index value is wrong value*/
        ed_index=0X00;
    }
    if(ed_index>8)
        ed_index=0;
    /*ed index maximum is 7 then set to num 0*/
    while (M24LRxx_ReadByte(NFC_ED_REG_ADRESS+1,&ed_count)!=I2C_CODE_OK)
    {/*read ed record count*/
    }
    if(ed_count==0XFF)
    {
        /*ed_count value is wrong value*/
        ed_count=0X00;
    }
    if(ed_count>=8)
        ed_count=8;
    /*ed record count max is 8*/
    M24LRxx_ReadBuffer(NFC_RUNTIME_PRE,4,time_arr);
    /*get last start up time*/
    runtime_pre=((time_arr[0]<<0)+(time_arr[1]<<8)+(time_arr[2]<<16)+(time_arr[3]<<24));
    if(runtime_pre==0XFFFFFFFF)
    {
        /*runtime_pre value is wrong value*/
        runtime_pre=0X00;
    }
    g_last_ontime=runtime_pre;
    
    if(g_nfc_time>runtime_pre)   
    {
        ed_time=g_nfc_time-runtime_pre; 
        /*ed_time is equal last turn off time minus last start up time*/
    }
    else
    {
        ed_time=0;  
    }
    ed_time/=60;// sencond to minute
    
    if(ONTIME_MIN_VALUE_MINUTES<=ed_time&&ed_time<=ONTIME_MAX_VALUE_MINUTES)//ed time is in right range
    {
        while (M24LRxx_WriteBuffer(NFC_ED_TIME_ADRESS+ed_index*2,2,ed_time)!=I2C_CODE_OK)
        {
        }
        if(ed_index++>=7)//record 8 times need to recycle
        {
            ed_index=0;
        }
        if(ed_count<8)
        {
            ed_count++;  //count max value is 8
            while(M24LRxx_WriteByte(NFC_ED_REG_ADRESS+1,ed_count)!=I2C_CODE_OK)//record count
            {
            }
        }
        while(M24LRxx_WriteByte(NFC_ED_REG_ADRESS,ed_index)!=I2C_CODE_OK) //record index
        {
        }
    }
    while (M24LRxx_WriteBuffer(NFC_RUNTIME_PRE,4,g_nfc_time)!=I2C_CODE_OK)//record last power off time
    {
    }
    g_last_ontime=g_nfc_time;//record start up time
    if(ed_count>=1)
    {
        while(M24LRxx_ReadBuffer(NFC_ED_TIME_ADRESS,ed_count*2,ed_arr)!=I2C_CODE_OK)
        {
        }
        for(i=0;i<ed_count;i++)
        {
            g_ed_time+=ed_arr[2*i];
            g_ed_time+=(ed_arr[2*i+1]<<8);//get all recorded g_ed_time
        }
        g_ed_time/=ed_count;//ed time is average value
    }
    else
    {
        g_ed_time=0;
    }
    return g_ed_time;//return g ed time.

}
/*******************************************************************************
* @Brief   nfc_mpc_addr_abs
* @Param   mem bank mpc_num
* @Note    
* @Return  mem_nfc_bank abs address
*******************************************************************************/
uint8_t nfc_mpc_addr_abs(uint8_t mpc_num)
{   
#define MPC_BANK0_NUM 0
#define MPC_BANK1_NUM 1 
#define MPC_CLM_NUM   4
#define MPC_CSM_NUM   3  
#define MPC_PSW1_NUM  16
#define MPC_PSW2_NUM  28
#define MPC_PSW1_COPY_NUM  6
#define MPC_PSW2_COPY_NUM  7 
#define MPC_INF_NUM   29 
#define MPC_ASTRO_NUM 30
#define MPC_EOL_NUMBER 15
#define MPC_1_10_NUMBER 31
    switch(mpc_num)
    {
        
    case MPC_BANK0_NUM:
        return 0;    
        break;
        
    case MPC_BANK1_NUM:
        return 24;
        break;
        
    case MPC_CLM_NUM:
        return 38;
        break;
        
    case MPC_CSM_NUM:
        return 49;
        break;
        
    case MPC_PSW1_NUM:
        return 65;
        break;
        
    case MPC_PSW2_NUM:
        return 75;
        break;
        
    case MPC_INF_NUM:
        return 85;
        break;
        
    case MPC_PSW1_COPY_NUM:
        return 99;
        break;
        
    case MPC_PSW2_COPY_NUM:
        return 102;
        break;
        
    case MPC_ASTRO_NUM:
        return 105;
        break;
    case MPC_EOL_NUMBER:
        return 132;
        break;
        
    case MPC_1_10_NUMBER:
        return 137;
        
    default:
        return 100;
        break;
    }
}
/*******************************************************************************
* @Brief   AstroInit
Ininization for Astro Dimming function
Judge two modes. Then ini Astro counter value.
* @Param   null
* @Note    
* @Return  0
*******************************************************************************/
uint16_t AstroInit(void)
{
    uint32_t	ctime, center, ptime;
    uint16_t i;
   
#ifdef ENASTRO   
	//  -------------------------------------------------------- get parameters
	dimmer.mode = mem_bank_nfc.mem_bank_astro.Astro_control ;  
  	dimmer.duration[0] = (mem_bank_nfc.mem_bank_astro.Astro_dim_starttime_h<<8)+mem_bank_nfc.mem_bank_astro.Astro_dim_starttime_l;// they may depends on ED
    dimmer.duration[1] = (mem_bank_nfc.mem_bank_astro.Astro_dim_duration1_h<<8)+mem_bank_nfc.mem_bank_astro.Astro_dim_duration1_l;
   	dimmer.duration[2] = (mem_bank_nfc.mem_bank_astro.Astro_dim_duration2_h<<8)+mem_bank_nfc.mem_bank_astro.Astro_dim_duration2_l;
  	dimmer.duration[3] = (mem_bank_nfc.mem_bank_astro.Astro_dim_duration3_h<<8)+mem_bank_nfc.mem_bank_astro.Astro_dim_duration3_l;
	dimmer.duration[4] = 0;
	dimmer.duration[5] = 0;
#ifdef FAST_TIME_ASTRO/*Fast Astro rate handle*/
    dimmer.duration[0]/=ASTRO_FAST_RATE;
    dimmer.duration[1]/=ASTRO_FAST_RATE;
    dimmer.duration[2]/=ASTRO_FAST_RATE;
    dimmer.duration[3]/=ASTRO_FAST_RATE;
    dimmer.duration[4]/=ASTRO_FAST_RATE;// FAST rate setting
#endif
    /* Astro dimming level setting*/
  	dimmer.level[0] = mem_bank_nfc.mem_bank_astro.Astro_nom_level;
   	dimmer.level[1] = mem_bank_nfc.mem_bank_astro.Astro_dim_level1;							// warning 255
   	dimmer.level[2] = mem_bank_nfc.mem_bank_astro.Astro_dim_level2;
   	dimmer.level[3] = mem_bank_nfc.mem_bank_astro.Astro_dim_level3;
   	dimmer.level[4] = mem_bank_nfc.mem_bank_astro.Astro_dim_level4;
	dimmer.level[5] = 30;  // minPhysicalLevel;
    /*Astro dimming fade time setting*/ 
    dimmer.fade[0] = mem_bank_nfc.mem_bank_astro.Astro_startup_fade_l*15;					// in seconds (mem_bank_nfc.mem_bank_astro.Astro_startup_fade_h<<8)+
	dimmer.fade[1] = mem_bank_nfc.mem_bank_astro.Astro_astro_fade*2;
    dimmer.fade[5] = mem_bank_nfc.mem_bank_astro.Astro_switchoff_fade*15;					// warning 65535
    if(dimmer.fade[5]==0XEF1)//no fade switch off 
    {
        dimmer.fade[5]=0X00;
        dimmer.stepcounter[5] = 1;
    }
    else
    {
#ifdef FAST_TIME_ASTRO
        dimmer.stepcounter[5] =dimmer.fade[5]/5; // has switch off feature
#else
        dimmer.stepcounter[5] =dimmer.fade[5];
#endif
    }
#ifdef FAST_TIME_ASTRO
    dimmer.fade[0]/=5;
    dimmer.fade[1]/=5;
    dimmer.fade[5]/=5;  // Fade rate decrease 2 times
#endif
	dimmer.fade[2] = dimmer.fade[1];
	dimmer.fade[3] = dimmer.fade[1];
	dimmer.fade[4] = dimmer.fade[1];
     
	// --------------------------------------------------------- on - time
    
	dimEnable = 1;//Set dimming flag
	dimStep = 0;  //set default dimStep
    
	if ( (dimmer.mode & ASTRO_TIME_BASE_MASK) == 0) 
    {
        // ------------------------------------------------------------------------- Astro Based
        dimmer.ontimeED = g_ed_time; 										// history    ed time   
        #ifdef 		FAST_TIME_ASTRO
        //dimmer.ontimeED = g_ed_time/60; 										// FAST
        #endif
        //dimmer.duration[0]=12-dimmer.duration[0];//need to check how to get dim start time, need to remove
		if (dimmer.ontimeED == 0) {
            
		  	dimEnable = 0; 														// NOT ed time ,dim - yes fade
		    dimmer.fade[5] = PHASE_DISABLE_VALUE_U16;
            
			dimStep = 0;
			dimmer.stepcounter[0] = (uint32_t)24 * 60 * 60;							// go Nominal
			for (i=1; i <= 5; i++) {
			    dimmer.stepcounter[i] = 0;	 									// no dimm. phases
  		    }
            
		} else 
        {           
		  	// ------------------------------------------------ absolute timing
            center = dimmer.ontimeED >> 1;
			dimmer.duration[0] = center - dimmer.duration[0];
            //if (dimmer.duration[0] < 0) dimmer.duration[0] = - dimmer.duration[0];  // absolute
	    }
        
	}
    else 
    {
        // ------------------------------------------------------------------------- Time Based
        
	  	if (dimmer.duration[0] < 0) dimmer.duration[0] = - dimmer.duration[0];  // absolute
        
		dimmer.fade[5] = PHASE_DISABLE_VALUE_U16;
        
	}
    
	if ( dimEnable ) 
    {       
        ctime = 0;
        for (i=0; i <= 3; i++)
        {
            ctime = ctime + dimmer.duration[i];
            dimmer.stepcounter[i] = ctime * TIME_MULTIPLY ; 				// t2 .. t4
        }
        for (i=0; i <= 3; i++)
        {									 		// t1 .. t4 >= 0
            if ( dimmer.stepcounter[i] < dimmer.fade[0])
            { 					// ti < startupFade
                dimmer.stepcounter[i] = 0;
                if(i!=4)
                dimmer.fade[i+1]=dimmer.fade[0]; //change Astro_fade time with Start up fade time
            }
        }
        // step0 = 0: no NomLev
        // step1 = 0: no Lev 1 ...
        // step2 = 0: no Lev 2 ...
        // step3 = 0: no Lev 3 ...
        
        if (dimmer.fade[5] < PHASE_DISABLE_VALUE_U16) { 					// Switch off !
            
            dimmer.stepcounter[4] = (uint32_t)dimmer.ontimeED * TIME_MULTIPLY - dimmer.fade[5];	 	// t5
            if (dimmer.stepcounter[4] < 10) dimmer.stepcounter[4] = 10;		// only for fast test. TBD
            for (i=0; i <= 3; i++) 
            {									 	// t1 .. t4  & t5
                if ( (dimmer.stepcounter[i]+ dimmer.fade[1]) > dimmer.stepcounter[4])
                { 		// ti + fade > t5
                    dimmer.stepcounter[i] = dimmer.stepcounter[4];
                }
            }
        }
        
        // ---------------------------------------------- relative timing
        ptime = dimmer.stepcounter[0];										// rel. t1
        for (i=1; i <= 3; i++)
        {											// rel. t2 .. t4
            ctime = dimmer.stepcounter[i];
            dimmer.stepcounter[i] = (ctime - ptime);           
            ptime = ctime;
        }
        
        if (dimmer.fade[5] < PHASE_DISABLE_VALUE_U16) {
            
            dimmer.stepcounter[4] = dimmer.stepcounter[4] - ptime;    		// rel. t5  (go fade)
            
        } else {
            
            dimmer.stepcounter[4] = (uint32_t)24 * 60 * 60; 						// no fade
            
        }
        // t5 = EDtime - offFade > 4h - offFade > 3h
        
        // step3 = 0: no Lev 4 ...  stay on previous level
        // step2 = 0: no Lev 3 ...
        // step1 = 0: no Lev 2 ...
        // step0 = 0: no Lev 1 ...
        for(i=4;i>0;i--)//judge last non zero dimming level
        {
            if(dimmer.stepcounter[i] > 0)
            {
               last_dim_level=i; 
               break;
            }
        }
        dimStep = 0;       
        for (i=0; i < 5; i++) 
        {
            if ( dimmer.stepcounter[i] > 0 ) 
            {
                dimStep = i; 												// dimStep 0 .. 4
                dimFlag=1<<dimStep;
                break;
            }
        }
        
	} // enabled
    
    
    //dimmer.presentLevel = dimmer.level[dimStep];
    
	
    
    //SLOW_DaliDeviceSetLinearLevel( dimmer.level[dimStep], dimmer.fade[0] );	// fade n 0
    
    
#endif   
	return 0;
}
/*******************************************************************************
* @Brief   Astro_reset
* @Param   null
* @Note    this is used to reset all the on time informatio when getting reset order in Astro based mode
* @Return  0
*******************************************************************************/
#ifdef ENASTRO
void Astro_reset(void)
{
    uint32_t s_date[4]={0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF,0XFFFFFFFF};
    uint8_t i=0;
    
    if(mem_bank_nfc.mem_bank_astro.Astro_control&ASTRO_RESET_TIME_MASK)//reset ontime
    {
        while(M24LRxx_WriteByte(NFC_ED_REG_ADRESS+1,0)!=I2C_CODE_OK)//reset record count
        {
        }
        while(M24LRxx_WriteByte(NFC_ED_REG_ADRESS,0)!=I2C_CODE_OK)
        { /*reset ed index*/
        }
        for(i=0;i<4;i++)// reset ontime
        {
            while (M24LRxx_WriteBuffer(NFC_ED_TIME_ADRESS+i*4,4,s_date[i])!=I2C_CODE_OK)
            {
            }
        }
    }
}
#endif
/*******************************************************************************
* @Brief   AstroTimer
* @Param   null
* @Note    
* @Return  0
*******************************************************************************/
#ifdef ENASTRO
uint16_t AstroTimer(void)
{
    
#define RUNCOUNT 200
    //bool newPdOverride;
    static uint8_t count=200;
    static uint8_t flag=0;
    static uint8_t astro_flag=0,astro_first=0;;
    static uint32_t dim_count=0;
    static uint32_t s_dim_percent=0;
    uint8_t i=0;
    uint16_t temp;
    uint8_t  tx_buff[9];
    static uint8_t min_level_count=50;
	if ( count++>=200&&g_astro_flag)// every 1 second
    { 															        
        count=0;
	 	if(min_level_count++>50)
        {
            min_level_count=0;
            g_min_dim_level=Power_GetMinLevel();
            //g_min_dim_level=4000;
            for(i=0;i<6;i++)
            {
                if(dimmer.level[i]*100<g_min_dim_level)
                   dimmer.level[i]= (g_min_dim_level/100);
            }
        }
        if (dimStep >= 5) 
        {
            if((dimmer.mode & ASTRO_TIME_BASE_MASK) == 0)//Astro Based
            {
                if(mem_bank_nfc.mem_bank_astro.Astro_switchoff_fade==0XFF)// no swith off then keep level4
                    
                {
                    return 0;									// >>>>>>>>>>>>>>  exit point !
                }
                
                else   /*has switch off fade*/
                {
                    if(dimmer.fade[5]!=0)//fade time is not 0
                    {
                        if(dim_count++<dimmer.fade[dimStep])
                        {
                            /*not first dimming level*/
                            temp=dimmer.level[last_dim_level]*100;
                            /*get last dimming level*/
                            if(g_min_dim_level>(dimmer.level[last_dim_level]*100))
                            {
                                /*calculate current setting dimming level, if is increase the value.*/
                                s_dim_percent=temp+((((g_min_dim_level))-temp)*dim_count)/dimmer.fade[5];
                                
                            }
                            else
                                
                            {
                                /*calculate current setting dimming level, if is decrease the value.*/
                                s_dim_percent=temp-((temp-((g_min_dim_level)))*dim_count)/dimmer.fade[5];
                                
                            }
                            /*update dimming level*/
                            Power_SetAstroDimmingLevel(s_dim_percent);
                            return 0;
                        }
                        else
                        {
                            dim_count=0;
                            dimmer.fade[5]=0;
                            return 0;
                        }
                    }
                    else
                    {
                        Power_SetAstroDimmingLevel(g_min_dim_level);
                        return 0;
                    }
                }
            }
            else
            {
                return 0;// Time based no switch off feature.
            }
        }
        
        if(dimFlag==(1<<dimStep)&&flag==0)
        {
           
            flag=1;// flag for current Step
            
            dimFlag=0;
            
            astro_flag=1;// flag for Astro dimming counter at current dimming step
        
        }
        if(flag)
        {
            if ( dimmer.stepcounter[ dimStep ] )
            {
                /*counter couting*/
                dimmer.stepcounter[dimStep]-- ;   
               
                if(astro_flag==1)  
                {   
                    if(dimmer.fade[dimStep]<=0)
                    {
                        dimmer.fade[dimStep]=1;//make sure the fade time is not zero
                    }
                    /*Astro dimming handle*/
                    if(dim_count++<dimmer.fade[dimStep])
                    {
                        /*level change within the fade time*/
                        if(astro_first==0)
                        {
                            /*if it is first dimming step*/
                            temp=g_min_dim_level;
                            /*calculate dimming level linear change rate based on fade time*/
                            if(temp<(dimmer.level[dimStep]*100))// if level is bigger than samllest level
                            {
                                s_dim_percent=temp+((((dimmer.level[dimStep])*100)-temp)*dim_count)/dimmer.fade[dimStep];
                            }
                            else
                            {  /* min level is bigger than set value, set min value directly.*/
                                s_dim_percent=temp;//3000+((temp-3000)*dim_count)/dimmer.fade[dimStep];//need to check
                            }
                            /*update PWM*/
                            Power_SetAstroDimmingLevel(s_dim_percent);
                       
                        }
                        else
                        {   
                            /*not first dimming level*/
                            temp=dimmer.level[dimStep-1]*100;
                            /*get last dimming level*/
                            if(dimmer.level[dimStep]>dimmer.level[dimStep-1])
                            {
                                /*calculate current setting dimming level, if is increase the value.*/
                                s_dim_percent=temp+((((dimmer.level[dimStep])*100)-temp)*dim_count)/dimmer.fade[dimStep];
                            
                            }
                            else
                            
                            {
                                /*calculate current setting dimming level, if is decrease the value.*/
                                s_dim_percent=temp-((temp-((dimmer.level[dimStep])*100))*dim_count)/dimmer.fade[dimStep];
                            
                            }
                            /*update dimming level*/
                            Power_SetAstroDimmingLevel(s_dim_percent);
                        }
                    }
                    else
                    {
                        /*fade time is over, wait current phase time over*/
                        if(astro_first==0)
                        {
                            /*clear first dimming flag*/
                            astro_first=1;
                        
                        }
                        /*reset counter value*/
                        dim_count=0;
                        /*reset astro dimming step flag*/
                        astro_flag=0;
                    }
                }
                
            }
            else
            {
                /* current setp counter is over*/
                if ( dimmer.stepcounter[dimStep] <= 0)
                {
                    /*move to next step*/
                    dimStep++;
                    /*max step num is 5*/
                    if (dimStep > 5) dimStep = 5;
                    flag=0;
                    /* clear flag for current setp astro dimming*/
                    dimFlag=(1<<dimStep);
                    /*move to next dimflag*/
                    dim_count=0;
                    /*clear dim_count*/
                    astro_flag=0;
                    /*clear current step dimming falg*/
                }
            }
        }              
  	} 
    else
    {
        if(nfcMirrorState==NFC_MIRROR_STATE_IDLE)
        {
            /*nfc is idle*/
            if (getMainsTimerMinutes() >= H24_CYCLE_MINUTES&&(dimmer.mode & ASTRO_TIME_BASE_MASK) == 0)// >>>  24 h TEST <<<<<<<<<<
            {					                
                while (M24LRxx_WriteBuffer(NFC_RUNTIME_PRE,4,g_nfc_time)!=I2C_CODE_OK) //invalid on-time  and clear current working time      				                   
                {
                }
                /*Reset current on time*/
                g_last_ontime=g_nfc_time;
                AstroInit(); 						// restart Astro !        
                /*reset astro dimming first flag*/
                astro_first=0;
            }
        }
    }
	return 0;
    
}
#endif
/*******************************************************************************
* @Brief   getMainsTimerMinutes
Return current power on time in minutes
* @Param   null
* @Note    
* @Return  on time in minutes
*******************************************************************************/
/*******************************************************************************/
uint16_t getMainsTimerMinutes() 
{
    return ((g_nfc_time-g_last_ontime)/60);
}
/*******************************************************************************
* @Brief   nfc_mobile_handle
Mobile mode handle.
Fisrt setting current @ address 00 and 01 / 0500 means 500mA
Check T4T update flag, if yes then back to T4T mode
* @Param   
* @Note    
* @Return  
*******************************************************************************/
void nfc_mobile_handle()
{
    uint8_t temp_data[6],count_1=0;
    uint16_t current_set=0;
    static uint8_t count=200,flag_write=0;
    
    if(count++>=200)
    {
        /*set delay time*/
        count=0;
        /*get current data*/
        M24LRxx_ReadBuffer(NFC_MOBILE_CURRENT,2,temp_data); 
        
        current_set+=temp_data[1]&0X0F;
        
        current_set+=(((temp_data[1]&0XF0)>>4)*10);
        
        current_set+=((temp_data[0]&0X0F)*100);
        
        current_set+=(((temp_data[0]&0XF0)>>4)*1000); 

        Power_SetCurrent(current_set, SET_MODE_NORMAL);
        /*update current*/
        M24LRxx_ReadByte(NFC_T4T_UPDATE_REGISTER,&temp_data[0]);
        /*read updat flag data*/
        if(temp_data[0]==0X51||temp_data[0]==0X52||temp_data[0]==0X53)
        {
            /*T4T update flag*/
            M24LRxx_WriteBuffer(NFC_MOBILE_REGISTER,3,0); 
            
            MCU_SystemReset()   ;
//            /*clear mobile setting flag*/
//            g_moble_flag=0;
//            /*clear moble flag*/
//            g_nfc_ini_flag=0;
//            /*reset nfc ini flag*/
//            NFC_TaskInit();
//            /*reini NFC*/
        }
        if(flag_write==0)
        {
            /*if first time, then set T4T csm current to min value*/
            flag_write=1;
            
            M24LRxx_WriteBuffer(NFC_CURRENT_ADDRESS,2,0);
            
            M24LRxx_WriteBuffer(4,2,0);
            
        }
        
        M24LRxx_ReadBuffer(NFC_GTIN_ADRESS,6,temp_data);
        /*get GTIN number*/
        for(count_1=0;count_1<6;count_1++)
        {
            if(temp_data[count_1]!=g_gtin_num[count_1])
            {
              /*if GTIN num is not right then update with right value*/  
              M24LRxx_WriteByte((NFC_GTIN_ADRESS+count_1),g_gtin_num[count_1]); 
              
            }
        }
    }   
}
/*******************************************************************************
* @Brief   nfc_crc_check
Check nfc_tag data and back up data
* @Param   nfc_address : 0, nfc_tag; 1, nfc back up
* @Note    crc check for NFC tag, if not right back 1 and save temp tag data in
mem_bank_nfc_nvm_t.mem_bank_nfc_temp and this data is the same with did crc comparation
* @Return  1, crc is ok 0, crc is wrong
*******************************************************************************/
uint8_t nfc_crc_check(nfc_data_address nfc_address)
{
    
    uint32_t structIndex = 0;
    uint32_t typeItemCnt = 0;
    uint32_t crc;
    uint32_t i=0;;
    uint8_t nfcBuf[64];
    
#ifdef ENASTRO   
    uint8_t s_arr[]={105,24,49,132,85,91,95,38,65,75,0,99,102,137,61,87,92};// this is for nfcData.item to mem_bank_nfc
#else    
    uint8_t s_arr[]={24,49,85,91,95,38,65,75,0,99,102,61,87,92};
#endif   
    
    switch (nfc_address)
    {
        
    case NFC_DATA_TAG:
        /*nfc tag type*/
        structIndex = getStructIndex(NFC_DATA_TYPE_FCHANGING);
        /*Get  FC index num*/
        crcReset();
        /*reset crc*/
        if(structIndex < nfcData.totItemNum)                // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
        {
            /*index num is in right range*/
            for(typeItemCnt=0;typeItemCnt<nfcData.typeItemNum[NFC_DATA_TYPE_FCHANGING];typeItemCnt++)
            {
                /*check every fc item*/
                M24LRxx_ReadBuffer(12 + nfcData.item[structIndex + typeItemCnt].tagAddr, nfcData.item[structIndex + typeItemCnt].size, nfcBuf); 
                /*read every fc item*/
                memcpy((uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp+s_arr[structIndex + typeItemCnt],nfcBuf,nfcData.item[structIndex + typeItemCnt].size);
                /*copy nfcBuf to mem_bank_bank_temp*/
                for(i=0;i<nfcData.item[structIndex + typeItemCnt].size;i++)
                {
                    /*upload crc for every byte in each fc item*/ 
                    crcLoadByte(nfcBuf[i]);
                    
                }
            }
            
            M24LRxx_ReadBuffer(nfcData.typeCrcAddr[NFC_DATA_TYPE_FCHANGING], 4, (uint8_t *)&crc);
            /*read fc crc*/
            if(crc!=crcGet())
            {
                /*not equal then break return 0*/
                return 0;
                
                break;
                
            }
            else
            {
                /*Get  QC index num*/
                structIndex = getStructIndex(NFC_DATA_TYPE_QCONST);
                
                if(structIndex < nfcData.totItemNum)                // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
                {
                    crcReset();
                    /*reset crc*//*index num is in right range*/
                    for(typeItemCnt=0;typeItemCnt<nfcData.typeItemNum[NFC_DATA_TYPE_QCONST];typeItemCnt++)
                    {
                        
                        M24LRxx_ReadBuffer(12 + nfcData.item[structIndex + typeItemCnt].tagAddr, nfcData.item[structIndex + typeItemCnt].size, nfcBuf); 
                        /*check every qc item*//*read every fc item*/
                        memcpy((uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp+s_arr[structIndex + typeItemCnt],nfcBuf,nfcData.item[structIndex + typeItemCnt].size);
                        /*copy nfcBuf to mem_bank_bank_temp*/
                        for(i=0;i<nfcData.item[structIndex + typeItemCnt].size;i++)
                        {
                            
                            crcLoadByte(nfcBuf[i]);
                            /*upload crc for every byte in each qc item*/ 
                        }
                    }
                    /******************/
                    M24LRxx_ReadBuffer(nfcData.typeCrcAddr[NFC_DATA_TYPE_QCONST], 4, (uint8_t *)&crc);
                    /*read qc crc*/
                    if(crc!=crcGet())
                    {
                        return 0;
                        /*not equal then break return 0*/
                        break;
                    }
                }
            }
        }
        /*crc is right return 1*/
        return 1;
        break;
        
    case NFC_DATA_BACKUP:
        /*back up data type*/
        structIndex = getStructIndex(NFC_DATA_TYPE_FCHANGING);
        /*Get  FC index num*/
        crcReset();
        /*reset crc*/
        if(structIndex < nfcData.totItemNum)                // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
        {
            /*index num is in right range*/
            for(typeItemCnt=0;typeItemCnt<nfcData.typeItemNum[NFC_DATA_TYPE_FCHANGING];typeItemCnt++)
            {
                /*check every fc item*/
                M24LRxx_ReadBuffer(12 + nfcData.item[structIndex + typeItemCnt].tagAddr+NFC_SECOND_ADRESS, nfcData.item[structIndex + typeItemCnt].size, nfcBuf); 
                /*read every fc item*/
                memcpy((uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp+s_arr[structIndex + typeItemCnt],nfcBuf,nfcData.item[structIndex + typeItemCnt].size);
                /*copy nfcBuf to mem_bank_bank_temp*/                
                for(i=0;i<nfcData.item[structIndex + typeItemCnt].size;i++)
                {
                    /*upload crc for every byte in each fc item*/ 
                    crcLoadByte(nfcBuf[i]);
                    
                }
            }
            
            M24LRxx_ReadBuffer(nfcData.typeCrcAddr[NFC_DATA_TYPE_FCHANGING]+NFC_SECOND_ADRESS, 4, (uint8_t *)&crc);
            /*read fc crc*/
            if(crc!=crcGet())
            {
                
                return 0;
                /*not equal then break return 0*/
                break;
                
            }
            else
            {
                structIndex = getStructIndex(NFC_DATA_TYPE_QCONST);
                /*Get  QC index num*/
                if(structIndex < nfcData.totItemNum)                // If (structIdx == nfcData.totItemNum), aDataType is not present i.e. (nfcData.typeItemNum[aDataType] == 0)
                {
                    /*index num is in right range*/
                    crcReset();
                    /*reset crc*/
                    for(typeItemCnt=0;typeItemCnt<nfcData.typeItemNum[NFC_DATA_TYPE_QCONST];typeItemCnt++)
                    {
                        /*check every qc item*/
                        M24LRxx_ReadBuffer(12 + nfcData.item[structIndex + typeItemCnt].tagAddr+NFC_SECOND_ADRESS, nfcData.item[structIndex + typeItemCnt].size, nfcBuf); 
                        /*read every qc item*/
                        memcpy((uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp+s_arr[structIndex + typeItemCnt],nfcBuf,nfcData.item[structIndex + typeItemCnt].size);
                        /*copy nfcBuf to mem_bank_bank_temp*/                        
                        for(i=0;i<nfcData.item[structIndex + typeItemCnt].size;i++)
                        {
                            /*upload crc for every byte in each qc item*/ 
                            crcLoadByte(nfcBuf[i]);
                            
                        }
                    }
                    
                    M24LRxx_ReadBuffer(nfcData.typeCrcAddr[NFC_DATA_TYPE_QCONST]+NFC_SECOND_ADRESS, 4, (uint8_t *)&crc);
                    /*read qc crc*/
                    if(crc!=crcGet())
                    {
                        return 0;
                        /*not equal then break return 0*/
                        break;
                    }
                }
            }
        }
        /*crc is right return 1*/
        return 1;
        break;
        
    default: 
        
        return 0;
        
        break;
        
    }
    

    
}
/*******************************************************************************
* @Brief   nfc_nvm_data_write
save data to flash, when update from T4T
* @Param   null
* @Note    
* @Return  null
*******************************************************************************/
void nfc_nvm_data_write()
{
    uint8_t *p;
    uint8_t i;
    
    crcReset(); 
    p=(uint8_t*)&(mem_bank_nfc_nvm_t.mem_bank_nfc_temp);
    for(i=0;i<sizeof(mem_bank_nfc_nvm_t.mem_bank_nfc_temp);i++)
    {
        /*upload crc for every byte in each fc item and crc is calculated all in a time*/
        crcLoadByte(*(p+i));        
    }
    mem_bank_nfc_nvm_t.nfc_data_nvm_crc=crcGet();//calculate crc value
    
    __disable_interrupt();// disable all interrupt
    
    XMC_FLASH_ProgramVerifyPage((uint32_t*)FLASH_START_ADDRESS,(uint32_t*)&mem_bank_nfc_nvm_t);
    /*wirte data and crc value*/
    __enable_interrupt();// enable all interrupt    
}
/*******************************************************************************
* @Brief   nfc_nvm_data_read
read data from flash 
* @Param   null
* @Note    
* @Return  nvm data right or not
*******************************************************************************/
uint8_t nfc_nvm_data_read()
{
#define NFC_DATA_BLOCK_NUM 10
    uint32_t s_crc_data=0;
    uint8_t *p;
    uint8_t i;
    static uint32_t  s_read[40];
    mem_bank_nfc_nvm s_nvm_bank;   
    
    XMC_FLASH_ReadBlocks((uint32_t*)(FLASH_START_ADDRESS),s_read,NFC_DATA_BLOCK_NUM);//read the data   
    memcpy((uint8_t*)&s_nvm_bank,(uint8_t*)s_read,sizeof(s_nvm_bank));//copy data
    crcReset(); 
    p=(uint8_t*)(s_read);
    for(i=0;i<sizeof(s_nvm_bank.mem_bank_nfc_temp);i++)
    {
        /*upload crc for every byte in each fc item*/ 
        crcLoadByte(*(p+i));        
    }
    s_crc_data=crcGet();//calculate crc value
    if(s_nvm_bank.nfc_data_nvm_crc==s_crc_data)
        /*if crc is right*/
    {
        memcpy((uint8_t*)&mem_bank_nfc_nvm_t.mem_bank_nfc_temp,(uint8_t*)&s_nvm_bank.mem_bank_nfc_temp,sizeof(mem_bank_nfc_nvm_t.mem_bank_nfc_temp));//copy data
        /*copy data in mem_bank_nfc_temp and return 1 else return 0*/
        return 1;
    }
    else
    {
        return 0;
    }
    
}
/*******************************************************************************
* @Brief   nfc_nvm_time_write
save time in nvm and judge if time in tag is right or not, if no update with nvm value
* @Param   null
* @Note    
* @Return 
*******************************************************************************/
void nfc_nvm_time_write(void)
{   
    
    if(abs(g_nfc_time-g_time_nvm_pre)>(2*NVM_TIME_SAVE_PERIOD))//judge g_nfc_time data range in right range
    {
        g_nfc_time=g_time_nvm_pre;
    }
    else
    {
        
        if((g_nfc_time-g_time_nvm_pre)>NVM_TIME_SAVE_PERIOD)//judge if time is more than 10h
        {
            
            g_flag_flash_write=1;
            
            g_time_nvm_pre=g_nfc_time;// reset g_time_nvm_pre 
            
        }
    }
    
}
/*******************************************************************************
* @Brief   nfc_eol_handle
hanlde eol feature 
* @Param   null
* @Note    
* @Return 
*******************************************************************************/
void nfc_eol_handle(void)
{
    static uint8_t eol_flag=0;// 0: default ;1: no eol; 2; eol is enable 3; eol handle is ongoing; 4:eol handle is finished
    static uint32_t time_cout=0;
    if(eol_flag==0)
    {
        if(mem_bank_nfc.mem_bank_eol.Eol_enable==0x01)//eol is enable
        {
            if(g_nfc_time>(mem_bank_nfc.mem_bank_eol.Eol_time*3600*1000))
            {
                eol_flag=2;
            }
        }
        else
        {
            eol_flag=1;
        }
    }
    else
    {
        if(eol_flag==2)
        {
            Power_SetEolDimmingLevel(2500);//set to 25%
            eol_flag=3; //handling 
        }
        if(eol_flag==3)
        {
            if(time_cout++>=(200*600))
            {
                eol_flag=4;//finished eol handle
                mem_bank_nfc.mem_bank_eol.Eol_status=0x01;
                Power_SetEolDimmingLevel(10000);//set to 25%
                time_cout=0;
            }
        }
    }
    
}
/*******************************************************************************
* @Brief   nfc_flash_write
save time in nvm and judge if time in tag is right or not, if no update with nvm value
* @Param   null
* @Note    
* @Return 
*******************************************************************************/
void nfc_flash_write(void)
{
    static uint32_t s_data[4];
    uint8_t i;
    
    if(g_flag_flash_write)
    {
        g_flag_flash_write=0;
        
        crcReset();
        
        s_data[0]=g_nfc_time;
        /*reset crc value*/
        for(i=0;i<4;i++)
        {
            
            crcLoadByte(*((uint8_t*)s_data+i));
            /*calculate crc value*/
        }
        s_data[1]=crcGet();
        SysTick->CTRL=0X00;//close systick
        //__disable_interrupt();// disable all interrupt
        
        XMC_FLASH_ProgramVerifyPage((uint32_t*)FLASH_TIME_ADDRESS,s_data);
  
        SysTick_Config(SYSCLK_Hz / SYSTICK_INT_Hz);//open systick
       // __enable_interrupt();// enable all interrupt   
    }
}
#endif /* MODULE_NFC */

/** @}  End of group HW */
