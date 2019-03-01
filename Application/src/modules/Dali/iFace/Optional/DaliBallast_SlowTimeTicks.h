/******************** (C) COPYRIGHT 2009 - 2009 OSRAM OEC ******************************************************************************************
* File Name       : DaliBallast_SlowTimeTicks.h
* Description     : Optional interface to the synchronise function calls to DaliBallast SlowTimeTick (25 ms)
*                   List of called (!) functions at slow time ticks. These functions are defined weak in WeakFunktions.c
* Author(s)       : Jutta Avril (JA).
* Created by      : 25.04.16   JA
* Modified by     : DATE       (AUTHOR)    DESCRIPTION OF CHANGE
***************************************************************************************************************************************************/


///<- Functions declared WEAK in DaliBallast.c, may be overwritten by external functions
void DaliBallast_STT3(uint8_t channels);
void DaliBallast_STT7(uint8_t channels);

void DaliBallast_STT2(uint8_t channels);
void DaliBallast_STT4(uint8_t channels);
void DaliBallast_STT6(uint8_t channels);
void DaliBallast_STT8(uint8_t channels);
void DaliBallast_STT10(uint8_t channels);
