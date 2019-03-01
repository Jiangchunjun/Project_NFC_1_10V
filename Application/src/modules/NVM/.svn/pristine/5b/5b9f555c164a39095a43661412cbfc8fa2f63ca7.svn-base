#ifndef __MESSAGE_FLASH_SIZE_H
#define __MESSAGE_FLASH_SIZE_H

/** \addtogroup NvmInternal
 * \{
 *
 * \file
 * \brief Flash data size message for nvm.c
 *
 * \defgroup NvmFlashSizeMessage Nvm Size Message
 * \brief Private file for nvm size message generation during compilation
 * \{
 */

#define NVM_SIZE_BYTE3   (__NVM_AREA_TOT_SIZE_BYTE >> 12 )
#define NVM_SIZE_BYTE2 ( (__NVM_AREA_TOT_SIZE_BYTE - (NVM_SIZE_BYTE3 << 12) ) >> 8 )
#define NVM_SIZE_BYTE1 ( (__NVM_AREA_TOT_SIZE_BYTE - (NVM_SIZE_BYTE3 << 12) - (NVM_SIZE_BYTE2 << 8) ) >> 4 )

#if NVM_SIZE_BYTE3     == 0
  #define NVM_SIZE_BYTE3S 0
#elif NVM_SIZE_BYTE3   == 1
  #define NVM_SIZE_BYTE3S 1
#elif NVM_SIZE_BYTE3   == 2
  #define NVM_SIZE_BYTE3S 2
#elif NVM_SIZE_BYTE3   == 3
  #define NVM_SIZE_BYTE3S 3
#elif NVM_SIZE_BYTE3   == 4
  #define NVM_SIZE_BYTE3S 4
#elif NVM_SIZE_BYTE3   == 5
  #define NVM_SIZE_BYTE3S 5
#elif NVM_SIZE_BYTE3   == 6
  #define NVM_SIZE_BYTE3S 6
#elif NVM_SIZE_BYTE3   == 7
  #define NVM_SIZE_BYTE3S 7
#elif NVM_SIZE_BYTE3   == 8
  #define NVM_SIZE_BYTE3S 8
#elif NVM_SIZE_BYTE3   == 9
  #define NVM_SIZE_BYTE3S 9
#elif NVM_SIZE_BYTE3  == 10
  #define NVM_SIZE_BYTE3S A
#elif NVM_SIZE_BYTE3  == 11
  #define NVM_SIZE_BYTE3S B
#elif NVM_SIZE_BYTE3  == 12
  #define NVM_SIZE_BYTE3S C
#elif NVM_SIZE_BYTE3  == 13
  #define NVM_SIZE_BYTE3S D
#elif NVM_SIZE_BYTE3  == 14
  #define NVM_SIZE_BYTE3S E
#elif NVM_SIZE_BYTE3  == 15
  #define NVM_SIZE_BYTE3S F
#else
  #define NVM_SIZE_BYTE3S OVR
#endif

#if NVM_SIZE_BYTE2     == 0
  #define NVM_SIZE_BYTE2S 0
#elif NVM_SIZE_BYTE2   == 1
  #define NVM_SIZE_BYTE2S 1
#elif NVM_SIZE_BYTE2   == 2
  #define NVM_SIZE_BYTE2S 2
#elif NVM_SIZE_BYTE2   == 3
  #define NVM_SIZE_BYTE2S 3
#elif NVM_SIZE_BYTE2   == 4
  #define NVM_SIZE_BYTE2S 4
#elif NVM_SIZE_BYTE2   == 5
  #define NVM_SIZE_BYTE2S 5
#elif NVM_SIZE_BYTE2   == 6
  #define NVM_SIZE_BYTE2S 6
#elif NVM_SIZE_BYTE2   == 7
  #define NVM_SIZE_BYTE2S 7
#elif NVM_SIZE_BYTE2   == 8
  #define NVM_SIZE_BYTE2S 8
#elif NVM_SIZE_BYTE2   == 9
  #define NVM_SIZE_BYTE2S 9
#elif NVM_SIZE_BYTE2  == 10
  #define NVM_SIZE_BYTE2S A
#elif NVM_SIZE_BYTE2  == 11
  #define NVM_SIZE_BYTE2S B
#elif NVM_SIZE_BYTE2  == 12
  #define NVM_SIZE_BYTE2S C
#elif NVM_SIZE_BYTE2  == 13
  #define NVM_SIZE_BYTE2S D
#elif NVM_SIZE_BYTE2  == 14
  #define NVM_SIZE_BYTE2S E
#elif NVM_SIZE_BYTE2  == 15
  #define NVM_SIZE_BYTE2S F
#endif


#if NVM_SIZE_BYTE1     == 0
  #define NVM_SIZE_BYTE1S 0
#elif NVM_SIZE_BYTE1   == 1
  #define NVM_SIZE_BYTE1S 1
#elif NVM_SIZE_BYTE1   == 2
  #define NVM_SIZE_BYTE1S 2
#elif NVM_SIZE_BYTE1   == 3
  #define NVM_SIZE_BYTE1S 3
#elif NVM_SIZE_BYTE1   == 4
  #define NVM_SIZE_BYTE1S 4
#elif NVM_SIZE_BYTE1   == 5
  #define NVM_SIZE_BYTE1S 5
#elif NVM_SIZE_BYTE1   == 6
  #define NVM_SIZE_BYTE1S 6
#elif NVM_SIZE_BYTE1   == 7
  #define NVM_SIZE_BYTE1S 7
#elif NVM_SIZE_BYTE1   == 8
  #define NVM_SIZE_BYTE1S 8
#elif NVM_SIZE_BYTE1   == 9
  #define NVM_SIZE_BYTE1S 9
#elif NVM_SIZE_BYTE1  == 10
  #define NVM_SIZE_BYTE1S A
#elif NVM_SIZE_BYTE1  == 11
  #define NVM_SIZE_BYTE1S B
#elif NVM_SIZE_BYTE1  == 12
  #define NVM_SIZE_BYTE1S C
#elif NVM_SIZE_BYTE1  == 13
  #define NVM_SIZE_BYTE1S D
#elif NVM_SIZE_BYTE1  == 14
  #define NVM_SIZE_BYTE1S E
#elif NVM_SIZE_BYTE1  == 15
  #define NVM_SIZE_BYTE1S F
#endif

#endif

/** \} */ // NvmFlashSizeMessage
/** \} */ // NvmInternal
