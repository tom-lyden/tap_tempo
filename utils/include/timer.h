//
// Created by tomly on 07/09/2026.
//

#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef enum
{
	TIMER_STOPPED = 0,
	TIMER_RUNNING = 1,
	TIMER_EXPIRED = 2,
} timer_state_t;

typedef uint32_t get_ticks_t(void);

typedef struct timer
{
	get_ticks_t* get_ticks;
	uint32_t start_time;
	uint32_t duration;
	bool is_started;
} timer_t;

void Timer_Init(timer_t* timer, get_ticks_t* get_ticks);
void Timer_Stop(timer_t* timer);
void Timer_Start(timer_t* timer, uint32_t duration);
timer_state_t Timer_GetState(const timer_t* timer);

#endif // TIMER_H
