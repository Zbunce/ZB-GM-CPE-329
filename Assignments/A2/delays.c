/*
 *  delays.c
 *  Code file for both the ms and us delay functions.
 *
 *  Date: April 4, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */

#define F_1p5_MeHz  15      //Defines various frequency values in almost MHz (10^5)
#define F_3_MeHz    30      //MeHz labels are used to indicate this
#define F_6_MeHz    60      //Blame data type truncation
#define F_12_MeHz   120
#define F_24_MeHz   240
#define F_48_MeHz   480

//Takes in desired time delay in ms and clock frequency in MeHz
void delay_ms(int time_ms, int freq_MeHz)
{
    int i;
    unsigned int j;
    //Unit Conversion: MeHz * ms = 10^5 * 10^-3 = 10^2 = 100
    int time_fix = time_ms / 10 + 1;
    unsigned int freq_cnv = freq_MeHz * 100;         //Multiplies in unit conversion to stable variable
    for (i = time_fix; i > 0; i--) {         //Wait the amount of ms specified
        for (j = freq_cnv; j > 0; j--);//    //Wait the amount of MeHz of the system clk for each ms
    }
}

//Takes in desired time delay in us and clock frequency in MeHz
void delay_us(int time_us, int freq_MeHz)
{
    int i;
    int time_fix;
    //Unit Conversion: MeHz * us = 10^5 * 10^-6 = 10^-1 = 0.1; Accounted for in decrement
    switch (freq_MeHz)
    {
    case F_1p5_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 13;  //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    case F_3_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 16;  //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    case F_6_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 24;  //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    case F_12_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 39;  //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    case F_24_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 72;  //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    case F_48_MeHz:
        time_fix = time_us * freq_MeHz / 100 - 402; //Fixes the count
        for (i = time_fix; i > 0; i--);             //Wait the amount of us specified
        break;
    }
}
