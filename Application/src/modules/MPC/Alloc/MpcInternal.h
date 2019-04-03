// ---------------------------------------------------------------------------------------------------------------------
// Microcontroller Toolbox - Firmware Framework for Full Digital SSL Ballasts
// ---------------------------------------------------------------------------------------------------------------------
// (c) Osram GmbH
//     DS D EC - MCC
//     Parkring 33
//     85748 Garching
//
//
// The content of this file is intellectual property of OSRAM GmbH. It is
// confidential and not intended for any public release. All rights reserved.
//
//
// Initial version: 2016-06, W.Limmer@osram.de
//
// Change History:
//
// $Author: m.nordhausen $
// $Revision: 9844 $
// $Date: 2017-09-19 15:26:38 +0800 (Tue, 19 Sep 2017) $
// $Id: MpcInternal.h 9844 2017-09-19 07:26:38Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcInternal.h $
//

/** \addtogroup MPC Multi Purpose Centers
 * \{
 *
 * \file
 * \brief Mpc Interface between a implementation and the generic routing layer
 *
 * This is mainly
 * - the data structurs a Mpc module needs to handle and serve
 * - the macros to build the tables for data entities
 */

/*****************************************************
 * Order of needed includes before:
  #include <stddef.h> or #include "global.h" which incl. stddef.h
  #include "Mpc.h"
 ****************************************************/

#ifndef __MPC_IMPLEMENTATION_H
#define __MPC_IMPLEMENTATION_H
   
#include "Mpc.h"

#ifndef BYTEORDER_LITTLE_ENDIAN
STATIC_ASSERT(0, "MPC interface only tested for little endian machines")
#endif


/** \brief Struct for the data exchange between Mpc Generic and a MPC implementation.
 *
 * Used by Mpc Generic (the Mpc router) to organize the access of the Mpc's payload including the
 * data individual permissions.
 *
 * Beside this, a reasonable part of the data fields is dedicated to the iterator access for read() and write()
 * of the Mpcs to store the state and result of last access.
 *
 * Mainly used by the Mpcs table access functions, but also by the Mpc's read() / write() (out of range callbacks),
 * when tables aren't doing the \b complete job as well as by the legacy layer handling the exisiting Mpcs.
 *
 * Further documentation is given at the single data fields to explain the mechanism.
 */
typedef struct {
 /** \brief Port number of the interface using the buffer.
  *
  * Preset by the generic function before this struct is passed to the individual read()/write() functions of a Mpc.
  *
  * Possibly needed by a Mpc to figure out the correct access permissions, which maybe dependent on the port
  * from where the access is done.
  */
  mpc_port_t    port;

 /** \brief Mpc number using the buffer.
  *
  * Preset by the generic function before this struct is passed to the individual read()/write() functions of a Mpc.
  *
  * Needed by some Mpcs which might be present more times (e.g. the legacy password page): Then, this
  * field is to give a part of the "address information" to them.
  */
  uint8_t       num;

 /** \brief Channel number served by the last read/write() function call.
  *
  * Set by the
  * - \b Generic read() function after atomic read of a data to prepare a possible iterator read() in the next step.
  *   NOT to be touched by the Mpc implementations.
  * - \b Generic write() function after data is written to prepare a possible iterator write() in the next step.
  *   NOT to be touched by the Mpc implementations.
  */
  int8_t        channel;

/** \brief Address of last read/write() function call.
  *
  * Set by the
  * - \b Generic read() function after atomic read of a data to prepare a possible iterator read() in the next step.
  *   NOT to be touched by the Mpc implementations.
  * - \b Generic write() function after data is written to prepare a possible iterator write() in the next step.
  *   NOT to be touched by the Mpc implementations.
  */
  uint8_t       adr;

  /** \brief Length of the data in the buffer.
   *
   * Set by the
   * - Generic Read- or WriteTable() functions in the manner described by the other enmumerated use cases just below.
   * - read() function of an Mpc implementations after atomic read of a data: \n
   *   Describing the size of the data accessed and therefore the amount of data now available in the buffer.
   * - write() function of an Mpc: \n
   *   Describing the size of data affected by the address. All data bytes have to be provided before the
   *   Mpc write() is able to do the atomic write (after/when the last byte is provided).
   */
   uint8_t       length;

 /** \brief Buffer for atomic read/write of a multibyte data.
  *
  * Also used for single bytes access to keep the architecture clean.
  */
  union {
    uint8_t     buffer[4];      ///< Used to put read/down single bytes
    uint32_t    uint32;         ///< Used for atomic int32 access
  };

 /** \brief Permissions needed to access a data.
  *
  * <b> Preset to zero </b> by the MpcGeneric read() wrapper function before every call of an Mpc
  * read()-function implementation.
  *
  * Set by the read()-function of a Mpc implementation when special permission(s) to read a specific data are
  * needed. This set can be done globally within a Mpc (at entry, before the typical switch-cases-statement or
  * individually for single entries/addresses).
  *
  * The Mpc itself has \b NOT to duty to check the permissions: It's sufficient to fill this field. The check is done
  * and evaluated by the wrapper function (MpcGeneric) before it delivers real data to the application.
  *
  * Unused for write()-functions, where the permissions are to be handled locally in the implementations.
  */
  uint32_t      perm;

 /** \brief Fallback (return) value if permissions for read access are denied.
  *
  * Preset by the routing function to \b MPC_FALLBACK before this struct is passed to the individual reads() of a Mpc.
  *
  * Might be overwritten by the read() function to other values. Most typical this would be 0xFF as defined for
  * many DALI membank entries.
  *
  * Unused for write()-functions.
  */
  uint32_t      fallback;
} mpc_io_buffer_t;


// ---------------------------------------------------------------------------------------------------------------------
// Typedefs and macros for the data entities (data points) table

typedef enum {
  MPC_TYP_RAW = 0,                  ///< Raw data access: unstructured area up to 32 bytes without multi-byte latching
  MPC_TYP_CON,                      ///< Read a constant directly from the uint32_t constants table
  MPC_TYP_VAR,                      ///< Read/write from/to a memory location specified by the uint32_t constants table
  MPC_TYP_FUN                       ///< Call a function specified by the uint32_t constants table to read/write a data
} mpc_data_typ_enum;


/// \todo As we know, bitfields are compiler dependant.
///       With IAR, they may be even 6 bytes wide. Moreover, it's not sufficient to restrict to 32 bits max,
///       also the order of the bits is important. The compiler might optimize single fields to a byte sacrificing
///       headroom and going to use more bytes than needed.
///       CONSIDER to use hand-made bitfields (shift) again!
typedef struct {
  mpc_data_typ_enum type    :2;   ///< Type of a data entity, see \ref mpc_data_typ_enum
  uint16_t          len     :5;   ///< Length of the data entity in bytes
  uint16_t          perm_ri :4;   ///< Location of the read permissions in the uint32 constants table (tab index)
  uint16_t          perm_wi :4;   ///< Location of the write permissions in the uint32 constants table (tab index)
  uint16_t          fb_idx  :4;   ///< Index in the uint32_t constants table for the fallback value
  uint16_t          data_ri :7;   ///< Index in the uint32_t constants table for the constant value or
                                  ///< the address of the variable or read function
                                  ///< (write function has to follow in the next slot of the constants array)
  uint16_t          multch_i:4;   ///< Index in the uint32_t constants table for the size of the struct
                                  ///< (adder for multi-channel access)
} mpc_data_entity_t;


// Define a macro to be able to switch struct/bitfield to a bitmask later
#define MPC_ENTITY(T, L, PR, PW, FBI, DRI, CHI)  { .type     = (T),   \
                                                   .len      = (L),   \
                                                   .perm_ri  = (PR),  \
                                                   .perm_wi  = (PW),  \
                                                   .fb_idx   = (FBI), \
                                                   .data_ri  = (DRI), \
                                                   .multch_i = (CHI)  \
                                                 }

// ---------------------------------------------------------------------------------------------------------------------
// Typedef and macros for the layout tab

typedef struct {                  // take uint16_t as base, otherwise the bitfiled will have uint32_t as width ...
  uint16_t          pos    :5;    ///< Position of byte in the buffer (or in the struct when unbuffered r/w applies)
  uint16_t          first  :1;    ///< Flag for atomic read of a complete word into the buffer \n
                                  ///< Done after permissions check and only for MPC_TYP_CON, MPC_TYP_VAR and MPC_TYP_FUN
  uint16_t          last   :1;    ///< Flag for atomic write of the buffer into a variable / pass as arg to a function \n
                                  ///< Done after permissions check and only for MPC_TYP_VAR and MPC_TYP_FUN
  uint16_t          spare  :1;    ///< Curently spare; possible expansions: memory class / NFC yes/no / ...
  uint16_t          entity :8;    ///< Number of the data entity in mpcTabTemplateEntities[]
} mpc_layout_t;


#ifdef BYTEORDER_LITTLE_ENDIAN  // this table for sure is dependant on the byte order of the machine
 #define MPC_LAYOUT_NA        {.entity = 255}

 #define MPC_LAYOUT8(e)       {.pos = 0, .first = 1, .last = 1, .entity = (e),}

 #define MPC_LAYOUT16_MSB(e)  {.pos = 1, .first = 1, .last = 0, .entity = (e),}
 #define MPC_LAYOUT16_LSB(e)  {.pos = 0, .first = 0, .last = 1, .entity = (e),}

 #define MPC_LAYOUT24_MSB(e)  {.pos = 2, .first = 1, .last = 0, .entity = (e),}
 #define MPC_LAYOUT24_MID(e)  {.pos = 1, .first = 0, .last = 0, .entity = (e),}
 #define MPC_LAYOUT24_LSB(e)  {.pos = 0, .first = 0, .last = 1, .entity = (e),}

 #define MPC_LAYOUT32_MSB(e)  {.pos = 3, .first = 1, .last = 0, .entity = (e),}
 #define MPC_LAYOUT32_M2SB(e) {.pos = 2, .first = 0, .last = 0, .entity = (e),}
 #define MPC_LAYOUT32_L2SB(e) {.pos = 1, .first = 0, .last = 0, .entity = (e),}
 #define MPC_LAYOUT32_LSB(e)  {.pos = 0, .first = 0, .last = 1, .entity = (e),}
#endif



// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
/** \brief Struct for meta data and buffer for atomic multibyte access of an MPC
 *
 * MpcRead()/Write() functions lay down their results in the struct mpc_io_buffer_t. This enum (positive values)
 * is to indicate where the relevant byte can be found/is to placed by the wrapper function.

   ----    outdated   - to be removed soon   ----

 *
 * To be used together with mpc_returncodes_enum (negative values), which flag possible issues during access.
 */
typedef enum {
  MPC_IO_BYTE         =  0,     ///< Position of a single byte (signed or unsigned)

  MPC_IO_I16_MSB      =  1,     ///< Most significant byte of a 16 bit integer (signed or unsigned)
  MPC_IO_I16_LSB      =  0,     ///< Least significant byte of a 16 bit integer (signed or unsigned)

  MPC_IO_I32_MSB      =  3,     ///< Most significant byte of a 32 bit integer (signed or unsigned)
  MPC_IO_I32_M2SB     =  2,     ///< 2nd most significant byte of a 32 bit integer (signed or unsigned)
  MPC_IO_I32_L2SB     =  1,     ///< 2nd least significant byte of a 32 bit integer (signed or unsigned)
  MPC_IO_I32_LSB      =  0      ///< Least significant byte of a 32 bit integer (signed or unsigned)
} mpc_bytelocation_enum;


//    ----    outdated   - to be removed soon   ----
#ifdef BYTEORDER_LITTLE_ENDIAN
#define MPC_BYTE_POS(len, adr) ((len) - 1 - (adr)) // when a data is 4 byte long and MSB is ment: it's at pos 3 = 4 - 1 - 0
#endif

#endif // #ifdef __MPC_IMPLEMENTATION_H

/** \} */
