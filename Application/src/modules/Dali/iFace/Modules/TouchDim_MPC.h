/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : TouchDimMPC.h
* Author             : Jutta Avril
* Date               : 12/12/2012
* Description        : MemoryBank, Id=4, TouchDim/Corridor, ZDD2784484
*******************************************************************************/

#define MPC_TouchDim_LastADDRESS     0x15
#define MPC_TouchDim_ID                 4
#define MPC_TouchDim_VERSION            1

//void TouchDimMPC_Init( void ); Called by TouchDim_Init(), no call from external provided!
void TouchDimMPC_Reset( void );

mpc_rw_enum TouchDimMPC_GetValue( uint8_t address, uint8_t *value );
mpc_rw_enum TouchDimMPC_SetValue( uint8_t address, uint8_t value );



