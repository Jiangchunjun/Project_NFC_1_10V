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
// Initial version: 2016-07, W.Limmer@osram.de
//
// Change History:
//
// $Author: w.limmer $
// $Revision: 11321 $
// $Date: 2017-12-21 00:58:53 +0800 (Thu, 21 Dec 2017) $
// $Id: MpcAllocation.h 11321 2017-12-20 16:58:53Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcAllocation.h $
//


/** \addtogroup MPC Multi Purpose Centers
 * \{
 *
 * \file
 * \brief Header for the allocation of all Mpcs
 */

/*****************************************************
 * Order of needed includes before:
  #include <Global.h> or #include <stddef.h>
  #include "Mpc.h"
  #include "MpcInternal.h"
 ****************************************************/

#ifndef __MPC_ALLOCATION_H
#define __MPC_ALLOCATION_H

/// \todo For the time being / until we have updated all modules to read the default config by themselves
#include "MpcDefaultConfiguration.h"


typedef mpc_rw_enum (*mpc_legacy_read_fp)  (uint8_t address, uint8_t * pValue, uint8_t channel, uint8_t port );
typedef mpc_rw_enum (*mpc_legacy_write_fp) (uint8_t address, uint8_t    value, uint8_t channel, uint8_t port );
typedef void        (*mpc_init_fp)         (void);
typedef void        (*mpc_reset_fp)        (uint8_t channel);

// ---------------------------------------------------------------------------------------------------------------------
/** \brief Define a struct to collect all generic information of a Mpc.
 *
 * This generic information might be
 *  - For new Mpcs: Pointers to the tables and generic functions
 *  - For legacy membanks (layout_len = -1): Pointers to the 4 legacy functions Init(), Reset(), Set- and GetValue()
 */
typedef struct {
  const mpc_keydata_t           keydata;        // Keydata of the Mpc (public)
  const int16_t                 tables_len;     // negative for legacy Mpcs
  union {
    struct {
      const mpc_data_entity_t * entities;
      const uint32_t          * constants;
      const mpc_layout_t      * layout;
      mpc_returncodes_enum  ( * Cmd      ) (mpc_cmd_t cmd, uint8_t channel, mpc_port_t port);
      mpc_returncodes_enum  ( * ReadOoT  ) (uint16_t  adr, uint8_t channel, mpc_io_buffer_t * mpcIop);
      //mpc_returncodes_enum( * WriteOoT ) (uint16_t  adr, uint8_t channel, mpc_io_buffer_t * mpcIop, uint8_t val);
    };
    struct {
      mpc_legacy_read_fp        GetValue;
      mpc_legacy_write_fp       SetValue;
      mpc_init_fp               Init;
      mpc_reset_fp              Reset;
    };
  };
} mpc_descriptor_t;


// ---------------------------------------------------------------------------------------------------------------------
// Global arrays
extern const mpc_keydata_t *  MpcKeydata[];

// Interface functions
      int                 MpcDissolveMapping(int item_nr, mpc_port_t port);
const mpc_descriptor_t *  MpcDescriptorGet(int mpc_nr);

#endif // __MPC_ALLOCATION_H

/** \} */
