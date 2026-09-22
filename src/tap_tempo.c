//
// Created by tomly on 17/09/2026.
//

#include <tap_tempo.h>
#include <math_utils.h>
#include <cqueue.h>

#define INPUT_RESET_TO_MAX_TICKS_RATIO (3 / 2)

static void set_durations(tap_tempo_t* tap_tempo, uint32_t period_ticks);
static void start_indicator_phase(tap_tempo_t* tap_tempo);
static inline uint32_t tempo_period_ticks(const tap_tempo_cfg_t* tap_tempo_cfg, uint32_t tempo);

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg)
{
	tap_tempo->interval_tick_sum = 0;
	tap_tempo->previous_tap_ticks = 0;
	tap_tempo->has_previous_tap = false;

	CQueue_Init(&tap_tempo->interval_queue, tap_tempo->interval_buffer, TAP_TEMPO_INTERVAL_BUFFER_CAPACITY);

	tap_tempo->min_interval_ticks = tempo_period_ticks(cfg, TAP_TEMPO_MAX_BPM);
	tap_tempo->max_interval_ticks = tempo_period_ticks(cfg, TAP_TEMPO_MIN_BPM);
	tap_tempo->reset_interval_ticks = tempo_period_ticks(cfg, TAP_TEMPO_MIN_BPM) * INPUT_RESET_TO_MAX_TICKS_RATIO;
	tap_tempo->duty_cycle_percentage = CLAMP(cfg->duty_cycle_percentage, MIN_DUTY_CYCLE, MAX_DUTY_CYCLE);

	uint8_t clamped_tempo = CLAMP(cfg->initial_tempo, MIN_TEMPO, MAX_TEMPO);
	uint32_t period_ticks = tempo_period_ticks(cfg, clamped_tempo);
	set_durations(tap_tempo, period_ticks);

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
		tap_tempo->has_previous_tap = false;
		tap_tempo->previous_tap_ticks = 0;
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

	if (!tap_tempo->has_previous_tap)
	{
		tap_tempo->previous_tap_ticks = current_ticks;
		tap_tempo->has_previous_tap = true;
		return;
	}

	uint32_t new_interval = current_ticks - tap_tempo->previous_tap_ticks;
	tap_tempo->previous_tap_ticks = current_ticks;

	if (CQueue_IsFull(&tap_tempo->interval_queue))
	{
		uint32_t oldest_interval = 0;
		if (!CQueue_Pop(&tap_tempo->interval_queue, &oldest_interval))
			return;

		tap_tempo->interval_tick_sum -= oldest_interval;
	}

	if (!CQueue_Push(&tap_tempo->interval_queue, new_interval))
		return;

	tap_tempo->interval_tick_sum += new_interval;

	uint8_t num_intervals = CQueue_Count(&tap_tempo->interval_queue);
	if (num_intervals < TAP_TEMPO_MIN_INTERVALS_FOR_TEMPO)
		return;

	uint32_t period_ticks = tap_tempo->interval_tick_sum / num_intervals;
	set_durations(tap_tempo, period_ticks);

	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_HIGH;
	start_indicator_phase(tap_tempo);
}

void TapTempo_ButtonRelease(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;

	Timer_Start(&tap_tempo->input_reset_timer, tap_tempo->reset_interval_ticks);
}

static void set_durations(tap_tempo_t* tap_tempo, uint32_t period_ticks)
{
	uint32_t clamped_period_ticks = CLAMP(period_ticks, tap_tempo->min_interval_ticks, tap_tempo->max_interval_ticks);

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

static inline uint32_t tempo_period_ticks(const tap_tempo_cfg_t* tap_tempo_cfg, uint32_t tempo)
{
	return 60 * tap_tempo_cfg->tick_frequency_hz / tempo;
}
