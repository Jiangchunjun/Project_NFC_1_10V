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

#ifndef _POWER_CONFIG_100W_H
#define _POWER_CONFIG_100W_H

/* Power Control Loop Task Period define, unit: ms */
#define POWER_TASK_PERIOD       12//(12)       /* 12ms */
#define POWER_TASK_PERIOD_LOW   8//(8)       /* 5ms */

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
#define UOUT_PARAM_B2           (1.52125869E-09)
#define UOUT_PARAM_B1           (7.09334347E-02)
#define UOUT_PARAM_B0           (-2.30685654E-01)

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
#define CURRENT_LEVEL_0         (100)       /* min    ~ 100mA  */
#define CURRENT_LEVEL_1         (220)       /* 100mA  ~ 220mA  */
#define CURRENT_LEVEL_2         (350)       /* 220mA  ~ 350mA  */
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
#define INIT_CTRL_DUTY_0        (50)        /* 5.0%  */
#define INIT_CTRL_DUTY_1        (80)        /* 8.0% */
#define INIT_CTRL_DUTY_2        (107)       /* 10.7% */
#define INIT_CTRL_DUTY_3        (119)       /* 11.9% */
#define INIT_CTRL_DUTY_4        (140)       /* 14.0% */
#define INIT_CTRL_DUTY_5        (174)       /* 17.4% */
#define INIT_CTRL_DUTY_6        (227)       /* 22.7% */
#define INIT_CTRL_DUTY_7        (445)       /* 44.5% */
#define INIT_CTRL_DUTY_8        (536)       /* 53.6% */
#define INIT_CTRL_DUTY_9        (568)       /* 56.8% */
#define INIT_CTRL_DUTY_10       (606)       /* 60.6% */
#define INIT_CTRL_DUTY_11       (638)       /* 63.8% */
#define INIT_CTRL_DUTY_12       (675)       /* 67.5% */

/*----------- PWM Adjust Speed and Choose Condition define ---------*/
/* PWM Adjustment Speed define */
#define PWM_SPEED_HIGH          (20)//(20)        /* PWM High speed adjustment:   +/- 2.0%  */
#define PWM_SPEED_MID           (8)//(8)         /* PWM Middle speed adjustment: +/- 0.8%  */
#define PWM_SPEED_LOW           (1)         /* PWM Low speed adjustment:    +/- 0.1%  */
#define PWM_SPEED_STEP          (0)         /* PWM step adjustment: call step function*/

/* PWM Adjustment Speed for Low Current (<250mA) define */
#define PWM_SPEED_L3            3//(10)        /* PWM adjust: +/- 1.0%  */
#define PWM_SPEED_L2            2//(2)         /* PWM adjust: +/- 0.2%  */
#define PWM_SPEED_L1            1//(5)         /* PWM adjust: 5 steps   */ //Moon change to 1

/*--------------- Output Power Stable Condition define -------------*/
/* Current Control PWM Speed Threshold define */
#define IOUT_THRESHOLD_HIGH_2   (200)       /* PWM High speed threshold:   > 250mA */
#define IOUT_THRESHOLD_MID_2    (35)        /* PWM Middle speed threshold: > 35mA  */
#define IOUT_THRESHOLD_LOW_2    (10)        /* PWM Middle speed threshold: > 10mA  */
#define PWM_STABLE_UOUT_2       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_2       (5)         /* Unit: 0.1%, PWM Stable Current Range: 0.5% */
/*----- above divide use threshold 2 --------*/
#define IOUT_DIVIDER_2_1        (700)       /* Current divider for section2 and section1 */
/*----- below divide use threshold 1 --------*/
#define IOUT_THRESHOLD_HIGH_1   (200)       /* PWM High speed threshold:   > 350mA */
#define IOUT_THRESHOLD_MID_1    (50)        /* PWM Middle speed threshold: > 50mA  */
#define IOUT_THRESHOLD_LOW_1    (20)        /* PWM Middle speed threshold: > 12mA  */
#define PWM_STABLE_UOUT_1       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_1       (10)        /* Unit: 0.1%, PWM Stable Current Range: 1.0% */
/*----- above divide use threshold 1 --------*/
#define IOUT_DIVIDER_1_0        (300)       /* Current divider for section1 and section0 */
/*----- below divide use threshold 0 --------*/
#define IOUT_THRESHOLD_HIGH_0   (70)        /* PWM High speed threshold:    > 60mA */
#define IOUT_THRESHOLD_MID_0    (50)        /* PWM Middle speed threshold:  > 40mA */
#define IOUT_THRESHOLD_LOW_0    (30)        /* PWM Middle speed threshold: > 20mA */
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
#define IOUT_COMPENSATION       (0.8749)//(1.01)      /* Output current has about 1% higher when disable DEBUG PRINT */
#define IOUT_OFFSET             (7.8)       /* Iout offset value */

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
#define OVP_TRIGGER_EXCEED          (20)    /* Unit: V */
#define OVP_AVERAGE_EXCEED          (10)    /* Unit: V */

/* OCP & OVP trigger counter for avoid error trigger condition */
#define OCP_OVP_TRIGGER_COUNTER     (3)     /* exceed such times will active OCP/OVP protection */


#define ADC_STEP_DIFFER_I       (300)//50
#define ADC_BUFFER_SIZE_IOUT    (60)

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
