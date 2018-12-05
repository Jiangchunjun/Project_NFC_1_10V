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
// $Id: i2c_driver_xmc1300.c 2911 2016-02-11 15:19:38Z G.Marcolin $
// $URL: http://app-mchsvn01.mch.osram.de/svn/EC/Platforms/00%20Tests%20and%20Doc/UnitTestNfc/test_iar/uf_v2601_apac/src/modules/I2cExt/driver_xmc1300/i2c_driver_xmc1300.c $
//
// ---------------------------------------------------------------------------------------------------------------------

/** \addtogroup I2cDriver
 * \{
 * \defgroup I2cDriverXmc1300
 * \{
 *
 * \file
 * \brief I2c driver for Xmc1300
 */

#ifdef MODULE_I2C
     
#include <stdint.h>
#include <stdbool.h>

#include <XMC1300.h>
#include <core_cm0.h>

#include "Config.h"

#include "i2c_driver.h"

/**********************************************************************************************************************/

#define I2C_TDF_MTxData 			(0x0U)
#define I2C_TDF_STxData 			(0x1U)
#define I2C_TDF_MRxAck0 			(0x2U)
#define I2C_TDF_MRxAck1 			(0x3U)
#define I2C_TDF_MStart 				(0x4U)
#define I2C_TDF_MRStart 			(0x5U)
#define I2C_TDF_MStop 				(0x6U)

/**********************************************************************************************************************/

#define USIC_CH_TBCTR_DPTRSIZE_Pos  	(0U)
#define USIC_CH_TBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_TBCTR_DPTRSIZE_Pos)

#define USIC_CH_RBCTR_DPTRSIZE_Pos  	(0U)
#define USIC_CH_RBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_RBCTR_DPTRSIZE_Pos)
#define PORT_PDISC_PDIS0                (0)

/**********************************************************************************************************************/

#define CLK002_DELAYCNT             (0x3FFU)

/**********************************************************************************************************************/

#define I2C_ENABLE                  (0x4U)

#define I2C_DCTQ2_VALUE             (0x18UL)        // 25 timequanta DCTQ value

#define I2C001_SCTR_SDIR            (0x01U)         // Direction set, msb transmitted or received first

#define I2C001_SCTR_FLE             (0x3FU)         // Frame Length :  64 Bits

#define I2C001_SCTR_WLE             (0x07U)         // Word Length :  8 Bits

#define I2C001_TX_LIMIT             (0x01U)         // FIFO Tigger Level

#define I2C001_RX_LIMIT             (0x00U)         // FIFO Tigger Level

/**********************************************************************************************************************/

#define NVIC_NODE_ID                (9)
#define NVIC_PRIORITY               (3)

/**********************************************************************************************************************/

#define USIC_FRACTIONAL_DIV_SEL     (0x02U)         // Fractional divider mode selected

/**********************************************************************************************************************/

static USIC_CH_TypeDef *USIC;
const uint16_t I2C_BaudParamsBuf[] = {0x280, 0x7};//0x7 for 100kHz ,0X01 400kHz

/** ********************************************************************************************************************
* \brief I2C read/write/error irq manager
*
***********************************************************************************************************************/

void i2c_master_irq(void)
{
	//------------  Receive data from slave --------------
    if(USIC->PSR & USIC_CH_PSR_AIF_Msk)
    {
        USIC->PSCR = USIC_CH_PSR_AIF_Msk;

        i2c_drv_master_rx_irq_callback();
    }

    if(USIC->PSR & USIC_CH_PSR_RIF_Msk)
    {
        USIC->PSCR = USIC_CH_PSR_RIF_Msk;

        i2c_drv_master_rx_irq_callback();
    }

    //------------- Transmit data to slave ---------------
    if(USIC->PSR & USIC_CH_PSR_TBIF_Msk)
    {
        USIC->PSCR = USIC_CH_PSR_TBIF_Msk;

        i2c_drv_master_tx_irq_callback();
    }

    //---------------- Error handling --------------------
    if(USIC->PSR_IICMode & USIC_CH_PSR_IICMode_ERR_Msk)
    {
        USIC->PSCR = USIC_CH_PSR_IICMode_ERR_Msk;

        i2c_drv_master_err_irq_callback(I2C_CODE_FRAME_ERR);

    }

    if(USIC->PSR_IICMode & USIC_CH_PSR_IICMode_NACK_Msk)
    {
        USIC->PSCR = USIC_CH_PSR_IICMode_NACK_Msk;

        i2c_drv_master_err_irq_callback(I2C_CODE_NACK);

        //// remove ////
        //P0_1_toggle();
        ////////////////
    }
}

/*********************************************** Interface functions **************************************************/

/** ********************************************************************************************************************
* \brief I2C driver init
*
***********************************************************************************************************************/

void i2c_drv_master_init(void)
{
    uint32_t UsicCcrMode = 0;                              // Variable to store the CCR_MODE values for various USIC channels

#ifdef I2C__SCL_PIN_2_0__SDA_PIN_2_1
    USIC = USIC0_CH0;

    WR_REG(PORT2->IOCR0, PORT2_IOCR0_PC0_Msk, PORT2_IOCR0_PC0_Pos, 0x1F);                           // Pin 2.0 - ALT7 - SCL
    WR_REG(PORT2->IOCR0, PORT2_IOCR0_PC1_Msk, PORT2_IOCR0_PC1_Pos, 0x1E);                           // Pin 2.1 - ALT6 - SDA

    WR_REG(PORT2->PDISC, PORT2_PDISC_PDIS0_Msk, PORT2_PDISC_PDIS0_Pos, PORT_PDISC_PDIS0);           // Pad 0.0 is enabled
    WR_REG(PORT2->PDISC, PORT2_PDISC_PDIS1_Msk, PORT2_PDISC_PDIS1_Pos, PORT_PDISC_PDIS0);           // Pad 0.1 is enabled

#elif defined( I2C__SCL_PIN_0_8__SDA_PIN_0_6 )
    USIC = USIC0_CH1;

    WR_REG(PORT0->IOCR8, PORT0_IOCR8_PC8_Msk, PORT0_IOCR8_PC8_Pos, 0x1F);                           // Pin 0.8 - ALT7 - SCL
    WR_REG(PORT0->IOCR4, PORT0_IOCR4_PC6_Msk, PORT0_IOCR4_PC6_Pos, 0x1F);                           // Pin 0.6 - ALT7 - SDA

#elif defined( I2C__SCL_PIN_1_3__SDA_PIN_1_2 )
    USIC = USIC0_CH1;

    WR_REG(PORT1->IOCR0, PORT1_IOCR0_PC3_Msk, PORT1_IOCR0_PC3_Pos, 0x1E);                           // Pin 1.3 - ALT6 - SCL
    WR_REG(PORT1->IOCR0, PORT1_IOCR0_PC2_Msk, PORT1_IOCR0_PC2_Pos, 0x1F);                           // Pin 1.2 - ALT7 - SDA

#else
    #error "I2C pins not defined !!!"
#endif

    SCU_GENERAL->PASSWD = 0x000000C0UL;
    WR_REG(SCU_CLK->CLKCR, SCU_CLK_CLKCR_CNTADJ_Msk, SCU_CLK_CLKCR_CNTADJ_Pos, CLK002_DELAYCNT);
    SET_BIT(SCU_CLK->CGATCLR0, SCU_CLK_CGATCLR0_USIC0_Pos);

    while ((SCU_CLK->CLKCR)&(SCU_CLK_CLKCR_VDDC2LOW_Msk))
    {}

    SCU_GENERAL->PASSWD = 0x000000C3UL;

    //------------------- I2C initialization --------------------
    USIC->CCR &= ~((uint32_t)(I2C_ENABLE  & USIC_CH_CCR_MODE_Msk));       // Disable I2C mode before configuring all USIC registers to avoid unintended edges

    // Enable the USIC Channel
    USIC->KSCFG |= (((uint32_t)(0x01) & USIC_CH_KSCFG_MODEN_Msk)) | \
                   (((uint32_t)(0x01) << USIC_CH_KSCFG_BPMODEN_Pos));

    //-------- Config of USIC Channel Fractional Divider --------
    // Fractional divider mode selected
    USIC->FDR |=  ((((uint32_t)USIC_FRACTIONAL_DIV_SEL << USIC_CH_FDR_DM_Pos) & \
                      USIC_CH_FDR_DM_Msk) | \
                    ((*I2C_BaudParamsBuf << USIC_CH_FDR_STEP_Pos) & \
                      USIC_CH_FDR_STEP_Msk));

    // The PreDivider for CTQ, PCTQ = 0, The Denominator for CTQ, DCTQ = 24
    USIC->BRG |= ((I2C_DCTQ2_VALUE << USIC_CH_BRG_DCTQ_Pos) | \
                 ((*(I2C_BaudParamsBuf + 1) << USIC_CH_BRG_PDIV_Pos) & \
                   USIC_CH_BRG_PDIV_Msk));


    //------------- Config of USIC Shift Control ----------------
    // Transmit/Receive MSB first is selected, Transmission Mode (TRM) = 3, Passive Data Level (PDL) = 1, Frame Length (FLE) = 63 (3F), Word Length (WLE) = 7
    USIC->SCTR |= ((((uint32_t)I2C001_SCTR_SDIR << USIC_CH_SCTR_SDIR_Pos)&  \
                      USIC_CH_SCTR_SDIR_Msk) | \
                    ((uint32_t)(0x01) << USIC_CH_SCTR_PDL_Pos ) | \
                    ((uint32_t)(0x03) <<  USIC_CH_SCTR_TRM_Pos) |
                   (((uint32_t)I2C001_SCTR_FLE  << USIC_CH_SCTR_FLE_Pos) & \
                      USIC_CH_SCTR_FLE_Msk ) | \
                   (((uint32_t)I2C001_SCTR_WLE << USIC_CH_SCTR_WLE_Pos) & \
                      USIC_CH_SCTR_WLE_Msk));

    //---- Config of USIC Transmit Control/Status Register ------
    // TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
    USIC->TCSR |= ((((uint32_t)(0x01) << USIC_CH_TCSR_TDEN_Pos) & \
                      USIC_CH_TCSR_TDEN_Msk) | \
                   (((uint32_t)(0x01) << USIC_CH_TCSR_TDSSM_Pos) & USIC_CH_TCSR_TDSSM_Msk));

    //------- Configuration of Protocol Control Register --------
    // Symbol timing = 25 time quanta
    USIC->PCR_IICMode |= (((((uint32_t)(0x1U) << \
                              USIC_CH_PCR_IICMode_STIM_Pos)) & \
                    (uint32_t)USIC_CH_PCR_IICMode_STIM_Msk));

    // Configuration of Transmitter Buffer Control Register, limit for transmit FIFO interrupt generation is set based on UI
    USIC->TBCTR |= ((((uint32_t)I2C001_TX_LIMIT << USIC_CH_TBCTR_LIMIT_Pos ) & \
                       USIC_CH_TBCTR_LIMIT_Msk));

    // Configuration of Receiver Buffer Control Register, limit for receive FIFO interrupt generation is set based on UI, filling level mode is selected
    USIC->RBCTR |= ((((uint32_t)I2C001_RX_LIMIT << USIC_CH_RBCTR_LIMIT_Pos) & \
                        USIC_CH_RBCTR_LIMIT_Msk) |  \
                     (((uint32_t)(0x01) << USIC_CH_RBCTR_LOF_Pos) & \
                        USIC_CH_RBCTR_LOF_Msk));

    //------------------------ Enable ---------------------------
    USIC->CCR |= ((I2C_ENABLE) & USIC_CH_CCR_MODE_Msk);

    NVIC_SetPriority((IRQn_Type)NVIC_NODE_ID, NVIC_PRIORITY);
    NVIC_EnableIRQ((IRQn_Type)NVIC_NODE_ID);

    //---------------------- Config USIC ------------------------
    UsicCcrMode |= (uint32_t)RD_REG(USIC->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos); // Read CCR reg

    WR_REG(USIC->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos,0);                                // USIC disabled before configuring all USIC registers to avoid unintended edges

    //---------------------- Interrupts -------------------------
    WR_REG(USIC->CCR, USIC_CH_CCR_AIEN_Msk, USIC_CH_CCR_AIEN_Pos, 1);                               // Alternative Rx Irq enable
    WR_REG(USIC->CCR, USIC_CH_CCR_RIEN_Msk, USIC_CH_CCR_RIEN_Pos, 1);                               // Rx Irq enable
    WR_REG(USIC->CCR, USIC_CH_CCR_TBIEN_Msk, USIC_CH_CCR_TBIEN_Pos, 1);                             // Tx buffer Irq enable
    WR_REG(USIC->PCR_IICMode, USIC_CH_PCR_IICMode_ERRIEN_Msk,  USIC_CH_PCR_IICMode_ERRIEN_Pos, 1);  // Err Irq enable
    WR_REG(USIC->PCR_IICMode, USIC_CH_PCR_IICMode_NACKIEN_Msk, USIC_CH_PCR_IICMode_NACKIEN_Pos, 1); // Non-Ack Irq enable

    //-------------------- Input selection ----------------------
    WR_REG(USIC->PCR_IICMode, USIC_CH_PCR_IICMode_HDEL_Msk, USIC_CH_PCR_IICMode_HDEL_Pos, 0);       // HDEL

#ifdef I2C__SCL_PIN_2_0__SDA_PIN_2_1
    WR_REG(USIC->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 5);                // DX0F input selected
    WR_REG(USIC->DX1CR, USIC_CH_DX1CR_DSEL_Msk, USIC_CH_DX1CR_DSEL_Pos, 4);                // DX1E input selected
#endif

#ifdef I2C__SCL_PIN_0_8__SDA_PIN_0_6
    WR_REG(USIC->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 2);                // DX0F input selected
    WR_REG(USIC->DX1CR, USIC_CH_DX1CR_DSEL_Msk, USIC_CH_DX1CR_DSEL_Pos, 1);                // DX1E input selected
#endif

#ifdef I2C__SCL_PIN_1_3__SDA_PIN_1_2
    WR_REG(USIC->DX0CR, USIC_CH_DX0CR_DSEL_Msk, USIC_CH_DX0CR_DSEL_Pos, 1);                // DX0F input selected
#endif

    //------------------ Config Rx Tx buffer  -------------------
    WR_REG(USIC->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk, USIC_CH_TBCTR_DPTRSIZE_Pos, 0x01000006);		// DPTR = 6, SIZE = 1, Tx FIFO buffer contains 2 entries
    WR_REG(USIC->RBCTR, USIC_CH_RBCTR_DPTRSIZE_Msk, USIC_CH_RBCTR_DPTRSIZE_Pos, 0x01000004);		// DPTR = 4, SIZE = 1, Rx FIFO buffer contains 2 entries

    WR_REG(USIC->CCR, USIC_CH_CCR_MODE_Msk, USIC_CH_CCR_MODE_Pos, UsicCcrMode);                     // USIC enabled after configuring all registers to avoid unintended edges
}

/** ********************************************************************************************************************
* \brief Starts an I2C frame
*
* \param addr
*        slave address
*
* \param rw_cmd
*        read/write command
*
* \param rstart
*        w/o stop
*
***********************************************************************************************************************/

void i2c_drv_master_start(uint32_t addr, i2c_master_cmd_t rw_cmd, bool rstart)
{
    uint32_t data;

    data = (rstart ? (uint32_t)I2C_TDF_MRStart : (uint32_t)I2C_TDF_MStart);
    data <<= 8;
    data |= addr;

    if(rw_cmd == I2C_MASTER_CMD_READ)
    {
        data |= 0x00000001;
    }

    USIC->TCSR &= ~USIC_CH_TCSR_TDEN_Msk;                     // Disable TBUF data Tx

    USIC->IN[0] = data;                                       // Load data in Tx buf

    USIC->FMR &= ~0x00000003;
    USIC->FMR |= 0x00000002;                                  // Bit TDV (tx data valid) and TE (trigger event) cleared

    USIC->FMR &= ~0x00000003;
    USIC->FMR |= 0x00000001;                                  // Set TDV

    USIC->TCSR |= (0x00000001 << USIC_CH_TCSR_TDEN_Pos);      // Enable TBUF data Tx
}

/** ********************************************************************************************************************
* \brief Transmits a write frame
*
* \param data
*        datato transmit
*
***********************************************************************************************************************/

void i2c_drv_master_tx_write(uint8_t data)
{
    uint16_t d  = ((uint32_t)I2C_TDF_MTxData << 8);
    d |= data;

    USIC->IN[0] = d;
}

/** ********************************************************************************************************************
* \brief Transmits a read frame
*
* \param ack
*        w/o ack
*
***********************************************************************************************************************/

void i2c_drv_master_tx_read(i2c_ack_t ack)
{
    uint32_t data;

    data = ((ack == I2C_ACK_YES) ? (uint32_t)I2C_TDF_MRxAck0 : (uint32_t)I2C_TDF_MRxAck1);
    data <<= 8;

    USIC->IN[0] = (data|0x000000FF);
}

/** ********************************************************************************************************************
* \brief Transmits a stop
*
***********************************************************************************************************************/

void i2c_drv_master_stop(void)
{
    uint32_t data;

    data = ((uint32_t)I2C_TDF_MStop << 8);

    USIC->IN[0] = data;
}

/** ********************************************************************************************************************
* \brief Gets read byte
*
* \retval read byte
*
***********************************************************************************************************************/

uint8_t i2c_drv_master_get_rx_data(void)
{
    return (uint8_t)USIC->RBUF;
}

/**********************************************************************************************************************/

/** \} */ // I2cDriverXmc1300
/** \} */ // I2cDriver

#endif /* MODULE_I2C */
