/*
 * uart.c
 *
 *  Created on: 24.01.2012
 *      Author: ёрий
 */

#include <avr/io.h>
#include "uart.h"

char UARTReadChar()
{
   while(!(UCSR0A & (1<<RXC0)));
   return UDR0;
}

void UARTWriteChar(char data)
{
   while(!(UCSR0A & (1<<UDRE0)));
   UDR0=data;
}

void UARTWriteString(char *str)
{
	while(*str)
		UARTWriteChar(*str++);
}

void UARTInit(void)
{
	UCSR0A=0x00;
	UCSR0B=0x18;
	UCSR0C=0x06;
	UBRR0H=0x00;
	UBRR0L=0x08;
}
