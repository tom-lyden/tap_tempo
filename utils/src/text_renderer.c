//
// Created by tomly on 14/10/2026.
//

#include <text_renderer.h>

#include <stdint.h>
#include <string.h>

#include "text_renderer_internal.h"

static void render_char(const text_renderer_t* renderer, const uint8_t* char_map, int origin_x, int origin_y);
static void render_spacing(const text_renderer_t* renderer, int origin_x, int origin_y);

void Text_Renderer_Render(const text_renderer_t* renderer)
{
	uint16_t length = strlen(renderer->text);
	
	int text_width = length * renderer->scale * (FONT_WIDTH + FONT_SPACING) - (length > 0 ? renderer->scale * FONT_SPACING : 0);
	int text_height = renderer->scale * FONT_HEIGHT;
	
	int origin_x = renderer->pos_x;
	int origin_y = renderer->pos_y;
	
	switch (renderer->alignment)
	{
		case TEXT_ALIGNMENT_TOP_LEFT:
			break;
		case TEXT_ALIGNMENT_TOP_CENTER:
			origin_x -= text_width / 2;
			break;
		case TEXT_ALIGNMENT_TOP_RIGHT:
			origin_x -= text_width;
			break;
		case TEXT_ALIGNMENT_BOTTOM_LEFT:
			origin_y -= text_height;
			break;
		case TEXT_ALIGNMENT_BOTTOM_CENTER:
			origin_x -= text_width / 2;
			origin_y -= text_height;
			break;
		case TEXT_ALIGNMENT_BOTTOM_RIGHT:
			origin_y -= text_height;
			origin_x -= text_width;
			break;
		case TEXT_ALIGNMENT_MIDDLE_LEFT:
			origin_y -= text_height / 2;
			break;
		case TEXT_ALIGNMENT_MIDDLE_CENTER:
			origin_x -= text_width / 2;
			origin_y -= text_height / 2;
			break;
		case TEXT_ALIGNMENT_MIDDLE_RIGHT:
			origin_y -= text_height / 2;
			origin_x -= text_width;
			break;
		default:
			__builtin_unreachable();
	}
	
	for (uint16_t i = 0; i < length; i++)
	{
		render_char(renderer, GET_CHAR(renderer->text[i]), origin_x, origin_y);
		
		origin_x += renderer->scale * FONT_WIDTH;
		
		if (i == length - 1) // No need to render final spacing
			return;
		
		render_spacing(renderer, origin_x, origin_y);
		origin_x += renderer->scale * FONT_SPACING;
	}
}

static void render_char(const text_renderer_t* renderer, const uint8_t* char_map, int origin_x, int origin_y)
{
	for (uint8_t i = 0; i < FONT_WIDTH; i++)
	{
		uint8_t column = char_map[i];
		
		for (uint8_t j = 0; j < renderer->scale; j++)
		{
			int target_x = origin_x + j + i * renderer->scale;
			
			if (target_x < 0)
				continue;
			
			if (target_x >= renderer->width) // No reason to continue if rendering beyond screen
				return;
			
			for (uint8_t k = 0; k < FONT_HEIGHT; k++)
			{
				bool pixel = column & (1U << k);
				
				for (uint8_t l = 0; l < renderer->scale; l++)
				{
					int target_y = origin_y + l + k * renderer->scale;
					
					if (target_y < 0 || target_y >= renderer->height)
						continue; // Can still render partial letters sliced horizontally
				
					renderer->set_pixel(renderer->ctx, target_x, target_y, pixel);
				}
			}
		}
	}
}

static void render_spacing(const text_renderer_t* renderer, int origin_x, int origin_y)
{
	if (origin_x <= -renderer->scale * FONT_SPACING || origin_x >= renderer->width)
		return;
	
	if (origin_y <= -renderer->scale * FONT_HEIGHT || origin_y >= renderer->height)
		return;
	
	for (uint8_t i = 0; i < renderer->scale; i++)
	{
		int target_x = origin_x + i;
		
		if (target_x < 0 || target_x >= renderer->width)
			continue;
		
		for (uint8_t j = 0; j < renderer->scale * FONT_HEIGHT; j++)
		{
			int target_y = origin_y + j;
			
			if (target_y < 0 || target_y >= renderer->height)
				continue;
			
			renderer->set_pixel(renderer->ctx, target_x, target_y, false);
		}
	}
}
