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
// Indent style: Replace tabs by spaces, 2 spaces per indentation level
//
// Change History / Major Milestones:
//    2017-03, W.Limmer@osram.de: Initial version
//
//
// $Author: w.limmer $
// $Revision: 10486 $
// $Date: 2017-10-25 14:19:53 +0800 (Wed, 25 Oct 2017) $
// $Id: MskEncryption.h 10486 2017-10-25 06:19:53Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Toolbox1.0/Encryption/releases/Rel1.2/MskEncryption.h $
//

#ifndef __MSK_ENCRYPTION_H
#define __MSK_ENCRYPTION_H

/** \ingroup MasterServiceKey
 * \{
 * \file \brief Header file of password encryption/challenge-response support for master and service keys of MSK
 */

uint32_t MskPasswordEncrypt(uint32_t password);
void     MskOsramAuthCalcResponses(uint32_t challenge, uint32_t * responses);

/** \} */
#endif
