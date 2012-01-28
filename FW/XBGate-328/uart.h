/*
 * uart.h
 *
 *  Created on: 24.01.2012
 *      Author: ёрий
 */

#ifndef UART_H_
#define UART_H_

	char UARTReadChar(void);
	void UARTWriteChar(char data);
	void UARTWriteString(char *str);
	void UARTInit(void);

#endif /* UART_H_ */
