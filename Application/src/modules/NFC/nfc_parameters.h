
#ifndef __NFC_PARAMETERS_H__
#define __NFC_PARAMETERS_H__
/** ----------------------------------------------------------------------------
// nfc_parameters.h
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
// $Revision: 13860 $
// $Date: 2018-06-19 21:45:44 +0800 (Tue, 19 Jun 2018) $
// $Id: nfc_parameters.h 13860 2018-06-19 13:45:44Z j.eisenberg $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nfc/tags/v2.7/Src/nfc_parameters.h $

*   \file
*   \brief Local header file for the whole NFC Module
*/

/** NFC start address of TOC */
#define NFC_TOC_START_ADDR                                                  0x14

/** NFC start address of OEM memory, not reachable by NFC module  */
#define NFC_OEM_MEMORY_START_ADDR                                          0x700

/** NFC start address of TOC */
#define NFC_TAG_LAST_ADDR                                                  0x7FF

/** NFC sector length */
#define NFC_TAG_SECTOR_LENGTH                                               0x80

/** NFC control register length */
#define NFC_CTRL_REG_LENGTH                                                 0x20

/** NFC status register prr error. Set if MB write from µC to tag failed */
#define NFC_STATUS_REGISTER_ERROR_PRR                                        0x1

/** NFC status register I2C error. Set if bus is stopped due to disturbed 
I2C during runtime. This flag is set at power down when I2C is switched on again*/
#define NFC_STATUS_REGISTER_ERROR_I2C_STOPPED                                0x2

/** This parameter defines the maximum number of bytes written from the
 * NFC TAG Copy to the MPC Generic, to avoid a too high system load */
#define NFC_RUNTIME_MPC_WRITE_BYTE_CNT                                         2

/** NFC Fast Timer period given as cycle counts of function NfcCyclic() */
#define NFC_FAST_TIMER_CNT                                                   1200//300

/** NFC Slow Timer period given as cycle counts of function NfcCyclic()  //2k4 Hz 
 * set to 1h = 3600s * 2400 */
#define NFC_SLOW_TIMER_CNT                                              8640000

/** number of NFC cycles tag register remains set and waits for SW response
 * timeout and TAG CTRL reset after: 40 * 250 ms = 10 sec */
#define NFC_CTRL_REGISTER_WAIT_CYCLES                                         40

/** CRC length in NFC tag */
#define NFC_CRC_LENGTH                                                         2

#define NFC_SET_SLEEP_WAIT_CYCLES                                            125

#endif // __NFC_PARAMETERS_H__
