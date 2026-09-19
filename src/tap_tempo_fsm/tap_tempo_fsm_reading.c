//
// Created by tomly on 16/09/2026.
//

#include <tap_tempo_fsm_reading.h>

void TapTempoFSM_ReadingState_Init(tap_tempo_fsm_reading_state_t* state, get_ticks_t* get_ticks)
{
	Timer_Init(&state->timer, get_ticks);
}

void TapTempoFSM_ReadingState_Enter(void* ctx)
{

}

void TapTempoFSM_ReadingState_Update(void* ctx)
{

}

void TapTempoFSM_ReadingState_Exit(void* ctx)
{

}

void TapTempoFSM_ReadingState_ButtonPress(void* ctx)
{

}

void TapTempoFSM_ReadingState_ButtonRelease(void* ctx)
{

}
