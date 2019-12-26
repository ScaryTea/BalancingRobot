#include "ir.h"
#include <stdbool.h>

#define BETWEEN(val, anc, delta) \
	((val) > (anc) - (delta) && (val) < (anc) + (delta))

uint16_t capture1 = 0, capture2 = 0;
bool is_first = true;

static int32_t index = -1;
static bool START_OCCURED = false;
static uint32_t tmp, prevcom;
static uint32_t *command;

static void process_pulse(uint16_t pulse)
{
	if (BETWEEN(pulse, START_PULSE, DELTA)) {
		index = 0;
		START_OCCURED = true;
	} else if (BETWEEN(pulse, ONE_PULSE, DELTA)) {
		tmp |= 1 << (31 - index);
		index++;
	} else if (BETWEEN(pulse, ZERO_PULSE, DELTA)) {
		tmp &= ~(1 << (31 - index));
		index++;
	} else if (BETWEEN(pulse, END_PULSE, DELTA)) {
		if ((START_OCCURED) && (index == 32)) {
			*command = tmp;
			prevcom = tmp;
		} else {
			*command = 0;
		}
		START_OCCURED = false;
		index = -1;
	} else if (BETWEEN(pulse, RPT_CMD_PULSE, DELTA)) {
		*command = prevcom;
	}
}

void TIM3_IRQHandler(void)
{
	TIM3->CCER ^= TIM_CCER_CC1P;

	capture1 = capture2;
	capture2 = TIM3->CCR1;

	if (is_first) {
		is_first = false;
		return;
	}
	is_first = true;
	
	uint16_t pulse_len = capture2 - capture1;
	process_pulse(pulse_len);
} 

void ir_init(uint32_t *command_var)
{
	command = command_var;

	/* Enable PortA and Alternate function tacting */
	SET_BIT(RCC->APB2ENR, 	RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB2ENR,	RCC_APB2ENR_AFIOEN);

	/* Input floating A6 */
	MODIFY_REG(GPIOA->CRL,
		GPIO_CRL_CNF6 | GPIO_CRL_MODE6,
		GPIO_CRL_CNF6_0);

	/* Enable Timer3 tacting */
	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

	/* Input mode channel 1 */
	TIM3->CCMR1 |= 0x01 << TIM_CCMR1_CC1S_Pos;

	/* Capture can be loaded into capture register */
	TIM3->CCER |= TIM_CCER_CC1E;

	/* 10 kHz interruprs */
	TIM3->PSC = F_CPU / 10000 - 1;

	/* Enable interrupt generation */
	TIM3->DIER |= TIM_DIER_CC1IE;

	/* Capture on rising edge, this is changed every */
	/* interrupt to measure the LOW length, not the whole period */
	TIM3->CCER &= ~TIM_CCER_CC1P;

	/* Generate update event and enable counter */
	TIM3->EGR |= TIM_EGR_UG;
	TIM3->CR1 |= TIM_CR1_CEN;
	
	NVIC_EnableIRQ(TIM3_IRQn);
}

