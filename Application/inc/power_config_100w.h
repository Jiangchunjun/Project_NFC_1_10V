/*
***************************************************************************************************
*                            Power Control Loop Config File for 100W
*
* File   : power_config_100w.h
* Author : Douglas Xie
* Date   : 2016.08.24
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#if defined(OT_NFC_IP67_100W)


// EAN
#define GLOBAL_DEVICE_EAN                                   4062172060677
#define GLOBAL_DMAT_NO                                      0x30010259UL
#define GLOBAL_BASIC_CODE_STRING                            "AM27585 "
//#define NFC_TIME_ADRRESS
//#define FLASH_TIME_ADDRESS                                   0X10010010
// DALI
#define POWERon_DELAY_TIME_ms                                               (120)

// ADC Mult Factor
#define GLOBAL_ULINE1_ADC_FACTOR                            120804UL            // (3.45V/4096) * 1e6 * ((4700kOhm + 33kOhm) / 33kOhm) = 120.804

// NVM SIZE
#define GLOBAL_NVM_SIZE_KBYTE                               64

// NFC enable
#define GLOBAL_NFC_ENABLED                                  true

/** Module BOOSTPFC */
#define GLOBAL_BOOSTPFC_CHOKE                               CHOKE_D30000602
#define GLOBAL_BOOSTPFC_INDUCTIVITY_uH                      1500
#define GLOBAL_BOOSTPFC_VRAIL_OPER_AC                       VOLTS(400)
#define GLOBAL_BOOSTPFC_VRAIL_OPER_DC                       VOLTS(400)
#define GLOBAL_BOOSTPFC_VRAIL_VLINE_DELTA                   VOLTS(50)
#define GLOBAL_BOOSTPFC_VRAIL_INCREASE_MAXVOLTS             VOLTS(430)


/** Module CurrentSet */
// Label values
#define I_LED_MIN                                           150UL
#define I_LED_MAX                                           700UL


// [W]
#define GLOBAL_MIN_POWER                                    20
// [W]
#define GLOBAL_MIN_POWER_EL                                 27


//uA
#define LEDSET_MAXCURRENT                                   I_LED_MAX*1000
//uA
#define LEDSET_MINCURRENT                                   I_LED_MIN*500
//uA
#define CURRENTSET_ABS_MINCURRENT_UA                        (1400)


// Currentset Vin power reduction
#define CURRENTSET_U_P_RED_VOLTAGE_LIMIT_AC                 VOLTS(277)
#define CURRENTSET_U_P_RED_VOLTAGE_LIMIT_DC                 VOLTS(195)
#define CURRENTSET_U_P_RED_VOLTAGE_LOW_AC                   VOLTS(248)
#define CURRENTSET_U_P_RED_VOLTAGE_LOW_DC                   VOLTS(176)
#define CURRENTSET_U_P_RED_VOLTAGE_FACTOR_AC                (15165UL)
#define CURRENTSET_U_P_RED_VOLTAGE_FACTOR_DC                (23147UL)
#define CURRENTSET_U_P_RED_VOLTAGE_FACTOR_AC_MAX            (0.6)
#define CURRENTSET_U_P_RED_VOLTAGE_FACTOR_DC_MAX            (0.6)

#define CURRENTSET_STABLE_TIME_ms                           (700)

// Temperature [K] limit for thermal power reduction
#define TEMP_LIMIT_NOM_K                                    96
#define TEMP_DELTA_EL_K                                     30


/** Module Kernel */
///< [W] Max output power at the LED string
#define GLOBAL_POWER_OUTPUT_MAX_W                           75

// LEDset conversion data
#if LEDSET_R_CORRECTION==0
#warning ::::::::::::::::::::::::::: Using original LEDset resistor (wrong for MP) :::::::::::::::::::::::::::
#define LEDSET_M                                            (30591)
#define LEDSET_Q                                            (1783687)
#define LEDSET_ADC_LIMIT                                    (15000)
#define LEDSET_RESISTOR_SUM_OHM                             (2970.0)
#else
#define LEDSET_M                                            (32913)
#define LEDSET_Q                                            (1900065)
#define LEDSET_ADC_LIMIT                                    (15000)
#define LEDSET_RESISTOR_SUM_OHM                             (3030.0)
#endif

// Power memory bank conversion data
#define GPC_P_STANDBY                                       (15)    //theoretical Standby Power
#define GPC_M                                               (9673)
#define GPC_Q                                               (124)

// Calibration
#define CALIBRATION_VLED_CORRECTION                         (0)


#ifndef _POWER_CONFIG_100W_H
#define _POWER_CONFIG_100W_H

/* Power Control Loop Task Period define, unit: ms */
#define POWER_TASK_PERIOD       12//(12) 5      /* 12ms */
#define POWER_TASK_PERIOD_LOW   8//(8)  3     /* 5ms */

/*---------------- Device Specific Information ---------------------*/
/* GTIN Number */
#define GTIN_NUM    {0X03,0XAF,0XA3,0XA2,0X7C,0X7C} /* 4052899495036 */

/*---------------- OVP and OCP in Control Loop ---------------------*/
/* Comment this define to enable OVP and OCP in control loop */
//#define ENABLE_LOOP_OVP_OCP

/*---------------- Calculate Formula Coefficient -------------------*/
/* ADC to Output Current(mA) Covertion Coefficient define */
/* Two Order: Iout = A2x^2 + A1x + A0 = (A2x + A1)x + A0 */
#define IOUT_PARAM_A2           (-1.77387069E-05)
#define IOUT_PARAM_A1           (1.06667621E+00)
#define IOUT_PARAM_A0           (-2.55304040E-02)

/* ADC to Output Voltage(V) Covertion Coefficient define */
/* Two Order: Uout = B2x^2 + B1x + B0 = (B2x + B1)x + B0 */
#define UOUT_PARAM_B2           (0)
#define UOUT_PARAM_B1           (6.88705E-02)
#define UOUT_PARAM_B0           (0)

/* OCP Current to PWM Duty(0.1%) Covertion Coefficient define */
#define OCP_TO_DUTY_D2          (-3.68547087E-06)
#define OCP_TO_DUTY_D1          (2.52956083E-01)
#define OCP_TO_DUTY_D0          (-6.81787750E+00)

/* OVP Voltage to PWM Duty(0.1%) Covertion Coefficient define */
#define OVP_TO_DUTY_E2          (0)
#define OVP_TO_DUTY_E1          (3.57142858E+00)
#define OVP_TO_DUTY_E0          (8.25549096E-13)

/*---------------- Output Power Limitation define ------------------*/
/* Output Range define */
#define OUTPUT_POWER            (100000)    /* Max Output Power = 100W = 100000mW */
#define UOUT_MAX                (143)       /* Max Voltage = 144V */
#define UOUT_MIN                (71)		/* Min Voltage = 71V */

/*---------------------- Power Level define ------------------------*/
/* Current level define */
#define CURRENT_LEVEL_0         (300)       /* min    ~ 100mA  */
#define CURRENT_LEVEL_1         (500)       /* 100mA  ~ 220mA  */
#define CURRENT_LEVEL_2         (500)       /* 220mA  ~ 350mA  */
#define CURRENT_LEVEL_3         (500)       /* 350mA  ~ 500mA  */
#define CURRENT_LEVEL_4         (600)       /* 500mA  ~ 600mA  */
#define CURRENT_LEVEL_5         (700)       /* 600mA  ~ 700mA  */
#define CURRENT_LEVEL_6         (800)       /* 700mA  ~ 800mA  */
#define CURRENT_LEVEL_7         (900)       /* 800mA  ~ 900mA  */
#define CURRENT_LEVEL_8         (1000)      /* 900mA  ~ 1000mA */
#define CURRENT_LEVEL_9         (1100)      /* 1000mA ~ 1100mA */
#define CURRENT_LEVEL_10        (1200)      /* 1100mA ~ 1200mA */
#define CURRENT_LEVEL_11        (1300)      /* 1200mA ~ 1300mA */
#define CURRENT_LEVEL_12        (1400)      /* 1300mA ~ 1300mA */

/*-------------- Power Level Init PWM Duty define -----------------*/
/* Initial Control PWM duty define */
#define INIT_CTRL_DUTY_DEFAULT  (30)        /* 3.0%  */
#define INIT_CTRL_DUTY_0        (100)        /* 3.5%  */
#define INIT_CTRL_DUTY_1        (100)        /* 8.0%  */
#define INIT_CTRL_DUTY_2        (100)       /* 12.0% */
#define INIT_CTRL_DUTY_3        (100)       /* 12.0% */
#define INIT_CTRL_DUTY_4        (100)       /* 23.8% */
#define INIT_CTRL_DUTY_5        (400)       /* 29.0% */
#define INIT_CTRL_DUTY_6        (400)       /* 35.0% */
#define INIT_CTRL_DUTY_7        (413)       /* 41.3% */
#define INIT_CTRL_DUTY_8        (430)       /* 45.0% */
#define INIT_CTRL_DUTY_9        (450)       /* 50.0% */
#define INIT_CTRL_DUTY_10       (500)       /* 56.8% */
#define INIT_CTRL_DUTY_11       (540)       /* 56.8% */
#define INIT_CTRL_DUTY_12       (580)       /* 56.8% */

/*----------- PWM Adjust Speed and Choose Condition define ---------*/
/* PWM Adjustment Speed define */
#define PWM_SPEED_HIGH          (20)//(20)  10      /* PWM High speed adjustment:   +/- 2.0%  */
#define PWM_SPEED_MID           (8)//(8) 3        /* PWM Middle speed adjustment: +/- 0.8%  */
#define PWM_SPEED_LOW           (1)         /* PWM Low speed adjustment:    +/- 0.1%  */
#define PWM_SPEED_STEP          (0)         /* PWM step adjustment: call step function*/

/* PWM Adjustment Speed for Low Current (<250mA) define */
#define PWM_SPEED_L3            3//(10) 3       /* PWM adjust: +/- 1.0%  */
#define PWM_SPEED_L2            2//(2)   2      /* PWM adjust: +/- 0.2%  */
#define PWM_SPEED_L1            1//(5)         /* PWM adjust: 5 steps   */ //Moon change to 1

/*--------------- Output Power Stable Condition define -------------*/
/* Current Control PWM Speed Threshold define */
#define IOUT_THRESHOLD_HIGH_2   (300)       /* PWM High speed threshold:   > 250mA */
#define IOUT_THRESHOLD_MID_2    (40)        /* PWM Middle speed threshold: > 35mA  */
#define IOUT_THRESHOLD_LOW_2    (20)        /* PWM Middle speed threshold: > 10mA  */
#define PWM_STABLE_UOUT_2       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_2       (5)         /* Unit: 0.1%, PWM Stable Current Range: 0.5% */
/*----- above divide use threshold 2 --------*/
#define IOUT_DIVIDER_2_1        (400)       /* Current divider for section2 and section1 */
/*----- below divide use threshold 1 --------*/
#define IOUT_THRESHOLD_HIGH_1   (300)       /* PWM High speed threshold:   > 350mA */
#define IOUT_THRESHOLD_MID_1    (35)        /* PWM Middle speed threshold: > 50mA  */
#define IOUT_THRESHOLD_LOW_1    (15)        /* PWM Middle speed threshold: > 12mA  */
#define PWM_STABLE_UOUT_1       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_1       (10)        /* Unit: 0.1%, PWM Stable Current Range: 1.0% */
/*----- above divide use threshold 1 --------*/
#define IOUT_DIVIDER_1_0        (50)       /* Current divider for section1 and section0 */
/*----- below divide use threshold 0 --------*/
#define IOUT_THRESHOLD_HIGH_0   (60)        /* PWM High speed threshold:    > 60mA */
#define IOUT_THRESHOLD_MID_0    (25)        /* PWM Middle speed threshold:  > 40mA */
#define IOUT_THRESHOLD_LOW_0    (10)        /* PWM Middle speed threshold: > 20mA */
#define PWM_STABLE_UOUT_0       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_0       (10)        /* Unit: 0.1%, PWM Stable Current Range: 1.0% */

/* Voltage Control PWM Speed Threshold define */
#define UOUT_THRESHOLD_HIGH     (80)        /* PWM High speed threshold:   > 80V */
#define UOUT_THRESHOLD_MID      (40)        /* PWM Middle speed threshold: > 40V */
#define UOUT_THRESHOLD_LOW      (20)        /* PWM Middle speed threshold: > 20V */

/* Max output voltage tolerance */
/* 700mA  max output 154V */
/* 1400mA max output 75V */
#define POWER_UOUT_TOLERANCE    (1.10)       /* Change Max Voltage to 108% * P/I, from 2016.11.11 */


/*------------------ Compensation Coefficient  ---------------------*/
/* Compensation Coefficient for calculate output current when disable DEBUG PRINT */
#define IOUT_COMPENSATION       (0.8664)//(1.01)      /* Output current has about 1% higher when disable DEBUG PRINT */
#define IOUT_OFFSET             (0)       /* Iout offset value */

/* Compensation coefficient for Uout detect */
#define UOUT_COMPENSATION       (1)    /* Make sure output power can reach max power */

/*--------------- OCP & OVP Trigger Condition define --------------*/
/* OCP default reference voltage:   16.8% - 700mA  */
/*                                  19.3% - 800mA  */
/*                                  24.2% - 1000mA */
/*                                  26.7% - 1100mA */
/*                                  29.1% - 1200mA */
/*                                  31.6% - 1300mA */
/*                                  36.4% - 1500mA */
/*                                  43.7% - 1800mA */
/*                                  48.4% - 2000mA */
/*                                  83.3% - 3500mA */
#define OCP_TRIGGER_DUTY_DEFAULT    (833)

/* OVP default reference voltage:   89.3% - 250V  */
/*                                  82.1% - 230V  */
/*                                  71.3% - 200V  */
/*                                  64.1% - 180V  */
/*                                  53.4% - 150V  */
/*                                  42.6% - 120V  */
#define OVP_TRIGGER_DUTY_DEFAULT    (534)

/* OCP Exceed Current define, OCP Current = Target Current + Exceed Current */
#define OCP_TRIGGER_EXCEED          (1000)  /* Unit: mA */
#define OCP_AVERAGE_EXCEED          (1500)  /* Unit: mA */

/* Constant OCP Current define (replace above mode) */
#define OCP_CURRENT_CONST           (4500)  /* Unit: mA */

/* OVP Exceed Voltage define, OVP Voltage = Target Max Voltage + Exceed Voltage */
#define OVP_TRIGGER_EXCEED          (15)    /* Unit: V */
#define OVP_AVERAGE_EXCEED          (10)    /* Unit: V */

/* OCP & OVP trigger counter for avoid error trigger condition */
#define OCP_OVP_TRIGGER_COUNTER     (3)     /* exceed such times will active OCP/OVP protection */


#define ADC_STEP_DIFFER_I       (400)//30
#define ADC_BUFFER_SIZE_IOUT    (40)

/*ONE_TEN PARAMETERS SETTING*/
#define ONE_TEN_LOW_VOLTAGE 147 //180mV
#define ONE_TEN_HIGH_VOLTAGE 3824//4670mv
/* 1-10V ADC to Dimming Level Coefficient define */
/* One Order: Dimming = C1x + C0 */
#define ONE2TEN_PARAM_C1        (2.447648+00)//(2.44215938E+00)
#define ONE2TEN_PARAM_C0         (640.196+00)//(-6.42673522E-01)
#endif /* _POWER_CONFIG_100W_H */
#endif

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
