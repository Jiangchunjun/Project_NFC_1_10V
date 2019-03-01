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
// $Author: w.limmer $
// $Revision: 11321 $
// $Date: 2017-12-21 00:58:53 +0800 (Thu, 21 Dec 2017) $
// $Id: Mpc.c 11321 2017-12-20 16:58:53Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/Mpc.c $
//

/** \defgroup MPC Multi Purpose Centers
 * \{
 * \file
 * \brief MPC Generic Interface
 *
 * This file implements the generic interface to access all Mpc standard functions. The functions
 * are intended to be called by the application.
 *
 */


#include <stdio.h>              // Debug prints

#define MODULE_MPC
#include "Global.h"
#include "Config.h"

#include "Acl.h"                // Access control layer

#include "Mpc.h"                // Generic application interface
#include "MpcInternal.h"        // Internal interface to the Mpc implementations
#include "MpcAllocation.h"      // Generic application interface


// change defined value to 1 to switch on debug print
// Always pass the debug code to the compiler, otherwise it will break sooner or later see
// http://stackoverflow.com/questions/1644868/c-define-macro-for-debug-printing incl. arguments for "do ... while(0)"
#define DEBUGPRINT_MPC
// #define DEBUGPRINT(...)     do { if (  DEBUGPRINT_MPC + 0 ) { printf(__VA_ARGS__); fflush(stdout); }; } while (0)
#define DEBUGPRINT(...)     do { if (  DEBUGPRINT_MPC + 0 ) { printf(__VA_ARGS__); }; } while (0) // without fflush()


/** \brief Data buffer for atomic multi-byte access on Mpcs
 */
static mpc_io_buffer_t   mpcIoBuffer[MPC_PORTS_FIRST_UNUSED];

/** \brief Dummy data buffer for Mpc handling.
 *
 * Used as default (before asignment of the right one to avoid in any case at least pointer errors.
 */
static mpc_io_buffer_t   mpcDummyIoBuffer;

/** \brief Centralized DALI Lock bytes.
 *
 * One generically managed array of DALI lock bytes for every Mpc. Existent only for regular DALI membanks,
 * but not for artificial or special ones (DALI internal state for NFC, debug Mpc, ...)
 */
static uint8_t           mpcDaliLockbytes[MPC_MAX_NUMBER + 1];

/** \brief Counter to detect changes by Write
 *
 * will be increased by MpcWrite().
 */
static uint32_t changeCounter = 0;

#if ((!defined(MPC_PASSWORD1_NUMBER) && !defined(MPC_PASSWORD2_NUMBER)) || defined(UNIT_TESTING))
static int pwd_retval = 0;
#endif

// ---------------------------------------------------------------------------------------------------------------------
// Some legacy stuff
#if defined(MPC_PASSWORD1_NUMBER) || defined(MPC_PASSWORD2_NUMBER)
  // -------------------------------------------------------------------------------------------------------------------
  /** \brief Checks whether a Mpc is protected by a password page.
  *
  * Checks, whether a Mpc in total is protected by a password page and whether the password is set.
  * Useful mainly for legacy reasons, the new concept supports a more fine-granular approach.
  *
  * \note Move to ACL sooner or later.
  *
  * \param mpcdesc   Pointer to the descriptor of the Mpc.
  *
  * \retval false when write access is allowed,
  *         true when write is not allowed (= password protection for the page is active AND password is NOT given).
  */
  static inline int mpcIsPasswordProtected(const mpc_descriptor_t * mpcdesc) {
    bool MemoryBank_Password_GetLock(uint8_t bankIndex);  // Brute force, no include; Don't create a legacy dependency
    int passwordLocked = false;

    if (mpcdesc->keydata.attributes & MPC_ATTRIB_PASSWORD1_PROT) {
      passwordLocked = MemoryBank_Password_GetLock(0);
    }
    else if (mpcdesc->keydata.attributes & MPC_ATTRIB_PASSWORD2_PROT) {
      passwordLocked = MemoryBank_Password_GetLock(1);
    }
    return passwordLocked;
    }
#else
  // Empty fake function for platforms without legacy password pages
  static inline int mpcIsPasswordProtected(const mpc_descriptor_t * mpcdesc) {
    (void) mpcdesc;
    return pwd_retval;
  }
#endif

// ---------------------------------------------------------------------------------------------------------------------
 /** \brief Returns either permissions from \ref mpc_keydata_t "keydata.permissions", if present or from
 *   \ref mpc_keydata_t "keydata.mult_perm4legacy", a permissions array, linked to the Descriptor.
 *
 *  \param  mpcdesc pointer to descriptor of MPC
 *  \param  mpcIo   pointer to buffer of MPC
 *  \retval permission or 0 if no permission available
 *
 */
static uint32_t mpcGetLegacyPermissions(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo) {

  if(mpcdesc->keydata.permissions != 0) {
    mpcIo->perm = mpcdesc->keydata.permissions;
  }
  else {
    if (mpcdesc->keydata.mult_perm4legacy != NULL) {
      if (mpcIo->adr < mpcdesc->keydata.mult_perm4legacy[0])
      {
        // inc adr by 1, due to lastadress at 0
        mpcIo->perm = (1L << (mpcdesc->keydata.mult_perm4legacy[mpcIo->adr+1]));
      } else {
        mpcIo->perm = 0;
      }
    } else {
      mpcIo->perm = 0;
    }

  }
  return mpcIo->perm;
}

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Forward a command to a Mpc.
 *
 * Provide a pure routing of commands, no extra logic is part of this function. Especially DALI specific things
 * like to do reset command only when the lock byte is fine or to reset all Mpcs when bank 0 is addressed,
 * are \b NOT implemented.
 *  \param  mpc_nr Number of MPC or Membank, depending on port
 *  \param  cmd Number of command to perform
    \param port Port number of the port to allocate the Mpc
    \param channel NUmber of Channel
 *  \retval MPC_OK every thing ok
 *  \retval MPC_DENY pointer to init, reset or cmd function is null
 *  \retval MPC_OUT_OF_RANGE pointer to descriptor is null
 *  \retval other returnvalue of command, if special
 *  See also \ref mpc_returncodes_enum
 */
mpc_returncodes_enum MpcCmd (uint8_t item_nr, mpc_cmd_t cmd, mpc_port_t port, uint8_t channel) {

  const mpc_descriptor_t * mpcDesc = MpcDescriptorGet(MpcDissolveMapping(item_nr, port));

  if (mpcDesc) {
    if (mpcDesc->tables_len >= 0) {
      if (mpcDesc->Cmd) {
        return mpcDesc->Cmd(cmd, channel, port);
      }
    }
    else {
      switch(cmd) {
      case MPC_CMD_INIT:
        if (mpcDesc->Init) {
          mpcDesc->Init();
          return MPC_OK;
        }
        break; // for fallback when pointers aren't defined
      case MPC_CMD_RESET:
        if (mpcDesc->Reset) {
          mpcDesc->Reset(channel);
          return MPC_OK;
        }
        break; // for fallback when pointers aren't defined
      default:
        break; // just to avoid a compiler warning, not all possibilities are defined in legacy pages (LCOV_EXCL_LINE)
      }
    }
    return MPC_DENY;
  }
  // Fallback, also done when a expected function pointer is not present
  return MPC_OUT_OF_RANGE;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Init all Mpcs in the sequence of occurence in the table.
 */
void MpcInit (void) {
  // MNO MSK is already initialized. So get init. twice. Start at 1, or Start at 1, if MSK is defined?
  for (int mpc_nr = 0; mpc_nr < MPC_COUNT; mpc_nr++) {
    MpcCmd(mpc_nr, MPC_CMD_INIT, MPC_PORT_ROOT, 0);
  }
  for (int membank_nr = 0; membank_nr <= MPC_MAX_NUMBER; membank_nr++) {
    mpcDaliLockbytes[membank_nr] = 255;
  }
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Return a pointer to the keydata of a Mpc.
 *
 * \param mpc_nr  Mpc number to allocate (Mpc number or DALI membank number dep. on port)
 * \param port    Port number of the port to allocate the Mpc.
 *
 * \retval const mpc_keydata_t * to the keydata struct, NULL when Mpc (no further Mpc) does exist.
 */
// Test Coverage Exception: due to usage in NFC only, VirtECG uses DALI only [29.05.17]
const mpc_keydata_t * MpcKeydataGet (int16_t item_nr, mpc_port_t port) {
  STATIC_ASSERT(offsetof(mpc_descriptor_t, keydata) == 0, "Major issue with mpc internal struct typedefs");

  return (const mpc_keydata_t *) MpcDescriptorGet(MpcDissolveMapping(item_nr, port));
}

// ---------------------------------------------------------------------------------------------------------------------
/** \brief The entry point for DALI library to the new Mpc interface for read access.
 *
 * Assembles Mpc keydata and Mpc payload to one linear address room as requested by DALI. Also integrate the new
 * ACL (Access Control Layer) with the DALI lock byte (0x55 at address 2) in a generic way as expected by
 * DALI and without the support from the individual Mpc implementations.
 *
 * \retval mpc_returncodes_enum according to \ref MpcRead() function
 */
mpc_returncodes_enum MpcReadDali(uint8_t membank_nr, int16_t adr, uint8_t ch, uint8_t *buffer) {

  const mpc_descriptor_t * mpcDesc = MpcDescriptorGet(MpcDissolveMapping(membank_nr, MPC_PORT_DALI));


  DEBUGPRINT ("\n MpcReadDali : nr=%d, adr=%d, ch=%d", membank_nr, adr, ch);
  if (mpcDesc) {
    // Password-Lock influences also the reading of the DALI lock byte:
    // If a page is protected, DALI lock byte always reads as 0; if not, this routine falls back to normal reading
    if (adr == 2) {
      if (mpcIsPasswordProtected(mpcDesc)) {
        *buffer = 0;
        return (mpc_returncodes_enum) 1;    // one valid byte
      }
    }

    if (mpcDesc->tables_len < 0) {
      // Legacy pages provide keydata by their own
      // No differentiation indoor/outdoor (translation) needed with GetValue():
      // MPC_RW_OUT_OF_RANGE  == 0 (for indoor) <=> false (for outdoor)
      // MPC_RW_OK            == 1 (for indoor) <=> true  (for outdoor)
      // MPC_RW_DENY          == 2 : Can't happen with read
      return (mpc_returncodes_enum)(mpcDesc->GetValue)(adr, buffer, ch, MPC_PORT_DALI);
    }
    else {
      if (adr < mpcDesc->keydata.payload_off) {
        int r = 0;
        switch (adr) {
        case 0:
          r =  mpcDesc->keydata.length + mpcDesc->keydata.payload_off - 1;
          break;
        case 1:
          r = 0;          // checksum is not supported in DaliEdition2, "indicator byte" is 0
          break;
        case 2:
          if (membank_nr <= MPC_MAX_NUMBER) {   // needed, if(mpcDesc){} hasn't done the full job due to the "pseudo" Mpcs
            r = mpcDaliLockbytes[membank_nr];
          }
          else {
            // nearly unreachable
            return MPC_OUT_OF_RANGE; // LCOV_EXCL_LINE
          }
          break;
        case 3:
          r = mpcDesc->keydata.id;
          break;
        case 4:
          r = mpcDesc->keydata.version;
          break;
        }
        *buffer = r;
        return (mpc_returncodes_enum) 1;    // one valid byte
      }
      else {
        return MpcRead(membank_nr, (ch << 8) + adr - mpcDesc->keydata.payload_off, MPC_PORT_DALI, buffer);
      }
    }
  }
  // fall-through
  return MPC_OUT_OF_RANGE;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief A wrapper for legacy MpcXxx_GetValue() function to map to the new interface.
 *
 * Called by MpcRead() when we have a legacy implementation without tables.
 *
 * \warning If a legacy Mpc needs multi-port access, this multi-port support has to be provided by the Mpc itself.
 *          The old interface doesn't allow another solution, as no multi-byte information is passed out of the
 *          legacy membanks.
 */
static int MpcReadLegacy(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
  int               retval = MPC_RW_DENY;
  uint8_t           tempbyte;
  mpc_rw_enum       legacy_state;

  if (mpcdesc) {
    DEBUGPRINT ("\n MpcReadLegacy : nr=%d, adr=%d", mpcIo->num, adr);

    mpcIo->length = 1;

    legacy_state  = (mpcdesc->GetValue)(adr + mpcdesc->keydata.payload_off, &tempbyte, channel, mpcIo->port);
    // legacy_state  = (mpcdesc->GetValue)(adr, &tempbyte, channel, mpcIo->port);
    mpcIo->uint32 = tempbyte; // mpcIo.buffer[] might be used directly, but this is more safe

    switch (legacy_state) {
    case MPC_RW_OK:
      retval = 0; // Single byte is at position 0
      break;
    case MPC_RW_OUT_OF_RANGE:
      retval = MPC_OUT_OF_RANGE;
      break;
      case MPC_RW_DENY:
    default:  // avoid 'possibly uninitialized' warning down in MpcRead()
      retval = MPC_DENY;
      break;
    }

    DEBUGPRINT (" return byte %3d (byte pos/err code %d)", mpcIo->uint32, retval);
  }
  return retval;
};


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Mpc table read function.
 *
 * This function evaluates the tables of a Mpc implementation and provides an atomic multi-byte and multi-channel
 * access to the data entities (data points) decribed by the tables: It moreover supports a light-weight
 * endianness translation by its return value.
 *
 * It operates on a mpc_io_buffer_t structure passed as argument to support also a multi-port operation.
 *
 * The concept relies on a <b> successive access of all bytes of a multi-byte data entity</b>.
 * This is, that it is \b only possible to read a byte of a multi-byte structure, when the byte at the lowest
 * address of the data entity (normally the MSB) is accessed first.
 *
 * This is, because all bytes of a multi-byte entity are read at once into a buffer when the first byte is accessed
 * and successive accesses to the same data entity only read from that buffered value.
 *
 * This is the only reasonable way to create a multi-byte + multi-port interface for volatile data.
 * The DALI standard supports this by the auto-increment of the address pointer to read a membank as well as by
 * encouraging this buffering for multi-byte values in the membank descriptions (DALI 2.0)
 *
 * For more details and for a detailed interface description pls. refer to Mpc.h/.c.
 *
 * \param mpcdesc Pointer to the Mpc descriptor (tables and functions of the Mpc)
 * \param mpcIo   Pointer to a mpc_io_buffer_t to work with
 * \param adr     Byte address of the value to read
 * \param channel Channel of the address to read
 *
 * \retval        Position of the valid byte within the buffer or MPC_OUT_OF_RANGE in case of.
 */
int MpcReadTable(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel) {
  const mpc_data_entity_t * ep = NULL;
  const mpc_layout_t      * lp;
  mpc_data_typ_enum         etype;
  int   byteposition;

  DEBUGPRINT("\n MpcReadTable  : Tab len=%d, adr=%3d, ch=%3d", mpcdesc->keydata.length, adr, channel);

  if (adr < mpcdesc->tables_len) {
    lp = &mpcdesc->layout[adr];
    if (lp->entity != 255) { // if (lp && ...) is obsolete, as lp is a always available address in the layout array
      ep = &mpcdesc->entities[lp->entity];
    }
  }

  if (ep == NULL) {
    DEBUGPRINT("\n MpcReadTable  : Out of range");
    return MPC_OUT_OF_RANGE;
  }

  DEBUGPRINT("\n MpcReadTable  : Typ=0x%02x, val_ix=%3d (0x%08x), perm_ix=%d, fallback_ix=%d",
             ep->type, ep->data_ri, mpcdesc->constants[ep->data_ri], ep->perm_ri, ep->fb_idx);

  // Fetch (read) permissions if specified
  if (ep->perm_ri) {
    mpcIo->perm = mpcdesc->constants[ep->perm_ri];
  }

  // Fetch fallback if specified
  if (ep->fb_idx) {
    mpcIo->fallback = mpcdesc->constants[ep->fb_idx];
  }

  // Fill buffer length (always specified)
  mpcIo->length = ep->len;
  byteposition  = lp->pos;

  // Fetch value if first byte of a entity is read
  if (lp->first) {
    etype = ep->type;

    if (etype == MPC_TYP_CON) {
      mpcIo->uint32 = mpcdesc->constants[ep->data_ri];
    }
    else {
      void * p = (void*)mpcdesc->constants[ep->data_ri];
      if (p == NULL) {
        return MPC_OUT_OF_RANGE;
      }
      else if (etype == MPC_TYP_VAR) {
        if (channel && ep->multch_i) { // Do this only for variables, function get the channel as a parameter anyway
          p = (void*) ((size_t)p + mpcdesc->constants[ep->multch_i]);
        }
        if (mpcIo->length == 1) {
          mpcIo->uint32 = *(uint8_t*)p;   // Needed to avoid un-aligned word access -> bus error on Cortex M0
        }
        else if (mpcIo->length == 2) {
          mpcIo->uint32 = *(uint16_t*)p;  // Needed to avoid un-aligned word access -> bus error on Cortex M0
        }
        else if (mpcIo->length >= 3) {
          mpcIo->uint32 = *(uint32_t*)p;
        };
      }
      else if (etype == MPC_TYP_FUN) {
        // Typecast p to a function pointer taking the same arguments as this function.
        // For little endian machines it's possible to have only one case (uint32_t) ...
        mpcIo->uint32 = ((uint32_t (*)(mpc_io_buffer_t *, uint16_t, uint8_t)) p) (mpcIo, adr, channel);
      };
    }
  }

  DEBUGPRINT("\n MpcReadTable  : retval=%3d", byteposition);
  return byteposition;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Generic Read() of Mpc payload data.
 *
 * A generic interface to read the \b payload of a Mpc. This payload is only the Mpc specific data and
 * does \b not contain keydata or any port specific header bytes (e.g. length, ID and version for Osram Mpcs,
 * checksum at adr 1 for DALI, ...).
 *
 * \note
 *  - The exisitence and keydata of a Mpc (len, id and such, see above) can be retrieved by \ref MpcKeydataGet().
 *  - For DALI, the function \ref MpcReadDali() provides also key data fields as needed and moreover
 *    integrates to the new ACL (Access Control Layer) with legacy DALI lock byte (0x55 at address 3).
 *
 * To provide this generic read functionality, this function implements the routing to the single Mpc
 * implementations by using the allocation tables provided by MpcAllocation.c/.h.
 *
 * ### Usage
 *
 * There are 2 possibilities to use this function:
 *
 * 1. <b>Single-byte access</b>
 *
 *    The traditional way to read the data. This mode is enabled when a
 *    full and valid address (Mpc number, channel and byte address) is provided.
 *
 *    Even if this operation mode works byte-wise, it offers an <b> atomic access to volatile multi-byte </b>
 *    data entities. Therefore the reader have \b not to care about (otherwise) possible issues with unmatched
 *    low- and highbytes or such.
 *
 *    This atomic multi-<b>byte</b> access moreover is \"multi-<b>port</b> safe\": That is, that a read on the
 *    NFC port won't disrupt any reads on other ports like DALI and vice versa.
 *
 *    This operation mode is also required to set-up the second (multi-byte iterator) operation mode.
 *
 * 2. <b>Iterator mode (multi-byte)</b>
 *
 *    This mode reads one possibly multi-byte data point (data entity) completely at a time.
 *
 *    This mode is enabled when the int16_t channel/address parameter is -1 and therefore does not specify a
 *    specific address. Prior to this, the first, traditional operation mode is required to be used to set-up the
 *    starting address and other meta-data for this iterator behavior.
 *
 *    When this was done, the next call to this function with a negative address will return the next data entity
 *    in the buffer (with correct endianness) and will return the number of valid bytes in the buffer (as in all
 *    other operation modes). Additionally it adjusts internal data in a way, that the next call will proceed
 *    like before: Returning the next data entity.
 *
 *    When all data entities of a Mpc and one channel are processed, the return value is MPC_OUT_OF_RANGE
 *    to indicate the loop outside, that the Mpc is done.
 *
 *    \warning For multi-byte opertion a data buffer of 32 bytes has to be provided for future expansion.
 *             Currently max. 4 bytes are returned at one time.
 *
 * .
 * \param mpc_nr  uint8_t: Number of the Mpc to be read
 *
 * \param ch_adr  .
 *                - int16_t >= 0: combining the address (LSB) and the channel (MSB, for multi-output ballasts)
 *                - -1: Specify iterator access
 *
 * \param port    mpc_port_t: Specifies the "reader" of the Mpc. Accesses on different ports don't disturb each
 *                other
 *
 * \param buffer  uint8_t*: Pointer to the buffer for the read data (currently 4 bytes minimum, planned 32 bytes)
 *
 * \retval Either:
 * [date: 2017-03-27]
 *    - MPC_OUT_OF_RANGE: port unknown OR if address is out of range and OutOfRange Handler can't handle it.
 *    - MPC_OK: if byte is read and put to  buffer, also in iterator mode
 *    - MPC_DENY: if access is not granted. Fallback value is put to buffer.
 */
mpc_returncodes_enum MpcRead (uint8_t item_nr, int16_t ch_adr, mpc_port_t port, uint8_t *buffer) {
  const mpc_descriptor_t       * mpcDescP  = NULL;
  mpc_io_buffer_t              * mpcIop    = &mpcDummyIoBuffer;
  int retval                               = MPC_OUT_OF_RANGE;
  int ch, adr, last_len;

  last_len = 0;

  DEBUGPRINT ("\n MpcRead       : read mpc=%d, ch=%d, adr=%3d", item_nr, ch_adr>>8, ch_adr& 0xff);

  if (port >= MPC_PORTS_FIRST_UNUSED) {
    /// \todo Call ErrorWarnonly()
    return MPC_OUT_OF_RANGE;
  }

  mpcIop            = &mpcIoBuffer[port];
  mpcIop->port      = port;
  mpcIop->num       = item_nr;
  mpcIop->fallback  = MPC_FALLBACK;
  mpcIop->perm      = 0; // promised to the mpc implementations: permissions are preset to 'none' before calling read()

  if (ch_adr < 0) {
    if (mpcIop->channel >= 0) {
      // In iterator mode, first put the data read the last time - iterator init() or last iterator call() - to outside
      DEBUGPRINT (" ITER len=%d buffer=0x%x", mpcIop->length, mpcIop->uint32);
      for (int n = mpcIop->length - 1; n >= 0; n--) {
        *buffer++ = mpcIop->buffer[n];
      }
      last_len     = mpcIop->length;
      mpcIop->adr += last_len;
      ch  = mpcIop->channel;
      adr = mpcIop->adr;
    }
    else {
      return MPC_OUT_OF_RANGE;
    }
  }
  else {
    mpcIop->adr     = adr = ch_adr &  0xFF;
    mpcIop->channel = ch  = ch_adr >> 8;
  }

  mpcDescP = MpcDescriptorGet(MpcDissolveMapping(item_nr, port));
  if (mpcDescP) {
    DEBUGPRINT (", desc=%p, len=%3d", mpcDescP, mpcDescP->keydata.length);

    if (mpcDescP->tables_len >= 0) {  // normal, not a legacy entry
      retval = MpcReadTable (mpcDescP, mpcIop, adr, ch);
      if (retval == MPC_OUT_OF_RANGE && mpcDescP->ReadOoT) {
        retval = (mpcDescP->ReadOoT)(adr, ch, mpcIop);
      }
    }
    else {
      if (adr < mpcDescP->keydata.length) {
        retval = MpcReadLegacy(mpcDescP, mpcIop, adr, ch);
      }
      else {
        retval = MPC_OUT_OF_RANGE;
      }
    }
  }

  if (retval >= 0) { // There is a valid answer, only permissions have to be checked now
    DEBUGPRINT ("\n MpcRead       : read return value=%d, perm=%x, buf=0x%02x, fb=0x%x",
                retval, mpcIop->perm, mpcIop->uint32, mpcIop->fallback);
    if (mpcIop->perm != 0) {
      // MNO hier ganz dringend port anpassen: mix von zwei verschiedenen Port-defnitionen
      if (AclCheckPermissions(mpcIop->perm, mpcIop->port) != ACL_GRANT) {
        mpcIop->uint32 = mpcIop->fallback;
        DEBUGPRINT (" -> denied");
      } else { DEBUGPRINT (" -> granted" ); }
    } else { DEBUGPRINT (" -> no perm"); }
  }
  else {
    mpcIop->channel = MPC_OUT_OF_RANGE;
  }

  if (ch_adr < 0) {
    return (mpc_returncodes_enum) last_len;
  }
  else if (retval >= 0) {
    *buffer = mpcIop->buffer[retval];     // Endianness translation
    retval = 1;
  }

  return (mpc_returncodes_enum) retval;
}

// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
//   M P C   W R I T E   S E C T I O N

// ---------------------------------------------------------------------------------------------------------------------
/** \brief A wrapper for legacy MpcXxx_SetValue() function to map to the new interface.
 *
 * Called by MpcWritePayload() when a legacy implementation without tables is in charge.
 *
 * Relies on that the DALI lock byte and Mpc global write permissions (password pages) already are successfully
 * checked. No extra permissions are tested here (as it was usual with legacy Mpcs).
 *
 * \warning If a legacy Mpc needs multi-port access, this multi-port support has to be provided by the Mpc itself.
 *          The old interface doesn't allow another solution, as no multi-byte information is passed out of the
 *          legacy membanks.
 *
 * \param mpcdesc Pointer to the Mpc descriptor (tables and functions of the Mpc). MUST NOT be NULL.
 * \param mpcIo   Pointer to a mpc_io_buffer_t with adr, channel and the data buffer to work with
 * \param buffer  Pointer to the buffer where the data can be found
 * \retval        True mpc_returncodes_enum (only negative numbers)
 */
static inline mpc_returncodes_enum MpcWriteLegacy(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint8_t * buffer) {
  int         retval = MPC_RW_DENY;
  mpc_rw_enum legacy_retval;

  DEBUGPRINT("\n MpcWriteLegacy: adr=%3d, ch=%3d", mpcIo->adr, mpcIo->channel);

  mpcIo->length = 1;

  legacy_retval = (mpcdesc->SetValue)(mpcdesc->keydata.payload_off + mpcIo->adr, *buffer, mpcIo->channel, mpcIo->port);

  switch (legacy_retval) {
  case MPC_RW_OK:
    retval = MPC_OK;
    break;
  case MPC_RW_OUT_OF_RANGE:
    retval = MPC_OUT_OF_RANGE;
    break;
  case MPC_RW_DENY:
  default:  // avoid 'possibly uninitialized' warning down in MpcRead()
    retval = MPC_DENY;
    break;
  }

  return (mpc_returncodes_enum) retval;
};


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Write data to a new, table based Mpc.
 *
 * This function evaluates the tables of a Mpc implementation and provides an atomic multi-byte and multi-channel
 * access to the data entities (data points) decribed by the tables: It moreover supports a light-weight
 * endianness translation.
 *
 * It operates on a mpc_io_buffer_t structure passed as argument to support also a multi-port operation as well as
 * this struct provied the information about the address and channel to write.
 *
 * The concept relies on a <b> successive access of all bytes of a multi-byte data entity</b>.
 * This is, that it is \b mandatory to write all bytes of a multi-byte structure starting with the byte at the lowest
 * address of the data entity (normally the MSB).
 *
 * For more details and for a detailed interface description pls. refer to Mpc.h/.c.
 *
 * \param mpcdesc Pointer to the Mpc descriptor (tables and functions of the Mpc). MUST NOT be NULL.
 * \param mpcIo   Pointer to a mpc_io_buffer_t with adr, channel and the data buffer to work with
 * \param buffer  Pointer to the buffer where the data can be found
 *
 */
static inline int MpcWriteTable(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint8_t * buffer) {
  const mpc_data_entity_t * ep = NULL;
  const mpc_layout_t      * lp;
  mpc_data_typ_enum         etype;
  int  result, entrylen, bytelocation;

  uint16_t adr     = mpcIo->adr;
  uint8_t  channel = mpcIo->channel;

  result = 0;
  DEBUGPRINT("\n MpcWriteTable : adr=%3d, ch=%3d", adr, channel);

  if (adr < mpcdesc->tables_len) {
    lp = &mpcdesc->layout[adr];
    if (lp->entity != 255) {  // if (lp && ...) is obsolete, as lp is a always available address in the layout array
      ep = &mpcdesc->entities[lp->entity];
    }
  }

  if (ep == NULL) {
    return MPC_OUT_OF_RANGE;
  }

  DEBUGPRINT("\n MpcWriteTable : Table Typ=0x%02x, val_ix=%3d (0x%08x), perm_ix=%d",
             ep->type, ep->data_ri+1, mpcdesc->constants[ep->data_ri+1], ep->perm_wi);

  entrylen  = ep->len;

  if (ep->perm_wi) {                // Fetch permissions if specified and check; deny in case of
    mpcIo->perm = mpcdesc->constants[ep->perm_wi];
    if (AclCheckPermissions(mpcIo->perm, mpcIo->port) != ACL_GRANT) {
      return MPC_DENY;
    }
  }

  if (lp->first) {                  // Clear buffer when indicated, especially needed for MSB of a 24-bit mapping
    mpcIo->uint32 = 0;
  }

  bytelocation = lp->pos;           // Put the data byte in the buffer and write length data field
  mpcIo->buffer[bytelocation] = *buffer;
  mpcIo->length = ep->len;


  //MNO  Hier auf Constant abfragen, dann deny return + test
  if (lp->last) {
    etype = ep->type;
    if (etype == MPC_TYP_VAR) {
      void * p = (void*)mpcdesc->constants[ep->data_ri];
      if (entrylen == 1) {
        *(uint8_t*) p = mpcIo->uint32;
      }
      else if (entrylen == 2) {
        *(uint16_t*)p = mpcIo->uint32;
      }
      else {
        *(uint32_t*)p = mpcIo->uint32;
      }
      result = MPC_OK;
    }
    else if (etype == MPC_TYP_FUN) {
      void * p = (void*)mpcdesc->constants[ep->data_ri + 1];
      // Typecast p to a function pointer taking the same arguments as this function + the value.
      result = ((int(*)(mpc_io_buffer_t *, uint16_t, uint8_t, uint32_t)) p) (mpcIo, adr, channel, mpcIo->uint32);
    }
  }
  else {
    result = bytelocation;
  }
  return result;
};


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Generic Write() of Mpc payload data.
 *
 * A generic interface to write the \b payload of a Mpc. This payload is only the Mpc specific data and
 * does \b not contain keydata or any port specific header bytes (e.g. length, ID and version for Osram Mpcs,
 * checksum at adr 1 for DALI, ...)
 *
 * If a Mpc has Mpc-global permissions for write (mainly valid for legacy pages), these permissions are
 * checked here. Beyond this, this function is mainly a wrapper around WriteLegacy() and WriteTable().
 *
 * \param mpcdesc Pointer to the Mpc descriptor (legacy type or a new one). MUST NOT be NULL.
 * \param mpcIo   Pointer to a mpc_io_buffer_t with adr, channel and the data buffer to work with
 * \param buffer  Pointer to th buffer where the data can be found
 */
static inline int MpcWritePayload(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint8_t * buffer) {

  DEBUGPRINT("\n MpcWritePayload:         adr %3d, ch %3d = val %d\n", mpcIo->adr, mpcIo->channel, *buffer);

  acl_perm_t perm = mpcGetLegacyPermissions(mpcdesc, mpcIo);
  if ((perm == 0) || (AclCheckPermissions(perm, mpcIo->port) == ACL_GRANT)) {
    if (mpcdesc->tables_len >= 0) {
      return MpcWriteTable(mpcdesc, mpcIo, buffer);
    }
    else {
      return MpcWriteLegacy(mpcdesc, mpcIo, buffer);
    }
  }
  return MPC_DENY;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Open a Mpc for non-DALI iterator write access.
 *
 * Set the mpc_io_buffer of a specific port and initialize the attributes for a iterator write access via MpcWrite().
 *
 * Second: As long as legacy Mpcs are involved, this function also is needed to set the DALI lock byte to 0x55
 * to enable the requested write.
 *
 * \param mpc_nr  Number of the Mpc
 * \param adr     Address (payload address) of the first byte to be written by MpcWrite(); Normally zero.
 * \param channel Channel number to be addressed.
 * \param port    mpc_port_t: Specifies the writer of the Mpc. Accesses on different ports don't disturb each other.
 *
 * \retval        True mpc_returncodes_enum (only negative numbers)
 *                - MPC_OK: Mpc is present; Does not guarantee that the following write will succeed,
 *                          adr or channel might be out of range
 *                - MPC_OUT_OF_RANGE: Mpc does not exist
 */
mpc_returncodes_enum MpcOpen(uint8_t item_nr, uint8_t adr, uint8_t channel, mpc_port_t port) {
  const mpc_descriptor_t * mpcdesc;
  mpc_io_buffer_t        * mpcIo;

  DEBUGPRINT ("\n MpcOpen       : mpc=%2d, adr=%3d, ch=%d, port=%d", item_nr, adr, channel, port);

  mpcIo            = &mpcIoBuffer[port];
  mpcIo->num       = item_nr;
  mpcIo->adr       = adr;
  mpcIo->channel   = channel;
  mpcIo->port      = port;

  mpcdesc = MpcDescriptorGet(MpcDissolveMapping(item_nr, port));

  // Allow write access for legacy pages by setting the lock byte to 0x55
  if(mpcdesc) {
    if (mpcdesc->tables_len < 0) {  // Works also for DALI membank 0: Has no lock byte, but adr 2 will drop this (ro)
      (mpcdesc->SetValue)(2, 0x55, mpcIo->channel, mpcIo->port);
    }
    return MPC_OK;
  }
  return MPC_OUT_OF_RANGE;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Close a Mpc after a non-DALI write operation.
 *
 * As long as legacy Mpcs are involved, this function is needed to set the DALI lock byte (which was set by
 * MpcOpen() to 0x55) to 0xFF (again) after a write operation. Nothing else.
 *
 * \note    By design decission, DALI (if it ever has unlocked the Mpc) again has to unlock a \b legacy Mpc
 *          after a Nfc \b write access.
 *
 * \warning Call this function only after a preceding open(). Nearly no checks were executed, this function
 *          relies on a valid mpc_io_buffer (containing e.g. the mpc_nr) for the given port.
 *
 * \param  port Port numbner for which the operation is to be done. See above warning for limitations.
 */
void MpcClose(mpc_port_t port) {
  const mpc_descriptor_t * mpcdesc;
  const mpc_io_buffer_t  * mpcIo;

  DEBUGPRINT ("\n MpcClose      : port=%d", port);

  mpcIo = &mpcIoBuffer[port];
  mpcdesc = MpcDescriptorGet(MpcDissolveMapping(mpcIo->num, port));
  if(mpcdesc) {
    if (mpcdesc->tables_len < 0) {
      (mpcdesc->SetValue)(2, 0xFF, mpcIo->channel, port);
    }
  }
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Generic Write() of a Mpc payload data.
 *
 * A generic interface to write the \b payload of a Mpc. This payload is only the Mpc specific data and
 * does \b not contain keydata or any port specific header bytes (e.g. length, ID and version for Osram Mpcs,
 * checksum at adr 1 for DALI, ...).
 * \param port Port to use, see \ref mpc_port_t
 * \param val Value to write
 */
mpc_returncodes_enum MpcWrite(mpc_port_t port, uint8_t val) {
  const mpc_descriptor_t * mpcdesc;
  mpc_io_buffer_t * mpcIo;
  int retval;

  mpcIo = &mpcDummyIoBuffer;
  retval = MPC_OUT_OF_RANGE;

  DEBUGPRINT("\n MpcWrite      : mpc=%d, ch=%d, adr=%3d", mpcIo->num, mpcIo->channel, mpcIo->adr);

  if(port >= MPC_PORTS_FIRST_UNUSED) {
    /// \todo Call ErrorWarnonly()
    return (mpc_returncodes_enum) retval;
  }

  mpcIo = &mpcIoBuffer[port];
  mpcdesc =  MpcDescriptorGet(MpcDissolveMapping(mpcIo->num, port));

  if(mpcdesc) {
    if(mpcIo->adr < (mpcdesc->keydata.length)) {

      retval = MpcWritePayload(mpcdesc, mpcIo, &val);
      mpcIo->adr++;
      // Align all legacy return values (a colorful mixture) to new types:
      // a) ok or deny within address range
      // b) out of range when above length of the address area
      if(retval == MPC_OK) {
        return MPC_OK;
      }
      else {
        return MPC_DENY;
      }
    }
  }
  return MPC_OUT_OF_RANGE;
}


// ---------------------------------------------------------------------------------------------------------------------

/** \brief The entry point for DALI library to the new Mpc interface for write access.
 *
 * This function, called by MemoryBankGeneric_SetValue:
 *   * checks the passwort protection
 *   * changes the internal lockbyte ( and calls legacy code for membanks own setting)
 *   * checks Lockbyte
 *   * calls MpcWritePayload to do the actual writting
 *   * increases changeCounter if writting ok
 *
 *  Port will be set to Dali
 *
 * \param membank_nr  Number of Membank in Layout
 * \param adr         Adress of byte to write
 * \param ch          channel
 * \param val         byte to write
 *
 * \return  MPC_OK            : writing of lockbyte or val succeeded
 *          MPC_DENY          : attempt to write to Keydata (except lockbyte),
 *                              Passwort protected,
 *                              writing not permitted,
 *          MPC_OUT_OF_RANGE  : Mpc Decriptor not existent,
 *                              adr not valid
 */
mpc_returncodes_enum MpcWriteDali(uint8_t membank_nr, int16_t adr, uint8_t ch, uint8_t val) {
  mpc_io_buffer_t        * mpcIo;
  const mpc_descriptor_t * mpcdesc;
  int retval;

  DEBUGPRINT("\n MpcWriteDali   : mb %3d, adr %3d, ch %3d = val %d\n", membank_nr, adr, ch, val);

  mpcdesc = MpcDescriptorGet(MpcDissolveMapping(membank_nr, MPC_PORT_DALI));
  if (mpcdesc) {
    if (mpcIsPasswordProtected(mpcdesc)) {
       return MPC_DENY;
    }
    if (adr < mpcdesc->keydata.payload_off) {
      if (adr == 2) {
        mpcDaliLockbytes[membank_nr] = val; // boundary check already done in MpcDescriptorGet()
        if (mpcdesc->tables_len < 0) {
          // For legacy pages, also write lock byte to the membank itself
          (mpcdesc->SetValue)(2, val, ch, MPC_PORT_DALI);
        }
        return MPC_OK;
      }
      else {
        return MPC_DENY;  // No write access to other "keydata" than DALI lock byte
      }
    }
    else {
      // Payload area
      mpcIo           = &mpcIoBuffer[MPC_PORT_DALI];
      mpcIo->adr      = adr - mpcdesc->keydata.payload_off;
      // mpcIo->num   = 0; /// \todo For the time being; intention is to contain mpc_nr != membank_nr
      mpcIo->num      = membank_nr;
      mpcIo->port     = MPC_PORT_DALI;
      mpcIo->channel  = ch;

      // DALI lock byte: Check this for all pages which indicate to use it
      if (mpcdesc->keydata.attributes & MPC_ATTRIB_DALI_LOCKBYTE) {
        if (mpcDaliLockbytes[membank_nr] != 0x55) {
          return MPC_DENY;
        }
      }
      retval = MpcWritePayload(mpcdesc, mpcIo, &val);
      return (mpc_returncodes_enum) retval;
    }
  }
  // fall-through
  return MPC_OUT_OF_RANGE;
}


// ---------------------------------------------------------------------------------------------------------------------
/** \brief Query changeCounter (to trigger tag updates in the NFC module)
 *
 *  \return changeCounter
 */
uint32_t MpcGetChangeCounter(void) {
  return changeCounter;
}

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Function to increment the changeCounter.
 *
 * Used to trigger NFC tag updates.
 */
void MpcIncChangeCounter(void) {
  changeCounter++;
}


// ---------------------------------------------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------------------------------------------
#ifdef UNIT_TESTING
// The base functions might be static inline and hasn't a settled interface yet -> Wrapper to export it
int MpcReadTable_TDD(mpc_io_buffer_t * mpcIo, uint16_t adr, uint8_t channel, const mpc_descriptor_t * mpcdesc) {
  return MpcReadTable(mpcdesc, mpcIo, adr, channel);
}

int MpcWriteTable_TDD(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo, uint8_t * val) {
  return MpcWriteTable(mpcdesc, mpcIo, val);
}

uint32_t getLegacyPermissions_TDD(const mpc_descriptor_t * mpcdesc, mpc_io_buffer_t * mpcIo)
{
  return mpcGetLegacyPermissions(mpcdesc,mpcIo);
}

void set_pwdretval_TDD(int value)
{
  pwd_retval = value;
}

uint32_t MpcGetChangeCounter_TDD(void){
  return MpcGetChangeCounter();
}

#endif


/** \} */
