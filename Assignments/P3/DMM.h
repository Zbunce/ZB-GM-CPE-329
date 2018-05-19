/*
 * DMM.h
 *
 * Header file for managing the digital multimeter protocols
 *
 * Date: May 18, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef DMM_H_
#define DMM_H_

#define TIMER_DEL 240
#define FREQ_MULT 100000

void setDCV_DMM(int);
void setPktoPk_DMM(int);
void setTRMS_DMM(int);
void setDCOff_DMM(int);
void setFreq_DMM(int);

int  getDCV_DMM();
int  getACFlag_DMM();

void DISP_INIT();
void TIMER_INIT();
void measACV_DMM();
void measDCV_DMM();
void measTRMS_DMM();
void measPktoPk_DMM();
void measFreq_DMM();
void updateDisp_DMM();

#endif /* DMM_H_ */
