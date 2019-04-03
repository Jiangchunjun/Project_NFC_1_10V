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
// Initial version: 2016-07 W.Limmer@osram.de
//
// Change History:
//
// $Author: j.avril $
// $Revision: 12120 $
// $Date: 2018-02-21 21:20:13 +0800 (Wed, 21 Feb 2018) $
// $Id: MpcDescriptors.c 12120 2018-02-21 13:20:13Z j.avril $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/Alloc/branches/RC_2018-02-20/Src/MpcDescriptors.c $
//


/** \addtogroup MPC
 * \{
 * \file
 * \brief Supplement descriptors for legacy pages (which don't implemt them by theirselves)
 */

#define MODULE_DALI             // to include device type, important for some mem banks
#define MODULE_MPC

#include "Global.h"
#include "Config.h"

// New Struct
#include "Mpc.h"                // Generic application interface
#include "MpcInternal.h"        // Internal interface to the Mpc implementations
#include "MpcAllocation.h"      // Generic application interface
#include "MpcIncludeAll.h"
#include "Acl.h"
#include "MpcPerm4Legacy.h"     // include order is essential here !




#define MPC_STD_PAYLOAD_OFFSET  5

// ---------------------------------------------------------------------------------------------------------------------
#ifdef MPCs_DALI
const mpc_descriptor_t MpcLegacyDali0 = {
  .keydata          = {
    .id             = 0,                // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .version        = MPC_Dali_VERSION, // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .attributes     = 0,                // Bank 0 has no lockbyte
    .payload_off    = MPC_MEMBANK0_OFFSET,
    .permissions    = ACL_PERM_OSRAM,
    .length         = MEMORY_BANK0_LastADDRESS - (MPC_MEMBANK0_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Dali0_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Dali0_SetValue,
  .Init             = NULL,  // init triggered by Dali
  .Reset            = NULL,  // no reset needed
};

const mpc_descriptor_t MpcLegacyDali1 = {
  .keydata          = {
    .id             = 1,                // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .version        = MPC_Dali_VERSION, // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = MPC_MEMBANK1_OFFSET,
    .length         = MEMORY_BANK1_LastADDRESS - (MPC_MEMBANK1_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Dali1_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Dali1_SetValue,
  .Init             = NULL,  // init triggered by Dali
  .Reset            = NULL,  // init triggered by Dali
};

const mpc_descriptor_t MpcLegacyDali1_with_perm_master = {
  .keydata          = {
    .id             = 1,                // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .version        = MPC_Dali_VERSION, // used only with NFC, via DALI MB1 doesn't deliver an ID/Version
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = MPC_MEMBANK1_OFFSET,
    .permissions    = ACL_PERM_MASTER,
    .length         = MEMORY_BANK1_LastADDRESS - (MPC_MEMBANK1_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Dali1_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Dali1_SetValue,
  .Init             = NULL,  // init triggered by Dali
  .Reset            = NULL,  // init triggered by Dali
};

const mpc_descriptor_t MpcLegacyDali_T6 = {
  .keydata          = {
    .id             = MPC_DaliT6_ID,
    .version        = MPC_DaliT6_VERSION,
    .attributes     = 0,
    .payload_off    = MPC_DALI_PARAMS_OFFSET,
    .length         = MPC_DALI_T6_LENGTH,
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_DaliT6_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_DaliT6_SetValue,
  .Init             = NULL,  // init done by DaliLib
  .Reset            = NULL,  // no reset from outside the DaliLib
};
#endif


#ifdef MPC_DEBUG_DESCRIPTOR_DEFAULT_V1
// To use the debug bank with default decriptor version 1, the *appplication* (not the library) has to provide
// DebugMpc.h/.c defining and implementing
//
// void        MemoryBank_Debug_Init     (void);
// void        MemoryBank_Debug_Reset    (uint8_t channel);
// mpc_rw_enum MemoryBank_Debug_GetValue (uint8_t address, uint8_t * pValue, uint8_t channel, uint8_t port);
// mpc_rw_enum MemoryBank_Debug_SetValue (uint8_t address, uint8_t    value, uint8_t channel, uint8_t port);
//
// The header file moreover has to define
// #define MPC_DEBUG_ID                     254  // ID, possibly only in the descriptor and in NFC, not in DALI
// #define MPC_DEBUG_VERSION                  1  // Version, possibly only in the descriptor and in NFC, not in DALI
// #define MPC_DEBUG_DALI_PAYLOAD_OFFSET      3  // Platform specific, here: Only the 3 bytes of the DALI standard
// #define MPC_DEBUG_DALI_LASTADDRESS       ...  // Platform specific; As before: The address of the last location in DALI
const mpc_descriptor_t MpcLegacyDebug = {
  .keydata          = {
    .id             = MPC_DEBUG_ID,       // 254
    .version        = MPC_DEBUG_VERSION,  // 1
    .attributes     = 0,
    .payload_off    = MPC_DEBUG_DALI_PAYLOAD_OFFSET,
    .permissions    = ACL_PERM_OSRAM,
    .length         = MPC_DEBUG_DALI_LASTADDRESS - (MPC_DEBUG_DALI_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Debug_GetValue,
  .SetValue         = &MemoryBank_Debug_SetValue,
  .Init             = &MemoryBank_Debug_Init,
  .Reset            = &MemoryBank_Debug_Reset,
};
#endif


#ifdef MPC_INFO_NUMBER
const mpc_descriptor_t MpcLegacyInfo = {
  .keydata          = {
    .id             = MPC_Info_ID,
    .version        = MPC_Info_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_LAMP_OPER_TIME,
    .length         = MPC_Info_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Info_GetValue_port,
  .SetValue         = &MemoryBank_Info_SetValue_port,
  .Init             = &MemoryBank_Info_Init,
  .Reset            = &MemoryBank_Info_Reset,
};
#endif


#ifdef MPC_CSM_NUMBER
const mpc_descriptor_t MpcLegacyCsm_pw1 = {
  .keydata          = {
    .id             = MPC_CurrentSetting_ID,
    .version        = MPC_CurrentSetting_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD1_PROT,
    .payload_off    = 5,
    .permissions    = ACL_PERM_OPER_CURR,
    .length         = CSM_PAYLOAD_LENGTH,
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Csm_GetValue_port,
  .SetValue         = &MemoryBank_Csm_SetValue_port,
  .Init             = &MemoryBank_Csm_Init,
  .Reset            = &MemoryBank_Csm_Reset,
};


const mpc_descriptor_t MpcLegacyCsm = {
  .keydata          = {
    .id             = MPC_CurrentSetting_ID,
    .version        = MPC_CurrentSetting_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_OPER_CURR,
    .length         = CSM_PAYLOAD_LENGTH,
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Csm_GetValue_port,
  .SetValue         = &MemoryBank_Csm_SetValue_port,
  .Init             = &MemoryBank_Csm_Init,
  .Reset            = &MemoryBank_Csm_Reset,
};
#endif


#ifdef MPC_TFM_NUMBER
const mpc_descriptor_t MpcLegacyTfm_pw1 = {
  .keydata          = {
    .id             = MPC_TuningFactor_ID,
    .version        = MPC_TuningFactor_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD1_PROT,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = tfmpermissions,
    .length         = MPC_TuningFactor_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Tfm_GetValue_Port,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Tfm_SetValue_Port,
  .Init             = &MemoryBank_Tfm_Init,
  .Reset            = &MemoryBank_Tfm_Reset,
};

const mpc_descriptor_t MpcLegacyTfm = {
  .keydata          = {
    .id             = MPC_TuningFactor_ID,
    .version        = MPC_TuningFactor_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = tfmpermissions,
    .length         = MPC_TuningFactor_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Tfm_GetValue_Port,
  .SetValue         = &MemoryBank_Tfm_SetValue_Port,
  .Init             = &MemoryBank_Tfm_Init,
  .Reset            = &MemoryBank_Tfm_Reset,
};

#endif


#ifdef MPC_CLM_NUMBER
const mpc_descriptor_t MpcLegacyClm_pw1 = {
  .keydata          = {
    .id             = MPC_ConstantLumen_ID,
    .version        = MPC_ConstantLumen_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD1_PROT,
    .payload_off    = 5,
    .permissions    = ACL_PERM_CONST_LUMEN,
    .length         = CLM_PAYLOAD_LENGTH,
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Clm_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Clm_SetValue,
  .Init             = NULL,  // ClmInit currently needs extra parameters and can't be called in a generic way -> Info_init()
  .Reset            = &MemoryBank_Clm_Reset,
};
const mpc_descriptor_t MpcLegacyClm = {
  .keydata          = {
    .id             = MPC_ConstantLumen_ID,
    .version        = MPC_ConstantLumen_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_CONST_LUMEN,
    .length         = CLM_PAYLOAD_LENGTH,
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Clm_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Clm_SetValue,
  .Init             = NULL,  // ClmInit currently needs extra parameters and can't be called in a generic way -> Info_init()
  .Reset            = &MemoryBank_Clm_Reset,
};
#endif


#ifdef MPC_TPM_NUMBER
#pragma message ("TPM: No Port dependent Get- and SetValue Functions available")
const mpc_descriptor_t MpcLegacyTpm = {
  .keydata          = {
    .id             = MPC_TPM_ID,
    .version        = MPC_TPM_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_THERM_PROT,
    .length         = MPC_TPM_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Tpm_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Tpm_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Tpm_Init,
  .Reset            = &MemoryBank_Tpm_Reset,
};
#endif


#ifdef MPC_EOL_NUMBER
const mpc_descriptor_t MpcLegacyEol = {
  .keydata          = {
    .id             = MPC_EOL_ID,
    .version        = MPC_EOL_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_EOL,
    .length         = MPC_EOL_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Eol_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Eol_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Eol_Init,
  .Reset            = &MemoryBank_Eol_Reset,
};

const mpc_descriptor_t MpcLegacyEol_read_only = {
  .keydata          = {
    .id             = MPC_EOL_ID,
    .version        = MPC_EOL_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_NEVER,
    .length         = MPC_EOL_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Eol_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Eol_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Eol_Init,
  .Reset            = &MemoryBank_Eol_Reset,
};
#endif


#ifdef MPC_MAINS_NUMBER
const mpc_descriptor_t MpcLegacyMains = {
  .keydata          = {
    .id             = MPC_MAINS_ID,
    .version        = MPC_MAINS_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = mainspermissions,
    .length         = MPC_MAINS_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Mains_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Mains_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Mains_Init,
  .Reset            = &MemoryBank_Mains_Reset,
};

const mpc_descriptor_t MpcLegacyMains_read_only = {
  // Used for 1DIM to have the page with all connections, but write inhibited
  .keydata          = {
    .id             = MPC_MAINS_ID,
    .version        = MPC_MAINS_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_NEVER,
    .length         = MPC_MAINS_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Mains_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Mains_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Mains_Init,
  .Reset            = &MemoryBank_Mains_Reset,
};
#endif


#ifdef MPC_PRESDETECT_NUMBER
const mpc_descriptor_t MpcLegacyPresDet = {
  .keydata          = {
    .id             = MPC_PRES_DET_ID,
    .version        = MPC_PRES_DET_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = presdetectpermissions,
    .length         = MPC_PRES_DET_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_PresDet_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_PresDet_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_PresDet_Init,
  .Reset            = &MemoryBank_PresDet_Reset,
};

const mpc_descriptor_t MpcLegacyPresDet_read_only = {
  .keydata          = {
    .id             = MPC_PRES_DET_ID,
    .version        = MPC_PRES_DET_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_NEVER,
    .length         = MPC_PRES_DET_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_PresDet_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_PresDet_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_PresDet_Init,
  .Reset            = &MemoryBank_PresDet_Reset,
};
#endif


#ifdef MPC_STEP_NUMBER
#pragma message ("STEP DIM: Port dependent Get- and SetValue Functions in-between available -> use them")
const mpc_descriptor_t MpcLegacyStep = {
  .keydata          = {
    .id             = MPC_STEP_ID,
    .version        = MPC_STEP_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = steppermissions,
    .length         = MPC_STEP_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Step_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Step_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Step_Init,
  .Reset            =                       &MemoryBank_Step_Reset,
};

const mpc_descriptor_t MpcLegacyStep_read_only = {
  // Used for 1DIM to have the page with all connections, but write inhibited
  .keydata          = {
    .id             = MPC_STEP_ID,
    .version        = MPC_STEP_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_NEVER,
    .length         = MPC_STEP_LAST_ADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Step_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Step_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Step_Init,
  .Reset            =                       &MemoryBank_Step_Reset,
};
#endif


#ifdef MPC_PASSWORD1_NUMBER
const mpc_descriptor_t MpcLegacyPassword1 = {
  .keydata          = {
    .id             = MPC_Password_ID,
    .version        = MPC_Password_VERSION,
    .attributes     = 0,                                // Password 1 page, no lockbyte by definition
    .payload_off    = 5,
    .length         = MPC_Password_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Password_GetValue_PW1,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Password_SetValue_PW1,
  .Init             = &MemoryBank_Password_Init,
  .Reset            = &MemoryBank_Password_Reset,
  };
#endif


#ifdef MPC_PASSWORD2_NUMBER
#ifndef MPC_PASSWORD1_NUMBER
  STATIC_ASSERT(0, "Password2 page can't be used without Password1 page")
#endif
const mpc_descriptor_t MpcLegacyPassword2 = {
  .keydata          = {
    .id             = MPC_Password_ID,
    .version        = MPC_Password_VERSION,
    .attributes     = 0,                                // Password 2 page, no lockbyte by definition
    .payload_off    = 5,
    .length         = MPC_Password_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Password_GetValue_PW2,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Password_SetValue_PW2,
  .Init             = &MemoryBank_Password_Init,
  .Reset            = &MemoryBank_Password_Reset,
  };
#endif


#ifdef MPC_EMERGENCY_NUMBER
const mpc_descriptor_t MpcLegacyEmergency = {
  .keydata          = {
    .id             = MPC_Emergency_ID,
    .version        = MPC_Emergency_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_DC_EMERGENCY,
    .length         = MPC_Emergency_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Emergency_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Emergency_SetValue,
  .Init             = &MemoryBank_Emergency_Init,
  .Reset            = &MemoryBank_Emergency_Reset,
};

const mpc_descriptor_t MpcLegacyEmergency_pw2 = {
  .keydata          = {
    .id             = MPC_Emergency_ID,
    .version        = MPC_Emergency_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD2_PROT,
    .payload_off    = 5,
    .permissions    = ACL_PERM_DC_EMERGENCY,
    .length         = MPC_Emergency_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Emergency_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Emergency_SetValue,
  .Init             = &MemoryBank_Emergency_Init,
  .Reset            = &MemoryBank_Emergency_Reset,
};

const mpc_descriptor_t MpcLegacyEmergency_read_only = {
  .keydata          = {
    .id             = MPC_Emergency_ID,
    .version        = MPC_Emergency_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_NEVER,
    .length         = MPC_Emergency_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Emergency_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Emergency_SetValue,
  .Init             = &MemoryBank_Emergency_Init,
  .Reset            = &MemoryBank_Emergency_Reset,
};
#endif


#ifdef MPC_POWER_NUMBER
const mpc_descriptor_t MpcLegacyPower = {
  .keydata          = {
    .id             = MPC_Power_ID,
    .version        = MPC_Power_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .length         = MPC_Power_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Power_GetValue_Port,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Power_SetValue_port,
  .Init             = &MemoryBank_Power_Init,
  .Reset            = &MemoryBank_Power_Reset,
};
#endif


#ifdef MPC_DIM2DARK_NUMBER
const mpc_descriptor_t MpcLegacyDim2Dark_pw1 = {
  .keydata          = {
    .id             = MPC_Dim2Dark_ID,
    .version        = MPC_Dim2Dark_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD1_PROT,
    .payload_off    = 5,
    .permissions    = ACL_PERM_DIM2DARK,
    .length         = MPC_Dim2Dark_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Dim2Dark_GetValue_Port,
  .SetValue         = &MemoryBank_Dim2Dark_SetValue_port,
  .Init             = &MemoryBank_Dim2Dark_Init,
  .Reset            = &MemoryBank_Dim2Dark_Reset,
};

const mpc_descriptor_t MpcLegacyDim2Dark = {
  .keydata          = {
    .id             = MPC_Dim2Dark_ID,
    .version        = MPC_Dim2Dark_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_DIM2DARK,
    .length         = MPC_Dim2Dark_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_Dim2Dark_GetValue_Port,
  .SetValue         = &MemoryBank_Dim2Dark_SetValue_port,
  .Init             = &MemoryBank_Dim2Dark_Init,
  .Reset            = &MemoryBank_Dim2Dark_Reset,
};
#endif


#ifdef MPC_GENERALFEATURES_NUMBER
const mpc_descriptor_t MpcLegacyGeneralFeatures_pw1 = {
  .keydata          = {
    .id             = MPC_GeneralFeatures_ID,
    .version        = MPC_GeneralFeatures_VERSION,
    .attributes     = MPC_ATTRIB_PASSWORD1_PROT | MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_GENERAL_FEATURES,
    .length         = MPC_GeneralFeatures_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_GeneralFeatures_GetValue_Port,
  .SetValue         = &MemoryBank_GeneralFeatures_SetValue_port,
  .Init             = &MemoryBank_GeneralFeatures_Init,
  .Reset            = &MemoryBank_GeneralFeatures_Reset,
};

const mpc_descriptor_t MpcLegacyGeneralFeatures = {
  .keydata          = {
    .id             = MPC_GeneralFeatures_ID,
    .version        = MPC_GeneralFeatures_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,        // General features page, lockbyte after alignment with W.Limmer, J.Ottensmeyer
    .payload_off    = 5,
    .permissions    = ACL_PERM_GENERAL_FEATURES,
    .length         = MPC_GeneralFeatures_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = &MemoryBank_GeneralFeatures_GetValue_Port,
  .SetValue         = &MemoryBank_GeneralFeatures_SetValue_port,
  .Init             = &MemoryBank_GeneralFeatures_Init,
  .Reset            = &MemoryBank_GeneralFeatures_Reset,
};
#endif


#ifdef MPC_TDC_NUMBER
const mpc_descriptor_t MpcLegacyTDC = {
  .keydata          = {
    .id             = MPC_TouchDim_ID,
    .version        = MPC_TouchDim_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .length         = MPC_TouchDim_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1,
  .GetValue         = (mpc_legacy_read_fp)  &TouchDimMPC_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &TouchDimMPC_SetValue,
  .Init             = NULL,
  .Reset            = (mpc_reset_fp)        &TouchDimMPC_Reset,
};
#endif


#ifdef MPC_ASTRO_NUMBER
#pragma message ("ASTRO DIM: No Port dependent Get- and SetValue Functions available")
const mpc_descriptor_t MpcLegacyAstro_pw1 = {
  .keydata          = {
    .id             = MemoryBank_Astro_ID,
    .version        = MemoryBank_Astro_Version,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE | MPC_ATTRIB_PASSWORD1_PROT,
    .payload_off    = 5,
    .permissions    = 0,
    .length         = MemoryBank_Astro_LastAddress - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1, // indicator for legacy mpc
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Astro_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Astro_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Astro_Init,
  .Reset            =                       &MemoryBank_Astro_Reset,
};

const mpc_descriptor_t MpcLegacyAstro = {
  .keydata          = {
    .id             = MemoryBank_Astro_ID,
    .version        = MemoryBank_Astro_Version,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = 0,
    .mult_perm4legacy = astropermissions,
    .length         = MemoryBank_Astro_LastAddress - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1, // indicator for legacy mpc
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Astro_GetValue,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Astro_SetValue,
  .Init             = (mpc_init_fp)         &MemoryBank_Astro_Init,
  .Reset            =                       &MemoryBank_Astro_Reset,
};
#endif


#ifdef MPC_DGUARD_NUMBER
const mpc_descriptor_t MpcLegacyDguard = {
  .keydata          = {
    .id             = MPC_DriverGuard_ID,
    .version        = MPC_DriverGuard_VERSION,
    .attributes     = MPC_ATTRIB_DALI_LOCKBYTE,
    .payload_off    = 5,
    .permissions    = ACL_PERM_DRV_GUARD,
    .length         = MPC_DriverGuard_LastADDRESS - (MPC_STD_PAYLOAD_OFFSET - 1),
  },
  .tables_len       = -1, // indicator for legacy mpc
  .GetValue         = (mpc_legacy_read_fp)  &MemoryBank_Dguard_GetValue_Port,
  .SetValue         = (mpc_legacy_write_fp) &MemoryBank_Dguard_SetValue_Port,
  .Init             = &MemoryBank_Dguard_Init,
  .Reset            = &MemoryBank_Dguard_Reset,
};
#endif

#ifdef MPC_O2T_NUMBER
 //------------------------------------------------------------------------------
/// \brief Page Description
//------------------------------------------------------------------------------

#endif


/** \} */
