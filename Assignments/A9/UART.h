/**
 * UART.h
 *
 * Header for running the UART
 *
 * Date: May 11 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#ifndef UART_H_
#define UART_H_

int  getIntFlag_UART();
void clrIntFlag_UART();
char getRXByte();
void sendByte_UART();
void UART_INIT();

#endif /* UART_H_ */
