#include "stm32f1xx.h"

#ifndef F_CPU
#warning "F_CPU is not defined, using default value 8 MHz"
#define F_CPU 8000000
#endif

#define DELTA 		5
#define START_PULSE	45
#define END_PULSE	395
#define RPT_CMD_PULSE	954
#define ZERO_PULSE 	6
#define ONE_PULSE	17
#define COMMA_PULSE	23

typedef enum {
	BTN_CH_DOWN 	= 0x00FFA25D,
	BTN_CH		= 0x00FF629D,
	BTN_CH_UP 	= 0x00FFE21D,
	BTN_PREV 	= 0x00FF22DD,
	BTN_NEXT 	= 0x00FF02FD,
	BTN_PLAY_PAUSE 	= 0x00FFC23D,
	BTN_VOL_DOWN 	= 0x00FFE01F,
	BTN_VOL_UP 	= 0x00FFA857,
	BTN_EQ		= 0x00FF906F,
	BTN_0 		= 0x00FF6897,
	BTN_100 	= 0x00FF9867,
	BTN_200 	= 0x00FFB04F,
	BTN_1 		= 0x00FF30CF,
	BTN_2 		= 0x00FF18E7,
	BTN_3 		= 0x00FF7A85,
	BTN_4 		= 0x00FF10EF,
	BTN_5 		= 0x00FF38C7,
	BTN_6 		= 0x00FF5AA5,
	BTN_7 		= 0x00FF42BD,
	BTN_8 		= 0x00FF4AB5,
	BTN_9 		= 0x00FF52AD
} IR_CMD_t;

void ir_init(uint32_t *command_var);

