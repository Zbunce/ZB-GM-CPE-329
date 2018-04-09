/*
 *  delays.h
 *  Header file for both the ms and us delay functions.
 *  delay_us only reliable above 55 us.
 *  Delays under 30 us produce 2.5us delay.
 *
 *  Date: April 4, 2018
 *  Author: Zach Bunce, Garret Maxon
 */
#ifndef DELAYS_H_
#define DELAYS_H_
void delay_ms(int, int);
void delay_us(int, int);
#endif /* DELAYS_H_ */
