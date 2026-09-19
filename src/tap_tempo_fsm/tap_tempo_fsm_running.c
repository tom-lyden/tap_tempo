//
// Created by tomly on 16/09/2026.
//

#include <tap_tempo.h>
#include <tap_tempo_fsm.h>
#include <tap_tempo_fsm_running.h>
#include <timer.h>

static void start_led_phase(tap_tempo_t* tap_tempo);

void Running_Enter(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	
	tap_tempo->indicator_state = TAP_TEMPO_INDICATOR_STATE_HIGH;
	start_led_phase(tap_tempo);
}

void Running_Update(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	
	if (Timer_GetState(&tap_tempo->led_timer) != TIMER_EXPIRED)
		return;
		
	tap_tempo->indicator_state = tap_tempo->indicator_state == TAP_TEMPO_INDICATOR_STATE_HIGH 
		? TAP_TEMPO_INDICATOR_STATE_LOW 
		: TAP_TEMPO_INDICATOR_STATE_HIGH;
	
	start_led_phase(tap_tempo);
}

void Running_Exit(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	
	tap_tempo->set_indicator(TAP_TEMPO_INDICATOR_STATE_LOW);
	
	Timer_Stop(&tap_tempo->led_timer);
}

static void start_led_phase(tap_tempo_t* tap_tempo)
{
	tap_tempo->set_indicator(tap_tempo->indicator_state);
	
	uint32_t duration_ticks = tap_tempo->indicator_state == TAP_TEMPO_INDICATOR_STATE_LOW 
		? tap_tempo->low_duration_ticks 
		: tap_tempo->high_duration_ticks;
	
	Timer_Start(&tap_tempo->led_timer, duration_ticks);
}

void Running_ButtonPress(void* ctx)
{
	
}

void Running_ButtonRelease(void* ctx)
{
	
}
