//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <board_definition.h>
#include <math_utils.h>
#include <tap_tempo_fsm_reading.h>
#include <tap_tempo_fsm_running.h>
#include <tap_tempo_fsm.h>

#define TEMPO_PERIOD_TICKS(tempo) (60 * (TICK_FREQUENCY_HZ) / (tempo))

static const fsm_state_t states[TAP_TEMPO_STATES] =
{
	[RUNNING] = 
	{
			.on_enter = Running_Enter,
			.update   = Running_Update,
			.on_exit  = Running_Exit,
		},
	[READING_INPUT] = 
	{
			.on_enter = Reading_Enter,
			.update   = Reading_Update,
			.on_exit  = Reading_Exit,
		},
};

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	tap_tempo->button_press_count = tap_tempo->last_button_press_count = 0;
	
	uint32_t clamped_tempo = CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO);
	uint64_t period_ticks = TEMPO_PERIOD_TICKS(clamped_tempo);
	
	tap_tempo->high_duration_ticks = period_ticks * cfg->duty_cycle;
	tap_tempo->low_duration_ticks = period_ticks * (1 - cfg->duty_cycle);
	
	tap_tempo->set_indicator = cfg->set_indicator;
	
	Timer_Init(&tap_tempo->led_timer, cfg->get_ticks);
	Timer_Init(&tap_tempo->state_timer, cfg->get_ticks);
	FSM_Init(&tap_tempo->fsm, states, RUNNING, tap_tempo);
}

void TapTempo_Update(tap_tempo_t* tap_tempo)
{
	FSM_Update(&tap_tempo->fsm, tap_tempo);
}

int TapTempo_IsButtonPressed(const tap_tempo_t* tap_tempo)
{
	return tap_tempo->button_press_count != tap_tempo->last_button_press_count;
}
