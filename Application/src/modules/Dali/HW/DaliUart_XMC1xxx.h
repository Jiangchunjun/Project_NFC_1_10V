//******************************************************************************
#if (DEVICE_PERIPHCLK_MHz != DEVICE_SYSCLK_MHz)
  #undef DALI_CONFIG_TIMER_CLK_MHz
  #define DALI_CONFIG_TIMER_CLK_MHz       DEVICE_SYSCLK_MHz
  #define configDOUBLE_PRHP_CLK //MAX: damit PCLK = 2*MCLK
#endif

//----   D A L I    Interrupt Requests   ---------------------------------------
//
//----   config interrupt node   -----------------------------------------------
#if (DALI_CONFIG_CPTR_SRQ == 0)
  #define DALI_CPTR_IRQ                       CCU40_0_IRQn                        //common timer interrupt pointer 1
  #define DaliUart_RxCaptureISR                 CCU40_0_IRQHandler                  //address interrupt pointer 1 points on
  #define DALI_CPTR_IRQ_MSK            0x00200000UL            //IRQ21
#elif (DALI_CONFIG_CPTR_SRQ == 1)
  #define DALI_CPTR_IRQ                         CCU40_1_IRQn                        //common timer interrupt pointer 2
  #define DaliUart_RxCaptureISR                 CCU40_1_IRQHandler                  //address interrupt pointer 2 points on
  #define DALI_CPTR_IRQ_MSK            0x00400000UL            //IRQ22
#elif (DALI_CONFIG_CPTR_SRQ == 2)
  #define DALI_CPTR_IRQ                         CCU40_2_IRQn                        //common timer interrupt pointer 3
  #define DaliUart_RxCaptureISR                 CCU40_2_IRQHandler                  //address interrupt pointer 3 points on
  #define DALI_CPTR_IRQ_MSK            0x00800000UL            //IRQ23
#elif (DALI_CONFIG_CPTR_SRQ == 3)
  #define DALI_CPTR_IRQ                         CCU40_3_IRQn                        //common timer interrupt pointer 4
  #define DaliUart_RxCaptureISR                 CCU40_3_IRQHandler                  //address interrupt pointer 4 points on
  #define DALI_CPTR_IRQ_MSK            0x01000000UL            //IRQ24
#endif

#if (DALI_CONFIG_COMP_SRQ != DALI_CONFIG_CPTR_SRQ)
  #if (DALI_CONFIG_COMP_SRQ == 0)
    #define DALI_CMPR_IRQ                       CCU40_0_IRQn                        //common timer interrupt pointer 1
    #define DaliUart_TxCompareISR               CCU40_0_IRQHandler                  //address interrupt pointer 1 points on
    #define DALI_CMPR_IRQ_MSK          0x00200000UL            //IRQ21
  #elif (DALI_CONFIG_COMP_SRQ == 1)
    #define DALI_CMPR_IRQ                       CCU40_1_IRQn                        //common timer interrupt pointer 2
    #define DaliUart_TxCompareISR               CCU40_1_IRQHandler                  //address interrupt pointer 2 points on
    #define DALI_CMPR_IRQ_MSK          0x00400000UL            //IRQ22
  #elif (DALI_CONFIG_COMP_SRQ == 2)
    #define DALI_CMPR_IRQ                       CCU40_2_IRQn                        //common timer interrupt pointer 3
    #define DaliUart_TxCompareISR               CCU40_2_IRQHandler                  //address interrupt pointer 3 points on
    #define DALI_CMPR_IRQ_MSK          0x00800000UL            //IRQ23
  #elif (DALI_CONFIG_COMP_SRQ == 3)
    #define DALI_CMPR_IRQ                       CCU40_3_IRQn                        //common timer interrupt pointer 4
    #define DaliUart_TxCompareISR               CCU40_3_IRQHandler                  //address interrupt pointer 4 points on
    #define DALI_CMPR_IRQ_MSK          0x01000000UL            //IRQ24
  #endif
#else
  capture and compare timer have to use different interrupt requests (unit: %)
#endif

#define CLR_DALI_INTRPT_PENDING            PPB->NVIC_ICPR = (DALI_CMPR_IRQ_MSK | DALI_CPTR_IRQ_MSK)
#define ENABLE_DALI_INTRPT_NODE           PPB->NVIC_ISER = (DALI_CMPR_IRQ_MSK | DALI_CPTR_IRQ_MSK)
#define DISABLE_DALI_INTRPT_NODE          PPB->NVIC_ICER = (DALI_CMPR_IRQ_MSK | DALI_CPTR_IRQ_MSK)

#define FORCE_DALI_RxCAPTURE_IRQ                PPB->NVIC_ISPR = DALI_CPTR_IRQ_MSK     //force ISR
#define CLEAR_DALI_RxCAPTURE_FLAG               PPB->NVIC_ICPR = DALI_CPTR_IRQ_MSK    //clear interrupt pending flag
#define DALI_RxCAPTURE_IRQ_PENDING              ((PPB->NVIC_ISPR &= DALI_CPTR_IRQ_MSK) != 0)  //check if capture interrupt is pending

#define FORCE_DALI_TxCOMPARE_IRQ                PPB->NVIC_ISPR = DALI_CMPR_IRQ_MSK     //force ISR
#define CLEAR_DALI_TxCOMPARE_FLAG               PPB->NVIC_ICPR = DALI_CMPR_IRQ_MSK     //clear interrupt pending flag
#define DALI_TxCOMPARE_IRQ_PENDING              ((PPB->NVIC_ISPR &= DALI_CMPR_IRQ_MSK) != 0)  //check if compare interrupt is pending


//----   config service request   ----------------------------------------------
#define ENABLE_DALI_RxCAPTURE_IRQ               DALI_TIM_Rx->INTE = CCU4_CC4_INTE_E0AE_Msk         //disables all other cc service requests
#define DISABLE_DALI_RxCAPTURE_IRQ              DALI_TIM_Rx->INTE &= (~CCU4_CC4_INTE_E0AE_Msk)
#define DALI_RxCAPTURE_SRQ_ENABLED              ((DALI_TIM_Rx->INTE & CCU4_CC4_INTE_E0AE_Msk)!=0)   //ask for service request enabled
#if (DALI_CONFIG_CPTR_SRQ == 0)
  #define SET_DALI_Rx_SERVICE          DALI_TIM_Rx->SRS = 0x0000UL              //select CC40SR0 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_CPTR_SRQ == 1)
  #define SET_DALI_Rx_SERVICE          DALI_TIM_Rx->SRS = 0x0100UL              //select CC40SR1 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_CPTR_SRQ == 2)
  #define SET_DALI_Rx_SERVICE          DALI_TIM_Rx->SRS = 0x0200UL              //select CC40SR2 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_CPTR_SRQ == 3)
  #define SET_DALI_Rx_SERVICE          DALI_TIM_Rx->SRS = 0x0300UL              //select CC40SR3 for event0 (capture), CC40SR1 for period match (compare)
#endif

#define ENABLE_DALI_TxCOMPARE_IRQ              DALI_TIM_Tx->INTE = CCU4_CC4_INTE_PME_Msk          //disables all other cc service requests
#define DISABLE_DALI_TxCOMPARE_IRQ              DALI_TIM_Tx->INTE &= (~CCU4_CC4_INTE_PME_Msk)
#define DALI_TxCOMPARE_IRQ_ENABLED              ((DALI_TIM_Tx->INTE & CCU4_CC4_INTE_PME_Msk)!=0)    //ask for service request enabled
#if (DALI_CONFIG_COMP_SRQ == 0)
  #define SET_DALI_Tx_SERVICE          DALI_TIM_Tx->SRS = 0x0000UL              //select CC40SR0 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_COMP_SRQ == 1)
  #define SET_DALI_Tx_SERVICE          DALI_TIM_Tx->SRS = 0x0001UL              //select CC40SR1 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_COMP_SRQ == 2)
  #define SET_DALI_Tx_SERVICE          DALI_TIM_Tx->SRS = 0x0002UL              //select CC40SR2 for event0 (capture), CC40SR1 for period match (compare)
#elif (DALI_CONFIG_COMP_SRQ == 3)
  #define SET_DALI_Tx_SERVICE          DALI_TIM_Tx->SRS = 0x0003UL              //select CC40SR3 for event0 (capture), CC40SR1 for period match (compare)
#endif

//----   D A L I    Interrupt Requests   ---------------------------------------
//******************************************************************************



//******************************************************************************
//----   D A L I    R x   ------------------------------------------------------
//
//----  general timer definitions  ---------------------------------------------
#if (DALI_CONFIG_Rx_TIMER == 0)
  #define DALI_TIM_Rx                         CCU40_CC40                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Rx          CCU40->GIDLC |= CCU4_GIDLC_CS0I_Msk   //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Rx         CCU40->GIDLS |= CCU4_GIDLS_SS0I_Msk   //set idle mode stops clock
  #define ENA_SHADOW_TRANS_Rx                 CCU40->GCSS |= CCU4_GCSS_S0SE_Msk

#elif (DALI_CONFIG_Rx_TIMER == 1)
  #define DALI_TIM_Rx                         CCU40_CC41                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Rx          CCU40->GIDLC |= CCU4_GIDLC_CS1I_Msk   //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Rx         CCU40->GIDLS |= CCU4_GIDLS_SS1I_Msk   //set idle mode stops clock
  #define ENA_SHADOW_TRANS_Rx                 CCU40->GCSS |= CCU4_GCSS_S1SE_Msk

#elif (DALI_CONFIG_Rx_TIMER == 2)
  #define DALI_TIM_Rx                         CCU40_CC42                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Rx          CCU40->GIDLC |= CCU4_GIDLC_CS2I_Msk   //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Rx         CCU40->GIDLS |= CCU4_GIDLS_SS2I_Msk   //set idle mode stops clock
  #define ENA_SHADOW_TRANS_Rx                 CCU40->GCSS |= CCU4_GCSS_S2SE_Msk

#elif (DALI_CONFIG_Rx_TIMER == 3)
  #define DALI_TIM_Rx                         CCU40_CC43                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Rx          CCU40->GIDLC |= CCU4_GIDLC_CS3I_Msk   //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Rx         CCU40->GIDLS |= CCU4_GIDLS_SS3I_Msk   //set idle mode stops clock
  #define ENA_SHADOW_TRANS_Rx                 CCU40->GCSS |= CCU4_GCSS_S3SE_Msk
#endif



//----  RX timer port definitions  ---------------------------------------------
#define DALI_GPIO_Rx                          PORT0


//----     Rx Timer0      ------------------------------------------------------
#if (DALI_CONFIG_Rx_TIMER == 0)
  #if defined(DALI_CONFIG_RxPIN_P0_12)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P12_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR12 &= ~PORT0_IOCR12_PC12_Msk; PORT0->PHCR1 |= 0x00040000UL;    //by default GPIO is set to input with no pull up or pull down
    #define SLCT_EVENT0_INPUT                                    //by default inputA = P0.12 is selected for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_6)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P6_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR4 &= ~PORT0_IOCR4_PC6_Msk; PORT0->PHCR0 |= 0x04000000UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x01UL                   //select timer inputB = P0.6 for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_0)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P0_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR0 &= ~PORT0_IOCR0_PC0_Msk; PORT0->PHCR0 |= 0x00000004UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x02UL                   //select timer inputC = P0.0 for event0
  #endif



//----     Rx Timer1      ------------------------------------------------------
#elif (DALI_CONFIG_Rx_TIMER == 1)
  #if defined(DALI_CONFIG_RxPIN_P0_12)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P12_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR12 &= ~PORT0_IOCR12_PC12_Msk; PORT0->PHCR1 |= 0x00040000UL;    //by default GPIO is set to input with no pull up or pull down
  #define SLCT_EVENT0_INPUT                                    //by default inputA = P0.12 is selected for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_7)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P7_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR4 &= ~PORT0_IOCR4_PC7_Msk; PORT0->PHCR0 |= 0x40000000UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x01UL                //select timer inputB = P0.7 for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_1)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P1_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR0 &= ~PORT0_IOCR0_PC1_Msk; PORT0->PHCR0 |= 0x00000040UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x02UL                //select timer inputC = P0.1 for event0
  #endif



//----     Rx Timer2      ------------------------------------------------------
#elif (DALI_CONFIG_Rx_TIMER == 2)
  #if defined(DALI_CONFIG_RxPIN_P0_12)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P12_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR12 &= ~PORT0_IOCR12_PC12_Msk; PORT0->PHCR1 |= 0x00040000UL;    //by default GPIO is set to input with no pull up or pull down
    #define SLCT_EVENT0_INPUT                                    //by default inputA = P0.12 is selected for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_8)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P8_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR8 &= ~PORT0_IOCR8_PC8_Msk; PORT0->PHCR1 |= 0x00000004UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x01UL                  //select timer inputB = P0.8 for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_2)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P2_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR0 &= ~PORT0_IOCR0_PC2_Msk; PORT0->PHCR0 |= 0x00000400UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x02UL                //select timer inputB = P0.2 for event0
  #endif



//----     Rx Timer3      ------------------------------------------------------
#elif (DALI_CONFIG_Rx_TIMER == 3)
  #if defined(DALI_CONFIG_RxPIN_P0_12)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P12_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR12 &= ~PORT0_IOCR12_PC12_Msk; PORT0->PHCR1 |= 0x00040000UL;    //by default GPIO is set to input with no pull up or pull down
    #define SLCT_EVENT0_INPUT

  #elif defined(DALI_CONFIG_RxPIN_P0_9)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P9_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR8 &= ~PORT0_IOCR8_PC9_Msk; PORT0->PHCR1 |= 0x00000040UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x01UL                  //select timer inputB = P0.9 for event0

  #elif defined(DALI_CONFIG_RxPIN_P0_3)
    #define DALI_GPIO_Rx_PIN                  PORT0_IN_P3_Msk
    #define SET_IN_OUT_DEFLT_CONF             DALI_GPIO_Rx->IOCR0 &= ~PORT0_IOCR0_PC3_Msk; PORT0->PHCR0 |= 0x00004000UL;
    #define SLCT_EVENT0_INPUT                 DALI_TIM_Rx->INS |= 0x02UL                  //select timer inputB = P0.3 for event0
  #endif
#endif



//----  MACRO definitions to handle Rx -----------------------------------------
#define SLCT_EVENT0_DEFLT_INPUT          DALI_TIM_Rx->INS &= ~CCU4_CC4_INS_EV0IS_Msk      //by default inputA = P0.12 is selected for event0
#define DALI_GPIO_Rx_INIT                     {SET_IN_OUT_DEFLT_CONF; \
                                              SLCT_EVENT0_DEFLT_INPUT; \
                                              SLCT_EVENT0_INPUT;}

//                                              SET_LARGE_INPUT_HYST; \


#define CLR_DALI_RxCPTR_EDGE_MODE        DALI_TIM_Rx->INS &= ~CCU4_CC4_INS_EV0EM_Msk
#define SET_DALI_RxCPTR_ON_POS_EDGE        DALI_TIM_Rx->INS |= 0x00010000UL
#define SET_DALI_RxCPTR_ON_NEG_EDGE        DALI_TIM_Rx->INS |= 0x00020000UL
#define TOGGLE_DALI_RxCAPTURE                 DALI_TIM_Rx->INS ^= CCU4_CC4_INS_EV0EM_Msk         //toggle edge sensitivity
#ifdef DALI_CONFIG_RX_INVERT
  #define IS_DALI_RxLEVEL_HIGH                ((DALI_GPIO_Rx->IN & DALI_GPIO_Rx_PIN) ==0)
//  #define GET_DALI_RxLEVEL                    ((~(DALI_GPIO_Rx->IN)) & DALI_GPIO_Rx_PIN)      //read digital input value
  #define START_DALI_RxCAPTURE_FALLING        {CLR_DALI_RxCPTR_EDGE_MODE; \
                                              SET_DALI_RxCPTR_ON_POS_EDGE;}                    //falling edge sensitive (because signal is inverted!)
  #define START_DALI_RxCAPTURE_RISING         {CLR_DALI_RxCPTR_EDGE_MODE; \
                                              SET_DALI_RxCPTR_ON_NEG_EDGE;}                    //rising edge sensitive
#else
  #define IS_DALI_RxLEVEL_HIGH                ((DALI_GPIO_Rx->IN & DALI_GPIO_Rx_PIN) !=0)
//  #define GET_DALI_RxLEVEL                    (DALI_GPIO_Rx->IN & DALI_GPIO_Rx_PIN)
  #define START_DALI_RxCAPTURE_FALLING        {CLR_DALI_RxCPTR_EDGE_MODE; \
                                              SET_DALI_RxCPTR_ON_NEG_EDGE;}
  #define START_DALI_RxCAPTURE_RISING         {CLR_DALI_RxCPTR_EDGE_MODE; \
                                              SET_DALI_RxCPTR_ON_POS_EDGE;}
#endif

#define CLR_DALI_Rx_TIMER            DALI_TIM_Rx->TCCLR = CCU4_CC4_TCCLR_TRBC_Msk
#define START_DALI_Rx_TIMER            DALI_TIM_Rx->TCSET |= CCU4_CC4_TCSET_TRBS_Msk
#define START_DALI_TIMER_Rx                   {ENA_SHADOW_TRANS_Rx; \
                        CLR_DALI_Rx_TIMER; \
                                              START_DALI_Rx_TIMER;}

#define CLR_EVENT0_INPUT_CONF          DALI_TIM_Rx->INS &= 0xF9BCFFFFUL
#define CONF_EVENT0_INPUT            DALI_TIM_Rx->INS |= 0x02010000UL    //3 cycles low pass filter and capture with pos edge
#define CONF_EVENT_FUNC              DALI_TIM_Rx->CMC = 0x00000010UL    //event0 is connected with capture0 function , compare mode and other external functions are disabled
#define INIT_DALI_RxCAPTURE                   {CLR_EVENT0_INPUT_CONF; \
                                              CONF_EVENT0_INPUT; \
                                              CONF_EVENT_FUNC;}

#define READ_DALI_RxTIMER                     (uint16_t)(DALI_TIM_Rx->TIMER)                // limited to uint16
#define DALI_RxCAPTURE_REG                    (DALI_TIM_Rx->CV[1])
#define READ_DALI_RxCAPTURE_REG               (uint16_t)(DALI_TIM_Rx->CV[1])

//----   D A L I    R x   ------------------------------------------------------
//******************************************************************************


//******************************************************************************
//----   D A L I    T x   ------------------------------------------------------
#if (DALI_CONFIG_Tx_TIMER == 0)
  #define DALI_TIM_Tx                         CCU40_CC40                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Tx          CCU40->GIDLC |= CCU4_GIDLC_CS0I_Msk   //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Tx         CCU40->GIDLS |= CCU4_GIDLS_SS0I_Msk   //set idle mode stops clock
  #define SET_TxTIMER_STATE                   CCU4_GCSS_S0STS_Msk
  #define CLR_TxTIMER_STATE                   CCU4_GCSC_S0STC_Msk
  #define ENABLE_SHADOW_TRANS_Tx              CCU40->GCSS |= CCU4_GCSS_S0SE_Msk

#elif (DALI_CONFIG_Tx_TIMER == 1)
  #define DALI_TIM_Tx                         CCU40_CC41                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Tx          CCU40->GIDLC |= CCU4_GIDLC_CS1I_Msk    //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Tx         CCU40->GIDLS |= CCU4_GIDLS_SS1I_Msk    //set idle mode stops clock
  #define SET_TxTIMER_STATE                   CCU4_GCSS_S1STS_Msk
  #define CLR_TxTIMER_STATE                   CCU4_GCSC_S1STC_Msk
  #define ENABLE_SHADOW_TRANS_Tx              CCU40->GCSS |= CCU4_GCSS_S1SE_Msk

#elif (DALI_CONFIG_Tx_TIMER == 2)
  #define DALI_TIM_Tx                         CCU40_CC42                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Tx          CCU40->GIDLC |= CCU4_GIDLC_CS2I_Msk    //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Tx         CCU40->GIDLS |= CCU4_GIDLS_SS2I_Msk    //set idle mode stops clock
  #define SET_TxTIMER_STATE                   CCU4_GCSS_S2STS_Msk
  #define CLR_TxTIMER_STATE                   CCU4_GCSC_S2STC_Msk
  #define ENABLE_SHADOW_TRANS_Tx              CCU40->GCSS |= CCU4_GCSS_S2SE_Msk

#elif (DALI_CONFIG_Tx_TIMER == 3)
  #define DALI_TIM_Tx                         CCU40_CC43                          //pointer on timer specific config bytes
  #define ENABLE_DALI_TIMER_CLOCK_Tx          CCU40->GIDLC |= CCU4_GIDLC_CS3I_Msk    //clear idle mode starts clock
  #define DISABLE_DALI_TIMER_CLOCK_Tx         CCU40->GIDLS |= CCU4_GIDLS_SS3I_Msk    //set idle mode stops clock
  #define SET_TxTIMER_STATE                   CCU4_GCSS_S3STS_Msk
  #define CLR_TxTIMER_STATE                   CCU4_GCSC_S3STC_Msk
  #define ENABLE_SHADOW_TRANS_Tx              CCU40->GCSS |= CCU4_GCSS_S3SE_Msk
#endif


//----  timer port definitions  ------------------------------------------------
#if (DALI_CONFIG_Tx_TIMER != DALI_CONFIG_Rx_TIMER)
//----     Tx Timer0      ------------------------------------------------------
  #if (DALI_CONFIG_Tx_TIMER == 0)
    #if defined(DALI_CONFIG_TxPIN_P0_0)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P0_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x000000A0UL           //select push/pull output driven by CCU40Out0

    #elif defined(DALI_CONFIG_TxPIN_P0_5)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P5_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC5_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC5_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR4 |= 0x0000A000UL           //select push/pull output driven by CCU40Out0

    #elif defined(DALI_CONFIG_TxPIN_P0_6)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P6_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC6_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC6_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR4 |= 0x00A00000UL           //select push/pull output driven by CCU40Out0

    #elif defined(DALI_CONFIG_TxPIN_P1_0)
      #define DALI_GPIO_Tx                    PORT1
      #define DALI_GPIO_Tx_PIN                PORT1_IN_P0_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00000090UL           //select push/pull output driven by CCU40Out0

    #elif defined(DALI_CONFIG_TxPIN_P2_0)
      #define DALI_GPIO_Tx                    PORT2
      #define DALI_GPIO_Tx_PIN                PORT2_IN_P0_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT2_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT2_IOCR0_PC0_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00000090UL           //select push/pull output driven by CCU40Out0
    #endif

//----     Tx Timer1      ------------------------------------------------------
  #elif (DALI_CONFIG_Tx_TIMER == 1)
    #if defined(DALI_CONFIG_TxPIN_P0_1)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P1_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x0000A000UL           //select push/pull output driven by CCU40Out1

    #elif defined(DALI_CONFIG_TxPIN_P0_4)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P4_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC4_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC4_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR4 |= 0x000000A0UL           //select push/pull output driven by CCU40Out1

    #elif defined(DALI_CONFIG_TxPIN_P0_7)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P7_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC7_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR4 &= ~PORT0_IOCR4_PC7_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR4 |= 0xA0000000UL           //select push/pull output driven by CCU40Out1

    #elif defined(DALI_CONFIG_TxPIN_P1_1)
      #define DALI_GPIO_Tx                    PORT1
      #define DALI_GPIO_Tx_PIN                PORT1_IN_P1_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00009000UL           //select push/pull output driven by CCU40Out1

    #elif defined(DALI_CONFIG_TxPIN_P2_1)
      #define DALI_GPIO_Tx                    PORT2
      #define DALI_GPIO_Tx_PIN                PORT2_IN_P1_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT2_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT2_IOCR0_PC1_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00009000UL           //select push/pull output driven by CCU40Out1
    #endif // DALI_CONFIG_TxCHANNEL

//----     Tx Timer2      ------------------------------------------------------
  #elif (DALI_CONFIG_Tx_TIMER == 2)
    #if defined(DALI_CONFIG_TxPIN_P0_2)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P2_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC2_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC2_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00A00000UL           //select push/pull output driven by CCU40Out2

    #elif (defined(DALI_CONFIG_TxPIN_P0_8) && !defined(DALI_CONFIG_RxPIN_P0_8))
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P8_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR8 &= ~PORT0_IOCR8_PC8_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR8 &= ~PORT0_IOCR8_PC8_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR8 |= 0x000000A0UL           //select push/pull output driven by CCU40Out2

    #elif defined(DALI_CONFIG_TxPIN_P1_2)
      #define DALI_GPIO_Tx                    PORT1
      #define DALI_GPIO_Tx_PIN                PORT1_IN_P2_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC2_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC2_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x00900000UL           //select push/pull output driven by CCU40Out2

    #elif defined(DALI_CONFIG_TxPIN_P2_10)
      #define DALI_GPIO_Tx                    PORT2
      #define DALI_GPIO_Tx_PIN                PORT2_IN_P10_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR8 &= ~PORT2_IOCR8_PC10_Msk  //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR8 &= ~PORT2_IOCR8_PC10_Msk  //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR8 |= 0x00900000UL           //select push/pull output driven by CCU40Out2
    #endif


//----     Tx Timer3      ------------------------------------------------------
  #elif (DALI_CONFIG_Tx_TIMER == 3)
    #if defined(DALI_CONFIG_TxPIN_P0_3)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P3_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC3_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT0_IOCR0_PC3_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0xA0000000UL           //select push/pull output driven by CCU40Out3

    #elif defined(DALI_CONFIG_TxPIN_P0_9)
      #define DALI_GPIO_Tx                    PORT0
      #define DALI_GPIO_Tx_PIN                PORT0_IN_P9_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR8 &= ~PORT0_IOCR8_PC9_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR8 &= ~PORT0_IOCR8_PC9_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR8 |= 0x0000A000UL           //select push/pull output driven by CCU40Out3

    #elif defined(DALI_CONFIG_TxPIN_P1_3)
      #define DALI_GPIO_Tx                    PORT1
      #define DALI_GPIO_Tx_PIN                PORT1_IN_P3_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC3_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR0 &= ~PORT1_IOCR0_PC3_Msk   //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR0 |= 0x90000000UL           //select push/pull output driven by CCU40Out3

    #elif defined(DALI_CONFIG_TxPIN_P2_11)
      #define DALI_GPIO_Tx                    PORT2
      #define DALI_GPIO_Tx_PIN                PORT2_IN_P11_Msk
      #define DALI_GPIO_Tx_INIT_IDLE          DALI_GPIO_Tx->IOCR8 &= ~PORT2_IOCR8_PC11_Msk  //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_IDLE           DALI_GPIO_Tx->IOCR8 &= ~PORT2_IOCR8_PC11_Msk  //select input with no pull up/down
      #define DALI_GPIO_Tx_SET_ACTIVE         DALI_GPIO_Tx->IOCR8 |= 0x90000000UL           //select push/pull output driven by CCU40Out3
    #endif
  #endif
#else
  Rx and Tx have to use different timers (unit: %)
#endif
//----  timer port definitions  ------------------------------------------------


//!!!!shadow registers PRS, PSL will be reloaded with every period overflow!!!!
#define CLR_DALI_Tx_TIMER            DALI_TIM_Tx->TCCLR = CCU4_CC4_TCCLR_TCC_Msk
#define START_DALI_Tx_TIMER            DALI_TIM_Tx->TCSET |= CCU4_CC4_TCSET_TRBS_Msk
#define START_DALI_TIMER_Tx                   { ENABLE_DALI_TIMER_CLOCK_Tx; \
                                                ENA_SHADOW_TRANS_Tx; \
                                                CLR_DALI_Tx_TIMER; \
                                                START_DALI_Tx_TIMER; }

#define STOP_DALI_Tx_TIMER            DALI_TIM_Tx->TCCLR |= (CCU4_CC4_TCCLR_TRBC_Msk | CCU4_CC4_TCCLR_TCC_Msk)
#define STOP_DALI_TIMER_Tx                    {STOP_DALI_Tx_TIMER; \
                        DISABLE_DALI_TIMER_CLOCK_Tx;}
#define IS_DALI_Tx_TIMER_RUNNING              ((DALI_TIM_Tx->TCST & CCU4_CC4_TCST_TRB_Msk) != 0)

#ifdef DALI_CONFIG_TX_INVERT
  #define ENABLE_DALI_TxCOMPARE               DALI_TIM_Tx->PSL = 0x00UL;        //passive level is low (DALI passive level=high)
#else
  #define ENABLE_DALI_TxCOMPARE               DALI_TIM_Tx->PSL = 0x01UL;        //passive level is high (DALI passive level=high)
#endif

#define DALI_TxTIMER_REG                      (DALI_TIM_Tx->TIMER)              //read current timer value
//#define DALI_TxCOMPARE_REG                    (DALI_TIM_Tx->PRS)

#define FORCE_ACTIVE_DALI_TxCOMPARE           CCU40->GCSS |= SET_TxTIMER_STATE  //set state to active
#define FORCE_IDLE_DALI_TxCOMPARE             CCU40->GCSC |= CLR_TxTIMER_STATE  //set state to passive
//#define DALI_QUERY_HL_TxCOMPARE               (DALI_TIM_Tx->PSL == 0)           //reads the current used passive level
//#define DALI_QUERY_LH_TxCOMPARE               (DALI_TIM_Tx->PSL != 0)           //reads the current used passive level


//----   D A L I    T x   ------------------------------------------------------
//******************************************************************************


// -----------------------------------------------------------------------------
// ------ G e n e r a l   S e t t i n g s   ------------------------------------


// --- Definitions for DALI-Timer   --------------------------------------------
#if (DALI_CONFIG_TIMER_CLK_MHz == 32)                                                    //define prescaler factor depending on system clock
  #if defined(configDOUBLE_PRHP_CLK)                                            //some peripherals could be clocked with double of system clock
    #define PRESCLR                           0x06UL
    #define PRPHL_CLK_SLCTN                   SCU_CLK_CLKCR_PCLKSEL_Msk
  #else
    #define PRESCLR                           0x05UL
    #define PRPHL_CLK_SLCTN                   0x00UL
  #endif
  #define MCLK_DIVIDER              0x0100UL              //IDIV = 1, FDIV = 0 => 32MHz
//  #define SYS_TICK_TIME             3199                //system tick all 100us
#elif (DALI_CONFIG_TIMER_CLK_MHz == 16)
  #if defined(configDOUBLE_PRHP_CLK)
    #define PRESCLR                           0x05UL
    #define PRPHL_CLK_SLCTN                   SCU_CLK_CLKCR_PCLKSEL_Msk
  #else
    #define PRESCLR                           0x04UL
    #define PRPHL_CLK_SLCTN                   0x00UL
  #endif
  #define MCLK_DIVIDER              0x0200UL              //IDIV = 2, FDIV = 0 => 16MHz
//  #define SYS_TICK_TIME             1599                 //system tick all 100us
#elif (DALI_CONFIG_TIMER_CLK_MHz == 8)
  #if defined(configDOUBLE_PRHP_CLK)
    #define PRESCLR                           0x04UL
    #define PRPHL_CLK_SLCTN                   SCU_CLK_CLKCR_PCLKSEL_Msk
  #else
    #define PRESCLR                           0x03UL
    #define PRPHL_CLK_SLCTN                   0x00UL
  #endif
  #define MCLK_DIVIDER              0x0400UL              //IDIV = 4, FDIV = 0 => 8MHz
//  #define SYS_TICK_TIME             799                 //system tick all 100us
#else
  DALI_CONFIG_TIMER_CLK_MHz must be defined as 32, 16 or 8 MHz (unit: %)
#endif


#define DISABLE_PSSWRD_PRTCTN          SCU_GENERAL->PASSWD = 0xC0UL
#define CLR_MCLCK_DIVIDER            SCU_CLK->CLKCR &= 0xFFFF0000
#define SET_MCLCK_DIVIDER            SCU_CLK->CLKCR |= MCLK_DIVIDER
#define CLR_PCLK_MCLK_RATIO            SCU_CLK->CLKCR &= ~SCU_CLK_CLKCR_PCLKSEL_Msk    //peripheral clock = system clock
#define SET_PCLK_MCLK_RATIO            SCU_CLK->CLKCR |= PRPHL_CLK_SLCTN          //peripheral clock = 2*system clock
#define ENA_CCU4_CLOCK              SCU_CLK->CGATCLR0 |= SCU_CLK_CGATCLR0_CCU40_Msk
//#define SET_SYS_TICK_TIME            PPB->SYST_RVR = SYS_TICK_TIME

#define START_CCU4_PRESCALER          CCU40->GIDLC |= CCU4_GIDLC_SPRB_Msk
#define CLR_CCU4_SUSPEND            CCU40->GCTRL &= ~CCU4_GCTRL_SUSCFG_Msk
#define SET_CCU4_SAFE_STOP            CCU40->GCTRL |= 0x00000200UL

#define SET_DALI_Rx_PRESCALER          DALI_TIM_Rx->PSC = PRESCLR
#define SET_DALI_Rx_MAX_PERIOD          DALI_TIM_Rx->PRS = CCU4_CC4_PRS_PRS_Msk      //set period to 0xFFFF
#define CONF_DALI_Rx_TIMER            DALI_TIM_Rx->TC = CCU4_CC4_TC_CLST_Msk|CCU4_CC4_TC_CCS_Msk  //ena shadow transfer on clear and ignore full flag status

#define SET_DALI_Tx_PRESCALER          DALI_TIM_Tx->PSC = PRESCLR
#define CLR_DALI_Tx_PERIOD            DALI_TIM_Tx->PRS = 0x00UL
#define CONF_DALI_Tx_TIMER            DALI_TIM_Tx->TC = CCU4_CC4_TC_CLST_Msk      //ena shadow transfer on clear

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

