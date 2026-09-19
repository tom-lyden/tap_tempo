//
// Created by tomly on 16/09/2026.
//

#ifndef TAP_TEMPO_FSM_RUNNING_H
#define TAP_TEMPO_FSM_RUNNING_H

#include <tap_tempo_fsm_state.h>
#include <timer.h>

typedef struct
{
	tap_tempo_fsm_state_t base_state;
	timer_t timer;
} tap_tempo_fsm_running_state_t;

void TapTempoFSM_RunningState_Init(tap_tempo_fsm_running_state_t* state, get_ticks_t* get_ticks);

void TapTempoFSM_RunningState_Enter(void* ctx);
void TapTempoFSM_RunningState_Update(void* ctx);
void TapTempoFSM_RunningState_Exit(void* ctx);

void TapTempoFSM_RunningState_ButtonPress(void* ctx);
void TapTempoFSM_RunningState_ButtonRelease(void* ctx);

#endif // TAP_TEMPO_FSM_RUNNING_H
