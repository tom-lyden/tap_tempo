//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_TAP_TEMPO_H
#define TAP_TEMPO_TAP_TEMPO_H

#include <stdint.h>
#include <fsm.h>
#include <timer.h>

#define MIN_TEMPO (20)
#define MAX_TEMPO (240)
#define DEFAULT_TEMPO (120)

#define MIN_DUTY_CYCLE (1)
#define MAX_DUTY_CYCLE (99)
#define DEFAULT_DUTY_CYCLE (50)

typedef enum
{
	TAP_TEMPO_INDICATOR_STATE_LOW = 0,
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
	fsm_t fsm;
	timer_t led_timer;
	timer_t state_timer;
	uint32_t high_duration_ticks;
	uint32_t low_duration_ticks;
	tap_tempo_set_indicator_t* set_indicator;
	tap_tempo_indicator_state_t indicator_state;
	volatile uint32_t button_press_count;
	uint32_t last_button_press_count;
} tap_tempo_t;

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg);
void TapTempo_Update(tap_tempo_t* tap_tempo);

int TapTempo_IsButtonPressed(const tap_tempo_t* tap_tempo);

#endif //TAP_TEMPO_TAP_TEMPO_H
