#ifndef __CH2O_h__
#define __CH2O_h__
#include "stm32l4xx_hal.h"
#include "string.h"
#include "stdio.h"

float ZE08ch2o(uint8_t Buffer[]);
unsigned char FucCheckSum(unsigned char *i,unsigned char ln);
#endif
