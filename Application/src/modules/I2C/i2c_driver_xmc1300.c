/** ----------------------------------------------------------------------------
// i2c_driver_XMC1300.c
// -----------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EM
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Indent style: Replace tabs by spaces, 4 spaces per indentation level
//
// Initial version: 2017-07, maximilian.gerber@osram.de
//
// Change History:
//
// $Author: g.salvador $
// $Revision: 15910 $
// $Date: 2018-11-26 19:17:02 +0800 (Mon, 26 Nov 2018) $
// $Id: i2c_driver_xmc1300.c 15910 2018-11-26 11:17:02Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/I2c/tags/v1.7/Src/i2c_driver_xmc1300.c $
//
// -----------------------------------------------------------------------------

* \addtogroup I2cDriverAPI
* \{
*   \file
*   \addtogroup I2cDriverXMC1300
*   \{
*       \file
*       \brief I2c driver for Infineon XMC 1300 using UART FIFO buffer.
*   \}
* \}
*/

#include <stdint.h>
#include <stdbool.h>

#define MODULE_I2C
#include "Config.h"

#include "xmc_gpio.h"
#include "xmc_i2c.h"
#include "xmc_usic.h"
#include "xmc_scu.h"
#include "xmc1300_ports_hwdriver.h"

#include "crc16.h"

#include "i2c_driver.h"
#include "i2c_userinterface.h"
#include "i2c_parameters_xmc1300.h"
#include "i2c_local.h"


//-----------------------------------------------------------------------------
// local definitions
//-----------------------------------------------------------------------------


// The shift control signal is considered active without referring
// to the actual signal level. Data frame transfer is possible after
// each edge of the signal.

#define I2C_TRANSMISSION_MODE       (3U)

 // Word length
#define I2C_WORDLENGTH              (7U)
// Transmission data valid
#define I2C_SET_TDV                 (1U)


//-----------------------------------------------------------------------------
// local variables
//-----------------------------------------------------------------------------
static bool     i2c_trigger_read_request;

typedef enum XMC_I2C_CH_TDF_local
{
    I2C_CH_TDF_MASTER_SEND =         0U,
    I2C_CH_TDF_SLAVE_SEND =          (uint32_t)1U << 8U,
    I2C_CH_TDF_MASTER_RECEIVE_ACK =  (uint32_t)2U << 8U,
    I2C_CH_TDF_MASTER_RECEIVE_NACK = (uint32_t)3U << 8U,
    I2C_CH_TDF_MASTER_START =        (uint32_t)4U << 8U,
    I2C_CH_TDF_MASTER_RESTART =      (uint32_t)5U << 8U,
    I2C_CH_TDF_MASTER_STOP =         (uint32_t)6U << 8U
} XMC_I2C_CH_TDF_local_t;


//-----------------------------------------------------------------------------
// local structs
//-----------------------------------------------------------------------------
XMC_GPIO_CONFIG_t sda_pin_config;
XMC_GPIO_CONFIG_t scl_pin_config;
XMC_I2C_CH_CONFIG_t i2c_cfg;


//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------
i2c_driver_feedback_struct_t i2c_driver_feedback_struct;
extern i2c_local_state_t *i2c_local_struct;


//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------
//void USIC0_0_IRQHandler(void); // @todo: remove (needed only for debugging)
//void USIC0_1_IRQHandler(void); // @todo: remove (needed only for debugging)
//void USIC0_2_IRQHandler(void); // @todo: remove (needed only for debugging)
//void USIC0_3_IRQHandler(void); // @todo: remove (needed only for debugging)
//void USIC0_4_IRQHandler(void); // @todo: remove (needed only for debugging)
//void USIC0_5_IRQHandler(void); // @todo: remove (needed only for debugging)

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------
/**
* \brief Dummy function not needed for the XMC1300 itself. It is used to mantain
* the compatibility with SAMD21 driver
*/
void I2cDrvMasterInitDma(void) {return;}

//-----------------------------------------------------------------------------
/**
 * \brief I2C driver initialization
*/
void I2cDrvInit(void)
{

  // set I2C driver status
  i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_IDLE;

  // set trigger read request flag to false
  i2c_trigger_read_request = false;

  // I2C configuration
  i2c_cfg.address = 1U; // address of master (XMC)
  //    i2c_cfg.baudrate = 400000U;

  // Configure I2C channel
  // Code from XMC_I2C_CH_Init() of XMCLib v2.1.12 with XMC_I2C_CH_SetBaudrate() replaced, because it takes 6ms
  XMC_USIC_CH_Enable(I2C_USIC_CH);

  /* Data format configuration */
  I2C_USIC_CH->SCTR = ((uint32_t)I2C_TRANSMISSION_MODE << (uint32_t) USIC_CH_SCTR_TRM_Pos) | /* Transmision mode */
            ((uint32_t)I2C_WORDLENGTH << (uint32_t) USIC_CH_SCTR_WLE_Pos) | /* 8 data bits */
            USIC_CH_SCTR_FLE_Msk |  /* unlimited data flow */
            USIC_CH_SCTR_SDIR_Msk | /* MSB shifted first */
            USIC_CH_SCTR_PDL_Msk;   /* Passive Data Level */

  XMC_I2C_CH_SetSlaveAddress(I2C_USIC_CH, i2c_cfg.address);

  // XMC_I2C_CH_SetBaudrate() replaced
  // set I2C Frequency to 400kHz
  I2C_USIC_CH->PCR_IICMode |= (uint32_t) USIC_CH_PCR_IICMode_STIM_Msk;
  I2C_USIC_CH->FDR = (2 << USIC_CH_FDR_DM_Pos) | (960 << USIC_CH_FDR_STEP_Pos);
  I2C_USIC_CH->BRG = (24 << USIC_CH_BRG_DCTQ_Pos) | (2 << USIC_CH_BRG_PDIV_Pos);

  /* Enable transfer buffer */
  I2C_USIC_CH->TCSR = ((uint32_t)I2C_SET_TDV << (uint32_t) USIC_CH_TCSR_TDEN_Pos)
                      | USIC_CH_TCSR_TDSSM_Msk;

  /* Clear status flags */
  I2C_USIC_CH->PSCR = 0xFFFFFFFFU;

  /* Disable parity generation */
  I2C_USIC_CH->CCR = 0x0U;

  // Configure I2C pins
  XMC_I2C_CH_SetInputSource(I2C_USIC_CH, XMC_I2C_CH_INPUT_SDA, USIC0_C1_DX0_P1_2);

  // enable digital filter on SDA (DX0B)
  SET_BIT(I2C_USIC_CH->DXCR[0], USIC_CH_DX0CR_DFEN_Pos);

  XMC_I2C_CH_SetInputSource(I2C_USIC_CH, XMC_I2C_CH_INPUT_SCL, USIC0_C1_DX1_P1_3);

  // enable digital filter on SCL (DX1A)
  SET_BIT(I2C_USIC_CH->DXCR[1], USIC_CH_DX1CR_DFEN_Pos);

  // set hardware delay for more symmetrical signals
  WR_REG(I2C_USIC_CH->PCR, USIC_CH_PCR_IICMode_HDEL_Msk, USIC_CH_PCR_IICMode_HDEL_Pos, 15);

  // Configure UART FIFO
  // TX FIFO
  XMC_USIC_CH_TXFIFO_Configure(I2C_USIC_CH, I2C_FIFO_TX_START, I2C_FIFO_TX_SIZE, I2C_FIFO_TX_LIMIT);
  // RX FIFO, generate SRBI on exceeding of RX FIFO limit
  I2C_USIC_CH->RBCTR = (1 << USIC_CH_RBCTR_LOF_Pos);
  XMC_USIC_CH_RXFIFO_Configure(I2C_USIC_CH, I2C_FIFO_RX_START, I2C_FIFO_RX_SIZE, I2C_FIFO_RX_LIMIT);

  // Start I2C channel
  XMC_I2C_CH_Start(I2C_USIC_CH);

  // Configure pins
  // USIC0_CH1.DOUT0 (/USIC0_CH1.DX0B)
  sda_pin_config.mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT7;
  XMC_GPIO_Init(XMC_GPIO_PORT1, 2, &sda_pin_config);
  // USIC0_CH1.SCLKCOUT (/ USIC0_CH1.DX0A / USIC0_CH1.DX1A / USIC0_CH1.DOUT0)
  scl_pin_config.mode = XMC_GPIO_MODE_OUTPUT_OPEN_DRAIN_ALT6;
  XMC_GPIO_Init(XMC_GPIO_PORT1, 3, &scl_pin_config);

  //only for debugging
  ////    I2C_USIC_CH->PCR = 0;
  // SET_BIT(I2C_USIC_CH->PCR, USIC_CH_PCR_IICMode_ERRIEN_Pos); // error + wrong TDF
  // SET_BIT(I2C_USIC_CH->PCR, USIC_CH_PCR_IICMode_ARLIEN_Pos); // arbitration lost

  // NVIC_ClearPendingIRQ(USIC0_0_IRQn);
  // NVIC_EnableIRQ(USIC0_0_IRQn);
}

//-----------------------------------------------------------------------------
/**
* \brief Stops I2c HW driver
* \param
* \param
*/
void I2cDrvStop(void)
{
    // Stop I2C channel
    XMC_I2C_CH_Stop(I2C_USIC_CH);
}



//-----------------------------------------------------------------------------
/**
* \brief Starts an I2C frame
* \param dev_select_code: slave address
* \param length: length of the frame
*/
void I2cDrvStartTransfer(uint8_t dev_select_code, uint16_t length)
{
    // clear all flags inside the I2C Protocol Status Register
    // to be able to evaluate the flags for the new transmission/receiving
    I2C_USIC_CH->PSCR = 0x0001FFFF;

    // reset all Transmit/Receive Buffer Status Register flags
    // to be able to evaluate the flags for the new transmission/receiving
    I2C_USIC_CH->TRBSCR = USIC_CH_TRBSCR_CSRBI_Msk
                       | USIC_CH_TRBSCR_CRBERI_Msk
                       | USIC_CH_TRBSCR_CARBI_Msk
                       | USIC_CH_TRBSCR_CSTBI_Msk
                       | USIC_CH_TRBSCR_CTBERI_Msk
                       | USIC_CH_TRBSCR_CBDV_Msk;

    // enable tranmission start when TCSR.TDV=1
    I2C_USIC_CH->TCSR |= 1 << USIC_CH_TCSR_TDEN_Pos;
}

//-----------------------------------------------------------------------------
/**
* \brief Read 1 byte from I2c hardware module
* \retval read byte
*/
uint8_t I2cDrvReadByte(void)
{
  return XMC_USIC_CH_RXFIFO_GetData(I2C_USIC_CH);
}


//-----------------------------------------------------------------------------
/**
* \brief Dummy function not needed for the XMC1300 itself. It is used to mantain
* the compatibility with SAMD21 driver
*/
void I2cDrvStopTransfer(void) {return;}


//-----------------------------------------------------------------------------
/**
* \brief Dummy function not needed for the XMC1300 itself. It is used to mantain
* the compatibility with SAMD21 driver
*/
void I2cDrvSetAcknoledge(i2c_ack_t acknoledge) {return;}

//-----------------------------------------------------------------------------
/**
 * \brief Fill I2C TX write data into DMA buffer
 * \param dev_sel_code: (0xA6) write request
 * \param address: TAG start address
 * \param data: pointer to data array
 * \param length: number of bytes to write
 */
// ToDo add check for length <= 4
// ToDo add check status should be NOT - BUSY
//void I2cDrvWriteDataDma(uint8_t dev_sel_code, uint16_t address,
//                                  uint8_t *data, uint8_t length)
void I2cDrvWriteData(uint8_t dev_sel_code, uint16_t address,
                                  uint8_t *data, uint8_t length)
{
    uint8_t i1;

    // TDEN=0: disable transmission as long as FIFO buffer is not filled correctly
    I2C_USIC_CH->TCSR &= ~USIC_CH_TCSR_TDEN_Msk;

    // completely clear TX FIFO
    XMC_USIC_CH_TXFIFO_Flush(I2C_USIC_CH);

    // also reset TX FIFO data pointers
    XMC_USIC_CH_TXFIFO_Configure(I2C_USIC_CH, I2C_FIFO_TX_START, I2C_FIFO_TX_SIZE, I2C_FIFO_TX_LIMIT);

    // invalidate current content of TBUF, so that first FIFO entry is transferred automatically
    I2C_USIC_CH->FMR |= 2 << USIC_CH_FMR_MTDV_Pos; // MTDV=2 -> clear TCSR.TDV and TCSR.TE

    // fill TX FIFO
    // setup FIFO TX content - START ***********************************************************************************
    // start condition + device selct code + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (dev_sel_code | I2C_CH_TDF_MASTER_START | XMC_I2C_CH_CMD_WRITE));

    // fill address into data TX FIFO buffer
    // address high byte + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, ((address >> 8) | XMC_I2C_CH_CMD_WRITE));
    // address low byte + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (((uint8_t)(0x00FF & address)) | XMC_I2C_CH_CMD_WRITE));

    // fill data into TX FIFO buffer
    for (i1 = 0; i1 < length; i1++)
    {
        XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (data[i1] | XMC_I2C_CH_CMD_WRITE));
    }

    // send stop condition when all data was written
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, I2C_CH_TDF_MASTER_STOP);
    // setup FIFO TX content - END *************************************************************************************

    // set trigger read request flag to false
    i2c_trigger_read_request = false;

    // set I2C driver status
    i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_BUSY;
}

//-----------------------------------------------------------------------------
/**
* \brief Fill I2C RX read data into DMA buffer
* \param dev_sel_code: set to 0xA7 = read request
* \param address: i2c address
* \param data: pointer to data array
* \param length: data length
*/
// ToDo add check status should be NOT - BUSY
void I2cDrvReadData(uint8_t dev_sel_code, uint16_t address,
                                  uint8_t *data, uint8_t length)
{
    uint8_t i1;

    // TDEN=0: disable transmission as long as FIFO buffer is not filled correctly
    I2C_USIC_CH->TCSR &= ~USIC_CH_TCSR_TDEN_Msk;

    // completely clear TX FIFO
    XMC_USIC_CH_TXFIFO_Flush(I2C_USIC_CH);

    // also reset TX FIFO data pointers
    XMC_USIC_CH_TXFIFO_Configure(I2C_USIC_CH, I2C_FIFO_TX_START, I2C_FIFO_TX_SIZE, I2C_FIFO_TX_LIMIT);
    // additionally for RX FIFO: set LIMIT to length-1
    XMC_USIC_CH_RXFIFO_Configure(I2C_USIC_CH, I2C_FIFO_RX_START, I2C_FIFO_RX_SIZE, length-1);

    // invalidate current content of TBUF, so that first FIFO entry is transferred automatically
    I2C_USIC_CH->FMR |= 2 << USIC_CH_FMR_MTDV_Pos; // MTDV=2 -> clear TCSR.TDV and TCSR.TE

    /* fill TX FIFO */
    // setup FIFO TX content - START ***********************************************************************************
    // start condition + device selct code + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (dev_sel_code | I2C_CH_TDF_MASTER_START | XMC_I2C_CH_CMD_WRITE));

    // fill address into data TX FIFO buffer
    // address high byte + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, ((address >> 8) | XMC_I2C_CH_CMD_WRITE));
    // address low byte + write command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (((uint8_t)(0x00FF & address)) | XMC_I2C_CH_CMD_WRITE));
     // restart condition + device selct code + read command
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, (dev_sel_code | I2C_CH_TDF_MASTER_RESTART | XMC_I2C_CH_CMD_READ));

    // fill data into TX FIFO buffer
    //20170622mg: currenctly no CRC -> length is 2 bytes shorter
    // additionally -1, because first byte was already received after restart
    for (i1 = 0; i1 < length - 1; i1++)
    {
        XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, I2C_CH_TDF_MASTER_RECEIVE_ACK);
    }

    // send stop condition, when all desired bytes were received
    XMC_USIC_CH_TXFIFO_PutData(I2C_USIC_CH, I2C_CH_TDF_MASTER_STOP);
    // setup FIFO TX content - END

    // set I2C driver status
    i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_BUSY;

    // set trigger read request flag
    i2c_trigger_read_request = true;
}

//-----------------------------------------------------------------------------
/**
* \brief Dummy function not needed for the XMC1300 itself. It is used to mantain
* the compatibility with SAMD21 driver
 * \return dummy always 0
 */
bool I2cDrvIsRxBusy(void) {return 0;}

//-----------------------------------------------------------------------------
/**
* \brief Dummy function not needed for the XMC1300 itself. It is used to mantain
* the compatibility with SAMD21 driver
 * \return dummy always 0
 */
bool I2cDrvIsTxBusy(void) {return 0;}

//-----------------------------------------------------------------------------
/**
 * \brief Update i2c_driver_feedback_struct.status
*  \note Must not be called faster than every 200us to be able to distinguish
*   sending from receiving after 4th I2C frame.
*/
void I2cDrvUpdateStatus(void)
{
    uint32_t i2c_status;
    uint32_t rx_fifo_status;
    uint32_t tx_fifo_status;

    // read I2C status flags from Protocol Status Register PSR_IIC
    i2c_status = XMC_I2C_CH_GetStatusFlag(I2C_USIC_CH);
    // read TX FIFO status flags from Transmit/Receive Buffer Status Register
    tx_fifo_status = XMC_USIC_CH_TXFIFO_GetEvent(I2C_USIC_CH);
    // read RX FIFO status flags from Transmit/Receive Buffer Status Register
    rx_fifo_status = XMC_USIC_CH_RXFIFO_GetEvent(I2C_USIC_CH);

//    if (0 == i2c_status)
//    {
//        i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_IDLE;
//    }
//    else if (i2c_status & USIC_CH_PSR_IICMode_ERR_Msk)
    if (i2c_status & USIC_CH_PSR_IICMode_ERR_Msk)
    {
        i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_ERROR;
    }
    else if (i2c_status & USIC_CH_PSR_IICMode_NACK_Msk)
    {
        i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_NACK;
    }
    // if there was NO read request, a transmission is happening
    // the transmission is finished, when the TX FIFO buffer is empty and TBUF is idle
    else if ((false == i2c_trigger_read_request)
          && (XMC_USIC_CH_TXFIFO_EVENT_STANDARD == tx_fifo_status)
          && (XMC_USIC_CH_TBUF_STATUS_IDLE == XMC_USIC_CH_GetTransmitBufferStatus(I2C_USIC_CH)))
    {
        i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_DONE_TX;
    }
    // if there was a read request, a receiving is happening
    // the receiving is finished, when the RX FIFO buffer equals the amount of data that was instructed to be received
    else if ((true == i2c_trigger_read_request) && (XMC_USIC_CH_RXFIFO_EVENT_STANDARD == rx_fifo_status))
    {
        i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_DONE_RX;
//        // fake CRC-16 (in contrast to SAMD21, the XMC1300 does not offer an automatic CRC to check the data integrity
//        i2c_driver_feedback_struct.crc_16 = 0;
    }
    else
    {
      // this is, in fact, an error case!
      i2c_driver_feedback_struct.status = I2C_DRIVER_STATUS_BUSY;
    }

//    // clear all status flags
//    I2C_USIC_CH->PSCR |= 0x1FFFF;
}

//-----------------------------------------------------------------------------
/**
 * \brief Issue a new I2c transfer.
 * \note Can be RX or TX, RX will always have the priority, even if a writing
 * procedure is already in progress.
 */
void start_transfer(void)
{
    uint16_t dev_select_code;

    if (i2c_transfer_direction_rx == i2c_local_struct->transfer_direction)
    {
        // limit length of RX transfer portion, if hardware cannot handle the whole RX request length at once
        if ((i2c_local_struct->rx_request.length - i2c_local_struct->rx_request.bytes_received_cnt)
            >= MAX_RX_TRANSFER_LENGTH)
        {
            i2c_local_struct->rx_request.length_portion = MAX_RX_TRANSFER_LENGTH;
        }
        else
        {
          i2c_local_struct->rx_request.length_portion =
              i2c_local_struct->rx_request.length
            - i2c_local_struct->rx_request.bytes_received_cnt;
        }

        // read from tag register or memory area?
        // bit 7 set: read from tag register
        // bit 6 set: read from user memory area
        dev_select_code =
                (i2c_local_struct->rx_request.addr & 0xC000) ?
                M24LRxx_ADDR_REGISTER : M24LRxx_ADDR_DATA;

        // fill transfer data into DMA buffer and start transfer
        // mask most significant bit of address

        I2cDrvReadData (dev_select_code,
                          ((i2c_local_struct->rx_request.addr & 0x3FFF) + i2c_local_struct->rx_request.bytes_received_cnt),
                           i2c_local_struct->rx_request.data_ptr + i2c_local_struct->rx_request.bytes_received_cnt,
                           i2c_local_struct->rx_request.length_portion);

        // start DMA transfer - Read
        I2cDrvStartTransfer(dev_select_code, M24LRxx_DATA_ADDR_WIDTH);
    }
    else // i2c_transfer_direction_tx == i2c_local_state.transfer_direction
    {
        // read from tag register or memory area?
        dev_select_code =
                (i2c_local_struct->tx_request.addr & 0xC000) ?
                M24LRxx_ADDR_REGISTER : M24LRxx_ADDR_DATA;

        // fill transfer data into DMA buffer
        // mask most significant bit of address
        I2cDrvWriteData (dev_select_code,
                           (i2c_local_struct->tx_request.addr & 0x3FFF),
                           i2c_local_struct->tx_request.data,
                           i2c_local_struct->tx_request.length);

        // start DMA transfer - Write
        // length = data_length + 2 = 2 bytes address + length of data
        I2cDrvStartTransfer (dev_select_code,
                i2c_local_struct->tx_request.length + M24LRxx_DATA_ADDR_WIDTH);
    }
}

//-----------------------------------------------------------------------------
/**
* \brief Return true if all portions of the RX request were received
*/
bool is_rx_request_completed(void)
{
    return i2c_local_struct->rx_request.bytes_received_cnt == i2c_local_struct->rx_request.length;
}

//-----------------------------------------------------------------------------
/**
* \brief Update the CRC calculation after receiving each portion of a RX request
*/
void update_crc_calculation_on_rx(void)
{
  uint8_t i1;

    // copy data from Rx FIFO and add to CRC calculation
    for (i1 = 0; i1 < (i2c_local_struct->rx_request.length_portion); i1++)
    {
        i2c_local_struct->rx_request.data_ptr[i2c_local_struct->rx_request.bytes_received_cnt + i1] = I2cDrvReadByte();
        Crc16AppendByte_Inst(1, i2c_local_struct->rx_request.data_ptr[i2c_local_struct->rx_request.bytes_received_cnt + i1]);
    }

    // update bytes_received counter
    i2c_local_struct->rx_request.bytes_received_cnt += i2c_local_struct->rx_request.length_portion;
}

//-----------------------------------------------------------------------------
/**
* \brief Init CRC calculation at the beginning of a RX request
*/
void init_crc_calculation_on_rx(void)
{
    // init CRC calculation at the beginning of a receiving
    if (0 == i2c_local_struct->rx_request.bytes_received_cnt)
    {
        Crc16Init_Inst(1, CRC16_CCITT_FALSE_START_VALUE);
    }
}

//-----------------------------------------------------------------------------
/**
* \brief Check if the last reveived I2c frame had a crc error
* \return true if there is no crc error
*/
bool is_crc_correct(void)
{
    return 0 == Crc16Checksum_Inst(1, crc16_big_endian);
}



/** \} */ // I2cDriverXMC1300
/** \} */ // I2cDriver
