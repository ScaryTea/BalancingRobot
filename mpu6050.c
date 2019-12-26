#include "mpu6050.h"
#include "i2c.h"

#define MPU_ADDR 0b1101000

static float offsets_G[] = {-1.18, 2.21, -3.81};	/* {kX, kY, kZ} */

static uint8_t _sens = 0;
static float lsb_sens_G[] = {131, 65.5, 32.8, 16.4};
static uint16_t lsb_sens_A[] = {16384, 8192, 4096, 2048};

mpu6050_err mpu6050_init()
{
	I2C_init();
	I2C_write_byte(MPU_ADDR, 0x6B, 0);	/* Power on */

	/* Read Who_Am_I register, which is always 0x68 */
	uint16_t whoami = I2C_read_byte(MPU_ADDR, 0x75);
	if (whoami != 0x68)
		return MPU_ENORESP;
	return MPU_EOK;
}

mpu6050_err mpu6050_getdata(struct mpu6050_data *data)
{
	uint16_t *res = (uint16_t *)data;
	I2C_read_bytes(MPU_ADDR, 0x3B, res, 7, true);

	return MPU_EOK;
}
mpu6050_err mpu6050_setsensitivity(uint8_t sens, enum mpu_options opt)
{
	if (sens > 3)
		return MPU_EINVAL;
	if (opt & GYRO)
		I2C_write_byte(MPU_ADDR, 0x1B, sens << 3);
	if (opt & ACCEL)
		I2C_write_byte(MPU_ADDR, 0x1C, sens << 3);
	_sens = sens;

	return MPU_EOK;
}

mpu6050_err mpu6050_setDLPF(uint8_t lvl)
{
	if (lvl > 6)
		return MPU_EINVAL;
	I2C_write_byte(MPU_ADDR, 0x1A, lvl);

	return MPU_EOK;
}

void mpu6050_process(struct mpu6050_data *data)
{
	data->gyro_x = data->gyro_x / lsb_sens_G[_sens] - offsets_G[0];
	data->gyro_y = data->gyro_y / lsb_sens_G[_sens] - offsets_G[1];
	data->gyro_z = data->gyro_z / lsb_sens_G[_sens] - offsets_G[2];

	/* res = from + ((val - a) * (to - from)) / (b - a); */
	data->accel_x = -90 + (data->accel_x + lsb_sens_A[_sens]) * 180 / 2 / lsb_sens_A[_sens];
	data->accel_y = -90 + (data->accel_y + lsb_sens_A[_sens]) * 180 / 2 / lsb_sens_A[_sens];
	data->accel_z = -90 + (data->accel_z + lsb_sens_A[_sens]) * 180 / 2 / lsb_sens_A[_sens];
}



