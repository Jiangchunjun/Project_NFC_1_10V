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
// $Author: w.limmer $
// $Revision: 10715 $
// $Date: 2017-11-11 00:39:38 +0800 (Sat, 11 Nov 2017) $
// $Id: MSK_internal.h 10715 2017-11-10 16:39:38Z w.limmer $
// $URL: https://app-ehnsvn02.int.osram-light.com/svn/EC/Mpc/MSK/releases/Rel.%201.0.0/Src/MSK_internal.h $
//
//
#ifndef MSK_INTERNAL_H_
#define MSK_INTERNAL_H_

//#include "MSKDefaults.h"
//#include "Mpc.h"
//#include "MpcInternal.h"
//*********************************************************
//******** Const used for test only ***********************
//*********************************************************
#ifdef MSK_DUMMY_ENCRYPTION
  #define MASTER_PW_INIT_CLEAR        0x00000000
  #define MASTER_PW_INIT_ENCRYPTED    1234

  #define MASTER_PW_TEST_1_CLEAR      1234
  #define MASTER_PW_TEST_1_ENCRYPTED  2468
//  #define MASTER_PW_TEST_1_CONF       214
#define MASTER_PW_TEST_1_CONF       107

  #define SERVICE_PW_INIT_CLEAR       0x00000000
  #define SERVICE_PW_INIT_ENCRYPTED   1234

  #define SERVICE_PW_TEST_1_CLEAR     9
  #define SERVICE_PW_TEST_1_ENCRYPTED 1243
//  #define SERVICE_PW_TEST_1_CONF      9
#define SERVICE_PW_TEST_1_CONF      4
  #define ANY_PW_FOR_BUFFER_TEST      9999

#define UUID32_TEST                 0xFFFFFFFF
#else
  #define MASTER_PW_INIT_CLEAR        0x00000000
  #define MASTER_PW_INIT_ENCRYPTED    0x00000000

  #define MASTER_PW_TEST_1_CLEAR      1234
  #define MASTER_PW_TEST_1_ENCRYPTED  0x7636f114
//  #define MASTER_PW_TEST_1_CONF       214
#define MASTER_PW_TEST_1_CONF       107

  #define SERVICE_PW_INIT_CLEAR       0x00000000
  #define SERVICE_PW_INIT_ENCRYPTED   0x00000000

  #define SERVICE_PW_TEST_1_CLEAR     10
  #define SERVICE_PW_TEST_1_ENCRYPTED 0xd3f3c81c
//  #define SERVICE_PW_TEST_1_CONF      10
#define SERVICE_PW_TEST_1_CONF      5
  #define ANY_PW_FOR_BUFFER_TEST      9999
 #define UUID32_TEST                 0xFFFFFFFF
#endif

//*********************************************************


STATIC uint32_t mskPwEncrypt(uint32_t);



#endif
