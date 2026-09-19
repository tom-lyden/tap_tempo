//
// Created by tomly on 16/09/2026.
//

#include <fsm.h>
#include <stddef.h>

static void invoke_callback(fsm_callback_t callback, void* arg);

void FSM_Init(fsm_t* fsm, const fsm_state_t* initial_state, void* ctx)
{
	fsm->current_state = initial_state;

	fsm_callback_t callback = fsm->current_state->on_enter;
	invoke_callback(callback, ctx);
}

void FSM_SetState(fsm_t* fsm, const fsm_state_t* new_state, void* ctx)
{
	if (new_state == fsm->current_state)
		return;

	fsm_callback_t callback = fsm->current_state->on_exit;
	invoke_callback(callback, ctx);

	fsm->current_state = new_state;

	callback = fsm->current_state->on_enter;
	invoke_callback(callback, ctx);
}

void FSM_Update(const fsm_t* fsm, void* ctx)
{
	fsm_callback_t callback = fsm->current_state->update;
	invoke_callback(callback, ctx);
}

static void invoke_callback(fsm_callback_t callback, void* arg)
{
	if (callback == NULL)
		return;

	callback(arg);
}
