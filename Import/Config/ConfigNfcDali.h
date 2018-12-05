/**********************************************************************************************************************/

#ifndef __CONFIG_NFC_DALI_H
#define __CONFIG_NFC_DALI_H

/**********************************************************************************************************************/

#define DALI_INTERFACE
#define MB_MAX_SIZE_BYTE                (50)
#define MB_TOT_SIZE_BYTE                (600)

/**********************************************************************************************************************/

#define MPC_MB_0_NUMBER                 (0)
#define MPC_MB_1_NUMBER                 (1)

//----------------------

//#undef MPC_CLM_NUMBER
#undef MPC_EMERGENCY_NUMBER
//#undef MPC_INFO_NUMBER
//#undef MPC_PASSWORD1_NUMBER
//#undef MPC_PASSWORD2_NUMBER
#undef MPC_POWER_NUMBER
#undef MPC_TDC_NUMBER
#undef MPC_DEBUG_NUMBER
//#undef MPC_ASTRO_NUMBER
//#undef MPC_CSM_NUMBER
//#undef MPC_EOL_NUMBER
#undef MPC_MAINS_NUMBER
#undef MPC_PRESDETECT_NUMBER
#undef MPC_STEP_NUMBER
#undef MPC_STEP_NUMBER
#undef DALI_INTERFACE

//----------------------

/**********************************************************************************************************************/

//#define FAST_LOCAL_SYNC

#define TAG_ADDR_MB_0			    (256)//12
#define TAG_ADDR_MB_1			    (40)
#define TAG_ADDR_MB_CLM		    (120)
//#define TAG_ADDR_MB_EMERGENCY	    (170)
#define TAG_ADDR_MB_INFO		    80//(210)
//#define TAG_ADDR_MB_OT		    (250)
#define TAG_ADDR_MB_PASSWORD1	    192//(290)
#define TAG_ADDR_MB_PSSSWORD1_COPY  284
#define TAG_ADDR_MB_PASSWORD2	    204//(330)
#define TAG_ADDR_MB_PSSSWORD2_COPY  288
//#define TAG_ADDR_MB_POWER		    (380)
//#define TAG_ADDR_MB_TDC		    (420)
//#define TAG_ADDR_MB_DEBUG		    (460)
#define TAG_ADDR_MB_ASTRO		    (12)//change from 384 to 12 2017.3.12  with 255 limit
#define TAG_ADDR_MB_CSM			    (56)
#define TAG_ADDR_MB_EOL		    (72)//new adding new
//#define TAG_ADDR_MB_MAINS		    (620)
//#define TAG_ADDR_MB_PRESDETECT    (660)
//#define TAG_ADDR_MB_STEP		    (700)
//#define TAG_ADDR_MB_DALI		    (740)


/**********************************************************************************************************************/
#endif