/**
 * DAC.h
 *
 * Header for running the DAC
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#ifndef DAC_H_
#define DAC_H_

#define square      1       //Wave type definitions
#define triangle    2
#define sine        3
#define sawtooth    4
#define Vref        33      //Reference voltage * 10
#define SDOFF       BIT4    //Shutdown mode OFF
#define GAIN1       BIT5    //Unity gain mode
#define HIGH        0xFFF   //Definitions for square wave states
#define LOW         0x000

#define F_100Hz     100
#define F_200Hz     200
#define F_300Hz     300
#define F_400Hz     400
#define F_500Hz     500

void write_DAC(uint16_t);
void makeDC(int);
void makeWave(int, int, int, int, int);

#endif /* DAC_H_ */
