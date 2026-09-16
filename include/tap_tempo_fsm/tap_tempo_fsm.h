//
// Created by tomly on 16/09/2026.
//

#ifndef TAP_TEMPO_TAP_TEMPO_FSM_H
#define TAP_TEMPO_TAP_TEMPO_FSM_H

typedef enum
{
	RUNNING       = 0,
	READING_INPUT = 1,
	TAP_TEMPO_STATES
} tap_tempo_fsm_state_t;

void TapTempoFSM_Init(void);
void TapTempoFSM_Update(void);
void TapTempoFSM_SetState(tap_tempo_fsm_state_t state);

#endif //TAP_TEMPO_TAP_TEMPO_FSM_H
