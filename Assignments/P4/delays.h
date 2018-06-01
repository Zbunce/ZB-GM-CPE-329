/*
 * delays.h
 * Header file for both the ms and us delay functions.
 *
 * Created on: Apr 9, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef DELAYS_H_
#define DELAYS_H_

#define F_1p5_MeHz  15  //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30  //MeHz labels are used to indicate this
#define F_6_MeHz    60  //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

void delay_ms(int, int);
void delay_us(int, int);

#endif /* DELAYS_H_ */
