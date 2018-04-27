/**
 * DAC.h
 *
 * DAC management
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#ifndef DAC_H_
#define DAC_H_

#define square      1
#define triangle    2
#define sine        3
#define Vref        33
#define SDOFF BIT4
#define GAIN1 BIT5

void write_DAC(uint16_t);
void makeDC(int);
void makeWave(int, int, int, int, int);

#endif /* DAC_H_ */
