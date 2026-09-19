//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <board_definition.h>
#include <math_utils.h>
#include <cqueue.h>

#define TEMPO_PERIOD_TICKS(tempo) (60 * (TICK_FREQUENCY_HZ) / (tempo))
#define INPUT_RESET_TICKS (TEMPO_PERIOD_TICKS(MIN_TEMPO) << 1UL)

static void set_durations(tap_tempo_t* tap_tempo, uint64_t period_ticks);
static void start_indicator_phase(tap_tempo_t* tap_tempo);

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	uint32_t clamped_tempo = CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO);
	uint64_t period_ticks = TEMPO_PERIOD_TICKS(clamped_tempo);

	tap_tempo->duty_cycle_percentage = CLAMP(cfg->duty_cycle_percentage, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

	set_durations(tap_tempo, period_ticks);

	tap_tempo->set_indicator = cfg->set_indicator;
	tap_tempo->get_ticks = cfg->get_ticks;
	
	Timer_Init(&tap_tempo->indicator_timer, cfg->get_ticks);
	Timer_Init(&tap_tempo->reset_input_timer, cfg->get_ticks);
	
	CQueue_Init(&tap_tempo->period_queue, tap_tempo->queue_buffer, PERIOD_QUEUE_CAPACITY);
}

void TapTempo_Start(tap_tempo_t* tap_tempo)
{
	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_HIGH;
	start_indicator_phase(tap_tempo);
}

void TapTempo_Update(tap_tempo_t* tap_tempo)
{
	if (Timer_GetState(&tap_tempo->reset_input_timer) == TIMER_EXPIRED)
	{
		tap_tempo->last_captured_ticks = 0;
		tap_tempo->running_delta_tick_sum = 0;
		CQueue_Clear(&tap_tempo->period_queue);
		
		Timer_Stop(&tap_tempo->reset_input_timer);
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

	tap_tempo->set_indicator(TAP_TEMPO_INDICATOR_STATE_HIGH);
	Timer_Stop(&tap_tempo->indicator_timer);
	
	uint32_t current_ticks = tap_tempo->get_ticks();
	
	if (tap_tempo->last_captured_ticks == 0)
	{
		tap_tempo->last_captured_ticks = current_ticks;
		return;
	}
	
	uint32_t new_delta =  current_ticks - tap_tempo->last_captured_ticks;
	tap_tempo->last_captured_ticks = current_ticks;
	 
	if (CQueue_IsFull(&tap_tempo->period_queue))
	{
		uint32_t oldest_delta = 0;
		if (!CQueue_Pop(&tap_tempo->period_queue, &oldest_delta))
			return;
		
		tap_tempo->running_delta_tick_sum -= oldest_delta;
	}
	
	if (!CQueue_Push(&tap_tempo->period_queue, new_delta))
		return;
	
	tap_tempo->running_delta_tick_sum += new_delta;
	
	uint8_t num_deltas = CQueue_Count(&tap_tempo->period_queue);
	if (num_deltas < MIN_DELTAS_FOR_TEMPO)
		return;

	uint64_t period_ticks = tap_tempo->running_delta_tick_sum / num_deltas;
	set_durations(tap_tempo, period_ticks);
}

void TapTempo_ButtonRelease(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;

	Timer_Start(&tap_tempo->reset_input_timer, INPUT_RESET_TICKS);
	
	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_LOW;
	start_indicator_phase(tap_tempo);
}

static void set_durations(tap_tempo_t* tap_tempo, uint64_t period_ticks)
{
	tap_tempo->high_duration_ticks = PERCENTAGE(period_ticks, tap_tempo->duty_cycle_percentage);
	tap_tempo->low_duration_ticks = PERCENTAGE(period_ticks, 100 - tap_tempo->duty_cycle_percentage);
}

static void start_indicator_phase(tap_tempo_t* tap_tempo)
{
	tap_tempo->set_indicator(tap_tempo->indicator_state);

	uint32_t duration_ticks = tap_tempo->indicator_state == TAP_TEMPO_INDICATOR_STATE_LOW
		? tap_tempo->low_duration_ticks
		: tap_tempo->high_duration_ticks;

	Timer_Start(&tap_tempo->indicator_timer, duration_ticks);
}
