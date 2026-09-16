//
// Created by tomly on 07/09/2026.
//

#include <timer.h>

void Timer_Init(timer_t* timer, get_ticks_t* get_ticks)
{
	timer->get_ticks = get_ticks;

	Timer_Stop(timer);
}

void Timer_Start(timer_t* timer, uint32_t duration)
{
	timer->start_time = timer->get_ticks();
	timer->duration = duration;
	timer->is_started = true;
}

void Timer_Stop(timer_t* timer)
{
	timer->start_time = 0;
	timer->duration = 0;
	timer->is_started = false;
}

timer_state_t Timer_GetState(const timer_t* timer)
{
	if (!timer->is_started)
		return TIMER_STOPPED;

	return timer->duration > timer->get_ticks() - timer->start_time ? TIMER_RUNNING : TIMER_EXPIRED;
}
