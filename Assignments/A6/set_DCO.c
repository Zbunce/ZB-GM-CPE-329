/*
 *  set_DCO.c
 *  Code file for the DCO frequency change function.
 *
 *  Date: April 6, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */

#include "msp.h"
#define F_1p5_MeHz  15      //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30      //MeHz labels are used to indicate this
#define F_6_MeHz    60      //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

void set_DCO(int freq)
{
    CS->KEY = CS_KEY_VAL;                                   //Unlocks CS registers
    CS->CTL0 = 0;                                           //Clears CTL0 register

    switch (freq)
    {
    case F_1p5_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_0;                       			//Sets DC0 to 1.5 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; 	//Sets the clock refs
        break;
    case F_3_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_1;                       //Sets DC0 to 3 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; //Sets the clock references
        break;
    case F_6_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_2;                       //Sets DC0 to 6 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; //Sets the clock references
        break;
    case F_12_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_3;                       //Sets DC0 to 12 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; //Sets the clock references
        break;
    case F_24_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_4;                       //Sets DC0 to 24 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; //Sets the clock references
        break;
    case F_48_MeHz:
        // Transition to VCORE Level 1: AM0_LDO --> AM1_LDO
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY))
            ;
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
        while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));

        // Configure Flash wait-state to 1 for both banks 0 & 1
        FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL
                & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
        FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL
                & ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;

        CS->CTL0 = CS_CTL0_DCORSEL_5;                       //Sets DC0 to 48 MHz
        CS->CTL1 = CS->CTL1
                & ~(CS_CTL1_SELM_MASK | CS_CTL1_DIVM_MASK)| CS_CTL1_SELM_3; //Sets MCLK to DCO
        break;
    default:
        break;
    }
    CS->KEY = 0;                                        //Locks CS registers
}
