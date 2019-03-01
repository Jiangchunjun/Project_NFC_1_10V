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
// $Revision: 2995 $
// $Date: 2016-02-22 15:35:26 +0800 (Mon, 22 Feb 2016) $
// $Id: nvm.h 2995 2016-02-22 07:35:26Z G.Marcolin $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Nvm/tags/v1.2/api/nvm.h $
//
// ---------------------------------------------------------------------------------------------------------------------

#ifndef __NVM_H
#define __NVM_H

/** \addtogroup NvmAPI
 * \{
 *
 * \file
 * \brief Header file for nvm.c
 *
 */

/**********************************************************************************************************************/

typedef enum
{
    NVM_MEM_CLASS_0,
    NVM_MEM_CLASS_1,
    NVM_MEM_CLASS_2,
    NVM_MEM_CLASS_MAX
}
nvm_memclass_t;

/**********************************************************************************************************************/

typedef enum
{
	NVM_STATE_NO_DATA,
	NVM_STATE_OK,
	NVM_STATE_READ_ONLY,
	NVM_STATE_ERR,
    NVM_STATE_MAX
}
nvm_state_t;


/**********************************************************************************************************************/

typedef enum
{
	NVM_INIT_FAILED,
	NVM_INIT_NEW,
	NVM_INIT_OK,
    NVM_INIT_MAX
}
nvm_init_t;

/**********************************************************************************************************************/

typedef enum
{
	NVM_CMP_NOT_EQUAL,
	NVM_CMP_EQUAL,
    NVM_CMP_MAX
}
nvm_cmp_t;

/**********************************************************************************************************************/

typedef struct
{
	uint32_t crc;
	uint32_t addr;
	uint32_t erase;
	uint32_t write;
}
op_err_t;					// Operation (run-time) errors

/**********************************************************************************************************************/

typedef struct
{
	uint32_t readPageIndex;
	uint32_t readPageCycles;
	uint32_t writePageIndex;
	uint32_t writePageCycles;
}
nvm_dbg_t;

/**********************************************************************************************************************/

typedef struct
{
	nvm_state_t state;
	op_err_t    error;
	uint32_t    writeCycles;
	nvm_dbg_t   dbg;
}
nvm_t;

/**********************************************************************************************************************/

const nvm_t* nvmInit( void );

void nvmResume( void );

nvm_init_t nvmDataInit( void *pdata, uint32_t size, nvm_memclass_t mc );

void nvmWriteAll( void );

nvm_cmp_t nvmDataCompare( const void *pdata, uint32_t sizeByte );

uint32_t nvmGetWriteCycles(void);

/**********************************************************************************************************************/
/** \} */ // NvmAPI
#endif


