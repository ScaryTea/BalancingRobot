#include <stdint.h>

#ifndef F_CPU
#warning "F_CPU is not defined, using default value 8 MHz"
#define F_CPU 8000000
#endif

uint8_t Systick_Init(void);

void SysTick_Handler(void);

uint32_t micros(void);

uint32_t millis(void);

void delay_ms(uint32_t ms_delay);

