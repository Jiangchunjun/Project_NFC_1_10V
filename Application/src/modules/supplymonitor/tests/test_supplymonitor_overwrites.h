/*
 * unit_testing.h
 *
 *  Created on: 12.10.2015
 *      Author: s.elbaradie-ext
 */

#ifndef __TEST_SUPPLYMONITOR_OVERWRITES_H__
#define __TEST_SUPPLYMONITOR_OVERWRITES_H__

#ifdef DECLARE_LOCAL_OVERWRITES
#define EXTERN
#else
#define EXTERN extern
#endif

//------------------------------------------------------------------------------
// unit under test variable overwrites
//------------------------------------------------------------------------------
//EXTERN supplymonitor_mode_t          supplymonitor_mode_requested;
EXTERN const supply_monitor_constants_t * suMoCoP;
EXTERN supply_monitor_intern_t supplyMonitorPriv;
//EXTERN supply_monitor_intern_flags_t Supply.flags;
EXTERN supply_state_t sm_state;
EXTERN uint32_t fsm_cycle_cnt;
EXTERN uint32_t evaluate_timer;
//EXTERN statisch locale variablen hier einfügen OLAF
EXTERN uint32_t no_commutation_detected_cnt;
EXTERN uint32_t supplymonitor_deviation_cnt;
EXTERN uint32_t supplymonitor_Ulinepk_delta;
#endif /* __TEST_SUPPLYMONITOR_OVERWRITES_H__ */
