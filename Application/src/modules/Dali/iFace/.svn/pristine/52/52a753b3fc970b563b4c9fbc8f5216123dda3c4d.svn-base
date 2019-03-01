/******************** (C) COPYRIGHT 2012 OSRAM OEC *****************************
* File Name          : ProductionTest.h
* Author             : Jutta Avril
* Date               : 13/07/2012
* Description        : Interface to provide device-specific production tests by special osram commands.
*******************************************************************************/



void ProductionTest_Init( uint8_t channel );

typedef enum {
  TEST_DONE,
  TEST_NOT_SUPPORTED
} TEST_RESPONSE_t;

TEST_RESPONSE_t ProductionTest_Step( uint8_t step, uint8_t dtr1, uint8_t dtr2 );
TEST_RESPONSE_t ProductionTest_Query( uint8_t query, uint8_t* pDtr1, uint8_t* pDtr2, uint8_t channel );
