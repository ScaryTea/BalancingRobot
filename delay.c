#include "delay.h"
#include "stm32f1xx.h"

#define TICK F_CPU / 1000000

volatile uint32_t _ms = 0;
volatile uint32_t _us = 0;

uint8_t Systick_Init(void)
{
	/* Interrupt every 1 ms */
	if (SysTick_Config(F_CPU / 1000))
		return -1;

	/* Highest priority */
	NVIC_SetPriority(SysTick_IRQn, 0);
	
	return 0;
}

void SysTick_Handler(void)
{
	_ms++;
}

uint32_t micros(void)
{
	_us = _ms * 1000 + 1000 - SysTick->VAL / TICK;
	return _us;
}

uint32_t millis(void)
{
	return _ms;
}

void delay_ms(uint32_t ms_delay)
{
	uint32_t ms_cur = _ms;
	while (ms_cur + ms_delay - _ms > 0);
}

