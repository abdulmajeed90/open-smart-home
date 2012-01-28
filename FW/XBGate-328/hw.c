/*
 * hw.c
 *
 *  Created on: 24.01.2012
 *      Author: ����
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
