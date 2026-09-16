//
// Created by tomly on 16/09/2026.
//

#include <fsm.h>
#include <stddef.h>

static void invoke_callback(const fsm_callback_t* callback);

void FSM_Init(fsm_t* fsm, fsm_state_t* states, uint32_t initial_state)
{
	fsm->states = states;
	fsm->current_state = initial_state;

	fsm_callback_t* callback = &(fsm->states[fsm->current_state].on_enter);
	invoke_callback(callback);
}

void FSM_SetState(fsm_t* fsm, uint32_t new_state)
{
	if (new_state == fsm->current_state)
		return;

	fsm_callback_t* callback = &(fsm->states[fsm->current_state].on_exit);
	invoke_callback(callback);

	fsm->current_state = new_state;

	callback = &(fsm->states[fsm->current_state].on_enter);
	invoke_callback(callback);
}

void FSM_Update(fsm_t* fsm)
{
	fsm_callback_t* callback = &(fsm->states[fsm->current_state].update);
	invoke_callback(callback);
}

static void invoke_callback(const fsm_callback_t* callback)
{
	if (callback == NULL || callback->callback == NULL)
		return;

	callback->callback(callback->arg);
}
