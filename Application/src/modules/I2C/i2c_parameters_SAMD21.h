#ifndef __I2C_PARAMETERS_SAMD21_H__
#define __I2C_PARAMETERS_SAMD21_H__

/** ----------------------------------------------------------------------------
// i2c_parameters.h
// -----------------------------------------------------------------------------
// (c) Osram GmbH
// Development Electronics for SSL
// Parkring 33
// 85748 Garching
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Initial version: Sammy el Baradie,  27/07/2016
//
// $Author: j.eisenberg $
// $Revision: 13350 $
// $Date: 2018-05-17 19:43:04 +0800 (Thu, 17 May 2018) $
// $Id: i2c_parameters_SAMD21.h 13350 2018-05-17 11:43:04Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_parameters_SAMD21.h $
//
* \addtogroup I2cDriverAPI
* \{
*   \file
*   \addtogroup I2cDriverSAMD21
*   \{
*       \file
*       \brief Local header file for i2c_driver_samd21.c
*   \}
*   \}
*/

#ifdef __CONFIG_DMAC_SAM_D21_H__
#ifndef I2C_SERCOM_NUMBER
#error I2C_SERCOM_NUMBER not defined!!! Error in the config file.
#endif

#ifndef I2C_CLOCK_GENERATOR
#error I2C_CLOCK_GENERATOR not defined!!! Error in the config file.
#endif

#ifndef I2C_GCLK_SPEED_HZ
#error I2C_GCLK_SPEED_HZ not defined!!! Error in the config file.
#endif

#ifndef I2C_BAUD_RATE_HZ
#error I2C_BAUD_RATE_HZ not defined!!! Error in the config file.
#endif


#ifndef SAMD21_DMAC_CH_I2C_TX
#error SAMD21_DMAC_CH_I2C_TX not defined!!! It should be defined in ConfigDmacSamD21.h
#endif

#ifndef SAMD21_DMAC_CH_I2C_RX
#error SAMD21_DMAC_CH_I2C_RX not defined!!! It should be defined in ConfigDmacSamD21.h
#endif
#endif

#define I2C_CRC_START_VALUE         0xFFFF

#ifndef I2C_TRISE_S
#define I2C_TRISE_S                 (200E-9)
#endif

#define I2C_SPEED                   (0)

#define I2C_BAUD_REG                ((uint16_t)((I2C_GCLK_SPEED_HZ/(2*I2C_BAUD_RATE_HZ))-(I2C_GCLK_SPEED_HZ*I2C_TRISE_S/2)-5.0))

#define DMERGE(x,y)                 x##y
#define TMERGE(x,y,z)               x##y##z
#define QMERGE(w, x,y,z)            w##x##y##z

#define _I2C_SERCOM(n)              DMERGE(SERCOM,n)
#define I2C_SERCOM                  _I2C_SERCOM(I2C_SERCOM_NUMBER)

#define I2C_CLOCK_MASK_REG          PM->APBCMASK.reg
#define I2C_CLOCK_ENABLE_BIT_MASK   (1<<(2+I2C_SERCOM_NUMBER))

#define _i2c_master_irq(n)          TMERGE(SERCOM,n,_Handler)
#define i2c_master_irq              _i2c_master_irq(I2C_SERCOM_NUMBER)

#define _I2C_IRQ_NVIC(n)            TMERGE(SERCOM,n,_IRQn)
#define I2C_IRQ_NVIC                _I2C_IRQ_NVIC(I2C_SERCOM_NUMBER)

#define _I2C_CLOCK_SELECTION_ID(n)  TMERGE(GCLK_CLKCTRL_ID_SERCOM,n,_CORE)
#define I2C_CLOCK_SELECTION_ID      _I2C_CLOCK_SELECTION_ID(I2C_SERCOM_NUMBER)


#if I2C_SDA_USE_ALTERNATE==0
    #define I2C_SDA_AF          (0x02ul)
#else
    #define I2C_SDA_AF          (0x03ul)
#endif

#if I2C_SCL_USE_ALTERNATE==0
    #define I2C_SCL_AF          (0x02ul)
#else
    #define I2C_SCL_AF          (0x03ul)
#endif

// SERCOM0
#if I2C_SERCOM_NUMBER==0
    #if I2C_SDA_USE_ALTERNATE==0
    //PA8
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (8u)
    #else
    //PA4
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (4u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PA9
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (9u)
    #else
    //PA5
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (5u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM0_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM0_DMAC_ID_TX
#endif

// SERCOM1
#if I2C_SERCOM_NUMBER==1
    #if I2C_SDA_USE_ALTERNATE==0
    //PA16
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (16u)
    #else
    //PA0
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (0u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PA17
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (17u)
    #else
    //PA1
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (1u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM1_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM1_DMAC_ID_TX
#endif

// SERCOM2 PINS
#if I2C_SERCOM_NUMBER==2
    #if I2C_SDA_USE_ALTERNATE==0
    //PA12
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (12u)
    #else
    //PA8
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (8u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PA13
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (13u)
    #else
    //PA9
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (9u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM2_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM2_DMAC_ID_TX
#endif

// SERCOM3 PINS
#if I2C_SERCOM_NUMBER==3
    #if I2C_SDA_USE_ALTERNATE==0
    //PA22
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (22u)
    #else
    //PA16
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (16u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PA23
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (23u)
    #else
    //PA17
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (17u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM3_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM3_DMAC_ID_TX
#endif

// SERCOM4 PINS
#if I2C_SERCOM_NUMBER==4
    #if I2C_SDA_USE_ALTERNATE==0
    //PB12
        #define I2C_SDA_PORT        PORTB
        #define I2C_SDA_PIN         (12u)
    #else
    //PA12
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (12u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PB13
        #define I2C_SCL_PORT        PORTB
        #define I2C_SCL_PIN         (13u)
    #else
    //PA13
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (13u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM4_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM4_DMAC_ID_TX
#endif

// SERCOM5 PINS
#if I2C_SERCOM_NUMBER==5
    #if I2C_SDA_USE_ALTERNATE==0
    //PB16
        #define I2C_SDA_PORT        PORTB
        #define I2C_SDA_PIN         (16u)
    #else
    //PA22
        #define I2C_SDA_PORT        PORTA
        #define I2C_SDA_PIN         (22u)
    #endif

    #if I2C_SCL_USE_ALTERNATE==0
    //PB17
        #define I2C_SCL_PORT        PORTB
        #define I2C_SCL_PIN         (17u)
    #else
    //PA23
        #define I2C_SCL_PORT        PORTA
        #define I2C_SCL_PIN         (23u)
    #endif

    // DMAC Trigger
    #define I2C_DMAC_RX_TRIGGER     SERCOM5_DMAC_ID_RX
    #define I2C_DMAC_TX_TRIGGER     SERCOM5_DMAC_ID_TX
#endif



#endif // __I2C_PARAMETERS_SAMD21_H__
