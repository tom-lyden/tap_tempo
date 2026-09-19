//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_TAP_TEMPO_FSM_H
#define TAP_TEMPO_TAP_TEMPO_FSM_H

typedef enum
{
	RUNNING       = 0,
	READING_INPUT = 1,
	TAP_TEMPO_STATES
} tap_tempo_fsm_state_t;

typedef struct
{
	fsm_callback_t* on_button_press;
	fsm_callback_t* on_button_release;
} tap_tempo_fsm_state_ext_t;

#endif //TAP_TEMPO_TAP_TEMPO_FSM_H
