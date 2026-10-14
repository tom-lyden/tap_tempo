//
// Created by tomly on 13/10/2026.
//

#ifndef I2C_TEST_SSD1306_H
#define I2C_TEST_SSD1306_H

#include "ssd1306_internal.h"
#include <i2c.h>
#include <stdbool.h>
#include <stdint.h>

#define SSD1306_NUM_ROWS (64)
#define SSD1306_NUM_COLUMNS (128)

typedef enum
{
	SSD1306_BUFFER_STATE_DRAWING = 0,
	SSD1306_BUFFER_STATE_PENDING,
	SSD1306_BUFFER_STATE_TRANSMITTING
} ssd1306_buffer_state_t;

typedef struct
{
	uint8_t header[7];
	uint8_t bytes[SSD1306_NUM_COLUMNS];
} ssd1306_data_t;

typedef struct
{
	ssd1306_data_t data;
	i2c_req_t i2c_req;
	bool is_dirty;
} ssd1306_page_t;

typedef struct
{
	ssd1306_page_t pages[SSD1306_NUM_PAGES];
	ssd1306_buffer_state_t state;
} ssd1306_buffer_t;

typedef struct
{
	ssd1306_buffer_t buffers[SSD1306_NUM_BUFFERS];
	uint8_t current_buffer;
	i2c_bus_t bus;
} ssd1306_t;

bool SSD1306_Init(ssd1306_t* ctrl, i2c_bus_t bus);
void SSD1306_SetPixel(ssd1306_t* ctrl, uint8_t x, uint8_t y, bool value);
void SSD1306_SetAllPixels(ssd1306_t* ctrl, bool value);
void SSD1306_Draw(ssd1306_t* ctrl);
void SSD1306_Update(ssd1306_t* ctrl);

#endif // I2C_TEST_SSD1306_H
