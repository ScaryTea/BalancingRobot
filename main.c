#include "mpu6050.h"
#include "i2c.h"
#include "motor.h"
#include "ir.h"
#include "delay.h"

#include "stm32f1xx.h"

#include <stdint.h>
#include <stdbool.h>

enum motion_opt { NOMOTION, FRW, BCKW, LEFT, RIGHT };

float get_angle_ac(int16_t data)
{
	static float res = 0;
	float alpha = 0.05;
	res = (1 - alpha) * res + alpha * data;

	return res;
}

float get_PID(struct mpu6050_data *data, enum motion_opt opt)
{
	/* Proportional, integral and differential components */
	static float angle_inst = 0, angle_sum = 0, angle_speed = 0;

	/* Complement filter coeficient */
	static float K_compl = 0.005;

	static uint16_t dt = 0;
	static uint32_t time_ms_prev = 0;
	uint32_t time_ms = millis();
	dt = time_ms - time_ms_prev;
	time_ms_prev = time_ms;	

	/* For preventing continuos one direction moving */
	static uint32_t cont_mov_cnt = 0;
	static uint32_t ms_cnt = 0;

	ms_cnt += dt;

	/* Output amplifying coeficient */
	float k_amp = 0.95;

	/* Keeps altered values for some time */
	static uint16_t cnt_keep = 0;

	if (cnt_keep > 0) {
		cnt_keep--;
	} else {
		/* Return back default values */
		k_amp = 0.95;
		K_compl = 0.005;
	}

	/* For stabilizing the robot */
	static float bal_zero = 0.0;
	static float res_sum = 0.0;

	/* Smoothing the balancing procedure */
	float k_bal = 0.1;
	bal_zero = (1 - k_bal) * bal_zero + k_bal * res_sum * 0.001;

	/* Get angle from accel measurements */
	/* bal_zero provides necessary pitch for stability */
	float ac_angle = get_angle_ac(data->accel_y) + bal_zero;

	angle_speed = data->gyro_x;
	static float dv = 0.0;
	/* Kinda magic, speed minus acceleration, wow */
	dv = (angle_speed - dv) / (dt + 0.001);

	/* Complementary filter */
	angle_inst = (1 - K_compl) * (angle_inst +
		angle_speed * dt / 1000.0) + K_compl * ac_angle;

	if (opt == FRW)
		angle_inst += 4;
	else if (opt == BCKW)
		angle_inst -= 4;

	angle_sum += angle_inst;
	if (ac_angle == 0)
		angle_sum = 0;

	/* PID coefficients */
	float Kp = 3.0, Ki = 0.05, Kd = 0.08;

	/* If moving one direction more than 250 ms, make a pwm splash; */
	/* k_amp and K are keeping these values during next ~500 ms */
	/* to improve further stability after splash */
	if ((ms_cnt - cont_mov_cnt) > 250) {
		angle_sum *= 3;
		Ki = 0.1;
		k_amp = 1.6 * (dv + 0.1) * 0.5 / 1.5;
		K_compl = 0.0005;
		cnt_keep = 500 * 3.0 / (dv + 0.01);
		cont_mov_cnt = ms_cnt;
	}

	/* PID regulator */
	float res = k_amp * (Kp * angle_inst + Ki * angle_sum +
		Kd * angle_speed);

	/* Counts how long the movement is one way */
	static bool dir = false;
	if ((dir && res > 0) || (!dir && res < 0)) {
		cont_mov_cnt++;
	} else {
		cont_mov_cnt = ms_cnt;
		dir = !dir;
	}

	res_sum += res;

	return res;
}

int32_t map(int32_t val, int32_t a, int32_t b, int32_t from, int32_t to)
{
	return from + ((val - a) * (to - from)) / (b - a);
}

int main()
{
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPAEN);
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPCEN);

	MODIFY_REG(GPIOA->CRL, GPIO_CRL_MODE7 | GPIO_CRL_CNF7, GPIO_CRL_MODE7_1);
	MODIFY_REG(GPIOC->CRH, GPIO_CRH_MODE13 | GPIO_CRH_CNF13, GPIO_CRH_MODE13_1);

	Systick_Init();
	delay_ms(2000);

	uint32_t command = 0;
	ir_init(&command);

	motor_init();
	motor_config(MOT1 | MOT2, FORW, 0);

	mpu6050_init();
	mpu6050_setDLPF(2);
	mpu6050_setsensitivity(3, GYRO | ACCEL);

	struct mpu6050_data mpu_data;

	while (1) {
		mpu6050_getdata(&mpu_data);
		mpu6050_process(&mpu_data);
		enum motion_opt opt;
		switch (command) {
			case BTN_2: opt = FRW; break;
			case BTN_4: opt = LEFT; break;
			case BTN_6: opt = RIGHT; break;
			case BTN_8: opt = BCKW; break;
			default: opt = NOMOTION;
		}
		command = 0;
		int16_t pid = get_PID(&mpu_data, opt);
		int32_t pwm = map(pid > 0 ? pid : pid * -1, 0, 90, 3, 120);
		pwm = pwm > 100 ? 100 : pwm;

		if (opt == LEFT) {
			motor_config(MOT1, FORW, 40);
			motor_config(MOT2, BACKW, 40);
		} else if (opt == RIGHT) {
			motor_config(MOT1, BACKW, 40);
			motor_config(MOT2, FORW, 40);
		} else {
			motor_config(MOT1 | MOT2, pid > 0 ? BACKW : FORW, pwm);
		}
		delay_ms(1);
	}
}





