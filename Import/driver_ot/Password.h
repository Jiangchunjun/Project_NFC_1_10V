/******************** (C) COPYRIGHT 2012 - 2013 OSRAM AG *********************************************************************************
* File Name         : Password.h
* Description       : Interface to password handlers.
* Author(s)         : Jutta Avril (JA), PL-LMS TRT
* Created           : 14.03.2014 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**********************************************************************************************************************************/



typedef enum {PASSWORD_NOT_OK = 0, PASSWORD_OK
} passwordCheck_t;

// checks whether testPassword is ok or not
passwordCheck_t PasswordCheck(uint16_t testPassword, uint16_t encryptedPassword);

// returns encryption of newPassword
uint16_t PasswordUpdate(uint16_t newPassword);

