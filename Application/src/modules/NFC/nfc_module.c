/** ----------------------------------------------------------------------------
// nfc_module.c
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
// $Author: g.salvador $
// $Revision: 16930 $
// $Date: 2019-02-13 22:20:52 +0800 (Wed, 13 Feb 2019) $
// $Id: nfc_module.c 16930 2019-02-13 14:20:52Z g.salvador $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nfc/tags/v2.7/Src/nfc_module.c $
//
// Main NFC module state machine with all related functions
*   \file
*   \brief Master file for NFC module
*/

#include <string.h>

#define MODULE_NFC
#include "Global.h"
#include "Config.h"

#include "ErrorHandler.h"
#include "crc16.h"
#include "nvm.h"
#include "MpcDefs.h"
#include "MemoryBanks_Dali.h"

#include "Acl.h"
#include "Mpc.h"
#include "MpcInternal.h"
#include "MpcAllocation.h"

#include "Nfc.h"
#include "nfc_config.h"
#include "nfc_local.h"
#include "i2c_userinterface.h"
#include "nfc_parameters.h"

#include <gpio_xmc1300_tssop38.h>

//------------------------------------------------------------------------------
// local variables
//------------------------------------------------------------------------------
STATIC nfc_local_state_t nfc_local_state;
STATIC uint8_t nfc_tag_mem_tmp[sizeof(nfc_tag_identification_register_t)];

//------------------------------------------------------------------------------
// global variables
//------------------------------------------------------------------------------
nfc_init_status_t nfc_init_status;

//-----------------------------------------------------------------------------
// local functions
//-----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
/**
 * \brief initialize tag device identification register.
 * \retval busy or done nfc status
 * \note  This function is called if:
 *      - MCU was flashed
 *      - control registers are all 0xFF
 *      - one of the register values are corrupted (wrong CRC)
 */
STATIC nfc_fct_status_t initialize_tag_device_identification(void)
{
    uint8_t i1, i2;
    nfc_tag_identification_register_t *tag_reg_ident;

    // set pointer for Identification Registers to nfc_tag_mem_tmp
    tag_reg_ident = (nfc_tag_identification_register_t *) nfc_tag_mem_tmp;

    // Initialize CRC 16
    Crc16Init (CRC16_CCITT_FALSE_START_VALUE);

    // Manufacturer Code
    tag_reg_ident->manufacturer_code = OSRAM_MANUFACTURER_CODE;

    // GTIN
    i2 = 5;
    for (i1 = 0; i1 < 6; i1++)
    {
        tag_reg_ident->gtin[i1] =
                (((uint64_t) THIS_DEVICE_EAN) >> ((i2--) * 8));
    }

    // FW Major Version
    tag_reg_ident->fw_major_version = DEVICE_FW_VERSION_MAJOR;

    // FW Minor Version
    tag_reg_ident->fw_minor_version = DEVICE_FW_VERSION_MINOR;

    // HW Major Version
    tag_reg_ident->hw_major_version = (uint8_t) (Ballast_GetHW_Version() >> 8);

    // NFC Major Version
    tag_reg_ident->nfc_major_version = NFC_VERSION_MAJOR;

    // MPC Count
    tag_reg_ident->toc_items = MPC_NFC_INST;

    // Status Register Start Address
    tag_reg_ident->status_register_start_addr = REV_BYTE_ORDER16(
            nfc_tag_addr_config.status_register_start_addr);

    // Control Register Start Address
    tag_reg_ident->control_register_start_addr = REV_BYTE_ORDER16(
            nfc_tag_addr_config.control_register_start_addr);

    // Protected Area Start Address
    tag_reg_ident->protected_memory_start_addr = REV_BYTE_ORDER16(
            nfc_tag_addr_config.protected_memory_start_addr);

    // calculate CRC16 for Device Identification
    Crc16AppendByteArray(nfc_tag_mem_tmp,
                         sizeof(nfc_tag_identification_register_t) - 2);

    // set CRC16 as big-endian
    tag_reg_ident->crc16 = Crc16Checksum(crc16_big_endian);

    // write Device Identification to TAG
    if(true == I2cWrite(0, nfc_tag_mem_tmp,
                   sizeof(nfc_tag_identification_register_t)))
    {
        return nfc_fct_done;
    }

    return nfc_fct_busy;
}

// -----------------------------------------------------------------------------
/**
 * \brief initialize tag TOC register.
 * \retval busy or done nfc status
 * \note  This function is called, if:
 *      - MCU was flashed
 *      - control registers are all 0xFF
 *      - the register values are corrupted (wrong CRC)
 */
STATIC nfc_fct_status_t initialize_tag_toc(void)
{
    uint8_t toc_idx;
    uint16_t crc16;
    uint32_t mpc_tag_start_addr;
    nfc_toc_entry_t *toc_entry;
    uint8_t keydata_attribute;
    uint8_t toc_mb_attribute;

    toc_idx = nfc_local_state.mb_proc_idx;

    // when all TOC entries are written successfully to TAG....
    if (toc_idx >= MPC_NFC_INST)
    {
        // write CRC16 to TAG
        crc16 = Crc16Checksum(crc16_big_endian);

        if (true == I2cWrite(nfc_local_state.mb_entity.addr, (uint8_t *) &crc16,
                            2))
        {
            return nfc_fct_done;
        }
    }
    else
    {
        // when it is the 1st TOC entry .....
        if (0 == toc_idx)
        {
            // initialize address to TOC start
            // the variable mb_entity.addr is re-used here
            nfc_local_state.mb_entity.addr = NFC_TOC_START_ADDR;

            // Initialize CRC 16
            Crc16Init (CRC16_CCITT_FALSE_START_VALUE);
        }

        // set pointer for TOC entry to nfc_tag_mem_tmp
        toc_entry = (nfc_toc_entry_t *) nfc_tag_mem_tmp;

        // fill TOC entry
        // 'translate' MPC attributes to TOC attribute (interpreted by SW)
        keydata_attribute = toc_items[toc_idx].mpc_keydata->attributes;

        #ifdef NFC_CONFIG_UNPROTECTED_LAYOUT // Deprecated, only for transition phase
            // Deprecated MPC_ATTRIB_NFC_UNPROTECTED is not supported here as
            // this is the default behavior (again)
            STATIC_ASSERT(0x02 == MPC_ATTRIB_NFC_PROTECTED,
                          "ALLOC - NFC protected attribute mismatch");
            STATIC_ASSERT(0x04 == MPC_ATTRIB_NFC_READONLY,
                          "ALLOC - NFC Read-only attribute mismatch");
            toc_mb_attribute = toc_items[toc_idx].mpc_keydata->attributes &
                          (MPC_ATTRIB_NFC_READONLY | MPC_ATTRIB_NFC_PROTECTED);
        #else
            if (keydata_attribute & MPC_ATTRIB_NFC_READONLY)
            {
              toc_mb_attribute = 0x04;
            }
            else if (keydata_attribute & MPC_ATTRIB_NFC_UNPROTECTED)
            {
              toc_mb_attribute = 0x00;
            }
            else
            {
              toc_mb_attribute = 0x02;
            }
        #endif
        toc_entry->mb_attribute = toc_mb_attribute;

        toc_entry->mpc_id = toc_items[toc_idx].mpc_keydata->id;
        toc_entry->mpc_version = toc_items[toc_idx].mpc_keydata->version;
        toc_entry->mb_length = toc_items[toc_idx].mpc_keydata->length;

        mpc_tag_start_addr = REV_BYTE_ORDER16(NfcMpcTagStartAddr[toc_idx]);

        toc_entry->mb_tag_base_addr = mpc_tag_start_addr;

        // Write TOC entry to TAG
        nfc_local_state.is_pending_i2c_access = !(I2cWrite(
                nfc_local_state.mb_entity.addr, nfc_tag_mem_tmp,
                sizeof(nfc_toc_entry_t)));

        // if I2cWrite was successful ....
        if (false == nfc_local_state.is_pending_i2c_access)
        {
            // calculate CRC16 for Device Identification
            Crc16AppendByteArray(nfc_tag_mem_tmp, sizeof(nfc_toc_entry_t));

            // increment index for next TOC entry
            nfc_local_state.mb_proc_idx++;

            // increment address for next TOC entry
            nfc_local_state.mb_entity.addr += sizeof(nfc_toc_entry_t);
        }
    }

    return nfc_fct_busy;
}

// -----------------------------------------------------------------------------
/**
 * \brief initialize tag access rights.
 * \retval busy or done nfc status
 * \note  This function is called, if:
 *      - MCU was flashed
 *      - control registers are all 0xFF
 *      - the register values are corrupted (wrong CRC)
 */
STATIC nfc_fct_status_t initialize_tag_access_rights(void)
{
    if (nfc_local_state.access_right_init_idx == 0)
    {
        // present I2C password to change access rights (sss byte)
        if (true == I2cPresentI2cPwd(0))
        {
            nfc_local_state.access_right_init_idx++;
        }
    }
    else
    {
        // set pwd control bits and read / write access rights for each sector
        nfc_local_state.is_pending_i2c_access = !(I2cInitSectorProtection());

        // if I2cWrite was successful ....
        if (false == nfc_local_state.is_pending_i2c_access)
        {
            nfc_local_state.access_right_init_idx = 0;
            return nfc_fct_done;
        }
    }
    return nfc_fct_busy;
}

// -----------------------------------------------------------------------------
/**
 * \brief reset tag access rights.
 * \retval busy or done nfc status
 * \note called by the specific fsm state *nfc_fsm_state_reset_tag_access_rights*
 * triggered by NFCResetTagAccessRights() function
 */
STATIC nfc_fct_status_t reset_tag_access_rights(void)
{
    uint8_t sector_access_rights[16] = { 0 };

    // present I2C password to change access rights (sss byte)
    I2cPresentI2cPwd(0);

    // Write all sector security status bytes from tag register address 0
    nfc_local_state.is_pending_i2c_access =
        !(I2cWriteRegister (0, sector_access_rights, 16));

    // if I2cWrite was successful ....
    if (true == nfc_local_state.is_pending_i2c_access)
    {
        return nfc_fct_done;
    }
    return nfc_fct_busy;
}

// -----------------------------------------------------------------------------
/**
 * \brief convert addresses from REAL TAG structure to TAG_COPY structure
 * \param tag_addr: address following the REAL TAG structure
 * \retval address recalculated following the TAG COPY structure
 */
STATIC uint16_t convert_tag_addr_to_tag_copy_addr(uint16_t tag_addr)
{
    // validate that Tag address is Not less than start address of first MB
    if (tag_addr < nfc_tag_addr_config.first_mpc_start_addr)
    {
        // Index out of Range prevention !
        ErrorShutdown (NFC_ERROR_TAG_COPY_ADDR_CONVERSION);
    }

    return tag_addr - nfc_tag_addr_config.first_mpc_start_addr;
}

//------------------------------------------------------------------------------
/**
 * \brief Compare two data blocks byte wise
 * \param data1_p: pointer to the first block of data
 * \param data2_p: pointer to the second block of data
 * \param byte_length: the by amount of byte to be compared
 * \return true if the data are equal, false otherwise
 */
STATIC bool is_data_equal(const void *data1_p, const void *data2_p,
                          uint8_t byte_length)
{
    uint8_t* p1;
    uint8_t* p2;
    uint8_t i1;

    p1 = (uint8_t*) data1_p;
    p2 = (uint8_t*) data2_p;
    i1 = 0;

    while ((i1 < byte_length) && (*(p1 + i1) == *(p2 + i1)))
    {
        i1++;
    }

    return (i1 == byte_length);
}

//------------------------------------------------------------------------------
/**
 * \brief set the *nfc_local_state.mb_proc_idx* to the number of the next mb
 * that have to be processed
 * \param req_vector: pointer to the 32 bit variable in which each bit set to 1
 * represent a mb to be processed
 * \note This function search the next bit set to 1 in the passed 32 bit variable
 * incrementing accordingly the *nfc_local_state.mb_proc_idx*.
 * Note that at each evaluation the 32 bit variable is modified by a right shift
 */
STATIC void set_next_requested_mb_idx(uint32_t *req_vector)
{
    // search next request LSB which is set to 1
    while ((*req_vector) && (!((*req_vector) & 1)))
    {
        // right shift request vector
        (*req_vector) >>= 1;

        // increment MB process index
        nfc_local_state.mb_proc_idx++;
    }
}

//------------------------------------------------------------------------------
/**
 * \brief increment by one the *nfc_local_state.mb_proc_idx* and right shift
 * the 32 bit variable passed as parameter.
 * \param req_vector: pointer to the 32 bit variable in which each bit set to 1
 * represent a mb to be processed
 * \note This function is called at the end of every successfull process on the
 * mbs entities(read, write or update). It's needed to avoid to process again
 * the same mb when calling the next *set_next_requested_mb_idx*
 *
 */
STATIC void increment_requested_mb_idx(uint32_t *req_vector)
{
    // right shift request vector
    (*req_vector) >>= 1;

    // increment MB process index
    nfc_local_state.mb_proc_idx++;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write the next byte on the MPC (from tag to MCU)
 * \param idx: index of the MPC to write on
 * \note Based on the *nfc_local_state.mb_entity* structure and on the index of
 * the MPC we're writing on, this function retrieve the address of the byte to
 * be written from the *nfc_tag_copy[]* array and perform the MPC writing taking
 * care of initializing the iterator if is the first writing on that MPC
 */
STATIC void mpc_write_next_byte(uint8_t idx)
{
    uint16_t tag_copy_addr;

    tag_copy_addr = convert_tag_addr_to_tag_copy_addr(
            NfcMpcTagStartAddr[idx] + nfc_local_state.mb_entity.addr);

    // when address is 0 the MpcRead iterator is initialized
    if (0 == nfc_local_state.mb_entity.addr)
    {
        // initialize iterator start address first
        // only dummy data is written
        MpcOpen(toc_items[idx].keydata_idx, 0, toc_items[idx].ch, MPC_PORT_NFC);
    }

    // write byte from TAG copy to MPC Generic
    MpcWrite(MPC_PORT_NFC, nfc_tag_copy[tag_copy_addr]);

    // increment address
    nfc_local_state.mb_entity.addr++;
}

//-----------------------------------------------------------------------------
/**
 * \brief Read the next entity on the MPC (from MCU to tag)
 * \param idx: index of the MPC to read from
 * \note This function read next entity with real data from MpcGeneric taking
 * care of initializing the iterator if is the first reading from that MPC
 */
STATIC void mpc_read_next_entity(uint8_t idx)
{
    mpc_port_t port;
    int mpc_read_return;
    uint16_t channel = (toc_items[idx].ch << 8);
    uint8_t nfc_prot_attr = toc_items[idx].mpc_keydata->attributes;

    // set protection attribute:
    // read only and r/w protected MPCs: MPC_PORT_ROOT, otherwise MPC_PORT_NFC
    port = (MPC_ATTRIB_NFC_UNPROTECTED & nfc_prot_attr) ?
            MPC_PORT_NFC : MPC_PORT_ROOT;

    // when address is 0 the MpcRead iterator is initialized
    if (0 == nfc_local_state.mb_entity.addr)
    {
        // initialize iterator start address first
        // only dummy data is returned
        // index reference for MpcRead is MpcKeydata[]!
        // channel is uint16_t -> always positive thouggh function param is int
        MpcRead(toc_items[idx].keydata_idx, channel, port, (uint8_t *) &(nfc_local_state.mb_entity.data));
    }

    // read next entity with real data from MpcGeneric and return entity length
    mpc_read_return = MpcRead(toc_items[idx].keydata_idx, -1, port,
                              (uint8_t *) &(nfc_local_state.mb_entity.data));

    // set entity length to either entity length or 0 when all entities are read out
    nfc_local_state.mb_entity.length =
            (mpc_read_return == MPC_OUT_OF_RANGE) ? 0 : mpc_read_return;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write on REAL TAG via I2C a full *nfc_local_state.mb_entity*
 * \return true if the I2cWrite was succesfull (i2c tx_fifo was not full)
 * \note This function is called only within mb_update_from_mcu_to_tag()
 * It compares the MCU data with the internal TAG_COPY, and will write to the
 * TAG only if they're different
 */
STATIC bool tag_write_entity(void)
{
    uint16_t tag_addr;
    uint16_t tag_copy_addr;
    uint8_t i1;
    uint8_t *data;
    uint8_t length;
    bool is_i2c_write_successful;

    data = (uint8_t *) &(nfc_local_state.mb_entity.data);
    length = nfc_local_state.mb_entity.length;
    tag_addr = NfcMpcTagStartAddr[nfc_local_state.mb_proc_idx]
            + nfc_local_state.mb_entity.addr;

    tag_copy_addr = convert_tag_addr_to_tag_copy_addr(tag_addr);

    // default - when no I2CWrite is required (data is equal) - return successful
    is_i2c_write_successful = true;

    // compare data with TAG copy data
    if (false == is_data_equal(data, &nfc_tag_copy[tag_copy_addr], length))
    {
        // write to TAG and return if I2CWrite is successful (= true)
        is_i2c_write_successful = I2cWrite(tag_addr, (uint8_t *) data, length);

        // only when I2cWrite was successful, update Tag copy memory.
        // It is required for data compare fail to re-trigger pending I2CWrite()
        if (true == is_i2c_write_successful)
        {
            for (i1 = 0; i1 < length; i1++)
            {
                nfc_tag_copy[tag_copy_addr] = *data;
                tag_copy_addr++;
                data++;
            }
        }
    }

    return is_i2c_write_successful;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write on the REAL TAG the CRC of the mb we are currently working
 * \return true if the I2cWrite was succesfull (i2c tx_fifo was not full)
 * \note This function is called at the end of each MB TAG writings
 * The address to which retrieve the CRC on the TAG is NOT calculated
 * from an Index of the MB passed as parameter (like *mpc_read_next_entity()*)
 * but it is calculated with the actual *mb_proc_idx* and *mb_entity.addr*
 * So we have to be sure to call this function only AT THE END of every mb
 * writings
 */
STATIC bool tag_write_mb_crc(void)
{
    uint16_t tag_addr;
    uint16_t tag_copy_addr;
    uint16_t crc_16;
    bool is_i2c_write_successful;

    tag_addr = NfcMpcTagStartAddr[nfc_local_state.mb_proc_idx]
            + nfc_local_state.mb_entity.addr;

    tag_copy_addr = convert_tag_addr_to_tag_copy_addr(tag_addr);

    crc_16 = Crc16Checksum(crc16_big_endian);

    // default - when no I2CWrite is required (data is equal) - return  successful
    is_i2c_write_successful = true;

    // compare data with TAG copy data
    if (false == is_data_equal(&crc_16, &nfc_tag_copy[tag_copy_addr], 2))
    {
        // write to TAG and return if I2CWrite is successful (= true)
        is_i2c_write_successful = I2cWrite(tag_addr, (uint8_t *) &crc_16, 2);

        // only when I2cWrite was successful update Tag copy memory
        // required for data compare fail to re-trigger pending I2CWrite()
        if (true == is_i2c_write_successful)
        {
            // write CRC-16 LSB to Tag Copy - LSB
            nfc_tag_copy[tag_copy_addr] = (0xFF & crc_16);
            // write CRC-16 MSB - to Tag Copy - MSB
            nfc_tag_copy[tag_copy_addr + 1] = (crc_16 >> 8);
        }
    }

    return is_i2c_write_successful;
}

//-----------------------------------------------------------------------------
/**
 * \brief Read from the REAL TAG the mb specified by the idx parameter
 * \param idx: the index of the requested mb
 * \return true if the i2c reading was succesfull (there are no other pending
 * readings)
 * \note The I2cRead function always reads the appended 2 byte CRC16
 * See function I2Read() for details
 */
STATIC bool i2c_read_mb(uint8_t idx)
{
    // load MB via I2C request
    // NOTE:
    return I2cRead(
            NfcMpcTagStartAddr[idx],
            &nfc_tag_copy[convert_tag_addr_to_tag_copy_addr(
                          NfcMpcTagStartAddr[idx])],
            toc_items[idx].mpc_keydata->length);
}

//-----------------------------------------------------------------------------
/**
 * \brief Release the memory lock used for on-line programming whithin the
 * handshake mechanism
 * \note Called to drop the on-line programming in case of:
 *  - crc errors when reading mbs
 *  - at the end of the *nfc_fsm_state_mb_write_to_mcu* state
 */
STATIC void unlock_tag_memory(void)
{
    // set local status flag to unlocked
    nfc_local_state.is_tag_mem_locked = false;

    // update TAG Control Register - clear URR and MLR
    nfc_local_state.tag_ctrl_register->urr = 0;
    nfc_local_state.tag_ctrl_register->mlr = 0;
    nfc_local_state.is_write_tag_control_reg_requested = true;

    // to prevent race condition for TAG Control Register Read (Fast Timer)
    nfc_local_state.fast_timer_cnt = 0;
}

//-----------------------------------------------------------------------------
/**
 * \brief Check if the complete TAG control registers is 0xFF
 * \return true if TAG has to be reinitialized, false otherwise
 */
STATIC bool handle_tag_initialization(void)
{
    // TAG initialization
    // validate if complete TAG Control Register Memory is 0xFF
    if ((0xFFFFFFFF == nfc_local_state.tag_ctrl_register->urr)
            && (0xFFFFFFFF == nfc_local_state.tag_ctrl_register->prr)
            && (0xFF == nfc_local_state.tag_ctrl_register->mlr)
            && (0xFFFF == nfc_local_state.tag_ctrl_register->crc16))
    {
        // set TAG Initialization request
        nfc_local_state.is_tag_initialization_requested = true;

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
 * \brief Recognize the pattern of a faulty NFC chip by checking the Control
 * Regiter block-wise (4 bytes at a time).
 * Since the Control Register starts at address 0x100, it happens that
 * PRR and URR fall into two individual blocks, MLR and CRC fall into one single
 * block.
 * \return true if at least one of the three blocks (4 bytes each) that compose
 * the Control Register is filled with 0xFF, false otherwise.
 */
/*
STATIC bool isCrcErrorDueToBlockCorruption(void)
{
    // TAG Control Register evaluation
    // We can recognize a Control Register corruption due to the faulty NFC chip
    // by checking if at least one of the memory block (4 bytes) is full of 0xff
    if   ((0xFFFFFFFF == nfc_local_state.tag_ctrl_register->prr)
     ||   (0xFFFFFFFF == nfc_local_state.tag_ctrl_register->urr)
     ||  ((0xFF == nfc_local_state.tag_ctrl_register->mlr)
       && (0xFFFF == nfc_local_state.tag_ctrl_register->crc16)))
    {
        return true;
    }

    return false;
}
*/
//-----------------------------------------------------------------------------
/**
 * \brief
 * Recognize if there is an MLR misalignment between fw and sw by checking the
 * Control Register right after the previous writing of the MLR
 *
 * \return
*/
STATIC bool isThereCtrlRegisterMlrMisalignment(void)
{
    if((nfc_local_state.tag_ctrl_register->prr == nfc_local_state.shadow_tag_ctrl_register.prr)
     &&   (nfc_local_state.tag_ctrl_register->urr == nfc_local_state.shadow_tag_ctrl_register.urr)
     &&   (nfc_local_state.tag_ctrl_register->mlr == ((uint8_t)(nfc_local_state.shadow_tag_ctrl_register.mlr-127))))
    {
        return true;
    }
    return false;
}

//-----------------------------------------------------------------------------
/**
 * \brief handle PRR values performing the appropriate actions
 * \param prr: the prr register
 * \return true if different from 0
 */
STATIC bool handle_tag_register_prr(uint32_t prr)
{
    uint32_t prr_little_endian;

    // Is PRR requested ....
    if (0 != prr)
    {
        // convert to little-endian and mask out to max number of MBs
        prr_little_endian = (REV_BYTE_ORDER32(prr)) & NFC_INST_REQUEST_ALL;

        // set MB read request register to PRR
        nfc_local_state.tag_req_mb_read = prr_little_endian;

        // set MB write request register
        nfc_local_state.tag_req_mb_write = nfc_local_state.tag_req_mb_read;

        // change loaded TAG PRR to little endian for further processing
        nfc_local_state.tag_ctrl_register->prr = prr_little_endian;

        // lock TAG Memory when power on condition is true
        if (true == nfc_local_state.is_power_on_condition)
        {
            nfc_local_state.is_tag_mem_locked = true;
            nfc_local_state.ignore_crc_error = false;
            nfc_local_state.is_new_data_present_at_power_on = 1;
        }
        else
        {
          nfc_local_state.is_new_data_present_at_power_on = 0;
        }

        return true;
    }
    else
    {
      nfc_local_state.is_new_data_present_at_power_on = 0;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
 * \brief handle URR values performing the appropriate actions
 * \param urr: the urr register
 * \return true if different from 0
 */
STATIC bool handle_tag_register_urr(uint32_t urr)
{
    // handle URR requests only when NOT in Power-On
    if (0 != urr)
    {
        // set MB update request register to URR
        // convert to little-endian and mask out to max number of MBs
        nfc_local_state.tag_req_mb_update =
        NFC_INST_REQUEST_ALL & (REV_BYTE_ORDER32(urr));

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
 * \brief handle MLR register performing the appropriate actions
 * \param mlr: the mlr register
 * \return true if different from 0
 */
STATIC bool handle_tag_register_mlr(uint8_t mlr)
{
    // handle MLR requests only when NOT in Power-On
    if (0 != mlr)
    {
        // set TAG Memory Locked
        nfc_local_state.is_tag_mem_locked = true;

        // set MLR response that TAG Memory is locked
        nfc_local_state.tag_ctrl_register->mlr = mlr + 127;

        // write TAG Control Register
        // clear PRR (in case of CRC error required)
        nfc_local_state.tag_ctrl_register->prr = 0;
        nfc_local_state.is_write_tag_control_reg_requested = true;

        // initialize the MLR counter for timeout in MLR handshaking
        nfc_local_state.mlr_counter = 0;

        // initialize the CRC error counter during mem lock session
        nfc_local_state.mem_lock_crc_error_counter = 0;

        // when  STATUS.Error is not 0 clear and update TAG Status Register
        if (0 != nfc_local_state.tag_status_register.error)
        {
            nfc_local_state.tag_status_register.error = 0;
            nfc_local_state.is_write_tag_status_reg_requested = true;
            // set local state to no CRC failure
            nfc_local_state.is_crc_error_read = false;
        }

        return true;
    }

    return false;
}

//-----------------------------------------------------------------------------
/**
 * \brief Initalize TAG Control Register
 * \note Initialize to 0 all the TAG control register fields. Due to this
 * initialization we need to re-write it on the TAG, so set to true the
 * correspondant flag. Reset also the fast timer to prevent from unwanted
 * readings.
 */
STATIC void tag_control_register_init(void)
{
    // Clear ALL Requests except PRR (only if no error)
    if (false == nfc_local_state.is_crc_error_read)
    {
        nfc_local_state.tag_ctrl_register->prr = 0;
    }
    nfc_local_state.tag_ctrl_register->urr = 0;
    nfc_local_state.tag_ctrl_register->mlr = 0;
    nfc_local_state.is_write_tag_control_reg_requested = true;

    // set local status flag to unlocked
    nfc_local_state.is_tag_mem_locked = false;

    // to prevent race condition for TAG Control Register Read (Fast Timer)
    nfc_local_state.fast_timer_cnt = 0;
}

//-----------------------------------------------------------------------------
/**
 * \brief Read the whole TOC register from the TAG
 * \return busy or done nfc status
 * \note This function read the whole TOC register and checks:
 * - data integrity via CRC
 * - correspondence between TAG data and internal MCU data
 */
STATIC nfc_fct_status_t tag_toc_register_read(void)
{
    uint8_t i1;
    uint8_t mpc_idx;
    uint8_t toc_field_idx;
    uint16_t tmp_adr;

    // when NO I2C RX Request is pending and I2C RX is NOT Busy
    if ((false == *(i2c_feedback_struct.is_rx_request_filled))
            & (i2c_state_busy_rx != i2c_feedback_struct.state))

    {
        // Is read TAG Device Identification Register requested ....
        if (true == nfc_local_state.is_read_tag_toc_reg_requested)
        {
            // read TAG TOC Register until I2cRead return true
            // than nfc_local_state.is_read_tag_deviceID_reg_requested is false
            nfc_local_state.is_read_tag_toc_reg_requested = !(I2cRead(
                    NFC_TOC_START_ADDR, nfc_tag_copy,
                    NFC_TAG_TOC_REGISTER_BYTE_COUNT));

            // wait until I2cRead is done
            return nfc_fct_busy;
        }

        // validate CRC16
        // if CRC is NOT correct ....
        if (false == i2c_feedback_struct.is_crc_correct)
        {
            // set TAG Initialization request
            nfc_local_state.is_tag_initialization_requested = true;

        }
        // CRC is correct
        else
        {
            // initialize the number of the mpc we're evaluating
            mpc_idx = 0;

            // initialize the field of the toc we're evaluating
            toc_field_idx = 0;

            // Data integrity checks
            for (i1 = 0; i1 < NFC_TAG_TOC_REGISTER_BYTE_COUNT; i1++)
            {
                switch (toc_field_idx)
                {
                    case 0:
                    {
                        if (nfc_tag_copy[i1]
                                != ((0x0F)
                                        & (toc_items[mpc_idx].mpc_keydata->attributes)))
                        {
                            nfc_local_state.is_tag_initialization_requested =
                                    true;
                            return nfc_fct_done;
                        }
                        break;
                    }

                    case 1:
                    {
                        if (nfc_tag_copy[i1]
                                != toc_items[mpc_idx].mpc_keydata->id)
                        {
                            nfc_local_state.is_tag_initialization_requested =
                                    true;
                            return nfc_fct_done;
                        }
                        break;
                    }

                    case 2:
                    {
                        if (nfc_tag_copy[i1]
                                != toc_items[mpc_idx].mpc_keydata->version)
                        {
                            nfc_local_state.is_tag_initialization_requested =
                                    true;
                            return nfc_fct_done;
                        }
                        break;
                    }

                    case 3:
                    {
                        if (nfc_tag_copy[i1]
                                != toc_items[mpc_idx].mpc_keydata->length)
                        {
                            nfc_local_state.is_tag_initialization_requested =
                                    true;
                            return nfc_fct_done;
                        }
                        break;
                    }

                    case 4:
                    {
                        tmp_adr = nfc_tag_copy[i1 + 1];
                        tmp_adr <<= 8;
                        tmp_adr |= nfc_tag_copy[i1];

                        if (tmp_adr
                                != REV_BYTE_ORDER16(NfcMpcTagStartAddr[mpc_idx]))
                        {
                            nfc_local_state.is_tag_initialization_requested =
                                    true;
                            return nfc_fct_done;
                        }
                        break;
                    }

                    default:
                    {
                        break;
                    }
                }

                if (++toc_field_idx >= 6)
                {
                    toc_field_idx = 0;
                    mpc_idx++;
                }
            }
        }

        // TAG Control Register Read is done
        return nfc_fct_done;
    }

    // I2C module is busy RX
    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Read TAG Device Identification Register
 * \return busy or done nfc status
 * \note This function read the whole Device Identification register and checks:
 * - data integrity via CRC
 * - correspondence between TAG data and internal MCU data
 */
STATIC nfc_fct_status_t tag_device_identification_register_read(void)
{
    uint8_t i1, i2;
    uint64_t gtin_copy;
    nfc_tag_identification_register_t *tag_reg_ident;

    // set pointer for Identification Registers to nfc_tag_mem_tmp
    tag_reg_ident = (nfc_tag_identification_register_t *) nfc_tag_mem_tmp;

    // when NO I2C RX Request is pending and I2C RX is NOT Busy
    if ((false == *(i2c_feedback_struct.is_rx_request_filled))
            & (i2c_state_busy_rx != i2c_feedback_struct.state))

    {
        // Is read TAG Device Identification Register requested ....
        if (true == nfc_local_state.is_read_tag_deviceID_reg_requested)
        {
            // read TAG Device Identification Register until I2cRead return true
            // than nfc_local_state.is_read_tag_deviceID_reg_requested is false
            nfc_local_state.is_read_tag_deviceID_reg_requested = !(I2cRead(
                    0, nfc_tag_mem_tmp, NFC_TAG_DEVICE_ID_REGISTER_BYTE_COUNT));

            // wait until I2cRead is done
            return nfc_fct_busy;
        }

        // validate CRC16
        // if CRC is NOT correct ....
        if (false == i2c_feedback_struct.is_crc_correct)
        {
            // set TAG Initialization request
            nfc_local_state.is_tag_initialization_requested = true;

        }
        // CRC is correct
        else
        {
            // if the manufacturer code has been alterated
            if (OSRAM_MANUFACTURER_CODE != tag_reg_ident->manufacturer_code)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the fw_major_version has been alterated
            else if (DEVICE_FW_VERSION_MAJOR != tag_reg_ident->fw_major_version)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the fw_minor_version has been alterated
            else if (DEVICE_FW_VERSION_MINOR != tag_reg_ident->fw_minor_version)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the hw_major_version has been alterated
            else if (((uint8_t) (Ballast_GetHW_Version() >> 8)) != tag_reg_ident->hw_major_version)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the nfc_major_version has been alterated
            else if (NFC_VERSION_MAJOR != tag_reg_ident->nfc_major_version)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the number of TOC items has been alterated
            else if (MPC_NFC_INST != tag_reg_ident->toc_items)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the Status Register Start Address has been alterated
            else if(REV_BYTE_ORDER16(nfc_tag_addr_config.status_register_start_addr) !=
                    tag_reg_ident->status_register_start_addr)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the Control Register Start Address has been alterated
            else if(REV_BYTE_ORDER16(nfc_tag_addr_config.control_register_start_addr) !=
                    tag_reg_ident->control_register_start_addr)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the Protected Area Start Address has been alterated
            else if(REV_BYTE_ORDER16(nfc_tag_addr_config.protected_memory_start_addr) !=
                    tag_reg_ident->protected_memory_start_addr)
            {
                // set TAG Initialization request
                nfc_local_state.is_tag_initialization_requested = true;
            }
            // if the GTIN has been alterated
            else
            {
                i2 = 5;
                for (i1 = 0; i1 < 6; i1++)
                {
                  gtin_copy = ((((uint64_t)THIS_DEVICE_EAN) >> ((i2--) * 8)));
                  gtin_copy &= 0xff;

                    if(tag_reg_ident->gtin[i1] != gtin_copy)
                    {
                        // set TAG Initialization request
                        nfc_local_state.is_tag_initialization_requested = true;
                    }
                }
            }
        }

        // TAG Control Register Read is done
        return nfc_fct_done;
    }

    // I2C module is busy RX
    return nfc_fct_busy;
}


//-----------------------------------------------------------------------------
/**
 * \brief Read TAG Control Register
 * \return busy or done nfc status
 * \note This function read the Control register and checks data integrity via
 * CRC. Then handle properly each fields prr, urr and mlr.
 */
STATIC nfc_fct_status_t tag_control_register_read(void)
{
    // when NO I2C RX Request is pending and I2C RX is NOT Busy
    if ((false == *(i2c_feedback_struct.is_rx_request_filled))
            & (i2c_state_busy_rx != i2c_feedback_struct.state))
    {
        // Is read TAG Control Register requested ....
        if (true == nfc_local_state.is_read_tag_control_reg_requested)
        {
            // read TAG Control Register until I2cRead return true
            // than nfc_local_state.is_read_tag_control_reg_requested is false
            nfc_local_state.is_read_tag_control_reg_requested = !(I2cRead(
                    nfc_tag_addr_config.control_register_start_addr,
                    nfc_tag_mem_tmp,
                    NFC_TAG_CONTROL_REGISTER_BYTE_COUNT));

            // wait until I2cRead is done
            return nfc_fct_busy;
        }

        // validate CRC16
        // if CRC is NOT correct ....
        if (false == i2c_feedback_struct.is_crc_correct)
        {
            // TAG initialization
            // validate if complete TAG Control Register Memory is 0xFF
            handle_tag_initialization();

            // in case of CRC error PRR content will be discarded and thus no
            // new data is available
            nfc_local_state.is_new_data_present_at_power_on = 0;

            if(false == nfc_local_state.is_ctrl_register_write_confirmed)
            {
               /* if(isCrcErrorDueToBlockCorruption())
                {
                    // Restore the Control Register content of the previous writing
                    nfc_local_state.tag_ctrl_register->prr = nfc_local_state.shadow_tag_ctrl_register.prr;
                    nfc_local_state.tag_ctrl_register->urr = nfc_local_state.shadow_tag_ctrl_register.urr;
                    nfc_local_state.tag_ctrl_register->mlr = nfc_local_state.shadow_tag_ctrl_register.mlr;

                    // Try to retrigger again the previous writing
                    nfc_local_state.is_write_tag_control_reg_requested = true;

                    // TAG Control Register Read is done
                    return nfc_fct_done;
                }*/

            }

            // when the TAG is not locked by an external tool
            if (false == nfc_local_state.is_tag_mem_locked)
            {
                // when the CRC is not valid reinitialize Tag Control Registers
                tag_control_register_init();
            }
            // when the TAG is locked by an external tool it can happen that
            // we're reading while the tool is writing, ending up with a CRC
            // error because of the partial writing by the tool.
            else
            {
                // We have to discriminate a CRC error due to block corruption
                // from a CRC error due to a reading happened in between of an
                // RF writing.

                // In case of CRC error due to a reading in between of RF
                // writings we have to read out the control registers 3 times
                // before to declare as "valid" the CRC error and reinitialize
                // the TAG control register
                if (++nfc_local_state.mem_lock_crc_error_counter <
                        MAX_CRC_ERROR_DURING_MEM_LOCK)
                {
                    nfc_local_state.is_read_tag_control_reg_requested = true;
                }
                else
                {
                    nfc_local_state.mem_lock_crc_error_counter = 0;
                    tag_control_register_init();
                }
            }
        }
        // CRC is correct
        else
        {
            if (true == nfc_local_state.is_power_on_condition)
            {
                handle_tag_register_prr(nfc_local_state.tag_ctrl_register->prr);
            }
            else
            {
                // TAG Memory is NOT locked ....
                if (false == nfc_local_state.is_tag_mem_locked)
                {
                    // handle URR register
                    if (false
                            == handle_tag_register_urr(
                                    nfc_local_state.tag_ctrl_register->urr))
                    {
                        // handle MLR register when URR is 0
                        handle_tag_register_mlr(
                                nfc_local_state.tag_ctrl_register->mlr);
                    }
                }
                // TAG Memory is locked ....
                else
                {
                    // only handle PRR if NOT in Error mode
                    if (0 == nfc_local_state.tag_status_register.error)
                    {
                        handle_tag_register_prr(
                                nfc_local_state.tag_ctrl_register->prr);

                        // When TAG is locked, after a Control Register write,
                        // in the next reading we have to counter-check if we've
                        // written exactly what we wanted.
                        //
                        // The TAG locking begins when there is the detection of
                        // MLR != 0 and ends right after the data transfer
                        // from the tag to the mcu (at the end of the  state
                        // nfc_fsm_state_mb_write_to_mcu but before the Control
                        // Register writing)
                        //
                        // This means there is only one case in which we're
                        // writing the Control Register when the TAG is locked
                        // which is the MLR answer.

                        // The MLR answer hasn't been confirmed yet
                        if(false == nfc_local_state.is_ctrl_register_write_confirmed)
                        {
                            // Check if the Control Register reading shows the
                            // MLR being the initial value of the handshaking
                            // If this is the case means that there was either
                            // a overwriting by the sw or a no reception of the
                            // written data and then the MLR reply has to be
                            // retriggered!
                            if(isThereCtrlRegisterMlrMisalignment())
                            {
                                // Restore the Control Register content of the previous writing
                                // Actually only the mlr is required to be restored
                                nfc_local_state.tag_ctrl_register->prr =
                                   nfc_local_state.shadow_tag_ctrl_register.prr;
                                nfc_local_state.tag_ctrl_register->urr =
                                   nfc_local_state.shadow_tag_ctrl_register.urr;
                                nfc_local_state.tag_ctrl_register->mlr =
                                   nfc_local_state.shadow_tag_ctrl_register.mlr;

                                // Retrigger again the Control Register writing
                                nfc_local_state.is_write_tag_control_reg_requested = true;


                                // TAG Control Register Read is done
                                return nfc_fct_done;
                            }
                            else
                            {
                               // If the reading is different from the initial
                               // MLR value then there has to be either
                               // - the valid MLR answer previously placed, or
                               // - a valid Control Register content already
                               //   placed by the sw.
                               // Infact here we are sure the CRC is correct.
                               // Then we can state that the previous writing is
                               // confirmed.

                               nfc_local_state.is_ctrl_register_write_confirmed = true;
                            }
                        }

                        // handle MLR timeout. MLR is set, but PRR = 0
                        if (nfc_local_state.mlr_counter
                                > NFC_CTRL_REGISTER_WAIT_CYCLES)
                        {
                            // reset counter
                            nfc_local_state.mlr_counter = 0;
                            // unlock tag memory
                            unlock_tag_memory();
                        }
                        nfc_local_state.mlr_counter++;

                        // if we got  PRR = 0, URR = 0 and MLR = 0 while the TAG
                        // is locked, means the external tool has re-initialized
                        // it because of its internal timeout.
                        // Then we have to unlock the tag memory, being ready
                        // to start again a new handshake, if required
                        /*
                      if((nfc_local_state.tag_ctrl_register->prr == 0)&&
                           (nfc_local_state.tag_ctrl_register->urr == 0)&&
                           (nfc_local_state.tag_ctrl_register->mlr == 0))
                        {
                            // reset counter
                            nfc_local_state.mlr_counter = 0;

                            // set local status flag to unlocked
                            nfc_local_state.is_tag_mem_locked = false;

                        }*/
                    }
                    else
                    {
                      // We have something wrong happening here...
                      nfc_local_state.mlr_counter = 0;

                    }
                }
            }
        }

        // TAG Control Register Read is done
        return nfc_fct_done;
    }

    // I2C module is busy RX
    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write TAG Control Register with its CRC
 * \return busy or done nfc status
*/
nfc_fct_status_t tag_control_register_write(void)
{
    // Initialize CRC 16
    Crc16Init (CRC16_CCITT_FALSE_START_VALUE);

    // calculate CRC16 for Device Identification
    Crc16AppendByteArray(nfc_tag_mem_tmp, NFC_TAG_CONTROL_REGISTER_BYTE_COUNT);

    // write CRC16
    nfc_local_state.tag_ctrl_register->crc16 = Crc16Checksum(crc16_big_endian);

    // Save the Control Register content in order to retrigger the same writing
    // in case of block corruption
    nfc_local_state.shadow_tag_ctrl_register.prr
                                       = nfc_local_state.tag_ctrl_register->prr;
    nfc_local_state.shadow_tag_ctrl_register.urr
                                       = nfc_local_state.tag_ctrl_register->urr;
    nfc_local_state.shadow_tag_ctrl_register.mlr
                                       = nfc_local_state.tag_ctrl_register->mlr;

    nfc_local_state.is_ctrl_register_write_confirmed = false;

    // write all TAG Control Registers including CRC
    if (true == I2cWrite(nfc_tag_addr_config.control_register_start_addr,
                        nfc_tag_mem_tmp,
                        NFC_TAG_CONTROL_REGISTER_BYTE_COUNT + 2))
    {
        return nfc_fct_done;
    }

    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write TAG Status Register with its CRC
 * \return busy or done nfc status
*/
nfc_fct_status_t tag_status_register_write(void)
{
    // Initialize CRC 16
    Crc16Init (CRC16_CCITT_FALSE_START_VALUE);

    // calculate CRC16 for Device Identification
    Crc16AppendByteArray((uint8_t *) &nfc_local_state.tag_status_register,
                         NFC_TAG_STATUS_REGISTER_BYTE_COUNT);

    // write CRC16
    nfc_local_state.tag_status_register.crc16 = Crc16Checksum(crc16_big_endian);

    // write all TAG Status Registers including CRC
    if (true == I2cWrite(nfc_tag_addr_config.status_register_start_addr,
                        (uint8_t *) &nfc_local_state.tag_status_register,
                        NFC_TAG_STATUS_REGISTER_BYTE_COUNT + 2))
    {
        return nfc_fct_done;
    }

    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Write to the MPC all the MBs specified into the
 * *nfc_local_state.tag_req_mb_write* register
 * \return busy or done nfc status
 */
STATIC nfc_fct_status_t mb_write_requested(void)
{
    uint8_t mb_length;
    uint8_t actual_byte_cnt;


    // during MB read - stop Fast Timer processing (read TAG Control Register)
    nfc_local_state.fast_timer_cnt = 0;

    // check if all MBs written to MCU (no pending requests)
    if (0 == nfc_local_state.tag_req_mb_write)
    {
        // all MBs written to MCU (no pending requests), we are done
        return nfc_fct_done;
    }

    // next requested MB Index
    // search next request LSB which is set to 1
    set_next_requested_mb_idx(&nfc_local_state.tag_req_mb_write);

    // set MB length
    mb_length = toc_items[nfc_local_state.mb_proc_idx].mpc_keydata->length;

    // initialize actual byte count
    actual_byte_cnt = (true == nfc_local_state.is_power_on_condition)?
                    255 : NFC_RUNTIME_MPC_WRITE_BYTE_CNT;

    // as long as byte address offset is less than MB length
    // AND the actual byte count is greater than 0, do...
    while ((nfc_local_state.mb_entity.addr < mb_length) && (0 < actual_byte_cnt))
    {
        // write next byte of TAG copy memory to MPC Generic
        mpc_write_next_byte(nfc_local_state.mb_proc_idx);

        // decrement byte counter of written bytes within a NfcCyclic() call
        actual_byte_cnt--;
    }

    // if completed actual MB writing, then ...
    if (nfc_local_state.mb_entity.addr >= mb_length)
    {
        // increment MB process index
        increment_requested_mb_idx(&nfc_local_state.tag_req_mb_write);

        // initialize entity address
        nfc_local_state.mb_entity.addr = 0;

        // when MB is written complete close Lock Byte for MPC
        MpcClose (MPC_PORT_NFC);
    }

    // we are not done yet
    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Read from the REAL TAG all the MBs specified into the
 * *nfc_local_state.tag_req_mb_read* register
 * \return busy or done nfc status
 */
nfc_fct_status_t mb_read_requested(void)
{
    // during MB read - stop Fast Timer processing (read TAG Control Register)
    nfc_local_state.fast_timer_cnt = 0;

    // when NO I2C RX Request is pending and I2C RX is NOT Busy
    if ((false == *(i2c_feedback_struct.is_rx_request_filled))
            & (i2c_state_busy_rx != i2c_feedback_struct.state))
    {
        if ((false == nfc_local_state.ignore_crc_error)
                && (false == i2c_feedback_struct.is_crc_correct))
        {
            // set CRC Read error flag
            nfc_local_state.is_crc_error_read = true;

            // decrement MB processing index, due to post increment required
            // Since it is post incremented at every mb read, it is supposed to
            // be at least 1 if we detect the CRC in the first mb reading.
            // The check if it is > 0 is for safe reasons if we have spurious
            // CRC errors from previous i2c reading. In this case we fall down
            // here without any mb reading, and so with the mb_proc_idx = 0.
            if(nfc_local_state.mb_proc_idx > 0)
            {
                nfc_local_state.mb_proc_idx--;
            }

            // stop loading further MBs when CRC Error occurs
            return nfc_fct_done;
        }

        // check if all MBs read (no pending requests)
        if (0 == nfc_local_state.tag_req_mb_read)
        {
            // all MBs read (no pending requests), we are done
            return nfc_fct_done;
        }

        // next requested MB Index
        // search next request LSB which is set to 1
        set_next_requested_mb_idx(&nfc_local_state.tag_req_mb_read);

        // check if MBs are requested
        if (0 != nfc_local_state.tag_req_mb_read)
        {
            // request I2C to read MB
            // when request can be processed, shift/increment to next MB
            // else raise same request again (MB request index not incremented)
            if (true == i2c_read_mb(nfc_local_state.mb_proc_idx))
            {
                // increment MB process index
                increment_requested_mb_idx(&nfc_local_state.tag_req_mb_read);
            }

            // ToDo: update request register on NFC TAG (I2CWrite())

            // not yet done - still busy
            return nfc_fct_busy;
        }
        else
        {
            // all MBs read (no pending requests), we are done
            return nfc_fct_done;
        }
    }
    return nfc_fct_busy;
}


//-----------------------------------------------------------------------------
/**
 * \brief Write to the REAL TAG all the MBs specified into the
 * *nfc_local_state.tag_req_mb_update* register
 * \return busy or done nfc status
 */
nfc_fct_status_t mb_update_from_mcu_to_tag(void)
{
    // during MB read - stop Fast Timer processing (read TAG Control Register)
    nfc_local_state.fast_timer_cnt = 0;

    // completed ? Can we write back the TAG Control register
    if (0 == nfc_local_state.tag_req_mb_update)
    {
        // done - MB update was successful
        return nfc_fct_done;
    }

    // set MB processing index to next requested MB of tag_req_mb_update
    set_next_requested_mb_idx(&nfc_local_state.tag_req_mb_update);
    // Error - out of index - hard error case - should never happen !
    if (nfc_local_state.mb_proc_idx >= MPC_NFC_INST)
    {
        // MB processing index is greater than total number of MB
        ErrorShutdown (NFC_ERROR_MB_INDEX_GREATER_THAN_MB_CNT);
    }

    // when the I2C access is Not pending
    if (false == nfc_local_state.is_pending_i2c_access)
    {
        // read next entity
        mpc_read_next_entity(nfc_local_state.mb_proc_idx);
    }

    // when read next entity return valid entity (length != 0)
    if (0 != nfc_local_state.mb_entity.length)
    {
        // write entity to Tag memory and Tag copy
        // when tag_write was not successful (return is false)
        // thus is_pending_i2c_access is set to true
        nfc_local_state.is_pending_i2c_access = !tag_write_entity();

        // when tag write was successful
        if (false == nfc_local_state.is_pending_i2c_access)
        {
            // increment entity address
            nfc_local_state.mb_entity.addr += nfc_local_state.mb_entity.length;

            // update CRC
            Crc16AppendByteArray((uint8_t *) &nfc_local_state.mb_entity.data,
                                 nfc_local_state.mb_entity.length);
        }
    }
    else
    {
        // write CRC to TAG
        nfc_local_state.is_pending_i2c_access = !tag_write_mb_crc();

        // when the I2C access is Not pending
        if (false == nfc_local_state.is_pending_i2c_access)
        {
            // increment MB index
            increment_requested_mb_idx(&nfc_local_state.tag_req_mb_update);

            // set entity address to MB start address
            nfc_local_state.mb_entity.addr = 0;

            // initialize CRC calculation
            Crc16Init (CRC16_CCITT_FALSE_START_VALUE);
        }
    }

    return nfc_fct_busy;
}

//-----------------------------------------------------------------------------
/**
 * \brief Check if the *nfc_local_state.fast_timer_cnt* has expired
 * \return true if the fast timer has expired
 * \note This function increment the *nfc_local_state.fast_timer_cnt* at every
 * call and check if it has expired. Hence it has to be called cyclically to have
 * the counter counting
 */
bool is_fast_timer_expired(void)
{
    if (nfc_local_state.fast_timer_cnt < NFC_FAST_TIMER_CNT)
    {
        nfc_local_state.fast_timer_cnt++;
        return false;
    }
    else
    {
        nfc_local_state.fast_timer_cnt = 0;
        return true;
    }
}

//-----------------------------------------------------------------------------
/**
 * \brief Check if the *nfc_local_state.slow_timer_cnt* has expired
 * \return true if the slow timer has expired
 * \note This function increment the *nfc_local_state.slow_timer_cnt* at every
 * call and check if it has expired. Hence it has to be called cyclically to have
 * the counter counting
 */
bool is_slow_timer_expired(void)
{
    if (nfc_local_state.slow_timer_cnt < NFC_SLOW_TIMER_CNT)
    {
        nfc_local_state.slow_timer_cnt++;
        return false;
    }
    else
    {
        nfc_local_state.slow_timer_cnt = 0;
        return true;
    }
}

//-----------------------------------------------------------------------------
// global functions
//-----------------------------------------------------------------------------



// -----------------------------------------------------------------------------
/**
 * \brief   assignment of MPC start addresses in NFC tag
 * \param   mpc_keydata_ptr: pointer to keydata array
 * \param   mpc_count: number of included MPC
 * \param   nfc_inst: number of included MPC instances
 *
 */
void nfc_mpc_start_addr_create(mpc_keydata_t const * const * mpc_keydata_ptr,
                               uint8_t mpc_count, uint8_t nfc_inst)
{
    uint8_t i, j, t, mpc_inst;
    uint16_t nfc_last_user_mem_addr = 0;
    uint8_t idx = 0;
    uint16_t tag_copy_byte_size;
    uint32_t next_free_temp;

    nfc_init_arrays(nfc_inst);

    // start positions for the tag areas to which place the MBs.
    // this address can vary depending on the tag layout configuration chosen
    // by the system integrator
    uint16_t next_free[3] =
    {
      // first mpc (located in read-only memory)
      nfc_tag_addr_config.first_mpc_start_addr,

      // start address of non-protected MPCs
      nfc_tag_addr_config.control_register_start_addr + NFC_CTRL_REG_LENGTH,

      // start address of protected memory
      nfc_tag_addr_config.protected_memory_start_addr
    };

    // for each mpc
    for (i = 0; i < mpc_count; i++)
    {
        // set to mpc_inst the number of istances required by the mpc
        mpc_inst = mpc_keydata_ptr[i]->instances;
        if (mpc_inst == 0)
        {
            mpc_inst = 1;
        }

        // Set the variable "t" depending on the attribute specified by the
        // field in the descriptor
        #ifdef NFC_CONFIG_UNPROTECTED_LAYOUT // Deprecated, only for transition phase
            t = 1;
            if (mpc_keydata_ptr[i]->attributes & MPC_ATTRIB_NFC_READONLY)
            {
                t = 0;
            }
            if (mpc_keydata_ptr[i]->attributes & MPC_ATTRIB_NFC_PROTECTED)
            {
                t = 2;
            }
        #else
            t = 2;
            if (mpc_keydata_ptr[i]->attributes & MPC_ATTRIB_NFC_READONLY)
            {
                t = 0;
            }
            if (mpc_keydata_ptr[i]->attributes & MPC_ATTRIB_NFC_UNPROTECTED)
            {
                t = 1;
            }
        #endif

        // for each instance of the mpc
        for (j = 0; j < mpc_inst; j++)
        {
            // assign next free address in NFC tag start address array
            NfcMpcTagStartAddr[idx] = next_free[t];

            // assign keydata pointer and MPC + NFC index mapping information
            // to struct array element
            toc_items[idx].mpc_keydata = mpc_keydata_ptr[i];
            toc_items[idx].keydata_idx = i;
            toc_items[idx].ch = j;

            // actual start address + payload length + CRC16 length
            next_free_temp = next_free[t] + mpc_keydata_ptr[i]->length
                    + NFC_CRC_LENGTH;

            // alignment required? add 4 and mask first 2 bits
            // otherwise assign directly
            if (next_free_temp & 0x000003)
            {
                // + alignment compensation + 0xFFFC (4 byte alignment)
                next_free[t] = (next_free_temp + 4) & 0xFFFFFC;
            }
            else
            {
                next_free[t] = next_free_temp;
            }

            // check max address and max length
            if (next_free[t] > nfc_last_user_mem_addr)
            {
                nfc_last_user_mem_addr = next_free[t];
            }

            idx++;

            // check if idx is higher than nfc_inst
            if (idx > nfc_inst)
            {
                ErrorShutdown(NFC_ERROR_IDX1);
            }
        }
    }

    // check if idx is lower than nfc_inst
    if (idx != nfc_inst)
    {
        ErrorShutdown(NFC_ERROR_IDX2);
    }

    // check that the First Mpc start address has to be placed AFTER the TOC end
    if (nfc_tag_addr_config.first_mpc_start_addr < \
       (NFC_TOC_START_ADDR + NFC_TAG_TOC_REGISTER_BYTE_COUNT + NFC_CRC_LENGTH))
    {
        ErrorShutdown(NFC_ERROR_TOC);
    }

    // check that the last data placed in the read-only sector is not
    // overlapping the Status Register
    if (next_free[0] >= nfc_tag_addr_config.status_register_start_addr)
    {
        ErrorShutdown(NFC_ERROR_RO);
    }

    if (next_free[1] >= nfc_tag_addr_config.protected_memory_start_addr)
    {
        ErrorShutdown(NFC_ERROR_PROT);
    }

    if (next_free[2] >= NFC_OEM_MEMORY_START_ADDR)
    {
        ErrorShutdown(NFC_ERROR_OEM);
    }

    // Calculate tag copy byte size
    tag_copy_byte_size = nfc_last_user_mem_addr
            - nfc_tag_addr_config.first_mpc_start_addr;

    // Check that is lower than the reserved area: throw an error otherwise
    if (tag_copy_byte_size > NFC_TAG_COPY_BYTE_SIZE)
    {
        ErrorShutdown(NFC_ERROR_OOM);
    }
}


// -----------------------------------------------------------------------------
/**
 * \brief NFC module init function which should be called after power-up
 */
void NfcInit(void)
{
    // set initial FSM state
    nfc_local_state.fsm_state = nfc_fsm_state_idle;

    // initialize power on condition to true
    nfc_local_state.is_power_on_condition = true;

    // set is TAG Memory Locked to false
    nfc_local_state.is_tag_mem_locked = false;

    // set is TAG initialization requested to false
    nfc_local_state.is_tag_initialization_requested = false;

    // set tag access rights reset to false
    nfc_local_state.is_tag_reset_access_rights_requested = false;

    // set is write TAG Control Register requested to false
    nfc_local_state.is_write_tag_control_reg_requested = false;

    // initialize MB write TAG request vector
    nfc_local_state.tag_req_mb_write = 0;

    // initialize MB read TAG request vector (set bit for each MB)
    nfc_local_state.tag_req_mb_read = NFC_INST_REQUEST_ALL;

    // initialize MB update Tag request vector (set bit for each MB)
    nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

    // set pointer for TAG Control Registers to nfc_tag_mem_tmp
    nfc_local_state.tag_ctrl_register =
            (nfc_tag_control_register_t *) nfc_tag_mem_tmp;

    // initialize fast timer to 0
    nfc_local_state.fast_timer_cnt = 0;

    // initialize slow timer to 0
    nfc_local_state.slow_timer_cnt = 0;

    // set TAG Status Register to ECG on
    nfc_local_state.tag_status_register.ecg_on = NfcServices_GetEcgOnValue(NFC_ON);

    // set TAG Status Register to NO Error
    nfc_local_state.tag_status_register.error = 0;

    // set TAG Status Register to Reserved to 0
    nfc_local_state.tag_status_register.reserved = 0;

    // request to write Status Register to set status to ECG on
    nfc_local_state.is_write_tag_status_reg_requested = true;

    // set local state to no CRC failure
    nfc_local_state.is_crc_error_read = false;

    // set to ignore CRC failure (power-up)
    nfc_local_state.ignore_crc_error = true;

    // initialize MPC Change Counter
    nfc_local_state.mpc_change_counter = MpcGetChangeCounter();

    // initialize MLR counter
    nfc_local_state.mlr_counter = 0;

    // initialize access right init index
    nfc_local_state.access_right_init_idx = 0;

    // nvm write cycles == 0 -> MCU has just been flashed
    if (nvmGetWriteCycles() == 0)
    {

        nfc_local_state.is_tag_initialization_requested = true;
        tag_control_register_init();
    }
    else
    {
        // set fast timer to expired to force immediately TAG Control reg read
        nfc_local_state.fast_timer_cnt = NFC_FAST_TIMER_CNT;

        // set the flag to check the correctness of Device Identification reg
        nfc_local_state.is_read_tag_deviceID_reg_requested = true;

        // set the flag to check the correctness of TOC reg
        nfc_local_state.is_read_tag_toc_reg_requested = true;
    }

    nfc_local_state.is_wakeup_from_standby = false;

    // set the is_new_data_present_at_power_on flag to the "unknown" state
    nfc_local_state.is_new_data_present_at_power_on = -1;

    // set MPC start addresses in NFC tag
    nfc_mpc_start_addr_create(MpcKeydata, MPC_COUNT, MPC_NFC_INST);

    // initialize I2C
    I2cInit();
}

//-----------------------------------------------------------------------------
/**
 * \brief Reset the rights access
 */
void NfcResetTagAccessRights(void)
{
    nfc_local_state.is_tag_reset_access_rights_requested = true;
}

//-----------------------------------------------------------------------------
/**
 * \brief Switch off the Nfc Module if required
 * \note Note that the I2c module will not be stopped, to let the eventually
 * ongoing TAG writings take place without corruptions
 */
void NfcSwitchOff(void)
{
  nfc_local_state.fsm_state = nfc_fsm_state_suspended;
}
//-----------------------------------------------------------------------------
/**
 * \brief Switch on the Nfc Module after a switch off
 * (device is already running)
 */
void NfcSwitchOn(void)
{
  NfcRuntimeInit();
}

//-----------------------------------------------------------------------------
/**
 * \brief Set sleep mode for NFC module
 */
void NfcSetSleep(void)
{
    nfc_local_state.set_sleep_cnt = 0;

    // set TAG Status Register
    nfc_local_state.tag_status_register.ecg_on = NfcServices_GetEcgOnValue(NFC_STANDBY);

    nfc_local_state.tag_status_register.error = 0;

    nfc_local_state.is_write_tag_status_reg_requested = true;

    // write TAG Status Register
    while(true == nfc_local_state.is_write_tag_status_reg_requested) // TODO: what if never true? NFC defect, device off?
    {
        // process I2cCyclic
        NfcCyclic();
        nfc_local_state.set_sleep_cnt++;

        if (nfc_local_state.set_sleep_cnt > NFC_SET_SLEEP_WAIT_CYCLES)
        {
            nfc_local_state.is_write_tag_status_reg_requested = false;
        }
    }

}

//-----------------------------------------------------------------------------
/**
 * \brief Set wake up from sleep mode for NFC module
 */
void NfcSetWakeup(void)
{
    // don't set initial FSM state - we don't know if initialization was ongoing

    // initialize power on condition to true
    nfc_local_state.is_power_on_condition = true;

    // set is TAG Memory Locked to false
    nfc_local_state.is_tag_mem_locked = false;

    // reset MPC index to prevent overflow (required!)
    nfc_local_state.mb_proc_idx = 0;

    // initialize MB read TAG request vector (set bit for each MB)
    nfc_local_state.tag_req_mb_read = NFC_INST_REQUEST_ALL;

    // initialize MB update Tag request vector (set bit for each MB)
    nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

    // set fast timer to expired to force immediately TAG Control register read
    nfc_local_state.fast_timer_cnt = NFC_FAST_TIMER_CNT; // reads and processes PRR

    // initialize slow timer to 0
    nfc_local_state.slow_timer_cnt = 0;

    // set TAG Status Register to ECG on
    nfc_local_state.tag_status_register.ecg_on = NfcServices_GetEcgOnValue(NFC_ON);

    // request to write Status Register to set status to ECG on
    nfc_local_state.is_write_tag_status_reg_requested = true;

    // tell the state machine to update Tag if MPC was changed during sleep (otherwise it is assumed that we had a line-switch on and the counter is reset in the state machine
    nfc_local_state.is_wakeup_from_standby = true;

}


//-----------------------------------------------------------------------------
/**
 * \brief Save all the latest data into the TAG before the power down
 * \note This function needs to be called when the ECG is switched off.
 * - set TAG STATUS Register to ECG off
 * - initialize TAG Status Error Register
 * - update all MPCs
 * - initialize TAG Status Register
 *
 */
void NfcOnPowerDown(nfc_power_down_use_cases useCase)
{
    // discard all I2C Transfer and empty FIFO
    I2cDiscardAllTransfers();

    // during power down we're calling more frequently the I2cCyclic() function
    // than we need to increase the wait time between a succesfull writing and
    // the first attempt for the next
    switch (useCase)
    {
        case nfc_power_down:
        {
            I2cSetCallTimings(M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_POWERDOWN);

            // set TAG Status Register
            nfc_local_state.tag_status_register.ecg_on =
                                            NfcServices_GetEcgOnValue(NFC_OFF);

            break;
        }
        case nfc_sleep_mode:
        {
            I2cSetCallTimings(M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_SLEEPMODE);

            // set TAG Status Register
            nfc_local_state.tag_status_register.ecg_on =
                                        NfcServices_GetEcgOnValue(NFC_STANDBY);

            break;
        }
        case nfc_what_ever:
        {
            break;
        }
    }




    nfc_local_state.tag_status_register.error = 0;

    // switch on NFC again if it was switched off during runtime due to an error
    if (nfc_local_state.fsm_state == nfc_fsm_state_suspended)
    {
      // set error flag in status register that tag was re-initialized!
      nfc_local_state.tag_status_register.error = NFC_STATUS_REGISTER_ERROR_I2C_STOPPED;

      // NfcSwitch on triggers a re-initialization
      NfcSwitchOn();
      while ((nfc_fct_done != initialize_tag_device_identification())
      &&(i2c_feedback_struct.state != i2c_state_error))
      {
          // process I2cCyclic
          I2cCyclic();
          NfcWatchdogResetRequest();
          pwrDownSleep();
      }

      nfc_local_state.mb_proc_idx = 0;

      while ((nfc_fct_done != initialize_tag_toc())
      &&(i2c_feedback_struct.state != i2c_state_error))
      {
          // process I2cCyclic
          I2cCyclic();
          NfcWatchdogResetRequest();
          pwrDownSleep();
      }
    }


    // write TAG Status Register
    while ((nfc_fct_done != tag_status_register_write())
      &&(i2c_feedback_struct.state != i2c_state_error))
    {
        // process I2cCyclic
        I2cCyclic();
        NfcWatchdogResetRequest();
        pwrDownSleep();
    }

    // set Request to Update all MPCs
    nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

    // set MB process index to 0
    nfc_local_state.mb_proc_idx = 0;

    // set local MPC Entity structure start address
    nfc_local_state.mb_entity.addr = 0;

    // set to false, no request triggered
    nfc_local_state.is_pending_i2c_access = false;

    // initialize CRC calculation
    Crc16Init (CRC16_CCITT_FALSE_START_VALUE);

    // update all MPCs
    while ((nfc_fct_done != mb_update_from_mcu_to_tag())
         &&(i2c_feedback_struct.state != i2c_state_error))
    {
        // process I2cCyclic
        I2cCyclic();
        NfcWatchdogResetRequest();
        pwrDownSleep();
    }

    // set TAG Control Register
    nfc_local_state.tag_ctrl_register->urr = 0;
    nfc_local_state.tag_ctrl_register->prr = 0;
    nfc_local_state.tag_ctrl_register->mlr = 0;

    // write TAG Control Register
    while ((nfc_fct_done != tag_control_register_write())
         &&(i2c_feedback_struct.state != i2c_state_error))
    {
        // process I2cCyclic
        I2cCyclic();
        NfcWatchdogResetRequest();
        pwrDownSleep();
    }

    while (!I2cAreAllPendingTransfersDone())
    {
        // process only a predetermined fix amount of writings
        I2cFlush();
        NfcWatchdogResetRequest();
    }

    // set back the regular wait time after the while loop just for safe reasons
    // (e.g.: if you're not powering down really but entering into the sleep
    // state you want back the regular waiting time when you wake-up
    I2cSetCallTimings(M24LRxx_PAGE_WRITE_WAIT_TIME_TICKS_RUNTIME);
}

//-----------------------------------------------------------------------------
/** \brief Get the information whether new data was programmed in the NFC tag
 * during \b power \b off.
 *
 * The NFC module needs a certain time to read the complete NFC tag. This time
 * normally is too long for a normal startup and too long and too unsteady to
 * fulfill e.g. the DALI requirements.
 *
 * Therefore it's important to know \b early in the startup process, whether
 * the ballast was (re-)programmed during the last powerdown (offline programming).
 * For example, a new lower current set value is programmed (in the tag only,
 * but not yet available for the application) and starting with the previous one
 * might be dangerous for the LED module.
 *
 * This function is to provide the information about possibly new data as early
 * as possible to the application in the statup process.
 * - When no offline programming happend (no new data is available),
 *   the startup can be done normally and all timing requirements can be met.
 * - When there is new data, the application is supposed not to start the light
 *   and not to do other important things before the complete NFC tag is read
 *   (see also \ref NfcInitStatus()).
 *
 * \returns
 *        - -1 information still not available
 *        -  0 no new data present at power on
 *        -  1 new data present at power on
 */
int8_t NfcIsNewDataPresentAtPowerOn(void)
{
  // For "NFC3.0", consider to consolidate this function with NfcInitStatus()
  // OTT-611
  return nfc_local_state.is_new_data_present_at_power_on;
}


//-----------------------------------------------------------------------------
/**
 * \brief Get the information if the eventually power on data are available
 * \return true when the NFC initialization has finished AND
 * there are new data programmed in the TAG during switch OFF
 */
bool NfcIsPowerOnTagDataAvailable(void)
{
  // if we're still in the power on condition data is not available
  if(nfc_local_state.is_power_on_condition == true)
  {
      return false;
  }
  else
  {
    // otherwise we will return true only if the PRR was set
    if(nfc_local_state.is_new_data_present_at_power_on == 1)
    {
      return true;
    }
    else
    {
      return false;
    }
  }
}


//-----------------------------------------------------------------------------
/**
 * \brief  Returns the NFC tag initialization progress
 *
 * Purpose of this function is to inform the application, when all NFC and MPC
 * operations needed for a correct ballast startup are finshed.
 *
 * The ballast should wait for this event, until it proceeds to switch
 * on the light or it does other severe things like a software triggered "reset".
 * This is important especially when an \b offline \b programming  was done
 * during the last power-off cycle (see also \ref NfcIsNewDataPresentAtPowerOn()).
 *
 * If the NFC startup sequence is not finalized before the ballast is reset,
 * is switched off again or it starts the normal operation too early, it might
 * happen:
 * - The ballast starts in the complete wrong (old) operation mode
 *   (switchable ballasts as Outdoor or Tunable White DT6-DT8)
 * - The output current is wrong, a LED module is overloaded
 * - The MSK authentifications are lost, before all related MPCs
 *   programmings are processed.
 *
 * \warning As mentioned, during run-up after offline programming also do not
 *          \b software-reset the ballast before status FINISHED is reached.
 *
 * \returns
 *       - UNDEFINED if conditions are ambiguous
 *       - IN_PROGRESS if NFC initialization is still in progress
 *       - FINISHED if conditions both for mb update and I2C data transfer
 *       are fulfilled
 *
 */
nfc_init_status_t NfcInitStatus(void)
{
  // The initialization process is intended to be finished when the
  // *mb_update_from_mcu_to_tag* has been performed and all pending
  // I2C transfers are finished, WL 2018-06: ?
  nfc_init_status_t init_status = UNDEFINED;

  if ( (!(nfc_local_state.is_power_on_condition)) && (I2cAreAllPendingTransfersDone()))

  {
    init_status = FINISHED;
  }

  if ( (nfc_local_state.is_power_on_condition) && (!(I2cAreAllPendingTransfersDone())))

  {
    init_status = IN_PROGRESS;
  }

  return init_status;
}


//-----------------------------------------------------------------------------
/**
 * \brief Get the information if this is the FIRST power UP after firmware flash
 * \return true if this is the FIRST power UP after firmware flash
 */
bool NfcInitialPowerUp(void)
{
  if((nvmGetWriteCycles() == 0) && (nfc_local_state.is_power_on_condition == 1))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
/**
 * \brief NFC main method,
 * \note This function has to be called cyclically from main application.
 */
void NfcCyclic(void)
{
    bool is_trigger_mpc_change_counter;
    bool is_trigger_slow_timer;
    
    static uint8_t save=0;
    if(save)
    {
          nvmWriteAll();
      save=0;
      //if(I2cAreAllPendingTransfersDone()&&nfc_local_state.fsm_state==nfc_fsm_state_idle)
      {
        NfcOnPowerDown(nfc_run_time);
      }
      
      WDT->SRV = 0xAFFEAFFEUL;   //os3_wdt_force_sys_reset();
      
    }
    
    // evaluate if MPC Change Counter is triggered (changed)
    is_trigger_mpc_change_counter = MpcGetChangeCounter()
            != nfc_local_state.mpc_change_counter;

    is_trigger_slow_timer = is_slow_timer_expired();

    if (i2c_state_stop == i2c_feedback_struct.state)
    {
        nfc_local_state.fsm_state = nfc_fsm_state_suspended;
        nfc_local_state.is_power_on_condition = false;
    }


    switch (nfc_local_state.fsm_state)
    {
        case nfc_fsm_state_suspended:

          // in thie suspended state we disable both the fast and slow timers
          nfc_local_state.fast_timer_cnt = 0;
          nfc_local_state.slow_timer_cnt = 0;

          // if the suspension timer has expired we try to trigger an nfc_module
          // initialization and go back to the idle state right after that
   /*       if(true == is_suspension_timer_expired())
          {
            NfcRuntimeInit();
          }*/
        break;

        case nfc_fsm_state_idle:

            nfc_local_state.mpc_change_counter_triggered =
                    is_trigger_mpc_change_counter;
            nfc_local_state.slow_timer_expired = is_trigger_slow_timer;

            // handle fast timer (highest priority task)
            if (true == is_fast_timer_expired())
            {
                // request to read TAG Control Register
                nfc_local_state.is_read_tag_control_reg_requested = true;

                // load TAG Control Register
                nfc_local_state.fsm_state =
                        nfc_fsm_state_tag_control_register_read;
            }
            else if (true == nfc_local_state.is_read_tag_deviceID_reg_requested)
            {
                // read the DeviceID register content and check the CRC
                nfc_local_state.fsm_state =
                        nfc_fsm_state_device_identification_register_read;
            }
            else if (false != nfc_local_state.is_tag_initialization_requested)
            {
                // initialize TAG
                nfc_local_state.fsm_state =
                        nfc_fsm_state_initialize_tag_device_identification;
            }
            else if (true == nfc_local_state.is_read_tag_toc_reg_requested)
            {
                // set MB process index to 0
                nfc_local_state.mb_proc_idx = 0;

                // initialize MB entity address
                nfc_local_state.mb_entity.addr = 0;

                // read the DeviceID register content and check the CRC
                nfc_local_state.fsm_state = nfc_fsm_state_toc_register_read;
            }
            else if (true == nfc_local_state.is_write_tag_status_reg_requested)
            {
                // Write TAG Status Register
                nfc_local_state.fsm_state =
                        nfc_fsm_state_tag_status_register_write;
            }

            // read = read from TAG
            else if (0 != nfc_local_state.tag_req_mb_read)
            {
                // set MB process index to 0
                nfc_local_state.mb_proc_idx = 0;

                // load all requested MBs
                nfc_local_state.fsm_state = nfc_fsm_state_mb_read_from_tag;
            }

            // write = write to MpcGeneric
            else if ((0 != nfc_local_state.tag_req_mb_write)
                    && (true == nfc_local_state.is_tag_mem_locked))
            {
                // set MB process index to 0
                nfc_local_state.mb_proc_idx = 0;

                // initialize MB entity address
                nfc_local_state.mb_entity.addr = 0;

                // load all requested MBs
                nfc_local_state.fsm_state = nfc_fsm_state_mb_write_to_mcu;
            }

            // update = write to TAG only when TAG is NOT locked
            // AND
            // (MB update is requested or MPC Change Counter is changed
            // or Slow Timer is expired)
            else if ((false == nfc_local_state.is_tag_mem_locked)
                    && ((0 != nfc_local_state.tag_req_mb_update)
                            || (true == is_trigger_mpc_change_counter)
                            || (true == is_trigger_slow_timer)))
            {
                if ((true == is_trigger_mpc_change_counter)
                        || (true == is_trigger_slow_timer))
                {
                    // Request to Update All MPCs
                    nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

                    // update local copy of MPC Change Counter actual value
                    nfc_local_state.mpc_change_counter = MpcGetChangeCounter();
                }

                // set MB process index to 0
                nfc_local_state.mb_proc_idx = 0;

                // set local MPC Entity structure start address
                nfc_local_state.mb_entity.addr = 0;

                // set to false, no request triggered
                nfc_local_state.is_pending_i2c_access = false;

                // initialize CRC calculation
                Crc16Init (CRC16_CCITT_FALSE_START_VALUE);

                // update all requested MBs into TAG
                nfc_local_state.fsm_state = nfc_fsm_state_mb_update_from_mcu_to_tag;
            }

            else if (true == nfc_local_state.is_write_tag_control_reg_requested)
            {
                // Write TAG Control Register
                nfc_local_state.fsm_state = nfc_fsm_state_tag_control_register_write;
            }

            else if (true == nfc_local_state.is_tag_reset_access_rights_requested)
            {
                // reset TAG access rights
                nfc_local_state.fsm_state = nfc_fsm_state_reset_tag_access_rights;
            }

        break;

        case nfc_fsm_state_mb_read_from_tag:
            if (nfc_fct_done == mb_read_requested())
            {
                // only if NOT ignore CRC Error
                if (false == nfc_local_state.ignore_crc_error)
                {
                    // set TAG Register PRR - clear all request flags of MBs read
                    NFC_CLEAR_REQUESTS_BELOW_IDX(
                            nfc_local_state.tag_ctrl_register->prr,
                            nfc_local_state.mb_proc_idx);
                    nfc_local_state.tag_ctrl_register->prr = REV_BYTE_ORDER32(
                            nfc_local_state.tag_ctrl_register->prr);

                    // clear all write requests for MBs which could not be loaded
                    NFC_CLEAR_REQUESTS_ABOVE_IDX(
                            nfc_local_state.tag_req_mb_write,
                            nfc_local_state.mb_proc_idx);

                    // clear all pending MB read request in case of error
                    nfc_local_state.tag_req_mb_read = 0;

                    // if the CRC error flag is set
                    if (true == nfc_local_state.is_crc_error_read)
                    {
                        // set STATUS.Error to NFC_STATUS_REGISTER_ERROR_PRR
                        nfc_local_state.tag_status_register.error |=
                                NFC_STATUS_REGISTER_ERROR_PRR;

                        // set TAG Status Register write request
                        nfc_local_state.is_write_tag_status_reg_requested = true;

                        // only for the case that mb_write is 0 due to CRC Error
                        // thus FSM state nfc_fsm_state_mb_write_to_mcu is not entered !
                        // unlock Tag Memory here, in all other cases after write
                        if (0 == nfc_local_state.tag_req_mb_write)
                        {
                            // always unlock TAG memory
                            unlock_tag_memory();

                            // always update all MBs
                            nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

                            // for Power on Condition ....
                            if (true == nfc_local_state.is_power_on_condition)
                            {
                                // request to load all MBs required to keep TAG Copy in sync
                                nfc_local_state.tag_req_mb_read = NFC_INST_REQUEST_ALL;

                                // set to ignore CRC failure, due to load is only required for TAG sync
                                nfc_local_state.ignore_crc_error = true;
                            }
                        }
                    }

                }
                else
                {
                    // disable ignore CRC failure always - default
                    nfc_local_state.ignore_crc_error = false;
                }

                // all requested MBs are loaded change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;
        case nfc_fsm_state_mb_write_to_mcu:
            if (nfc_fct_done == mb_write_requested())
            {
                // for Power on Condition ....
                if (true == nfc_local_state.is_power_on_condition)
                {
                    // request to load all MBs required to keep TAG Copy in sync
                    nfc_local_state.tag_req_mb_read = NFC_INST_REQUEST_ALL;

                    // set to ignore CRC failure, due to load is only required for TAG sync
                    nfc_local_state.ignore_crc_error = true;

                    // set the power on condition to false
 //                   nfc_local_state.is_power_on_condition = false;
                }

                // always unlock TAG memory after write is done
                unlock_tag_memory();

                // always update all MBs
                nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

                // all requested MBs are loaded, change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;
        case nfc_fsm_state_mb_update_from_mcu_to_tag:
            if (nfc_fct_done == mb_update_from_mcu_to_tag())
            {
                // set the power on condition to false
                nfc_local_state.is_power_on_condition = false;

                // always initialize TAG Control Register (clear ALL Requests)
                tag_control_register_init();

                // reset Slow Timer
                nfc_local_state.slow_timer_cnt = 0;

                // all requested MBs are loaded change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_tag_status_register_write:
            if (nfc_fct_done == tag_status_register_write())
            {

                // disable TAG Status Register write request
                nfc_local_state.is_write_tag_status_reg_requested = false;

                // update local copy of MPC Change Counter actual value
                // required here to update to not generate trigger when
                // NFC writes to MpcGeneric
                // The is_wakeup_from_standby switch has to be on
                if( false == nfc_local_state.is_wakeup_from_standby )
                {
                    nfc_local_state.mpc_change_counter = MpcGetChangeCounter();
                }
                else
                {
                    nfc_local_state.is_wakeup_from_standby = false;
                }

                // all TAG Status Register written, change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_device_identification_register_read:
            if (nfc_fct_done == tag_device_identification_register_read())
            {
                // disable device identification register read request
                nfc_local_state.is_read_tag_deviceID_reg_requested = false;

                // all device identification registers data are read:
                // change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_toc_register_read:
            if (nfc_fct_done == tag_toc_register_read())
            {
                // disable device identification register read request
                nfc_local_state.is_read_tag_toc_reg_requested = false;

                // all device identification registers data are read:
                // change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_tag_control_register_read:
            if (nfc_fct_done == tag_control_register_read())
            {
                // disable TAG Control Register read request
                nfc_local_state.is_read_tag_control_reg_requested = false;

                // all TAG Control Register loaded, change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_tag_control_register_write:
            if (nfc_fct_done == tag_control_register_write())
            {

                // disable TAG Control Register write request
                nfc_local_state.is_write_tag_control_reg_requested = false;

                // all TAG Control Register written, change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;
            }
        break;

        case nfc_fsm_state_initialize_tag_device_identification:
            if (nfc_fct_done == initialize_tag_device_identification())
            {
                // TAG Device Identification initialized,
                // change to Initialize TAG TOC
                nfc_local_state.fsm_state = nfc_fsm_state_initialize_tag_toc;

                // set MB process index to 0
                nfc_local_state.mb_proc_idx = 0;

                // set local MPC Entity structure start address
                nfc_local_state.mb_entity.addr = 0;

                // set to false, no request triggered
                nfc_local_state.is_pending_i2c_access = false;

                // so no need to re-check that toc again once the initialization
                // will be completed
                nfc_local_state.is_read_tag_toc_reg_requested = false;
            }
        break;

        case nfc_fsm_state_initialize_tag_toc:
            if (nfc_fct_done == initialize_tag_toc())
            {
                // all TAG TOC initialized, change FSM state to
                nfc_local_state.fsm_state =
                        nfc_fsm_state_initialize_tag_access_rights;

            }
        break;
        case nfc_fsm_state_initialize_tag_access_rights:
            if (nfc_fct_done == initialize_tag_access_rights())
            {
                // disable TAG initialization
                nfc_local_state.is_tag_initialization_requested = false;

                // tag initialized, change FSM state to Idle
                nfc_local_state.fsm_state = nfc_fsm_state_idle;

            }
        break;
        case nfc_fsm_state_reset_tag_access_rights:
            if (nfc_fct_done == reset_tag_access_rights())
            {
                // all TAG TOC initialized, change FSM state to Idle
                nfc_local_state.is_tag_reset_access_rights_requested = false;
                nfc_local_state.fsm_state = nfc_fsm_state_idle;

            }
        break;

        default:
        break;
    }

    // call I2C Cyclic function
    I2cCyclic();
}


/**
 * \brief NFC module init function
 * \note THis function is intended to be called at runtime
 */
void NfcRuntimeInit(void)
{
    // set initial FSM state
    nfc_local_state.fsm_state = nfc_fsm_state_idle;

    // initialize power on condition to true
    nfc_local_state.is_power_on_condition = false;

    // don't read TAG Control Register Read
    nfc_local_state.fast_timer_cnt = 0;

    // initialize slow timer to 0 -> update request is done below
    nfc_local_state.slow_timer_cnt = 0;

    // set is write TAG Control Register requested to false
    nfc_local_state.is_write_tag_control_reg_requested = false;

    // request to write Status Register to set status to ECG on
    nfc_local_state.is_write_tag_status_reg_requested = true;

    // don't write from tag to MCU
    nfc_local_state.tag_req_mb_write = 0;

    // initialize MB update Tag request vector (set bit for each MB)
    nfc_local_state.tag_req_mb_update = NFC_INST_REQUEST_ALL;

    // don't read from tag!
    nfc_local_state.tag_req_mb_read = 0;

    // set local state to no CRC failure
    nfc_local_state.is_crc_error_read = false;

    // CRC error checks
    nfc_local_state.ignore_crc_error = false;

    // initialize MLR counter
    nfc_local_state.mlr_counter = 0;

    // set local status flag to unlocked
    nfc_local_state.is_tag_mem_locked = false;

    // Clear ALL Requests
    nfc_local_state.tag_ctrl_register->prr = 0;
    nfc_local_state.tag_ctrl_register->urr = 0;
    nfc_local_state.tag_ctrl_register->mlr = 0;
    nfc_local_state.is_write_tag_control_reg_requested = true;

//    // start a new initialization!
//    nfc_local_state.is_tag_initialization_requested = true;

    // initialize I2C
    I2cInit();
}
