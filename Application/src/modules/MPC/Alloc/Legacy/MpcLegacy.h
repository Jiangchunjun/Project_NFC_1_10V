/******************** (C) COPYRIGHT 2015 OSRAM AG **********************************************************************************
* File Name         : Mpc.h
* Description       : Interface for read and write access to MultiPurposeCenters
* Author(s)         : Jutta Avril (JA), DS D LMS-COM DE-2
* Created           : 11.12.2015 (JA)
* Modified          : Date Author Description
******************************************************************************************************************************************/



mpc_rw_enum Mpc_Read( uint16_t bank, uint8_t address, uint8_t *value );
mpc_rw_enum Mpc_Write( uint16_t bank, uint8_t address, uint8_t value );
uint8_t Mpc_GetLength(uint16_t bank);