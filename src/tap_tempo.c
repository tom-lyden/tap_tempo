//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <board_definition.h>
#include <math_utils.h>
#include <fsm.h>
#include <tap_tempo_fsm.h>
#include <tap_tempo_fsm_reading.h>
#include <tap_tempo_fsm_running.h>

#define TEMPO_PERIOD_TICKS(tempo) (60 * (TICK_FREQUENCY_HZ) / (tempo))

static const tap_tempo_fsm_state_ext_t running_extension = 
{
	.on_button_press = Running_ButtonPress,
	.on_button_release = Running_ButtonRelease,
};

static const tap_tempo_fsm_state_ext_t reading_extension =
{
	.on_button_press = Reading_ButtonPress,
	.on_button_release = Reading_ButtonRelease,
};

static const fsm_state_t states[TAP_TEMPO_STATES] =
{
	[RUNNING] = 
	{
			.on_enter = Running_Enter,
			.update   = Running_Update,
			.on_exit  = Running_Exit,
			.extension = &running_extension,
		},
	[READING_INPUT] = 
	{
			.on_enter = Reading_Enter,
			.update   = Reading_Update,
			.on_exit  = Reading_Exit,
			.extension = &reading_extension,
		},
};

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	uint32_t clamped_tempo = CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO);
	uint64_t period_ticks = TEMPO_PERIOD_TICKS(clamped_tempo);
	
	uint8_t clamped_dc_percentage = CLAMP(cfg->duty_cycle_percentage, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
	
	tap_tempo->high_duration_ticks = PERCENTAGE(period_ticks, clamped_dc_percentage);
	tap_tempo->low_duration_ticks = PERCENTAGE(period_ticks, 100 - clamped_dc_percentage);
	
	tap_tempo->set_indicator = cfg->set_indicator;
	
	Timer_Init(&tap_tempo->led_timer, cfg->get_ticks);
	Timer_Init(&tap_tempo->state_timer, cfg->get_ticks);
	FSM_Init(&tap_tempo->fsm, states, RUNNING, tap_tempo);
}

void TapTempo_Update(tap_tempo_t* tap_tempo)
{
	FSM_Update(&tap_tempo->fsm, tap_tempo);
}

void TapTempo_ButtonPress(void* context)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*) context;
	const fsm_state_t* current_state = FSM_CurrentState(&tap_tempo->fsm);
	
	const tap_tempo_fsm_state_ext_t* extension = (const tap_tempo_fsm_state_ext_t *) current_state->extension;
	extension->on_button_press(context);
}

void TapTempo_ButtonRelease(void* context)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*) context;
	const fsm_state_t* current_state = FSM_CurrentState(&tap_tempo->fsm);
	
	const tap_tempo_fsm_state_ext_t* extension = (const tap_tempo_fsm_state_ext_t *) current_state->extension;
	extension->on_button_release(context);
}
