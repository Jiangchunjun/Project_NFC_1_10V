/******************** (C) COPYRIGHT OSRAM GmbH ****************************************************************************************************************
* File Name         : MpcServices.h
* Description       : Declaration of Interface-functions to provide device specific code, needed for the Osram-specific MemoryBanks.
*                     Functions must be defined by the developer of the device (s. DaliServicesTemplate.c)
* Author(s)         : Jutta Avril (JA)
* Created           : 04.08.2014 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**************************************************************************************************************************************************************/
/// \file

// function prototypes for application functions executed in different intervals / instants (called in main.c)
void MpcServices_Init( uint8_t channels );                                     // Called at startup to init the application

#ifdef MPC_POWER_NUMBER
uint32_t MpcServices_GetPowerConsumption( uint8_t channel );
#endif