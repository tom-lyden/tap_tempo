//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <math_utils.h>
#include <cqueue.h>

#define TEMPO_PERIOD_TICKS(tempo) (60 * (TICK_FREQUENCY_HZ) / (tempo))
#define MIN_TEMPO_PERIOD_TICKS (TEMPO_PERIOD_TICKS(MAX_TEMPO))
#define MAX_TEMPO_PERIOD_TICKS (TEMPO_PERIOD_TICKS(MIN_TEMPO))
#define INPUT_RESET_TICKS ((MAX_TEMPO_PERIOD_TICKS) * 3 / 2)

static void set_durations(tap_tempo_t* tap_tempo, uint32_t period_ticks);
static void start_indicator_phase(tap_tempo_t* tap_tempo);

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	tap_tempo->interval_tick_sum = 0;
	tap_tempo->last_tap_tick = 0;
	tap_tempo->has_previous_ticks = FALSE;
	
	tap_tempo->duty_cycle_percentage = CLAMP(cfg->duty_cycle_percentage, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);
	uint32_t period_ticks = TEMPO_PERIOD_TICKS(CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO));
	set_durations(tap_tempo, period_ticks);
	
	CQueue_Init(&tap_tempo->interval_queue, tap_tempo->interval_buffer, INTERVAL_BUFFER_CAPACITY);

	tap_tempo->set_indicator = cfg->set_indicator;
	tap_tempo->get_ticks = cfg->get_ticks;
	
	Timer_Init(&tap_tempo->indicator_timer, cfg->get_ticks);
	Timer_Init(&tap_tempo->input_reset_timer, cfg->get_ticks);
}

void TapTempo_Start(tap_tempo_t* tap_tempo)
{
	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_HIGH;
	start_indicator_phase(tap_tempo);
}

void TapTempo_Update(tap_tempo_t* tap_tempo)
{
	if (Timer_GetState(&tap_tempo->input_reset_timer) == TIMER_EXPIRED)
	{
		tap_tempo->has_previous_ticks = FALSE;
		tap_tempo->last_tap_tick = 0;
		tap_tempo->interval_tick_sum = 0;
		CQueue_Clear(&tap_tempo->interval_queue);
		
		Timer_Stop(&tap_tempo->input_reset_timer);
	}
	
	if (Timer_GetState(&tap_tempo->indicator_timer) == TIMER_EXPIRED)
	{
		tap_tempo->indicator_state = tap_tempo->indicator_state == TAP_TEMPO_INDICATOR_STATE_HIGH
			? TAP_TEMPO_INDICATOR_STATE_LOW
			: TAP_TEMPO_INDICATOR_STATE_HIGH;

		start_indicator_phase(tap_tempo);
	}
}

void TapTempo_ButtonPress(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;

	Timer_Stop(&tap_tempo->input_reset_timer);
	
	uint32_t current_ticks = tap_tempo->get_ticks();
	
	if (!tap_tempo->has_previous_ticks)
	{
		tap_tempo->last_tap_tick = current_ticks;
		tap_tempo->has_previous_ticks = TRUE;
		return;
	}
	
	uint32_t new_delta =  current_ticks - tap_tempo->last_tap_tick;
	tap_tempo->last_tap_tick = current_ticks;
	 
	if (CQueue_IsFull(&tap_tempo->interval_queue))
	{
		uint32_t oldest_delta = 0;
		if (!CQueue_Pop(&tap_tempo->interval_queue, &oldest_delta))
			return;
		
		tap_tempo->interval_tick_sum -= oldest_delta;
	}
	
	if (!CQueue_Push(&tap_tempo->interval_queue, new_delta))
		return;
	
	tap_tempo->interval_tick_sum += new_delta;
	
	uint8_t num_deltas = CQueue_Count(&tap_tempo->interval_queue);
	if (num_deltas < MIN_DELTAS_FOR_TEMPO)
		return;

	uint64_t period_ticks = tap_tempo->interval_tick_sum / num_deltas;
	set_durations(tap_tempo, period_ticks);
	
	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_HIGH;
	start_indicator_phase(tap_tempo);
}

void TapTempo_ButtonRelease(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;

	Timer_Start(&tap_tempo->input_reset_timer, INPUT_RESET_TICKS);
}

static void set_durations(tap_tempo_t* tap_tempo, uint32_t period_ticks)
{
	uint32_t clamped_period_ticks = CLAMP(period_ticks, MIN_TEMPO_PERIOD_TICKS, MAX_TEMPO_PERIOD_TICKS);
	
	tap_tempo->high_duration_ticks = PERCENTAGE(clamped_period_ticks, tap_tempo->duty_cycle_percentage);
	tap_tempo->low_duration_ticks = PERCENTAGE(clamped_period_ticks, 100 - tap_tempo->duty_cycle_percentage);
}

static void start_indicator_phase(tap_tempo_t* tap_tempo)
{
	tap_tempo->set_indicator(tap_tempo->indicator_state);

	uint32_t duration_ticks = tap_tempo->indicator_state == TAP_TEMPO_INDICATOR_STATE_LOW
		? tap_tempo->low_duration_ticks
		: tap_tempo->high_duration_ticks;

	Timer_Start(&tap_tempo->indicator_timer, duration_ticks);
}
