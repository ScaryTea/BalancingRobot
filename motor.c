#include "motor.h"

void motor_init()
{	
	/* Enable PortA and Alternate function tacting */
	SET_BIT(RCC->APB2ENR, 	RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB2ENR,	RCC_APB2ENR_AFIOEN);

	/* Alternate function push-pull for A0 & A1 */
	MODIFY_REG(GPIOA->CRL,
		GPIO_CRL_CNF0 	| GPIO_CRL_MODE0
		| GPIO_CRL_CNF1 | GPIO_CRL_MODE1,
		GPIO_CRL_CNF0_1 | GPIO_CRL_MODE0_1 
		| GPIO_CRL_CNF1_1 | GPIO_CRL_MODE1_1);

	/* Push-pull for A2-A5 */
	MODIFY_REG(GPIOA->CRL, 
		GPIO_CRL_MODE2 | GPIO_CRL_MODE3 
		| GPIO_CRL_MODE4 | GPIO_CRL_MODE5
		| GPIO_CRL_CNF2 | GPIO_CRL_CNF3 
		| GPIO_CRL_CNF4 |GPIO_CRL_CNF5, 
		GPIO_CRL_MODE2_1 | GPIO_CRL_MODE3_1
		| GPIO_CRL_MODE4_1 | GPIO_CRL_MODE5_1);

	/* Enable Timer2 tacting */
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	/* Enable preload & auto-reload preload reg-s */
	SET_BIT(TIM2->CCMR1, 	TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE);
	SET_BIT(TIM2->CR1,	TIM_CR1_ARPE);
	
	/* Channels 1 & 2 as outputs */
	CLEAR_BIT(TIM2->CCMR1, TIM_CCMR1_CC1S | TIM_CCMR1_CC2S);

	/* PWM mode 2: HIGH while CNT < CCR, else LOW */
	SET_BIT(TIM2->CCMR1, 6 << TIM_CCMR1_OC1M_Pos | 6 << TIM_CCMR1_OC2M_Pos);

	/* Enable pwm output to pins */
	SET_BIT(TIM2->CCER, TIM_CCER_CC1E | TIM_CCER_CC2E);

	/* Prescaler & auto reload register value */
	TIM2->PSC = 799;
	TIM2->ARR = 100;

	/* Generate update event and enable counter */
	TIM2->EGR |= TIM_EGR_UG;
	TIM2->CR1 |= TIM_CR1_CEN;
}

void motor_config(enum motor_sel sel, enum motor_dir dir, int8_t duty)
{
	if (sel & MOT1) {
		TIM2->CCR1 = duty;
		GPIOA->ODR &= ~(GPIO_ODR_ODR2 | GPIO_ODR_ODR3);
		GPIOA->ODR |= dir << GPIO_ODR_ODR2_Pos;
	}
	if (sel & MOT2) {
		TIM2->CCR2 = duty;
		GPIOA->ODR &= ~(GPIO_ODR_ODR4 | GPIO_ODR_ODR5);
		GPIOA->ODR |= dir << GPIO_ODR_ODR4_Pos;
	}

	TIM2->EGR |= TIM_EGR_UG;
}

