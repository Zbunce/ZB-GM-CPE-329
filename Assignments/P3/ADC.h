/**
 * ADC.h
 *
 * Header for running the ADC
 *
 * Date: May 11 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#ifndef ADC_H_
#define ADC_H_

#define RES_14_MAX  16384
#define VRef        33
#define MASK_LOW_32 0x0000000F
#define DEC_PNT     0x2E
#define SPACE       0x20

void ADC_INIT();
int getIntFlag_ADC();
void clrIntFlag_ADC();
int getAnData_ADC();
int calcVolt_ADC();
void sendVolt_ADC(int);

#endif /* ADC_H_ */
