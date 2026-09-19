//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_TAP_TEMPO_FSM_H
#define TAP_TEMPO_TAP_TEMPO_FSM_H

#include <fsm.h>

typedef enum
{
	TAP_TEMPO_STATE_RUNNING       = 0,
	TAP_TEMPO_STATE_READING_INPUT = 1,
	TAP_TEMPO_STATE_COUNT
} tap_tempo_fsm_state_t;

typedef struct
{
	fsm_callback_t* on_button_press;
	fsm_callback_t* on_button_release;
} tap_tempo_fsm_state_extension_t;

#endif //TAP_TEMPO_TAP_TEMPO_FSM_H
