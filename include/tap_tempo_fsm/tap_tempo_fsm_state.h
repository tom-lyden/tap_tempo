//
// Created by tomly on 19/09/2026.
//

#ifndef TAP_TEMPO_FSM_STATE_H
#define TAP_TEMPO_FSM_STATE_H

#include <fsm.h>

typedef struct
{
	fsm_state_t base_state;
	fsm_callback_t on_button_press;
	fsm_callback_t on_button_release;
} tap_tempo_fsm_state_t;

#endif // TAP_TEMPO_FSM_STATE_H
