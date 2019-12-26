#include "stm32f1xx.h"
#include <stdbool.h>

void I2C_init();

void I2C_write_byte(uint8_t dev_addr, uint8_t reg_address, uint8_t data);

void I2C_write_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len);

uint8_t I2C_read_byte(uint8_t dev_addr, uint8_t reg_address);

void I2C_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint16_t *data, 
		    uint8_t len, bool is_complement);
