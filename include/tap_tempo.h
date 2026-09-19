//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_H
#define TAP_TEMPO_H

#include <cqueue.h>
#include <stdint.h>
#include <timer.h>

#define MIN_TEMPO (20)
#define MAX_TEMPO (240)
#define DEFAULT_TEMPO (120)

#define MIN_DUTY_CYCLE (1)
#define MAX_DUTY_CYCLE (99)
#define DEFAULT_DUTY_CYCLE (50)

#define INTERVAL_BUFFER_CAPACITY (2)
#define MIN_DELTAS_FOR_TEMPO (2)

typedef enum
{
	TAP_TEMPO_INDICATOR_STATE_LOW  = 0,
	TAP_TEMPO_INDICATOR_STATE_HIGH = 1,
} tap_tempo_indicator_state_t;

typedef void tap_tempo_set_indicator_t(tap_tempo_indicator_state_t);

typedef struct
{
	get_ticks_t* get_ticks;
	tap_tempo_set_indicator_t* set_indicator;
	uint8_t duty_cycle_percentage;
	uint8_t initial_tempo;
} tap_tempo_cfg_t;

typedef struct
{
	timer_t indicator_timer;
	timer_t input_reset_timer;
	tap_tempo_set_indicator_t* set_indicator;
	get_ticks_t* get_ticks;
	cqueue_t interval_queue;
	uint32_t interval_buffer[INTERVAL_BUFFER_CAPACITY];
	uint64_t interval_tick_sum;
	uint32_t last_tap_tick;
	bool_t has_previous_ticks;
	uint32_t high_duration_ticks;
	uint32_t low_duration_ticks;
	uint8_t duty_cycle_percentage;
	tap_tempo_indicator_state_t indicator_state;
} tap_tempo_t;

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg);
void TapTempo_Start(tap_tempo_t* tap_tempo);
void TapTempo_Update(tap_tempo_t* tap_tempo);
void TapTempo_ButtonPress(void* ctx);
void TapTempo_ButtonRelease(void* ctx);

#endif // TAP_TEMPO_H
