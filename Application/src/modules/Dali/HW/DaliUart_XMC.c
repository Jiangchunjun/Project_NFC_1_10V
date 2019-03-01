/******************** (C) COPYRIGHT 2007 - 2015 OSRAM AG *********************************************************************************
* File Name         : DaliUart_XMC.c
* Description       : Hardware-dependent part of XMC1xxx for the DALI-Rx and DALI-Tx functions (InputCapture and OutputCompare)
*                     #ifdefs: configDaliUartSoftSync (times for fading and answer frame derived from command frame)
* Author(s)         : Jutta Avril (JA)
* Created           : 14.09.2013 (JA)
* Modified by       : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
**********************************************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

#define MODULE_DALI
#include "config.h"
#include "XMC1xxx.h"
//#include "DaliDefs.h"
//#include "DaliTypes.h"

#include "DaliUartHAL.h"
#include "DaliUart_XMC1xxx.h"
#include "xmc1300_ports_hwdriver.h"

typedef enum
{
    DALI_SetClock_NOT_SUPPORTED,
    DALI_SetClock_DONE,
    DALI_SetClock_NOT_DONE
} TypeDaliSetClock;

TypeDaliSetClock DaliHAL_SetClock(uint32_t mclk_MHz);

///<- data structure for DaliTx control
void DaliUart_RxCaptureISR(void);
void DaliUart_TxCompareISR(void);

///<- data structure for DaliTx control
#define MAX_IDX_TX     7                                                       // MAX_IDX_TX = (2^x -1), with x avoiding buffer overrun!
typedef struct {
  uint16_t pattern[MAX_IDX_TX+1][2];                                            // buffer for both idleState (2nd index 0) and activeState (2nd index 1)
  uint8_t sendIdx;                                                              //
  uint8_t receiveIdx;                                                           //
} tx_t;

typedef struct {
  tx_t tx;
} daliHAL_t;
daliHAL_t daliHAL;


#define MAX_IDX_DaliCapture     31                                              // MAX_IDX_DaliCapture = (2^x -1), with x avoiding buffer overrun!
typedef struct {                                                                // times are initialized/updated by DaliUart.c, read-only by DaliUart_HAL.c
  uint8_t index;
  uint16_t times[MAX_IDX_DaliCapture+1];
} TypeDaliCapture;
TypeDaliCapture daliCapture;	                                                //

/******************************************************************************/
TypeDaliVoltage DaliHAL_Init(void)
{
  ///<- Configure TIMER registers (Timer control registers are all 16 bit):
//MSK  ENABLE_DALI_TIMER_CLOCK_Rx;                                                   // enable Clock to DALI_TIM
  INIT_DALI_TIMER;                                                              // Clear CR1, CR2, set Timer to unlimited upcounting, set prescaler to count µs
  ENABLE_DALI_TIMER_CLOCK_Rx;                                                   //MSK // enable Clock to DALI_TIM
  INIT_DALI_RxCAPTURE;                                                          // select InputCapture mode
  ENABLE_DALI_TIMER_CLOCK_Tx;
  ENABLE_DALI_TxCOMPARE;
  FORCE_IDLE_DALI_TxCOMPARE;
//DALI_TIM_Tx->PSL |= CCU4_CC4_PSL_PSL_Msk;                                     // Set passiveLevel = High
  DALI_TIM_Tx->CMC = 0;                                                         // clear inputCapture settings
  ENABLE_SHADOW_TRANS_Tx;                                                       // enable loading of PSL immediately (timer is not running yet)
  DISABLE_DALI_TIMER_CLOCK_Tx;                                                  // stop clock after configuration

///<- Then configure GPIO (registers are all 32 bit:  CRL, CRH, IDR, ODR, BSRR, BRR, LCKR)
  DALI_GPIO_Rx_INIT;                                                            // Start clock, set pin to input, no pull-up,
#ifdef DALI_CONFIG_RX_PULLUP
  hw_gpio_set_mode(P0_6, XMC_GPIO_MODE_INPUT_PULL_UP);
#endif
  DALI_GPIO_Tx_INIT_IDLE;

/*
#ifdef LIB_DEBUG
  DALI_GPIO_Tx_SET_ACTIVE;				//set output always to push/pull in debug mode
  ENA_SHADOW_TRANS_Tx;
  SET_TxDALI_onCOMPARE;
  DALI_TIM_Tx->TCCLR = CCU4_CC4_TCCLR_TCC_Msk;
#endif
*/
  return DaliHAL_GetVoltage();
}

///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief                (Re)starts the DaliUart hardware in InputCapture mode.
/// \param                nothing
/// \retval               nothing
/// \calling extern       none
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
uint8_t DaliHAL_Start(void) {
  daliHAL.tx.receiveIdx = 0;
  daliHAL.tx.sendIdx = 0;
  CLEAR_DALI_RxCAPTURE_FLAG;
  if (IS_DALI_RxLEVEL_HIGH) {                                                   ///<- Start InputCapture: select polarity of captured edge ..
    daliCapture.index = 0;                                                      // .. rising edges are captured to even index (index ist pre-incremented)
    START_DALI_RxCAPTURE_FALLING;
  }
  else {
    daliCapture.index = 1;                                                      // .. falling edges are captured to odd index (index ist pre-incremented)
    START_DALI_RxCAPTURE_RISING;
  }
  START_DALI_TIMER_Rx;                                                          ///<- Start Timer (if not already running)
  ENABLE_DALI_RxCAPTURE_IRQ;
  return daliCapture.index;
}

///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief                Stops DaliUart functions and sets the ÂµC-parts used by DaliUart into power saving state.
/// \param                nothing
/// \retval               nothing
/// \calling extern       none
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
void DaliHAL_Stop( void ) {
#ifdef DALI_DATA_OUT      // defined in IAR-Project!
  if (daliUartHALStop < 0xFFFF) {daliUartHALStop++;}
#endif
  DALI_GPIO_Tx_SET_IDLE;                                                        // avoid power consumption (if DaliTx is active, answer is finalized "internally", because interrupts are not disabled)
}

//------------------------------------------------------------------------------
//
TypeDaliVoltage DaliHAL_GetVoltage(void) {
  if (IS_DALI_RxLEVEL_HIGH) {return DALI_HIGH;}
  else                      {return DALI_LOW;}
}


///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief                Provides the actual time to the non-hardware dependend part.
/// \param                nothing
/// \retval               actual 16 bit value of the Dali-Timer in µs
/// \calling extern       none
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
uint16_t DaliHAL_GetTimer_us(void) {
  return READ_DALI_RxTIMER;
}

///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief                Provides the captured times to the non-hardware dependend part.
/// \param                readIndex: index to last edge read
///                       *captTime: pointer where to store the captured time of readIndex in µs
///                       *actTime: pointer where to store the actual time in µs
///                       *readIndex: pointer to the index of last edge read (writable for limiting to the legal range)
/// \retval               index of the last edge captured
/// \calling extern       none
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//   | Start-bit   | D_n          | D_(n-1)
// __        ______ _______
//   |______|      |_______|______|______|
//   0      1      0       1                last bit of capture index
//-

uint8_t DaliHAL_GetCapture(uint16_t *captTime, uint16_t *actTime, uint8_t *readIndex) {
  *readIndex &= MAX_IDX_DaliCapture;                                            // limit read index to legal range
  *captTime = daliCapture.times[*readIndex];
  *actTime = READ_DALI_RxTIMER;                                                 //

  return daliCapture.index;                                                     // first read time, then index!
}

//-------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief ISR - Interrupt to store a captured edge.
///
/// Variable time with 1 µs resolution. Overrun of readIdx is not checked: Evaluation must create an error, if diffTimes between edges are close to 2^16
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
inline void DaliUart_RxCaptureISR( void )                                       //
{
  uint8_t captureIdx, risingEdge;

  captureIdx = (daliCapture.index+1) & MAX_IDX_DaliCapture;                     // preincrement index
  daliCapture.times[captureIdx] = READ_DALI_RxCAPTURE_REG;                      // capture also, if overrun, for timeout check
  daliCapture.index = captureIdx;
  TOGGLE_DALI_RxCAPTURE;

  risingEdge = captureIdx&0x01;                                                 // next edge to capture (falling edges are captured to odd index)
  if (   ( !(IS_DALI_RxLEVEL_HIGH) && (risingEdge == 0))                        // detect a missing edge: if level is low and next capture is falling edge ..
      ||    (IS_DALI_RxLEVEL_HIGH && (risingEdge != 0))  ) {                    // .. or vice versa
    FORCE_DALI_RxCAPTURE_IRQ;                                                   // captures the actual timer upon force.
  }
}

///--------------------------------------------------------------------------------------------------------------------------------
/// \brief                Triggers sending an answer to Dali command (manchester frame of 8 bit).
/// \param                startTimeDelay Delay time between last rising edge of receive frame and first falling edge of answer-frame.
/// \retval               none
/// \calling extern       none
//---------------------------------------------------------------------------------------------------------------------------------
//
bool DaliHAL_AddTxPattern(uint16_t idleStateTime_us, uint16_t periodTime_us) {
  uint8_t writeIdx = daliHAL.tx.receiveIdx;

  if (IS_DALI_Tx_TIMER_RUNNING) {                                               // add pattern
    uint8_t endIdx = (writeIdx+1) & MAX_IDX_TX;
    if (endIdx != daliHAL.tx.sendIdx) {
      daliHAL.tx.pattern[writeIdx][0] = idleStateTime_us;
      daliHAL.tx.pattern[writeIdx][1] = periodTime_us;
      daliHAL.tx.pattern[endIdx][0] = periodTime_us+1;                          // no edges: Compare > Period: indicates that telegram finished
      daliHAL.tx.pattern[endIdx][1] = periodTime_us;
      daliHAL.tx.receiveIdx = endIdx;
      return true;
    }
    else {
      return false;                                                             // buffer already full
    }
  }
  else {                                                                        // start transmission
    if (daliHAL.tx.receiveIdx == 0 ) {
      ENABLE_DALI_TIMER_CLOCK_Tx;
      DALI_TIM_Tx->CRS = idleStateTime_us;                                      // keep active level while startup time and ..
      DALI_TIM_Tx->PRS = periodTime_us;                                         // .. activate pattern upon next call
      daliHAL.tx.receiveIdx = 1;
      daliHAL.tx.sendIdx = 0;                                                   //
    }
    else {
      ENABLE_SHADOW_TRANS_Tx;                                                   // enable loading of startperiod immediately (timer is not running yet)
      START_DALI_Tx_TIMER;
      PPB->NVIC_ICPR = DALI_CMPR_IRQ_MSK;   	                                  // clear interrupt pending flag in NVIC
      DALI_TIM_Tx->SWR |= CCU4_CC4_SWR_RPM_Msk;                                 // clear "PeriodMatch"-flag
      DALI_TIM_Tx->INTE |= CCU4_CC4_INTE_PME_Msk;                               // enable interupt "PeriodMatch"
      DALI_GPIO_Tx_SET_ACTIVE;
      DALI_TIM_Tx->CRS = idleStateTime_us;                                      // 1st and 2nd pattern is not stored to buffer!
      DALI_TIM_Tx->PRS = periodTime_us;
      ENABLE_SHADOW_TRANS_Tx;                                                   // enable reload of new level/period with next period overflow
      daliHAL.tx.pattern[1][0] = periodTime_us+1;                               // no edges: Compare > Period to finalize frame
      daliHAL.tx.pattern[1][1] = periodTime_us;
    }
    return true;
  }
}

//---------------------------------------------------------------------------------------------------------------------------
/// \brief ISR - Interrupt on OutputCompare each interval of 416 Ö³ (nominal) - used to perform the DaliTx state machine.
//
//   | Start-bit   | D_n          | D_(n-1)
// __        ______ _______
//   |______|      |_______|______|______|
//   D->M   M->D   D-M
//
//---------------------------------------------------------------------------------------------------------------------------
//
inline void DaliUart_TxCompareISR( void )
{
  uint16_t period = DALI_TIM_Tx->PR;                                            // define variable to compile without warning (comparision of two ÂµC-registers):
  if ( DALI_TIM_Tx->CR > period ) {                                             // if last pattern (only to force IRQ after last frame-pattern)
    STOP_DALI_TIMER_Tx;                                                         // stop and clear timer
    DALI_TIM_Tx->INTE &= (~CCU4_CC4_INTE_PME_Msk);                              // DISABLE_DALI_TxCOMPARE_IRQ;
    DALI_GPIO_Tx_SET_IDLE;                                                      // read-modify-write must not be interrupted!
    DISABLE_DALI_TIMER_CLOCK_Tx;
    daliHAL.tx.receiveIdx = 0;                                                  // indicates, that all received pattern of telegram was sent
  }
  else {                                                                        // pattern running, write next pattern to shadow registers
    uint8_t sendIdx = daliHAL.tx.sendIdx;
    sendIdx = (sendIdx+1) & MAX_IDX_TX;
    daliHAL.tx.sendIdx = sendIdx;
    DALI_TIM_Tx->CRS = daliHAL.tx.pattern[sendIdx][0];
    DALI_TIM_Tx->PRS = daliHAL.tx.pattern[sendIdx][1];
    ENABLE_SHADOW_TRANS_Tx;                                                     // enable reload of new level/period with next period overflow
  }
}

/* Following function may be reactivated. If so, function declaration must be provided in a header different from DaliUart_HAL.h! */
///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief                Changes CPU-clock to the value received as parameter (if supported).
/// \param                CPU-clock in MHz to be applied together with correction of the prescaler to Dali-Rx and Dali-Tx timers()
/// \retval               Result DALI_SetClock_NOT_SUPPORTED / DALI_SetClock_DONE / DALI_SetClock_NOT_DONE
//-------------------------------------------------------------------------------------------------------------------------------------------------------------
//
TypeDaliSetClock DaliHAL_SetClock(uint32_t mclk_MHz) {
  uint32_t updateCLKCR;                                 // to update 32-bit register ClockVontrolRegister CLKCR in SystemVontrolUnit SCU, i.e. type UL
  uint32_t updatePrescaler;                             // to update 32-bit register, i.e. type UL

  updateCLKCR = 0x3FF00000;
  updateCLKCR |= (1UL << (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Pos) & (uint32_t)SCU_CLK_CLKCR_PCLKSEL_Msk;    // always set peripheral clock = 2x main clock!

  switch(mclk_MHz) {
  default:
    return DALI_SetClock_NOT_SUPPORTED;                                         // RETURN!!!!!!!!!
  case 1:
    updateCLKCR |= (64UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 0UL;
    break;
  case 2:
    updateCLKCR |= (32UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 1UL;
    break;
  case 4:
    updateCLKCR |= (16UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 2UL;
    break;
  case 8:
    updateCLKCR |= (8UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 3UL;
    break;
  case 16:
    updateCLKCR |= (4UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 4UL;
    break;
  case 32:
    updateCLKCR |= (2UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 5UL;
    break;
  case 64:
    // set divider IDIV for main clock (reset value of CLKCR: 0x3FF00400) -> IDIV=bit8..15
    updateCLKCR |= (1UL << (uint32_t)SCU_CLK_CLKCR_IDIV_Pos) & (uint32_t)SCU_CLK_CLKCR_IDIV_Msk;
    updatePrescaler = 6UL;
    break;
  }
  if (updatePrescaler > DALI_TIM_Rx->PSC) {
  __disable_interrupt();
    DALI_TIM_Rx->PSC = updatePrescaler;                                         // if PCLK is rising, change prescaler of timer before the one for PCLK
    DALI_TIM_Tx->PSC = updatePrescaler;
    //----CLOCK-SETUP---------------------------------------------------------------------------------
    SCU_GENERAL->PASSWD = 0x000000C0UL;                                                 // disable write protection to SCU registers
    //  WR_REG(SCU_CLK->CLKCR, SCU_CLK_CLKCR_CNTADJ_Msk, SCU_CLK_CLKCR_CNTADJ_Pos, 0x3FFU); // set delay for stabilizing VCC to 16us
    SCU_CLK->CLKCR = updateCLKCR;
    SCU_GENERAL->PASSWD = 0x000000C3UL;                                                 // enable write protection to SCU registers
  __enable_interrupt();
  }
  else if (updatePrescaler < DALI_TIM_Rx->PSC){
  __disable_interrupt();
    //----CLOCK-SETUP---------------------------------------------------------------------------------
    SCU_GENERAL->PASSWD = 0x000000C0UL;                                                 // disable write protection to SCU registers
  //  WR_REG(SCU_CLK->CLKCR, SCU_CLK_CLKCR_CNTADJ_Msk, SCU_CLK_CLKCR_CNTADJ_Pos, 0x3FFU); // set delay for stabilizing VCC to 16us
    SCU_CLK->CLKCR = updateCLKCR;
    SCU_GENERAL->PASSWD = 0x000000C3UL;                                                 // enable write protection to SCU registers
    //------------------------------------------------------------------------------------------------
    DALI_TIM_Rx->PSC = updatePrescaler;                                         // if PCLK is reduced, change prescaler of timer after the one for PCLK
    DALI_TIM_Tx->PSC = updatePrescaler;
  __enable_interrupt();
  } // no else: no update, if updatePrescaler == DALI_TIM_Rx->PSC
  return DALI_SetClock_DONE;
}

///--- p r i v a t e   f u n c t i o n s ---------------------------------------
/*
#define INIT_DALI_TIMER                       {DISABLE_PSSWRD_PRTCTN; \
											  CLR_MCLCK_DIVIDER; \
											  SET_MCLCK_DIVIDER; \
											  CLR_PCLK_MCLK_RATIO; \
											  SET_PCLK_MCLK_RATIO; \
											  ENA_CCU4_CLOCK; \
											  CLR_DALI_INTRPT_PENDING; \
											  ENABLE_DALI_INTRPT_NODE; \
                                              START_CCU4_PRESCALER; \
                                              CLR_CCU4_SUSPEND; \
                                              SET_CCU4_SAFE_STOP; \
                                              \
                                              SET_DALI_Rx_MAX_PERIOD; \
                                              SET_DALI_Rx_PRESCALER; \
                                              SET_DALI_Rx_SERVICE; \
                                              ENA_SHADOW_TRANS_Rx; \
                                              CONF_DALI_Rx_TIMER; \
  	  	  	  	  	  	  	  	  	  	  	  \
  	  	  	  	  	  	  	  	  	  	  	  CLR_DALI_Tx_PERIOD; \
                                              SET_DALI_Tx_PRESCALER; \
                                              SET_DALI_Tx_SERVICE; \
                                              CONF_DALI_Tx_TIMER;}
*/
/*
void DaliHALinitTimer(void) {

}
*/
