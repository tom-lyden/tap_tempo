//
// Created by tomly on 16/09/2026.
//

#ifndef TAP_TEMPO_FSM_H
#define TAP_TEMPO_FSM_H

#include <stdint.h>

typedef void fsm_callback_t(void*);

typedef struct
{
	fsm_callback_t* on_enter;
	fsm_callback_t* update;
	fsm_callback_t* on_exit;
	const void* extension;
} fsm_state_t;

typedef struct
{
	const fsm_state_t* states;
	uint32_t current_state;
} fsm_t;

void FSM_Init(fsm_t* fsm, const fsm_state_t* states, uint32_t initial_state, void* ctx);
void FSM_SetState(fsm_t* fsm, uint32_t new_state, void* ctx);
void FSM_Update(fsm_t* fsm, void* ctx);

const fsm_state_t* FSM_CurrentState(const fsm_t* fsm);

#endif //TAP_TEMPO_FSM_H
