/******************** (C) COPYRIGHT OSRAM GmbH ****************************************************************************************************************
* File Name         : DaliServices.h
* Description       : Declaration of the Interface-functions from the DALI Ballast library to device specific code, needed for the Dali protocol
*                     Functions must be defined by the developer of the device (s. DaliServicesTemplate.c)
* Author(s)         : Jutta Avril (JA) DS D LMS-COM DE-2
* Created           : 04.08.2014 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**************************************************************************************************************************************************************/
/// \file

// function prototypes for application functions executed in different intervals / instants
void DaliServices_Init( uint8_t channels );                                     // Called at startup to init the application
void DaliServices_Action( uint8_t channels );                                   // Called each Background loop
void DaliServices_FastTimer( uint8_t channels );                                // Called each GLOBAL_FastTIME_TICK_ms (2.5 ms)
void DaliServices_SlowTimer( uint8_t channels );                                // Called each GLOBAL_SlowTIME_TICK_ms (25.0 ms)

// Dedicated functions for Dali-support
uint8_t DaliServices_GetPowerOnDelay_FTT( void );
uint8_t DaliServices_GetFailure( uint8_t channel );                             // mandatory:
uint8_t DaliServices_GetOperatingMode( uint8_t channel );
// Dali Type6
uint8_t DaliServices_GetType6_GearType( void);
uint8_t DaliServices_GetType6_OperatingModes( void);
uint8_t DaliServices_GetType6_Features( void);
uint8_t DaliServices_GetMinFastFadeTime( void);

#ifdef DALI_CONFIG_CORRECT_FADE
void DaliServices_FadeOff( int32_t fadeRate, uint16_t actLevel, uint8_t channel );  //
#endif

// function prototypes to be called upon interruption / recovery of power supply
void DaliServices_PowerDown( void );                                         // Stops all application processes and sets the C-parts used by the application into power saving state
void DaliServices_Restart( uint8_t channels );                               // Restores the µC settings after power-down as needed by the application
void DaliServices_ResetDevice(void);                                         // Forces Watchdog-Timer. If not already done in DaliServices_WriteStepNvm(): Store non-volatile memory
void DaliServices_StoreNvm(void);
void ErrorShutdown(char* s);


void GOD_Request_nvmWriteAll(void);

void NfcPowerDownPrepare( void ) ;

void NfcWatchdogResetRequest( void );

void NfcPowerDownSleep( void ) ;


void NfcPowerDownResume( void ); 
