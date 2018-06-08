/**
 * main.c
 *
 * Runs velocity meter
 * +Y is considered forward
 *
 * +Z (Roof)
 * ^
 * |    +Y (Windshield)
 * |    ^
 * |   /
 * |  /
 * | /
 * |/
 * .--------->+X (Passenger Side)
 * (Dashboard)
 *
 * Date: May 22, 2018
 * Author: Zach Bunce, Garrett Maxon
 */



#include "msp.h"
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <math.h>
#include <string.h>
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "I2C.h"

//15 bit addresses so MSB sent doesn't matter
#define MAX_VEL_ADDR    0x1111
#define MAX_ACL_ADDR    0x2222
#define MAX_0t6_ADDR    0x3333

//Register addresses on MMA8452Q
#define SYSMOD_REG      0x0B
#define Y_MSB_REG       0x03
#define Y_LSB_REG       0x04
#define STATUS_REG      0x00
#define CTRL1_REG       0x2A
#define CTRL2_REG       0x2B
#define CTRL3_REG       0x2C
#define CTRL4_REG       0x2D
#define CTRL5_REG       0x2E

#define DR_400kHz       0x04
#define MODE_ACT        0x01

#define TIMER1_DEL       1200    //100us
#define TIMER2_DEL       1200    //100us
#define CNT_MAGN        10000

#define MASK_LOW_32 0x0000000F

//static int BCDtime;
static uint32_t msCount1;
static uint32_t msCount2;

static uint16_t curAccel_DN = 0;
static uint16_t accelSamp[10] = {0,0,0,0,0,0,0,0,0,0};
static uint32_t timeWatch[10] = {0,0,0,0,0,0,0,0,0,0};
static int accelIndex = 0;

static int curVel = 0;
static float curAccel = 0.0;
static float cur60Time = 0.0;

static int maxVel = 0;
static float maxAccel = 0.0;
static float max60Time = 0.0;

static int YDR_FLAG = 0;
static int accelStatus = 0;

void ACCEL_INIT();
void TIMER_INIT();
void writeDisp(int, int);
void calcYVel();
void measYAccel();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    int pageNum = 1;
//    int accelStatus = 0;

    set_DCO(CLK);
    LCD_INIT(CLK);
    I2C_INIT(ACCEL_ADDR);
    ACCEL_INIT();
    TIMER_INIT();

    P3->DIR &= ~BIT1;
    P3->OUT = BIT1;
    P3->REN = BIT1;                         // Enable pull-up resistor (P3.1 output high)

    P2->DIR &= ~BIT1;
    P2->OUT = BIT1;
    P2->REN = BIT1;

    // Enable pull-up resistor (P2.1 output high)
    //changes pages based on two buttons
    //the first button goes to the next page and wraps back to the first after the third page
    //the second button goes to the previous page and wraps back to page three after the first page

    //Update display values
    //Check against record
    //If record update EEPROM
    //Check button press
    //Print values to screen

    while(1) {
//        accelStatus = readByte_I2C(STATUS_REG) & BIT1;


        if(YDR_FLAG)
        {
            YDR_FLAG = 0;
            calcYVel();
        }

        if (curVel > maxVel)
        {
            maxVel = curVel;
        }

        if (curAccel > maxAccel)
        {
            maxAccel = curAccel;
        }

        if (cur60Time > max60Time)
        {
            max60Time = cur60Time;
        }
/*
        if (button1 == 0)
        {
            pageNum++;
            if (pageNum > 3)
            {
                pageNum = 1;
            }
        }
        else if (button2 == 0)
        {
            pageNum--;
            if (pageNum < 0)
                ;
            {
                pageNum = 3;
            }
        }
*/
        writeDisp(pageNum, CLK);
    }


    /*//LCD PRINT TEST
    while(1) {
        curVel = 591;
        maxVel = 974;
        curAccel = 8.723;
        maxAccel = 6.379;
        cur60Time = 78.24;
        max60Time = 45.71;
        writeDisp(1, CLK);
        delay_ms(1000, CLK);
        writeDisp(2, CLK);
        delay_ms(1000, CLK);
        writeDisp(3, CLK);
        delay_ms(1000, CLK);
    }*/
}

void writeDisp(int pageNum, int CLK)
{
    //three pages to display information on
    //Current Velocity & Acceleration (page 1) - Max Velocity & Acceleration(page 2) - 0-60 timer (page 3)

    char curVelStr[17];
    char curAccelStr[17];
    char maxVelStr[17];
    char maxAccelStr[17];
    char cur60TimeStr[17];
    char max60TimeStr[17];

    snprintf( curVelStr,    17, "CUR VEL: %d mph", curVel);
    snprintf( curAccelStr,  17, "CUR ACL: %.3f g", curAccel);
    snprintf( maxVelStr,    17, "MAX VEL: %d mph", maxVel);
    snprintf( maxAccelStr,  17, "MAX ACL: %.3f g", maxAccel);
    snprintf( cur60TimeStr, 17, "CUR 0-60: %.2fs", cur60Time);
    snprintf( max60TimeStr, 17, "MAX 0-60: %.2fs", max60Time);

    if (pageNum == 1)
    {
        write_string_LCD(curVelStr, 0x00, CLK);
        write_string_LCD(curAccelStr, 0x40, CLK);
    }
    else if (pageNum == 2)
    {
        write_string_LCD(maxVelStr, 0x00, CLK);
        write_string_LCD(maxAccelStr, 0x40, CLK);
    }
    else if (pageNum == 3)
    {
        write_string_LCD(cur60TimeStr, 0x00, CLK);
        write_string_LCD(max60TimeStr, 0x40, CLK);
    }

    /* if (pageNum == 1)
    {
        write_string_LCD("CUR VEL: ", 0x00, CLK);
        write_string_LCD(curVel, 0x09, CLK);       //This wont work cause numbers != chars
        write_string_LCD(" mph", 0x0C, CLK);
        write_string_LCD("CUR ACL: ", 0x40, CLK);
        write_string_LCD(currAccel, 0x49, CLK);
        write_string_LCD(" g", 0x4E, CLK);
    }
    else if (pageNum == 2)
    {
        write_string_LCD("MAX VEL: ", 0x00, CLK);
        write_string_LCD(maxVel, 0x09, CLK);
        write_string_LCD(" mph", 0x0C, CLK);
        write_string_LCD("MAX ACL: ", 0x40, CLK);
        write_string_LCD(maxAccel, 0x49, CLK);
        write_string_LCD(" g", 0x4E, CLK);
    }
    else if (pageNum == 3)
    {
        write_string_LCD("CUR 0-60: ", 0x00, CLK);
        write_string_LCD(cur60Time, 0x0A, CLK);
        write_string_LCD(" s", 0x0E, CLK);
        write_string_LCD("MAX 0-60: ", 0x40, CLK);
        write_string_LCD(max60Time, 0x4A, CLK);
        write_string_LCD(" s", 0x4E, CLK);
    } */
}

void measSixtyTime()
{
    //utilizes the timer
    //begins timer if velocity is above 0 mph
    //stops  timer when velocity is greater than 60 mph
    //time is in ms so multiply by 1000
    //1000 ms * 1000 != 1s

    //PROBALLY GOING TO OVERFLOW
    //A 20s 0-60 is only 20000ms. 32 bit ints are good for billions.
    float msTime = 0.0;
//    float time = 0.0;

    if(curVel <= 1)
    {
        msCount1 = 0;
    }
    else if (curVel >= 60)
    {
        msTime = msCount1;
    }

    cur60Time = msTime/10000;


/*     //Calculates time digits separately utilizing integer rounding
    int tens    =  time/(10000);
    int ones    = (time/10000) - (tens*10);
    int tenths  = (time/1000) - (ones*10);
    int hunths  = (time/100) - (tenths*10);
    int BCDtime = ((tens & MASK_LOW_32) << 12) | ((ones & MASK_LOW_32) << 8) |
                  ((tenths & MASK_LOW_32) << 4) | (hunths & MASK_LOW_32); */
}


void ACCEL_INIT()
{
    //Fetch previous max values from EEPROM before loop
    //Set Accel system mode
    writeByte_I2C(CTRL1_REG, (DR_400kHz | MODE_ACT));
}

//Turns out v= v0 + at is only true for constant acceleration
//So we're going to use a Riemann sum instead
//"Instantaneous" velocity will by the sum of the area of the last 10ms a vs t
//Instantaneous acceleration will actually be the inst. accel.
void measYAccel()
{
    //Reads Y output registers of accelerometer through I2C
    //As we only care about forward velocity, we assume straight line travel
    //Thus, we don't need X or Z

    uint8_t accelUN;
    uint8_t accelLN;

//    __disable_irq();
    accelUN = readByte_I2C(Y_MSB_REG);
    accelLN = readByte_I2C(Y_LSB_REG);
    curAccel_DN = ((accelUN << 4) & 0x0FF0) & ((accelLN >> 4) & 0x000F);
    accelSamp[accelIndex] = curAccel_DN;
    timeWatch[accelIndex] = msCount1;
//    __enable_irq();

    accelIndex++;
    if (accelIndex > 9) {
        accelIndex = 0;
    }

    curAccel = curAccel_DN / 1024; // 1/1024 g per digit

}

void calcYVel()
{
    int i;
    uint32_t timeWidth = 0;
    uint16_t areaSum = 0;
    for(i = 0; i < 10; i++) {
        timeWidth = (timeWatch[i] - timeWatch[i-1]);
        areaSum = areaSum + (timeWidth * accelSamp[i]);
    }
    //N * 1u(100us) * 0.0001s / 1u(100us) * 1g/1024N * 9.8m/s2/g * 2.237 mps/m/s = 1/467097 mph
    curVel = areaSum / 467097;
}

void TIMER_INIT()
{
    //TIMER_A0 setup
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = TIMER1_DEL;               //Division needed for desired timer increments
    __enable_irq();                             //Enables global interrupts
}


void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
    TIMER_A0->CCR[0] += TIMER1_DEL;


//    if (accelStatus) {
        measYAccel();
        YDR_FLAG = 1;
//    }


    msCount1++;
}

void TA0_N_IRQHandler(void) {
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;  //Clears CCR1 interrupt flag
	TIMER_A0->CCR[1] += TIMER2_DEL;
	
	msCount2++;


}