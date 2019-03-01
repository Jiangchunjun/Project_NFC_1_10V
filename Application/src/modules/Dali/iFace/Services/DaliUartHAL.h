/******************** (C) COPYRIGHT 2013 - 2016 OSRAM OEC **********************************************************
* File Name        : DaliUartHAL.h
* Author(s)        : Jutta Avril(JA)
* Created by       : 01.01.13   JA
* Description      : Interface to the µC-dependent code of the DALI Uart
***************************************************************************************************************************************************/

// function prototypes called by DaliBallastLibrary
typedef enum {
  DALI_LOW,
  DALI_HIGH,
  DALI_UNKNOWN
} TypeDaliVoltage;
TypeDaliVoltage DaliHAL_Init(void);                                             ///< \brief Initialises µC-registers and variables. Returns the status at Dali-terminals. Called from DaliUartInit( void )
TypeDaliVoltage DaliHAL_GetVoltage(void);

uint16_t DaliHAL_GetTimer_us(void);                                             ///< \brief returns the actual Timer in µs
uint8_t DaliHAL_GetCapture(uint16_t *captTime, uint16_t *actTime, uint8_t *readIndex);    ///< \brief Read-function to forwards captured times to daliUart. returns the actual captureIndex (last bit=0 means risingEdge, i.e. level is high)
bool DaliHAL_AddTxPattern(uint16_t idleStateTime_us, uint16_t periodTime_us);             ///< \brief Write-function to command outputCompare/PWM from daliUart.

uint8_t DaliHAL_Start( void );                                                  ///< \brief Starts the Dali-Timer, (Re)starts InputCapture
void DaliHAL_Stop( void );                                                      ///< \brief Stops DaliUart functions and sets the µC-parts used by DaliUart into power saving state
