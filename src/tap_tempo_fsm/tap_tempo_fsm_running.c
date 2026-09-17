//
// Created by tomly on 16/09/2026.
//

#include <tap_tempo.h>
#include <tap_tempo_fsm.h>
#include <tap_tempo_fsm_running.h>
#include <f401_re_hal.h>
#include <timer.h>

static void start_led_phase(tap_tempo_t* tap_tempo);

void Running_Enter(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	
	tap_tempo->led_state = GPIO_STATE_HIGH;
	start_led_phase(tap_tempo);
}

void Running_Update(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	
	if (TapTempo_IsButtonPressed(tap_tempo))
	{
		FSM_SetState(&tap_tempo->fsm, READING_INPUT, ctx);	
		return;
	}
	
	if (Timer_GetState(&tap_tempo->led_timer) != TIMER_EXPIRED)
		return;
		
	tap_tempo->led_state = tap_tempo->led_state == GPIO_STATE_HIGH ? GPIO_STATE_LOW : GPIO_STATE_HIGH;
	start_led_phase(tap_tempo);
}

void Running_Exit(void* ctx)
{
	tap_tempo_t* tap_tempo = (tap_tempo_t*)ctx;
	const tap_tempo_cfg_t* cfg = tap_tempo->cfg;
	
	GPIO_Write(cfg->led_port, cfg->led_pin, GPIO_STATE_LOW);
	
	Timer_Stop(&tap_tempo->led_timer);
}

static void start_led_phase(tap_tempo_t* tap_tempo)
{
	const tap_tempo_cfg_t* cfg = tap_tempo->cfg;
	
	GPIO_Write(cfg->led_port, cfg->led_pin, tap_tempo->led_state);
	
	uint32_t duration_ticks = tap_tempo->led_state == GPIO_STATE_LOW 
		? tap_tempo->low_duration_ticks 
		: tap_tempo->high_duration_ticks;
	
	Timer_Start(&tap_tempo->led_timer, duration_ticks);
}
