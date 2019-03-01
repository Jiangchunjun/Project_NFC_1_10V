/******************** (C) COPYRIGHT 2012 - 2013 OSRAM AG *********************************************************************************
* File Name         : MemoryBank_Emergency.h
* Description       : Interface to MemoryBank Emergency (ZDD 2856403-000-00) and related functions, MemoryBank-Id=11.
* Author(s)         : Jutta Avril (JA)
* Created           : 12.12.2012 (JA): Related spec. ZDD 2856403-000-00 (Version 0)
* Modified by       : DATE       (JA): Replace checksum by indicatorByte = 0 (ZDD 2702387
**********************************************************************************************************************************/

#define MPC_Emergency_ID             11
#define MPC_Emergency_VERSION         0                                         /// \brief The version of the Emergency page definition.
#define MPC_Emergency_LastADDRESS     7

#ifndef MPC_EMERGENCY_LIGHT_DC                                                  /// \brief Default values, if no device-specific definitions
  #define MPC_EMERGENCY_LIGHT_DC    255
#endif
#ifndef MPC_EMERGENCY_CONFIGURATION
  #define MPC_EMERGENCY_CONFIGURATION  0
#endif
#if (MPC_EMERGENCY_CONFIGURATION > 6)
#endif

// Private functions of DaliLibrary: Memory Bank handling
void MemoryBank_Emergency_Init(void);
void MemoryBank_Emergency_Reset( uint8_t channel );

mpc_rw_enum MemoryBank_Emergency_GetValue( uint8_t address, uint8_t *value, uint8_t channel );
mpc_rw_enum MemoryBank_Emergency_SetValue( uint8_t address, uint8_t value, uint8_t channel );
//not needed: void MemoryBank_Emergency_SlowTimer( uint8_t channel );

// Public functions:
void Emergency_StartDC(uint8_t channels);
void Emergency_StopDC(uint8_t channels);
bool Emergency_IsDaliLocked(uint8_t channel);                                   // returns true, if Dali Parameters cannot be updated
bool Emergency_IsActive(uint8_t channel);                                       // returns true, if DC-level is active
