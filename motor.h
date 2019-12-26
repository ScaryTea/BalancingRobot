#include "stm32f1xx.h"

enum motor_sel { MOT1 = 1, MOT2 };
enum motor_dir { FORW = 1, BACKW };

void motor_init();

void motor_config(enum motor_sel sel, enum motor_dir dir, int8_t duty);
