#include <f401_re_hal.h>
#include <board_definition.h>
#include <button.h>
#include <tap_tempo.h>

#define DEBOUNCE_MSEC (1)
#define DEBOUNCE_TICKS (((TICK_FREQUENCY_HZ) * (DEBOUNCE_MSEC)) / 1000)

static void set_led_gpio(tap_tempo_indicator_state_t tap_tempo_indicator_state);

int main(void)
{
	systick_cfg_t systick_cfg =
	{
		.tick_freq      = TICK_FREQUENCY_HZ,
		.clk_src        = SYSTICK_CLK_SRC_AHB_DIV_8,
		.enable_irq     = TRUE,
		.enable_counter = TRUE
	};

	if (SysTick_Init(&systick_cfg) != TRUE)
		return -1;

	gpio_config_t gpio_output_init =
	{
		.mode = GPIO_MODE_OUTPUT,
	};

	GPIO_Init(LED_GPIO_PORT, LED_PIN_RED, &gpio_output_init);
	GPIO_Init(LED_GPIO_PORT, LED_PIN_GREEN, &gpio_output_init);

	gpio_config_t gpio_input_init =
	{
		.mode = GPIO_MODE_INPUT,
		.pupd = GPIO_PUPD_PULLDOWN,
	};

	GPIO_Init(BUTTON_GPIO_PORT, BUTTON_PIN, &gpio_input_init);
	
	tap_tempo_t tap_tempo;
	tap_tempo_cfg_t tap_tempo_cfg =
	{
		.get_ticks = SysTick_GetTick,
		.set_indicator = set_led_gpio,
		.duty_cycle_percentage = DEFAULT_DUTY_CYCLE,
		.initial_tempo = 20
	};

	button_t button;
	button_cfg_t button_cfg =
	{
		.on_pressed = { .callback = TapTempo_ButtonPress, .arg = &tap_tempo },
		.on_released = { .callback = TapTempo_ButtonRelease, .arg = &tap_tempo },
		.debounce_ticks = DEBOUNCE_TICKS,
		.get_ticks      = SysTick_GetTick,
		.gpio_port      = BUTTON_GPIO_PORT,
		.gpio_pin       = BUTTON_PIN,
		.active_level   = BUTTON_ACTIVE_HIGH
	};

	if (!Button_Init(&button, &button_cfg))
	{
		while (1);
	}
	
	TapTempo_Init(&tap_tempo, &tap_tempo_cfg);

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
	GPIO_Write(LED_GPIO_PORT, LED_PIN_GREEN, led_state);
}
