/*
 * hw.c
 *
 *  Created on: 24.01.2012
 *      Author: ёрий
 */
#include <avr/interrupt.h >
#include "twi-i2c.h"
#include "uart.h"

void HW_Init(void)
{
	UARTInit();
	I2C_Init();
	sei();
}
