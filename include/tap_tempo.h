//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_H
#define TAP_TEMPO_H

#include <cqueue.h>
#include <stdint.h>
#include <timer.h>
#include <stdbool.h>

#define TAP_TEMPO_MIN_BPM (20)
#define TAP_TEMPO_MAX_BPM (240)
#define TAP_TEMPO_DEFAULT_BPM (120)

#define TAP_TEMPO_MIN_DUTY_CYCLE (1)
#define TAP_TEMPO_MAX_DUTY_CYCLE (99)
#define TAP_TEMPO_DEFAULT_DUTY_CYCLE (50)

#define TAP_TEMPO_INTERVAL_BUFFER_CAPACITY (2)
#define TAP_TEMPO_MIN_INTERVALS_FOR_TEMPO (2)

typedef enum
{
	TAP_TEMPO_INDICATOR_STATE_LOW  = 0,
	TAP_TEMPO_INDICATOR_STATE_HIGH = 1,
} tap_tempo_indicator_state_t;

typedef void tap_tempo_set_indicator_t(tap_tempo_indicator_state_t);

typedef struct
{
	get_ticks_t* get_ticks;
	uint32_t tick_frequency_hz;
	tap_tempo_set_indicator_t* set_indicator;
	uint8_t duty_cycle_percentage;
	uint8_t initial_tempo;
} tap_tempo_cfg_t;

typedef struct
{
	uint32_t min_interval_ticks;
	uint32_t max_interval_ticks;
	uint32_t reset_interval_ticks;
	uint8_t duty_cycle_percentage;

	timer_t indicator_timer;
	tap_tempo_set_indicator_t* set_indicator;
	uint32_t high_duration_ticks;
	uint32_t low_duration_ticks;
	tap_tempo_indicator_state_t indicator_state;

	timer_t input_reset_timer;
	get_ticks_t* get_ticks;
	cqueue_t interval_queue;
	uint32_t interval_buffer[TAP_TEMPO_INTERVAL_BUFFER_CAPACITY];
	uint64_t interval_tick_sum;
	uint32_t previous_tap_ticks;
	bool has_previous_tap;
} tap_tempo_t;

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg);
void TapTempo_Start(tap_tempo_t* tap_tempo);
void TapTempo_Update(tap_tempo_t* tap_tempo);
void TapTempo_ButtonPress(void* ctx);
void TapTempo_ButtonRelease(void* ctx);

#endif // TAP_TEMPO_H
