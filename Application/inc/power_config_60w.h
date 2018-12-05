/*
***************************************************************************************************
*                            Power Control Loop Config File for 60W
*
* File   : power_config_60w.h
* Author : Douglas Xie
* Date   : 2016.08.24
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/
#if defined(OT_NFC_IP67_60W)

#ifndef _POWER_CONFIG_60W_H
#define _POWER_CONFIG_60W_H

/* Power Control Loop Task Period define, unit: ms */
#define POWER_TASK_PERIOD       (15)        /* 15ms */
#define POWER_TASK_PERIOD_LOW   (5)         /* 5ms */

/*---------------- Device Specific Information ---------------------*/
/* GTIN Number */
#define GTIN_NUM    {0X03,0XAF,0XA3,0XA2,0X7C,0X64} /* 4052899495012 */

/*---------------- OVP and OCP in Control Loop ---------------------*/
/* Comment this define to enable OVP and OCP in control loop */
//#define ENABLE_LOOP_OVP_OCP

/*---------------- Calculate Formula Coefficient -------------------*/
/* ADC to Output Current(mA) Covertion Coefficient define */
/* Two Order: Iout = A2x^2 + A1x + A0 = (A2x + A1)x + A0 */
#define IOUT_PARAM_A2           (-7.21308818E-06)
#define IOUT_PARAM_A1           (1.06329397E+00)
#define IOUT_PARAM_A0           (5.24005171E+00)

/* ADC to Output Voltage(V) Covertion Coefficient define */
/* Two Order: Uout = B2x^2 + B1x + B0 = (B2x + B1)x + B0 */
#define UOUT_PARAM_B2           (1.28151866E-08)
#define UOUT_PARAM_B1           (4.24938962E-02)
#define UOUT_PARAM_B0           (-2.00280430E-01)

/* OCP Current to PWM Duty(0.1%) Covertion Coefficient define */
#define OCP_TO_DUTY_D2          (-3.68547087E-06)
#define OCP_TO_DUTY_D1          (2.52956083E-01)
#define OCP_TO_DUTY_D0          (-6.81787750E+00)

/* OVP Voltage to PWM Duty(0.1%) Covertion Coefficient define */
#define OVP_TO_DUTY_E2          (0)
#define OVP_TO_DUTY_E1          (5.95238097E+00)
#define OVP_TO_DUTY_E0          (1.37591516E-12)

/*---------------- Output Power Limitation define ------------------*/
/* Output Range define */
#define OUTPUT_POWER            (60000)     /* Max Output Power = 60W = 60000mW */
#define UOUT_MAX                (86)        /* Max Voltage = 86V */
#define UOUT_MIN                (43)		/* Min Voltage = 43V */

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
#define INIT_CTRL_DUTY_DEFAULT  (10)        /* 1.0%  */
#define INIT_CTRL_DUTY_0        (30)        /* 3.0%  */
#define INIT_CTRL_DUTY_1        (180)       /* 18.0%  */
#define INIT_CTRL_DUTY_2        (300)       /* 30.0% */
#define INIT_CTRL_DUTY_3        (380)       /* 38.0% */
#define INIT_CTRL_DUTY_4        (409)       /* 40.9% */
#define INIT_CTRL_DUTY_5        (433)       /* 43.3% */
#define INIT_CTRL_DUTY_6        (474)       /* 47.4% */
#define INIT_CTRL_DUTY_7        (527)       /* 52.7% */
#define INIT_CTRL_DUTY_8        (581)       /* 58.1% */
#define INIT_CTRL_DUTY_9        (620)       /* 62.0% */
#define INIT_CTRL_DUTY_10       (642)       /* 64.2% */
#define INIT_CTRL_DUTY_11       (660)       /* 66.0% */
#define INIT_CTRL_DUTY_12       (688)       /* 68.8% */

/*----------- PWM Adjust Speed and Choose Condition define ---------*/
/* PWM Adjustment Speed define */
#define PWM_SPEED_HIGH          (14)        /* PWM High speed adjustment:   +/- 1.4%  */
#define PWM_SPEED_MID           (5)         /* PWM Middle speed adjustment: +/- 0.5%  */
#define PWM_SPEED_LOW           (1)         /* PWM Low speed adjustment:    +/- 0.1%  */
#define PWM_SPEED_STEP          (0)         /* PWM step adjustment: call step function*/

/* PWM Adjustment Speed for Low Current (<250mA) define */
#define PWM_SPEED_L3            (10)        /* PWM adjust: +/- 1.0%  */
#define PWM_SPEED_L2            (2)         /* PWM adjust: +/- 0.2%  */
#define PWM_SPEED_L1            (4)         /* PWM adjust: 5 steps   */

/*--------------- Output Power Stable Condition define -------------*/
/* Current Control PWM Speed Threshold define */
#define IOUT_THRESHOLD_HIGH_2   (150)       /* PWM High speed threshold:   > 150mA */
#define IOUT_THRESHOLD_MID_2    (35)        /* PWM Middle speed threshold: > 35mA  */
#define IOUT_THRESHOLD_LOW_2    (10)        /* PWM Middle speed threshold: > 10mA  */
#define PWM_STABLE_UOUT_2       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_2       (5)         /* Unit: 0.1%, PWM Stable Current Range: 0.5% */
/*----- above divide use threshold 2 --------*/
#define IOUT_DIVIDER_2_1        (700)       /* Current divider for section2 and section1 */
/*----- below divide use threshold 1 --------*/
#define IOUT_THRESHOLD_HIGH_1   (250)       /* PWM High speed threshold:   > 250mA */
#define IOUT_THRESHOLD_MID_1    (60)        /* PWM Middle speed threshold: > 50mA  */
#define IOUT_THRESHOLD_LOW_1    (20)        /* PWM Middle speed threshold: > 15mA  */
#define PWM_STABLE_UOUT_1       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_1       (10)        /* Unit: 0.1%, PWM Stable Current Range: 1.0% */
/*----- above divide use threshold 1 --------*/
#define IOUT_DIVIDER_1_0        (250)       /* Current divider for section1 and section0 */
/*----- below divide use threshold 0 --------*/
#define IOUT_THRESHOLD_HIGH_0   (60)        /* PWM High speed threshold:    > 60mA */
#define IOUT_THRESHOLD_MID_0    (40)        /* PWM Middle speed threshold:  > 25mA */
#define IOUT_THRESHOLD_LOW_0    (20)         /* PWM Middle speed threshold: > 10mA */
#define PWM_STABLE_UOUT_0       (20)        /* Unit: 0.1%, PWM Stable Voltage Range: 2.0% */
#define PWM_STABLE_IOUT_0       (10)        /* Unit: 0.1%, PWM Stable Current Range: 1.0% */

/* Voltage Control PWM Speed Threshold define */
#define UOUT_THRESHOLD_HIGH     (80)        /* PWM High speed threshold:   > 80V */
#define UOUT_THRESHOLD_MID      (40)        /* PWM Middle speed threshold: > 40V */
#define UOUT_THRESHOLD_LOW      (20)        /* PWM Middle speed threshold: > 20V */

/* Max output voltage tolerance */
/* 700mA  max output 94V */
/* 1400mA max output 45V */
#define POWER_UOUT_TOLERANCE    (1.12)       /* Change Max Voltage to 108% * P/I, from 2016.11.11 */

/*------------------ Compensation Coefficient  ---------------------*/
/* Compensation Coefficient for calculate output current when disable DEBUG PRINT */
#define IOUT_COMPENSATION       (0.994)      /* Output current compensation */
#define IOUT_OFFSET             (0)         /* Iout offset value */

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

/* OVP default reference voltage:   88.9% - 150V */
/*                                  77.0% - 130V */
/*                                  65.0% - 110V */
/*                                  53.0% - 90V  */
/*                                  41.0% - 70V  */
/*                                  29.0% - 50V  */
#define OVP_TRIGGER_DUTY_DEFAULT    (530)

/* OCP Exceed Current define, OCP Current = Target Current + Exceed Current */
#define OCP_TRIGGER_EXCEED          (1000)  /* Unit: mA */
#define OCP_AVERAGE_EXCEED          (1000)  /* Unit: mA */

/* Constant OCP Current define (replace above mode) */
#define OCP_CURRENT_CONST           (4500)  /* Unit: mA */

/* OVP Exceed Voltage define, OVP Voltage = Target Max Voltage + Exceed Voltage */
#define OVP_TRIGGER_EXCEED          (45)    /* Unit: V */
#define OVP_AVERAGE_EXCEED          (10)    /* Unit: V */

/* OCP & OVP trigger counter for avoid error trigger condition */
#define OCP_OVP_TRIGGER_COUNTER     (3)     /* exceed such times will active OCP/OVP protection */

#endif /* _POWER_CONFIG_60W_H */
#endif

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
