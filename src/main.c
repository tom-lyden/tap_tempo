#include <f401_re_hal.h>
#include <board_definition.h>
#include <button.h>
#include <tap_tempo.h>
#include <stdbool.h>

#define TICK_FREQUENCY_HZ (1000)
#define DEBOUNCE_MSEC (1)
#define DEBOUNCE_TICKS (((TICK_FREQUENCY_HZ) * (DEBOUNCE_MSEC)) / 1000)

static void set_led_gpio(tap_tempo_indicator_state_t tap_tempo_indicator_state);

int main(void)
{
	if (Ticks_Init(TICK_FREQUENCY_HZ) != true)
	{
		while (1);
	}

	gpio_config_t gpio_output_init =
	{
		.mode = GPIO_MODE_OUTPUT,
	};

	GPIO_Init(LED_GPIO_PORT, LED_PIN, &gpio_output_init);

	gpio_config_t gpio_input_init =
	{
		.mode = GPIO_MODE_INPUT,
		.pupd = GPIO_PUPD_PULLDOWN,
	};

	GPIO_Write(LED_GPIO_PORT, LED_PIN, GPIO_STATE_LOW);

	GPIO_Init(BUTTON_GPIO_PORT, BUTTON_PIN, &gpio_input_init);

	tap_tempo_t tap_tempo;
	tap_tempo_cfg_t tap_tempo_cfg =
	{
		.get_ticks             = Ticks_GetTick,
		.tick_frequency_hz     = TICK_FREQUENCY_HZ,
		.set_indicator         = set_led_gpio,
		.duty_cycle_percentage = TAP_TEMPO_DEFAULT_DUTY_CYCLE,
		.initial_tempo         = TAP_TEMPO_DEFAULT_BPM
	};

	button_t button;
	button_cfg_t button_cfg =
	{
		.on_press       = { .callback = TapTempo_ButtonPress, .arg = &tap_tempo },
		.on_release     = { .callback = TapTempo_ButtonRelease, .arg = &tap_tempo },
		.debounce_ticks = DEBOUNCE_TICKS,
		.get_ticks      = Ticks_GetTick,
		.gpio_port      = BUTTON_GPIO_PORT,
		.gpio_pin       = BUTTON_PIN,
		.active_level   = BUTTON_ACTIVE_HIGH
	};

	if (!Button_Init(&button, &button_cfg))
	{
		while (1);
	}

	TapTempo_Init(&tap_tempo, &tap_tempo_cfg);
	TapTempo_Start(&tap_tempo);

	while (1)
	{
		Button_Update(&button);
		TapTempo_Update(&tap_tempo);
	}

	return 0;
}

static void set_led_gpio(tap_tempo_indicator_state_t state)
{
	gpio_state_t led_state = state == TAP_TEMPO_INDICATOR_STATE_HIGH ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
	GPIO_Write(LED_GPIO_PORT, LED_PIN, led_state);
}
