#include "i2c.h"
#include <stdint.h>

void I2C_init();
void I2C_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data);
uint8_t I2C_read_byte(uint8_t dev_addr, uint8_t reg_addr);
static void start();
static void send_addr(uint8_t addr, uint8_t rw);
static void send_data(uint8_t data);
static void stop();
static void ack_config(uint8_t on_off);

enum mode {WR = 0, RD};

void I2C_write_byte(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
	start();
	send_addr(dev_addr, WR);
	send_data(reg_addr);
	send_data(data);
	stop();
}

void I2C_write_bytes(uint8_t dev_addr, uint8_t reg_addr, uint8_t *data, uint8_t len)
{
	start();
	send_addr(dev_addr, WR);
	send_data(reg_addr);
	while (len--)
		send_data(*data++);
	stop();
}

uint8_t I2C_read_byte(uint8_t dev_addr, uint8_t reg_addr)
{
	start();
	send_addr(dev_addr, WR);
	send_data(reg_addr);
	start();
	send_addr(dev_addr, RD);
	ack_config(1);
	while(!(I2C1->SR1 & I2C_SR1_RXNE));
	uint8_t res = I2C1->DR;
	ack_config(0);
	stop();
	return res;
}

void I2C_read_bytes(uint8_t dev_addr, uint8_t reg_addr, uint16_t *data, 
		    uint8_t len, bool is_complement)
{
	start();
	send_addr(dev_addr, WR);
	send_data(reg_addr);
	start();
	send_addr(dev_addr, RD);
	ack_config(1);

	bool flag = true;
	len = is_complement ? len * 2 : len;

	while (len--) {
		while(!(I2C1->SR1 & I2C_SR1_RXNE));
		if (is_complement && flag) {
			*data = I2C1->DR << 8;
			flag = false;
			continue;
		}
		if (is_complement) {
			*data++ |= I2C1->DR;
			flag = true;
			continue;
		}
		*data++ = I2C1->DR;
	}

	ack_config(0);
	stop();
}

static void start()
{
	SET_BIT(I2C1->CR1, I2C_CR1_START);
	while (!(I2C1->SR1 & I2C_SR1_SB));	/* Start condition generated */
}

static void send_addr(uint8_t addr, enum mode rw)
{ 
	I2C1->DR = (addr << 1) | rw;
	while(!(I2C1->SR1 & I2C_SR1_ADDR)); 	/* Address matched */
	if(!(I2C1->SR2 & I2C_SR2_TRA));		/* Read SR2 to clear ADDR */
}

static void send_data(uint8_t data)
{	
	I2C1->DR = data;
	while (!(I2C1->SR1 & I2C_SR1_TXE));	/* Data register empty */
}

static void stop()
{
	SET_BIT(I2C1->CR1, I2C_CR1_STOP);
	while(I2C1->SR1 & I2C_SR2_MSL);		/* While (STM is Master) */
}

static void ack_config(uint8_t on_off)
{
	if (on_off)
		SET_BIT(I2C1->CR1, I2C_CR1_ACK);
	else
		CLEAR_BIT(I2C1->CR1, I2C_CR1_ACK);
}

void I2C_init()
{
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_IOPBEN);
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_I2C1EN);

	/* Port configuraion: alternate function open-drain */
	MODIFY_REG(GPIOB->CRL,
		   GPIO_CRL_MODE6 | GPIO_CRL_MODE7,
		   GPIO_CRL_CNF6 | GPIO_CRL_CNF7 | GPIO_CRL_MODE6_1 | GPIO_CRL_MODE7_1);
	
	/* I2C configuration */
	CLEAR_BIT(I2C1->CCR, I2C_CCR_DUTY);
	SET_BIT(I2C1->OAR1, I2C_OAR1_ADD1_7);		/* Interface addr */
	SET_BIT(I2C1->CR2, 0x08 << I2C_CR2_FREQ_Pos); 	/* 8 MHz peripheral clock */
	SET_BIT(I2C1->TRISE, 0x09); 			/* Rise time = 1000 ns */
	SET_BIT(I2C1->CCR, 0x28); 			/* f = 100 kHz */
	SET_BIT(I2C1->CR1, I2C_CR1_ACK);		/* Ack generating on */
	SET_BIT(I2C1->CR1, I2C_CR1_PE); 		/* Peripheral enable */
}
