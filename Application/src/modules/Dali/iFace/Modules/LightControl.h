//-------------------------------------------------------------------------------------------------------------------------------------------------------------
///   \file LightControl.h
///   \since 2017-02-03
///   \brief Interface declarations and definitions to light control (fading, holding, blinking etc.). Closed loop control prepared.
///
/// DISCLAIMER:
///   The content of this file is intellectual property of the Osram GmbH. It is
///   confidential and not intended for public release. All rights reserved.
///   Copyright © Osram GmbH
//-------------------------------------------------------------------------------------------------------------------------------------------------------------

///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// L i g h t C o n t r o l - I n t e r f a c e:   C o n st an t s      ---------------------------------------------------------------------------------------

  #define LC_MIN_LEVEL      0x0100                                              // absolute limit < lightControl.minLevel. Use this value to force limitation. Must be > 1
  #define LC_MAX_LEVEL      0xFE00                                              // absolute limit > lightControl.maxLevel. Use this value to force limitation. Must be < 0xFFFF
  #define LC_FastTIME_TICK_ms    GLOBAL_FastTIME_TICK_ms                        // define the time base of the light control module (for accurate results set 1, 2, 2.5, 5, 10, 20, 25, 50 ms!)

///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// L i g h t C o n t r o l - I n t e r f a c e:   F u n c t i o n   P r o t o t y p e s   --------------------------------------------------------------------

void LightControl_Init(uint8_t channel);
void LightControl_FastTimer(uint8_t channels);                                  // to be called each time slice of LC_FastTIME_TICK_ms

// LightControl configuration
typedef enum {
    LC_DimCURVE_LOG       = 0,                                                  // used as value, in case of LC_CONFIG_DimCURVE (according to DALI standard)
    LC_DimCURVE_LIN       = 1,
} TypeDimCURVE;
typedef enum {
    LC_CONFIG_MIN,
    LC_CONFIG_MAX,
    LC_CONFIG_CURVE
} TypeLightCONFIG;
void LightControl_Configure(TypeLightCONFIG selection, uint8_t parameter, uint8_t channel);
void LightControl_CheckLimits(uint8_t minLevelUpdate, uint8_t channel);


// LightControl queries
uint16_t LightControl_GetLevel(uint8_t channel);                                // returns the actual Dali level
uint16_t LightControl_ActLevel(uint8_t channel);                                // returns the actual level (maybe overwrite value)
uint16_t LightControl_GetLinearLevel(uint8_t channel);                          // returns the actual lamp level, i.e. the linear command
uint8_t LightControl_GetLastActiveLevel(uint8_t channel);
uint8_t LightControl_GetTarget(uint8_t channel);                                // returns the actual target level
uint8_t LightControl_GetPhysMin(TypeDimCURVE dimCurve);                         // calculates physMinLevel dependent on dimCurve and physMinLevel, queried by LightControlServices
uint8_t LightControl_GetMinLevel(uint8_t channel);                              // returns the actual, limited setting
uint8_t LightControl_GetMaxLevel(uint8_t channel);                              // returns the actual, limited setting

#define LightControlSTATE_FADE_FLAG     0x04
typedef enum {
//    LightControlSTATE_START             = 0,                                     // reserved for startup condition:
    LightControlSTATE_CONSTANT          = 0x01,
    LightControlSTATE_HOLDING           = 0x02,
    LightControlSTATE_FADING            = LightControlSTATE_FADE_FLAG,
    LightControlSTATE_FADE_START        = LightControlSTATE_FADE_FLAG + 0x10,
    LightControlSTATE_FADERATE          = LightControlSTATE_FADE_FLAG + 0x02,
    LightControlSTATE_FADESTEP          = LightControlSTATE_FADE_FLAG + 0x03,

//    LightControlSTATE_FBCONTROL   = 0x20,
    LightControlSTATE_BLINK             = 0x08,

    LightControlSTATE_OFF               = 0x40,
    LightControlSTATE_FADING_FINISHED   = 0x81,                                 // no longer fading, do not OR with LightControlSTATE_FADE_FLAG
    LightControlSTATE_HOLDING_FINISHED  = 0x82,
    LightControlSTATE_BLINKING_FINISHED = 0x88
} TypeLightControlState;
TypeLightControlState LightControl_GetState(uint8_t channel);                   // returns the actual state of background tasks
// definitions of the return value:
// #define LightControlSTATE_FINISHED    0x80                                   // ORed with the actual state, if finalized

bool LightControl_GetLimitation(uint8_t channel);                               // returns the actual state of the limitation flag

// LightControl commands
void LightControl_SetLevel(uint16_t target, uint8_t channel);                   //
void LightControl_AddStep(int16_t step, uint8_t channel);                       // for fading from external (e.g. TouchDim)
void LightControl_StartFadeTime(uint16_t target, uint16_t fadeTime_0sx, uint8_t channel);                         // start fading for selected fade time (in 0.1 s) to target in background
typedef enum {
  LC_FADERATE_UP = 0,
  LC_FADERATE_DOWN,
  LC_FADERATE_TO_OFF
} TypeLC_FadeRateDIR;
void LightControl_StartFadeRate(uint8_t fadeRate, TypeLC_FadeRateDIR fadeRateDir, uint8_t channel);
void LightControl_StopFade(uint8_t channel);                                    // stops a running fading

void LightControl_StartHolding(uint32_t holdTime_FTT, uint8_t channel);         // start holding for time "holdTime_FTT" in background
typedef enum {
  LightControlBLINK_ZERO_MAX,
  LightControlBLINK_MIN_MAX,
  LightControlBLINK_ZERO_ActMAX,
  LightControlBLINK_MAX,
  LightControlBLINK_MIN,
#if (DALI_CONFIG_DEVICE_TYPE == 7)
  LightControlBLINK_OFF,
#endif
  LightControlBLINK_ActMIN_ActMAX
} TypeLightControlBLINK;
void LightControl_StartBlinking(uint16_t blinkStep_FTT, uint8_t blinkSteps, TypeLightControlBLINK blinkMode, uint8_t channel);
void LightControl_StopBlinking(uint8_t channel);

void LightControl_OverwriteLevel(uint8_t level, uint8_t channel);

//bool LightControl_StartRegulation(uint8_t selectRegulation, uint8_t channel);       // future start one of the provided regulation  in background


///------------------------------------------------------------------------------------------------------------------------------------------------------------
/// L i g h t C o n t r o l - I n t e r f a c e:   E x p l a n a t i o n s   ----------------------------------------------------------------------------------
