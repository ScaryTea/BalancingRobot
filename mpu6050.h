#include <stdint.h>

typedef enum {
	MPU_EOK = 0,
	MPU_ENORESP,
	MPU_EINVAL,
	MPU_ERX,
	MPU_ETX
} mpu6050_err;

enum mpu_options {
	GYRO = 1,
	ACCEL
};

struct mpu6050_data {
	int16_t accel_x, accel_y, accel_z;
	int16_t temp;
	int16_t gyro_x, gyro_y, gyro_z;
};

mpu6050_err mpu6050_init();

mpu6050_err mpu6050_getdata(struct mpu6050_data *data);

mpu6050_err mpu6050_setsensitivity(uint8_t sens, enum mpu_options opt);

mpu6050_err mpu6050_setDLPF(uint8_t lvl);

void mpu6050_process(struct mpu6050_data *data);
