/*
 *  set_DCO.h
 *  Header file for the DCO frequency change function.
 *
 *  Date: April 6, 2018
 *  Author: Zach Bunce, Garret Maxon
 */
#ifndef SET_DCO_H_
#define SET_DCO_H_

#define F_1p5_MeHz  15  //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30  //MeHz labels are used to indicate this
#define F_6_MeHz    60  //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

void set_DCO(int);

#endif /* SET_DCO_H_ */
