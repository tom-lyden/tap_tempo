//
// Created by tomly on 16/09/2026.
//

#include <tap_tempo_fsm.h>
#include <fsm.h>
#include <tap_tempo_fsm_reading.h>
#include <tap_tempo_fsm_running.h>

static const fsm_state_t state_running =
{
	.on_enter = { .callback = Running_Enter },
	.update   = { .callback = Running_Update },
	.on_exit  = { .callback = Running_Exit },
};

static const fsm_state_t state_reading_input =
{
	.on_enter = { .callback = Reading_Enter },
	.update   = { .callback = Reading_Update },
	.on_exit  = { .callback = Reading_Exit },
};

static fsm_state_t states[TAP_TEMPO_STATES] =
{
	state_running,
	state_reading_input,
};

static fsm_t fsm;

void TapTempoFSM_Init(void)
{
	FSM_Init(&fsm, states, RUNNING);
}

void TapTempoFSM_Update(void)
{
	FSM_Update(&fsm);
}

void TapTempoFSM_SetState(tap_tempo_fsm_state_t state)
{
	FSM_SetState(&fsm, state);
}
