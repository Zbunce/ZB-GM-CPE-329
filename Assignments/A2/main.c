/*
 *  A2 Main
 *  Implements various led pulses.
 *
 *  Date: April 4, 2018
 *  Authors: Zach Bunce, Garrett Maxon
 */
//Toggles BIT0
//Indefinitely counts to 2k & then flips BIT0

#include "msp.h"

#define F_1p5M_Hz   1500000 //Defines various frequency values in Hz
#define F_3M_Hz     3000000
#define F_6M_Hz     6000000
#define F_12M_Hz    12000000
#define F_24M_Hz    24000000
#define F_48M_Hz    48000000

#define F_1p5k_kHz  1500    //Defines various frequency values in kHz
#define F_3k_kHz    3000
#define F_6k_kHz    6000
#define F_12k_kHz   12000
#define F_24k_kHz   24000
#define F_48k_kHz   48000

#define F_1p5_MeHz  15      //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30      //MeHz labels are used to indicate this
#define F_6_MeHz    60      //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

#define F_1p5_MHz  1.5      //Defines various frequency values in MHz
#define F_3_MHz    3
#define F_6_MHz    6
#define F_12_MHz   12
#define F_24_MHz   24
#define F_48_MHz   48


void set_DCO(int);
void delay_ms(int, int);
void delay_us(int, int);

int main(void)
{
    int t = 100;
    int f = F_6_MeHz;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;    // Stop WDT

    // P1.0 set as GPIO
    P1->SEL0 &= ~BIT0;               // Clear bit 0 of the P1->SEL0 register
    P1->SEL1 &= ~BIT0;               // Clear bit 0 of the P1->SEL1 register

    P1->DIR |= BIT0;                 // P1.0 set as output
    set_DCO(f);

    while (1)                        // continuous loop
    {
        P1->OUT ^= BIT0;                // Toggle P1.0 LED
        delay_us(t, f);

    }
}

//Takes in desired time delay in ms and clock frequency in MeHz
void delay_ms(int time_ms, int freq_MeHz)
{
    int i;
    unsigned int j;
    //Unit Conversion: MeHz * ms = 10^5 * 10^-3 = 10^2 = 100
    int time_fix = time_ms / 10;
    unsigned int freq_cnv = freq_MeHz * 100;         //Multiplies in unit conversion to stable variable
    for (i = time_fix; i > 0; i--) {         //Wait the amount of ms specified
        for (j = freq_cnv; j > 0; j--);// {    //Wait the amount of MeHz of the system clk for each ms
            //z++; //Nom
        //}
    }
}

//Takes in desired time delay in us and clock frequency in MeHz
void delay_us(int time_us, int freq_MeHz)
{
    int i;
    unsigned int j;
    //Unit Conversion: MeHz * us = 10^5 * 10^-6 = 10^-1 = 0.1; Accounted for in decrement
    int time_fix = time_us / 11;
    for (i = time_fix; i > 0; i--)  {            //Wait the amount of us specified
        for (j = freq_MeHz; j > 0; j -= 10); //Wait the amount of MeHz of the system clk for each us
    }
}

void set_DCO(int freq)
{
    CS->KEY = CS_KEY_VAL;                                   //Unlocks CS registers
    CS->CTL0 = 0;                                           //Clears CTL0 register

    switch (freq)
    {
    case F_1p5_MeHz:
        CS->CTL0 = CS_CTL0_DCORSEL_0;                       //Sets DC0 to 1.5 MHz
        CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3; //Sets the clock references
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
