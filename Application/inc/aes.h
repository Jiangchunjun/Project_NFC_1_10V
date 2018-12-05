/*
***************************************************************************************************
*                               AES Encrypt Algorithm
*
* File   : aes.h
* Author : Douglas Xie
* Date   : 2016.10.19
***************************************************************************************************
* Copyright (C) 2016 OSRAM Asia Pacific Management Company.  All rights reserved.
***************************************************************************************************
*/

#ifndef AES_H
#define AES_H


/* OSRAM Encrypt Function, one block of 16 bytes, length should be 16n */
void OsramEncrypt(unsigned char *data, unsigned int length, unsigned char key_version);

/* OSRAM Decrypt Function, one block of 16 bytes, length should be 16n */
void OsramDecrypt(unsigned char *data, unsigned int length, unsigned char key_version);

#endif /* AES_ENC_H */

/**************** (C) COPYRIGHT OSRAM Asia Pacific Management Company *********END OF FILE*********/
