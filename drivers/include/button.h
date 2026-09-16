//
// Created by tomly on 16/09/2026.
//

#ifndef LED_COUNTER_BUTTON_H
#define LED_COUNTER_BUTTON_H

#include <stdint.h>
#include <timer.h>
#include <f401_re_hal.h>

typedef enum
{
	BUTTON_ACTIVE_HIGH = 0,
	BUTTON_ACTIVE_LOW  = 1
} button_active_level_t;

typedef void button_callback_func_t(void*);

typedef struct
{
	button_callback_func_t* callback;
	void* arg;
} button_callback_t;

typedef struct
{
	button_active_level_t active_level;
	button_callback_t on_pressed;
	button_callback_t on_released;
	uint32_t debounce_ticks;
	get_ticks_t* get_ticks;
	gpio_port_t gpio_port;
	gpio_pin_t gpio_pin;
} button_cfg_t;

typedef struct
{
	button_active_level_t active_level;
	button_callback_t on_pressed;
	button_callback_t on_released;
	timer_t debounce_timer;
	uint32_t debounce_ticks;
	gpio_port_t gpio_port;
	gpio_pin_t gpio_pin;
	gpio_state_t stable_state;
	volatile uint32_t edge_counter;
	uint32_t last_seen_edge_counter;
} button_t;

bool_t Button_Init(button_t* button, const button_cfg_t* cfg);
void Button_Update(button_t* button);

#endif //LED_COUNTER_BUTTON_H
