// ---------------------------------------------------------------------------------------------------------------------
// Firmware for OutDoor 2Dim project 
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH

//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version:  2019-03-17
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
#include "xmc_flash.h"




uint8_t  g_moble_flag           = 0;
uint8_t  g_nfc_current_set      = 0;
uint8_t  g_nfc_update_flag      = 0;
uint8_t  g_pwm_update_flag      = 1;
//uint8_t  g_nfc_start_flag       = 0;
uint8_t  g_psw_update_flag      = 0;
uint32_t g_nfc_time             = 0;
uint32_t g_ed_time              = 0;
uint32_t g_last_ontime          =0;
uint8_t  g_nfc_time_id          = 0;
uint8_t  g_nfc_flag_record      = 0;
uint8_t  g_nfc_flag_pwm         = 0;
uint8_t  g_nfc_dim_percent      = 100;
uint8_t  g_nfc_current_percent  = 50;
uint8_t  g_astro_flag=0;//flag for Astros start purpose
uint8_t  g_inf_time_update=0;
uint8_t  g_flag_flash_write=0;
uint8_t  flag_t4t_update=0;
uint32_t g_time_nvm_pre=0;
uint16_t g_min_dim_level=0;
uint8_t last_dim_level=4;
const uint8_t g_gtin_num[6]=GTIN_NUM; 



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



/******************************************************************************
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

  
    static uint32_t s_count=200; 
    if(g_nfc_start_flag==1&&s_count++>200) //run in every 1s
    {
        s_count=0;  
                
        Power_SetConstantLumenValue(1100);
                       
    }
    
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
    
    uint8_t  g_nfc_flag_record=0; //set time period
    
    if(g_nfc_flag_record) // 60s flag check
    { 
        
        if((I2cAreAllPendingTransfersDone()==1)&&nfc_local_state.fsm_state==nfc_fsm_state_idle) //no nfc mirror task
        {
            
          if(I2cWrite(NFC_TIME_ADRRESS+g_nfc_time_id*4, (uint8_t *)&g_nfc_time,4)==true)
            
          {
            
            g_nfc_flag_record=0;  //close 60s flag
            
            if(g_nfc_time_id++>=47)// block++;if it is up to number 47 , need to set to num 0
            {
              
              g_nfc_time_id=0;  //set block num to 0
              
              s_time=g_nfc_time/60; // g_time set from second to minute.
        
              
              g_nfc_update_flag=1;  //set flag to store time in backup area  // need to save the time information
              
            }
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
    
    I2cRead(NFC_TIME_ADRRESS,&s_arr[0],192); //read first 60 byte time data //all the time data
    
   // I2cRead(NFC_TIME_ADRRESS+64,&s_arr[0+64],64); //read second 60 byte time data //all the time data
    
   // I2cRead(NFC_TIME_ADRRESS+128,&s_arr[0+128],64); //read third 60 byte time data //all the time data
    
    for(i=0;i<NFC_TIME_COUNT;i+=4)//need to take care of read result
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
    
    //s_run_time=mem_bank_nfc.mem_bank_inf.Oper_con_1+(mem_bank_nfc.mem_bank_inf.Oper_con_2<<8)+(mem_bank_nfc.mem_bank_inf.Oper_con_3<<16);
    /*get setting running time fromm T4T */
    s_run_time*=60;
    /* transfor from sencond to minute*/
    if(flag_t4t_update==1)
        /* if inf_time is update by T4T then update the data */   
    {
        
        flag_t4t_update=0;
        
//        g_nfc_time=s_run_time;
//        for(i=0;i<48;i++) 
//        {
//            while(M24LRxx_WriteBuffer(NFC_TIME_ADRRESS+i*4,4,g_nfc_time)!=I2C_CODE_OK)//reset all the time data with update value
//            {
//                /*write g_nfc_time in user nfc tag area*/
//            }
//        }
        
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
      if(I2cRead(NFC_ED_REG_ADRESS,&ed_index,1)==1)
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
    if (I2cRead(NFC_ED_REG_ADRESS+1,&ed_count,1)==1)
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
    I2cRead(NFC_RUNTIME_PRE,time_arr,4);
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
        while (I2cWrite(NFC_ED_TIME_ADRESS+ed_index*2,(uint8_t *)&ed_time,1)!=1)
        {
        }
        if(ed_index++>=7)//record 8 times need to recycle
        {
            ed_index=0;
        }
        if(ed_count<8)
        {
            ed_count++;  //count max value is 8
            while(I2cWrite(NFC_ED_REG_ADRESS+1,&ed_count,1)!=1)//record count
            {
            }
        }
        while(I2cWrite(NFC_ED_REG_ADRESS,&ed_index,1)!=1) //record index
        {
        }
    }
    while (I2cWrite(NFC_RUNTIME_PRE,(uint8_t *)&g_nfc_time,4)!=1)//record last power off time
    {
    }
    g_last_ontime=g_nfc_time;//record start up time
    if(ed_count>=1)
    {
        while(I2cRead(NFC_ED_TIME_ADRESS,ed_arr,ed_count*2)!=1)
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
        MemoryBank_Astro_GetValue(6,(uint8_t *)&dimmer.mode,0) ;  
        MemoryBank_Astro_GetValue(15,(uint8_t *)&dimmer.duration[0],0) ; 
        MemoryBank_Astro_GetValue(18,(uint8_t *)&dimmer.duration[1],0) ;
        MemoryBank_Astro_GetValue(21,(uint8_t *)&dimmer.duration[2],0) ; 
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
    MemoryBank_Astro_GetValue(7,(uint8_t *)&dimmer.level[0],0);
    MemoryBank_Astro_GetValue(14,(uint8_t *)&dimmer.level[1],0);
    MemoryBank_Astro_GetValue(17,(uint8_t *)&dimmer.level[2],0);
    MemoryBank_Astro_GetValue(20,(uint8_t *)&dimmer.level[3],0);
    MemoryBank_Astro_GetValue(23,(uint8_t *)&dimmer.level[4],0);
    dimmer.level[5] = 30;  // minPhysicalLevel;
    /*Astro dimming fade time setting*/ 
    
    MemoryBank_Astro_GetValue(8,(uint8_t *)&dimmer.fade[0],0);
    dimmer.fade[0]*=15;					// in seconds (mem_bank_nfc.mem_bank_astro.Astro_startup_fade_h<<8)+
    MemoryBank_Astro_GetValue(10,(uint8_t *)&dimmer.fade[1],0);
    dimmer.fade[1]*=2;
    
    MemoryBank_Astro_GetValue(11,(uint8_t *)&dimmer.fade[5],0);
    dimmer.fade[5]*=15;					// warning 65535
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
* @Brief   AstroTimer
* @Param   null
* @Note    
* @Return  0
*******************************************************************************/
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
                if(dimmer.fade[5]==0XEF1)// no swith off then keep level4
                    
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
        if((I2cAreAllPendingTransfersDone()==1)&&nfc_local_state.fsm_state==nfc_fsm_state_idle)
        {
            /*nfc is idle*/
            if (getMainsTimerMinutes() >= H24_CYCLE_MINUTES&&(dimmer.mode & ASTRO_TIME_BASE_MASK) == 0)// >>>  24 h TEST <<<<<<<<<<
            {					                
                //while (M24LRxx_WriteBuffer(NFC_RUNTIME_PRE,4,g_nfc_time)!=I2C_CODE_OK) //invalid on-time  and clear current working time      				                   
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
        if(1)//(mem_bank_nfc.mem_bank_eol.Eol_enable==0x01)//eol is enable
        {
            //if(g_nfc_time>(mem_bank_nfc.mem_bank_eol.Eol_time*3600*1000))
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
                //mem_bank_nfc.mem_bank_eol.Eol_status=0x01;
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
  
        //SysTick_Config(SYSCLK_Hz / SYSTICK_INT_Hz);//open systick
       // __enable_interrupt();// enable all interrupt   
    }
}
#endif /* MODULE_NFC */

/** @}  End of group HW */
