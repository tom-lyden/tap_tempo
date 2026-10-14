#include <f401_re_hal.h>
#include <board_definition.h>
#include <button.h>
#include <ssd1306.h>
#include <tap_tempo.h>
#include <stdbool.h>
#include <string.h>
#include <text_renderer.h>

#define TICK_FREQUENCY_HZ (1000)
#define DEBOUNCE_MSEC (10)
#define DEBOUNCE_TICKS (((TICK_FREQUENCY_HZ) * (DEBOUNCE_MSEC)) / 1000)
#define TEMPO_STR_BUF_LEN (4)

static void set_led_gpio(tap_tempo_indicator_state_t tap_tempo_indicator_state);
static void set_display(void* ctx, uint8_t new_tempo);
static void set_pixel(void* ctx, uint8_t x, uint8_t y, bool value);
static void uitoa(uint32_t value, char* str);
static void reverse_str(char* str);

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
	
	i2c_cfg_t i2c_cfg =
	{
		.sda_pin = SSD1306_I2C_SDA_PIN,
		.scl_pin = SSD1306_I2C_SCL_PIN,
		.clk_freq_hz = CLK_MODE_FAST_FREQ_HZ,
		.clk_mode = I2C_CLK_MODE_FAST_DC_2
	};
	
	I2C_Init(&i2c_cfg);
	
	ssd1306_t ssd1306;
	
	if (!SSD1306_Init(&ssd1306, SSD1306_I2C_BUS))
	{
		while (1);
	}

	tap_tempo_t tap_tempo;
	tap_tempo_cfg_t tap_tempo_cfg =
	{
		.get_ticks             = Ticks_GetTick,
		.tick_frequency_hz     = TICK_FREQUENCY_HZ,
		.set_indicator         = set_led_gpio,
		.on_new_tempo		   = set_display, 
		.on_new_tempo_ctx	   = &ssd1306,
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
		I2C_Update();
		
		SSD1306_Update(&ssd1306);
		
		TapTempo_Update(&tap_tempo);
	}

	return 0;
}

static void set_led_gpio(tap_tempo_indicator_state_t state)
{
	gpio_state_t led_state = state == TAP_TEMPO_INDICATOR_STATE_HIGH ? GPIO_STATE_HIGH : GPIO_STATE_LOW;
	GPIO_Write(LED_GPIO_PORT, LED_PIN, led_state);
}

static void set_display(void* ctx, uint8_t new_tempo)
{
	char tempo_str[TEMPO_STR_BUF_LEN] = { 0 };
	uitoa(new_tempo, tempo_str);
	
	ssd1306_t* ssd1306 = (ssd1306_t*)ctx;
	
	text_renderer_t text_renderer = 
	{
		.set_pixel = set_pixel,
		.ctx = ssd1306,
		.width = SSD1306_NUM_COLUMNS,
		.height = SSD1306_NUM_ROWS,
		.pos_x = SSD1306_NUM_COLUMNS / 2,
		.pos_y = SSD1306_NUM_ROWS / 2,
		.scale = 3,
		.alignment = TEXT_ALIGNMENT_MIDDLE_CENTER,
		.text = tempo_str
	};
	
	SSD1306_SetAllPixels(ssd1306, false);
	
	Text_Renderer_Render(&text_renderer);
	
	SSD1306_Draw(ssd1306);
}

static void set_pixel(void* ctx, uint8_t x, uint8_t y, bool value)
{
	ssd1306_t* ssd1306 = (ssd1306_t*)ctx;
	
	SSD1306_SetPixel(ssd1306, x, y, value);
}

static void uitoa(uint32_t value, char* str)
{
	const uint8_t base = 10;
	uint8_t i = 0;
	
	do
	{
		uint8_t digit = value % base;
		str[i] = digit + '0';
		
		value /= base;
		i++;
	} while (value > 0);
	
	str[i] = '\0';
	
	reverse_str(str);
}

static void reverse_str(char* str)
{
	char* front = str;
	char* back = str + strlen(str) - 1;
	
	while (front < back)
	{
		char tmp = *front;
		*front = *back;
		*back = tmp;
		
		front++;
		back--;
	}
}
