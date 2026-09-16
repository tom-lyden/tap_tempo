//
// Created by tomly on 16/09/2026.
//

#include <button.h>
#include <timer.h>
#include <f401_re_hal.h>

static void register_edge(void* context);
static void handle_debounced_state(button_t* button, gpio_state_t gpio_state);
static void invoke_callback(const button_callback_t* callback);

bool_t Button_Init(button_t* button, const button_cfg_t* cfg)
{
	button->active_level = cfg->active_level;
	button->on_pressed = cfg->on_pressed;
	button->on_released = cfg->on_released;
	button->debounce_ticks = cfg->debounce_ticks;
	button->gpio_port = cfg->gpio_port;
	button->gpio_pin = cfg->gpio_pin;
	button->stable_state = GPIO_Read(cfg->gpio_port, cfg->gpio_pin);
	button->edge_counter = 0;
	button->last_seen_edge_counter = 0;

	Timer_Init(&button->debounce_timer, cfg->get_ticks);

	gpio_interrupt_cfg_t gpio_interrupt =
	{
		.type         = GPIO_INTERRUPT_ANY_EDGE,
		.callback     = register_edge,
		.callback_arg = button
	};

	return GPIO_EnableInterrupt(cfg->gpio_port, cfg->gpio_pin, &gpio_interrupt);
}

void Button_Update(button_t* button)
{
	if (button->edge_counter != button->last_seen_edge_counter)
	{
		button->last_seen_edge_counter = button->edge_counter;
		Timer_Start(&button->debounce_timer, button->debounce_ticks);

		return;
	}

	if (Timer_GetState(&button->debounce_timer) != TIMER_EXPIRED)
		return;

	gpio_state_t gpio_state = GPIO_Read(button->gpio_port, button->gpio_pin);

	if (button->edge_counter != button->last_seen_edge_counter)
		return;

	handle_debounced_state(button, gpio_state);
}

static void register_edge(void* context)
{
	button_t* button = (button_t*)context;
	button->edge_counter++;
}

static void handle_debounced_state(button_t* button, gpio_state_t gpio_state)
{
	Timer_Stop(&button->debounce_timer);

	if (gpio_state == button->stable_state)
		return;

	button->stable_state = gpio_state;

	bool_t is_pressed = (gpio_state == GPIO_STATE_LOW && button->active_level == BUTTON_ACTIVE_LOW) ||
		(gpio_state == GPIO_STATE_HIGH && button->active_level == BUTTON_ACTIVE_HIGH);

	const button_callback_t* callback = is_pressed ? &button->on_pressed : &button->on_released;

	invoke_callback(callback);
}

static void invoke_callback(const button_callback_t* callback)
{
	if (!callback || !callback->callback)
		return;

	callback->callback(callback->arg);
}
