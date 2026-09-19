//
// Created by tomly on 16/09/2026.
//

#ifndef FSM_H
#define FSM_H

typedef void (*fsm_callback_t)(void*);

typedef struct
{
	fsm_callback_t on_enter;
	fsm_callback_t update;
	fsm_callback_t on_exit;
} fsm_state_t;

typedef struct
{
	const fsm_state_t* current_state;
} fsm_t;

void FSM_Init(fsm_t* fsm, const fsm_state_t* initial_state, void* ctx);
void FSM_SetState(fsm_t* fsm, const fsm_state_t* new_state, void* ctx);
void FSM_Update(const fsm_t* fsm, void* ctx);

#endif // FSM_H
