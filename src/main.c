#include <f401_re_hal.h>
#include <board_definition.h>
#include <button.h>

#define TICK_FREQUENCY_HZ (1000)
#define DEBOUNCE_MSEC (1)
#define DEBOUNCE_TICKS (((TICK_FREQUENCY_HZ) * (DEBOUNCE_MSEC)) / 1000)

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

	bool_t is_pressed = FALSE;

	button_t button;
	button_cfg_t button_cfg =
	{
		.on_pressed     = {.callback = register_press, .arg = &is_pressed},
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

	while (1)
	{
		Button_Update(&button);
	}

	return 0;
}
