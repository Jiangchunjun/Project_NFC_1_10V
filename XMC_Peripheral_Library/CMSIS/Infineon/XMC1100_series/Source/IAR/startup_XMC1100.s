;**********************************************************************************
;*
;* Part one of the system initialization code, contains low-level
;* initialization, plain thumb variant.
;*
;* Copyright 2013 IAR Systems. All rights reserved.
;*
;* $Revision: 20 $
;*
;******************* Version History **********************************************   
;  V5, Feb, 6, 2013 TYS:a) Add DAVE3_CE defination, 
;                       b) Remove Math,ADC,CCU8,POSIF,LEDTS,BCCU0 interrupt
;                       c) Change AllowPLLInitByStartup to AllowClkInitByStartup 
;  V6, May, 16,2013 TYS:a) Add XMC1100_SCU.inc 
;  V7, Nov, 20,2014 JFT:a) Removed dependencies with DAVE generated code
;  V8, Dec, 11,2014 JFT:a) Default clocking changed, MCLK=32MHz and PCLK=64MHz
;  V9, Sep, 03,2015 JFT:a) SSW default clocking changed, MCLK=8MHz and PCLK=16MHz avoid problems with BMI tool timeout   
;**********************************************************************************

; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY, which
; is where to find the SP start value.
; If vector table is not located at address 0, the user has to initialize the  NVIC vector
; table register (VTOR) before using interrupts.
;
; Cortex-M version
;

        MODULE  ?cstartup

/*****************************************************************************
 * <h> Clock system handling by SSW
 *   <h> CLK_VAL1 Configuration
 *    <o0.0..7>    FDIV Fractional Divider Selection
 *    <i> Deafult: 0. Fractional part of clock divider, MCLK = DCO1 / (2 x (IDIV + (FDIV / 256)))
 *    <o0.8..15>   IDIV Divider Selection (limited to 1-16)
 *                    <0=> Divider is bypassed
 *                    <1=> MCLK = 32 MHz
 *                    <2=> MCLK = 16 MHz
 *                    <3=> MCLK = 10.67 MHz
 *                    <4=> MCLK = 8 MHz
 *                    <254=> MCLK = 126 kHz
 *                    <255=> MCLK = 125.5 kHz
 *    <i> Deafult: 4. Interger part of clock divider, MCLK = DCO1 / (2 x (IDIV + (FDIV / 256))) = 8MHz
 *    <o0.16>      PCLKSEL PCLK Clock Select
 *                    <0=> PCLK = MCLK
 *                    <1=> PCLK = 2 x MCLK
 *    <i> Deafult: 2 x MCLK
 *    <o0.17..19>  RTCCLKSEL RTC Clock Select
 *                    <0=> 32.768kHz standby clock
 *                    <1=> 32.768kHz external clock from ERU0.IOUT0
 *                    <2=> 32.768kHz external clock from ACMP0.OUT
 *                    <3=> 32.768kHz external clock from ACMP1.OUT
 *                    <4=> 32.768kHz external clock from ACMP2.OUT
 *                    <5=> Reserved
 *                    <6=> Reserved
 *                    <7=> Reserved
 *    <i> Deafult: 32.768kHz standby clock 
 *    <o0.31>      do not move CLK_VAL1 to SCU_CLKCR[0..19]
 *  </h>
 *****************************************************************************/
#define CLKVAL1_SSW 0x00010400

/*****************************************************************************
 *  <h> CLK_VAL2 Configuration
 *    <o0.0>    disable VADC and SHS Gating
 *    <o0.1>    disable CCU80 Gating
 *    <o0.2>    disable CCU40 Gating
 *    <o0.3>    disable USIC0 Gating
 *    <o0.4>    disable BCCU0 Gating
 *    <o0.5>    disable LEDTS0 Gating
 *    <o0.6>    disable LEDTS1 Gating
 *    <o0.7>    disable POSIF0 Gating
 *    <o0.8>    disable MATH Gating
 *    <o0.9>    disable WDT Gating
 *    <o0.10>   disable RTC Gating
 *    <o0.31>   do not move CLK_VAL2 to SCU_CGATCLR0[0..10]
 *  </h>
 *****************************************************************************/
#define CLKVAL2_SSW 0x80000000

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)
        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table

        DATA
__vector_table
        DCD     sfe(CSTACK)
        DCD     Reset_Handler             ; Reset Handler
        DCD     0                         ; 0x8
        DCD     0                         ; 0xC
        DCD     CLKVAL1_SSW               ; 0x10 CLK_VAL1 - (CLKCR default)
        DCD     CLKVAL2_SSW               ; 0x14 CLK_VAL2 - (CGATCLR0 default)

        SECTION .vect_table:CODE:ROOT(2)
        THUMB
        LDR     R0,=HardFault_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=SVC_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=Undef_Handler
        BX      R0
        LDR     R0,=PendSV_Handler
        BX      R0
        LDR     R0,=SysTick_Handler
        BX      R0

        ; External Interrupts
        LDR     R0,=SCU_0_IRQHandler      ; Handler name for SR SCU_0
        BX      R0
        LDR     R0,=SCU_1_IRQHandler      ; Handler name for SR SCU_1
        BX      R0
        LDR     R0,=SCU_2_IRQHandler      ; Handler name for SR SCU_2
        BX      R0
        LDR     R0,=ERU0_0_IRQHandler     ; Handler name for SR ERU0_0
        BX      R0
        LDR     R0,=ERU0_1_IRQHandler     ; Handler name for SR ERU0_1
        BX      R0
        LDR     R0,=ERU0_2_IRQHandler     ; Handler name for SR ERU0_2
        BX      R0
        LDR     R0,=ERU0_3_IRQHandler     ; Handler name for SR ERU0_3
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=USIC0_0_IRQHandler    ; Handler name for SR USIC0_0
        BX      R0
        LDR     R0,=USIC0_1_IRQHandler    ; Handler name for SR USIC0_1
        BX      R0
        LDR     R0,=USIC0_2_IRQHandler    ; Handler name for SR USIC0_2
        BX      R0
        LDR     R0,=USIC0_3_IRQHandler    ; Handler name for SR USIC0_3
        BX      R0
        LDR     R0,=USIC0_4_IRQHandler    ; Handler name for SR USIC0_4
        BX      R0
        LDR     R0,=USIC0_5_IRQHandler    ; Handler name for SR USIC0_5
        BX      R0
        LDR     R0,=VADC0_C0_0_IRQHandler ; Handler name for SR VADC0_C0_0
        BX      R0
        LDR     R0,=VADC0_C0_1_IRQHandler ; Handler name for SR VADC0_C0_1
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=CCU40_0_IRQHandler    ; Handler name for SR CCU40_0
        BX      R0
        LDR     R0,=CCU40_1_IRQHandler    ; Handler name for SR CCU40_1
        BX      R0
        LDR     R0,=CCU40_2_IRQHandler    ; Handler name for SR CCU40_2
        BX      R0
        LDR     R0,=CCU40_3_IRQHandler    ; Handler name for SR CCU40_3
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0
        LDR     R0,=Undef_Handler         ; Not Available
        BX      R0

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        EXTERN  SystemInit
        SECTION .text:CODE:NOROOT(2)

        THUMB

        PUBWEAK Reset_Handler
        SECTION .text:CODE:REORDER:NOROOT(2)
Reset_Handler
        LDR     R0, =SystemInit
        BLX     R0
        LDR     R0, =__iar_program_start
        BX      R0

        PUBWEAK Undef_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
Undef_Handler
        B Undef_Handler


        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
HardFault_Handler
        B HardFault_Handler


        PUBWEAK SVC_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SVC_Handler
        B SVC_Handler


        PUBWEAK PendSV_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
PendSV_Handler
        B PendSV_Handler


        PUBWEAK SysTick_Handler
        SECTION .text:CODE:REORDER:NOROOT(1)
SysTick_Handler
        B SysTick_Handler


        PUBWEAK SCU_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCU_0_IRQHandler
        B SCU_0_IRQHandler

        PUBWEAK SCU_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCU_1_IRQHandler
        B SCU_1_IRQHandler


        PUBWEAK SCU_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
SCU_2_IRQHandler
        B SCU_2_IRQHandler


        PUBWEAK ERU0_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ERU0_0_IRQHandler
        B ERU0_0_IRQHandler


        PUBWEAK ERU0_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ERU0_1_IRQHandler
        B ERU0_1_IRQHandler


        PUBWEAK ERU0_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ERU0_2_IRQHandler
        B ERU0_2_IRQHandler


        PUBWEAK ERU0_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
ERU0_3_IRQHandler
        B ERU0_3_IRQHandler


        PUBWEAK USIC0_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_0_IRQHandler
        B USIC0_0_IRQHandler


        PUBWEAK USIC0_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_1_IRQHandler
        B USIC0_1_IRQHandler


        PUBWEAK USIC0_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_2_IRQHandler
        B USIC0_2_IRQHandler


        PUBWEAK USIC0_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_3_IRQHandler
        B USIC0_3_IRQHandler


        PUBWEAK USIC0_4_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_4_IRQHandler
        B USIC0_4_IRQHandler


        PUBWEAK USIC0_5_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
USIC0_5_IRQHandler
        B USIC0_5_IRQHandler


        PUBWEAK VADC0_C0_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VADC0_C0_0_IRQHandler
        B VADC0_C0_0_IRQHandler


        PUBWEAK VADC0_C0_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
VADC0_C0_1_IRQHandler
        B VADC0_C0_1_IRQHandler


        PUBWEAK CCU40_0_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CCU40_0_IRQHandler
        B CCU40_0_IRQHandler


        PUBWEAK CCU40_1_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CCU40_1_IRQHandler
        B CCU40_1_IRQHandler


        PUBWEAK CCU40_2_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CCU40_2_IRQHandler
        B CCU40_2_IRQHandler


        PUBWEAK CCU40_3_IRQHandler
        SECTION .text:CODE:REORDER:NOROOT(1)
CCU40_3_IRQHandler
        B CCU40_3_IRQHandler

        END
