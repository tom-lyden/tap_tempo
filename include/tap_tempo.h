//
// Created by tomly on 17/09/2026.
//

#ifndef TAP_TEMPO_TAP_TEMPO_H
#define TAP_TEMPO_TAP_TEMPO_H

#include <stdint.h>
#include <f401_re_hal.h>
#include <fsm.h>
#include <timer.h>

#define MIN_TEMPO (20)
#define MAX_TEMPO (240)
#define DEFAULT_TEMPO (120)

typedef struct
{
    get_ticks_t* get_ticks;
	gpio_port_t led_port;
	gpio_pin_t led_pin;
	gpio_port_t button_port;
	gpio_pin_t button_pin;
	float duty_cycle;
	uint8_t initial_tempo;
} tap_tempo_cfg_t;

typedef struct
{
	fsm_t fsm;
	timer_t led_timer;
	timer_t state_timer;
	const tap_tempo_cfg_t* cfg;
	uint32_t high_duration_ticks;
	uint32_t low_duration_ticks;
	gpio_state_t led_state;
	volatile uint32_t button_press_count;
	uint32_t last_button_press_count;
} tap_tempo_t;

void TapTempo_Init(tap_tempo_t* tap_tempo, const tap_tempo_cfg_t* cfg);
void TapTempo_Update(tap_tempo_t* tap_tempo);

bool_t TapTempo_IsButtonPressed(const tap_tempo_t* tap_tempo);

#endif //TAP_TEMPO_TAP_TEMPO_H
