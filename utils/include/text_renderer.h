//
// Created by tomly on 14/10/2026.
//

#ifndef TAP_TEMPO_TEXT_RENDERER_H
#define TAP_TEMPO_TEXT_RENDERER_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*set_pixel_t)(void* ctx, uint8_t x, uint8_t y, bool value);

typedef enum
{
	TEXT_ALIGNMENT_TOP_LEFT = 0,
	TEXT_ALIGNMENT_TOP_CENTER,
	TEXT_ALIGNMENT_TOP_RIGHT,
	TEXT_ALIGNMENT_BOTTOM_LEFT,
	TEXT_ALIGNMENT_BOTTOM_CENTER,
	TEXT_ALIGNMENT_BOTTOM_RIGHT,
	TEXT_ALIGNMENT_MIDDLE_LEFT,
	TEXT_ALIGNMENT_MIDDLE_CENTER,
	TEXT_ALIGNMENT_MIDDLE_RIGHT,
} text_alignment_t;

typedef struct
{
	set_pixel_t set_pixel;
	void* ctx;
	uint8_t width;
	uint8_t height;
	uint8_t pos_x;
	uint8_t pos_y;
	uint8_t scale;
	text_alignment_t alignment;
	const char* text;
} text_renderer_t;

void Text_Renderer_Render(const text_renderer_t* renderer);

#endif // TAP_TEMPO_TEXT_RENDERER_H
