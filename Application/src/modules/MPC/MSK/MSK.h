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
// Initial version: 2017-01, Olaf Busse, o.busse@osram.com
//
// Change History:
//
// $Author: m.nordhausen $
// $Revision: 10980 $
// $Date: 2017-11-28 15:17:45 +0800 (Tue, 28 Nov 2017) $
// $Id: MSK.h 10980 2017-11-28 07:17:45Z m.nordhausen $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/MSK/releases/Rel.%201.0.0/Src/MSK.h $
//

#ifndef MSK_H
#define MSK_H

/*****************************************************
 * Order of needed includes before:
  #include "Mpc.h"
  #include "MpcInternal.h"
  #include "MpcAllocation.h"
 ****************************************************/

extern const mpc_descriptor_t MSK_Descriptor;
extern const mpc_descriptor_t BIO_Descriptor;


// Ugly, intermediate hack: Remove soon again
#define MSK_Description     MSK_Descriptor      // For the time being: Hot fix to recover all that naming issue(s)
#define MSK_RO_Description  BIO_Descriptor      // For the time being: Hot fix to recover all that naming issue(s)
#define BIO_Description     BIO_Descriptor      // For the time being: Hot fix to recover all that naming issue(s)


#endif
