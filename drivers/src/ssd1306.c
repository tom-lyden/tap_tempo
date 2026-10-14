//
// Created by tomly on 13/10/2026.
//

#include <ssd1306.h>
#include "ssd1306_internal.h"

#include <string.h>

#define SSD1306_NUM_ROWS_PER_PAGE ((SSD1306_NUM_ROWS) / (SSD1306_NUM_PAGES))

static const i2c_device_address_t device_address = 
{
	.type = I2C_DEVICE_ADDRESS_7BIT,
	.address = SSD1306_TARGET_ADDRESS,
};

static const uint8_t init_cmd_buffer[] = 
{
	SSD1306_CTRL_BYTE,
	SSD1306_CMD_CHARGE_PUMP, SSD1306_DATA_CHARGE_PUMP(true), 
	SSD1306_CMD_SET_DISPLAY(true)
};

static const uint8_t page_header[] = 
{
	SSD1306_CMD_CONTINUATION, 
	SSD1306_CMD_SET_PAGE_START(0), 
	SSD1306_CMD_CONTINUATION, 
	SSD1306_CMD_SET_LOWER_COL_START(0), 
	SSD1306_CMD_CONTINUATION, 
	SSD1306_CMD_SET_HIGHER_COL_START(0), 
	SSD1306_DATA_SELECTION
};

// TODO what if there are several such screens initialized in succession?
static i2c_req_t init_device_i2c_req =
{
	.device_address = 
	{
		.type = I2C_DEVICE_ADDRESS_7BIT,
		.address = SSD1306_TARGET_ADDRESS
	},
	.data_buffer = init_cmd_buffer,
	.data_length = sizeof(init_cmd_buffer),
};

static bool is_transmission_done(const ssd1306_buffer_t* buffer);
static bool is_any_buffer_transmitting(const ssd1306_t* ctrl);
static void write_i2c_reqs(i2c_bus_t bus, ssd1306_buffer_t* buffer);

bool SSD1306_Init(ssd1306_t* ctrl, i2c_bus_t bus)
{
	ctrl->bus = bus;
	ctrl->current_buffer = 0;
	
	for (uint8_t i = 0; i < SSD1306_NUM_BUFFERS; i++)
	{
		ssd1306_buffer_t* buffer = &ctrl->buffers[i];
		buffer->state = SSD1306_BUFFER_STATE_DRAWING;
		
		for (uint8_t j = 0; j < SSD1306_NUM_PAGES; j++)
		{
			ssd1306_page_t* page = &buffer->pages[j];
			
			page->is_dirty = false;
			
			memcpy(&page->data.header, page_header, sizeof(page->data.header));
			page->data.header[SSD1306_PAGE_HEADER_PAGE_START_INDEX] = SSD1306_CMD_SET_PAGE_START(j);
			
			page->i2c_req.register_addr = NULL;
			page->i2c_req.register_addr_length = 0;
			page->i2c_req.device_address = device_address;
			page->i2c_req.data_buffer = (const uint8_t *)&page->data;
			page->i2c_req.data_length = sizeof(page->data);
		}
	}
	
	if (!I2C_Write(bus, &init_device_i2c_req))
		return false;
	
	SSD1306_SetAllPixels(ctrl, false);
	SSD1306_Draw(ctrl);
		
	// Force clear
	for (uint8_t j = 0; j < SSD1306_NUM_PAGES; j++)
	{
		ssd1306_page_t* page = &ctrl->buffers[ctrl->current_buffer].pages[j];
		page->is_dirty = true;
	}
	
	return true;
}

void SSD1306_SetPixel(ssd1306_t* ctrl, uint8_t x, uint8_t y, bool value)
{
	ssd1306_buffer_t* buffer = &ctrl->buffers[ctrl->current_buffer];
	
	buffer->state = SSD1306_BUFFER_STATE_DRAWING;
	
	uint8_t page = y / SSD1306_NUM_ROWS_PER_PAGE;
	uint8_t row = y % SSD1306_NUM_ROWS_PER_PAGE;
	
	uint8_t* bytes = buffer->pages[page].data.bytes;
	
	if (value)
	{
		bytes[x] |= (1U << row);
	}
	else
	{
		bytes[x] &= ~(1U << row);
	}
}

void SSD1306_SetAllPixels(ssd1306_t* ctrl, bool value)
{
	ssd1306_buffer_t* buffer = &ctrl->buffers[ctrl->current_buffer];
	
	buffer->state = SSD1306_BUFFER_STATE_DRAWING;
	
	uint8_t word = value ? 0xFF : 0x00;
	
	for (uint8_t page = 0; page < SSD1306_NUM_PAGES; page++)
		for (uint8_t column = 0; column < SSD1306_NUM_COLUMNS; column++)
			buffer->pages[page].data.bytes[column] = word;
}

void SSD1306_Draw(ssd1306_t* ctrl)
{
	ssd1306_buffer_t* buffer = &ctrl->buffers[ctrl->current_buffer];
	buffer->state = SSD1306_BUFFER_STATE_PENDING;
	
	ssd1306_buffer_t* other_buffer = &ctrl->buffers[ctrl->current_buffer ^ 1];
	
	for (uint8_t page = 0; page < SSD1306_NUM_PAGES; page++)
	{
		bool is_dirty = memcmp(&buffer->pages[page].data, &other_buffer->pages[page].data, sizeof(buffer->pages[page].data)) != 0;
		buffer->pages[page].is_dirty = is_dirty;
	}
}

void SSD1306_Update(ssd1306_t* ctrl)
{
	ssd1306_buffer_t* other_buffer = &ctrl->buffers[ctrl->current_buffer ^ 1];
	
	if (is_transmission_done(other_buffer))
		other_buffer->state = SSD1306_BUFFER_STATE_DRAWING;
	
	ssd1306_buffer_t* buffer = &ctrl->buffers[ctrl->current_buffer];
	
	if (!is_any_buffer_transmitting(ctrl) && buffer->state == SSD1306_BUFFER_STATE_PENDING)
	{
		buffer->state = SSD1306_BUFFER_STATE_TRANSMITTING;
		write_i2c_reqs(ctrl->bus, buffer);
		ctrl->current_buffer ^= 1;
	}
}

static bool is_transmission_done(const ssd1306_buffer_t* buffer)
{
	switch (buffer->state)
	{
		case SSD1306_BUFFER_STATE_DRAWING:
			return true;
		case SSD1306_BUFFER_STATE_PENDING:
			return false;
		case SSD1306_BUFFER_STATE_TRANSMITTING:
		{
			for (uint8_t page = 0; page < SSD1306_NUM_PAGES; page++)
			{
				if (buffer->pages[page].is_dirty && 
					buffer->pages[page].i2c_req.state != I2C_BUS_REQ_DONE &&
					buffer->pages[page].i2c_req.state != I2C_BUS_REQ_ERROR)
					return false;
			}
			
			break;
		}
		default:
			__builtin_unreachable();
	}
	
	return true;
}

static bool is_any_buffer_transmitting(const ssd1306_t* ctrl)
{
	for (uint8_t i = 0; i < SSD1306_NUM_BUFFERS; i++)
		if (ctrl->buffers[i].state == SSD1306_BUFFER_STATE_TRANSMITTING)
			return true;
	
	return false;
}

static void write_i2c_reqs(i2c_bus_t bus, ssd1306_buffer_t* buffer)
{
	for (uint8_t page = 0; page < SSD1306_NUM_PAGES; page++)
	{
		if (!buffer->pages[page].is_dirty)
			continue;
		
		I2C_Write(bus, &buffer->pages[page].i2c_req);
	}
}
