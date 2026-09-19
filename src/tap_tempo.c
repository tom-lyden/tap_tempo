//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <fsm.h>
#include <board_definition.h>
#include <math_utils.h>
#include <tap_tempo_fsm_state.h>
#include <tap_tempo_fsm_reading.h>
#include <tap_tempo_fsm_running.h>

#define TEMPO_PERIOD_TICKS(tempo) (60 * (TICK_FREQUENCY_HZ) / (tempo))

static const tap_tempo_fsm_running_state_t running_state =
{
	.tap_tempo_state = 
	{
		.base_state = 
		{
			.on_enter  = TapTempoFSM_RunningState_Enter,
			.update    = TapTempoFSM_RunningState_Update,
			.on_exit   = TapTempoFSM_RunningState_Exit,
		},
		.on_button_press   = TapTempoFSM_RunningState_ButtonPress,
		.on_button_release = TapTempoFSM_RunningState_ButtonRelease,
	}
};
	
	
static const tap_tempo_fsm_reading_state_t reading_state =
{
	.tap_tempo_state =
	{
		.base_state = 
		{
			.on_enter  = TapTempoFSM_ReadingState_Enter,
			.update    = TapTempoFSM_ReadingState_Update,
			.on_exit   = TapTempoFSM_ReadingState_Exit,
		},
		.on_button_press   = TapTempoFSM_ReadingState_ButtonPress,
		.on_button_release = TapTempoFSM_ReadingState_ButtonRelease,
	},
};

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	uint32_t clamped_tempo = CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO);
	uint64_t period_ticks = TEMPO_PERIOD_TICKS(clamped_tempo);

	uint8_t clamped_duty_cycle_percentage = CLAMP(cfg->duty_cycle_percentage, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

	tap_tempo->high_duration_ticks = PERCENTAGE(period_ticks, clamped_duty_cycle_percentage);
	tap_tempo->low_duration_ticks = PERCENTAGE(period_ticks, 100 - clamped_duty_cycle_percentage);

	tap_tempo->set_indicator = cfg->set_indicator;
	
	tap_tempo->running_state = running_state;
	tap_tempo->reading_state = reading_state;
	
	TapTempoFSM_ReadingState_Init(&tap_tempo->reading_state, cfg->get_ticks);
	TapTempoFSM_RunningState_Init(&tap_tempo->running_state, cfg->get_ticks);
	
	FSM_Init(&tap_tempo->fsm, (const fsm_state_t *)&tap_tempo->running_state, tap_tempo);
}

void TapTempo_Update(tap_tempo_t* tap_tempo)
{
	FSM_Update(&tap_tempo->fsm, tap_tempo);
}

void TapTempo_ButtonPress(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	const tap_tempo_fsm_state_t* current_state = (const tap_tempo_fsm_state_t *)tap_tempo->fsm.current_state;

	current_state->on_button_press(ctx);
}

void TapTempo_ButtonRelease(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	const tap_tempo_fsm_state_t* current_state = (const tap_tempo_fsm_state_t *)tap_tempo->fsm.current_state;

	current_state->on_button_release(ctx);
}
