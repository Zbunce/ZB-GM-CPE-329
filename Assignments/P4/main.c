/**
 * main.c
 *
 * Speedometer high level code
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
#include <string.h>
#include "set_DCO.h"
#include "delays.h"
#include "LCD.h"
#include "keypad.h"
#include "I2C.h"

//15 bit addresses so MSB doesn't matter
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

//CTRL1 Register
#define DR_400kHz       0x08
#define DR_100kHz       0x00
#define MODE_ACT        0x01
#define MODE_STBY       0x00

//Timer A0 CCR value
#define TIMER0_DEL      12000   //1ms

static int msCount;
static int stopTimer = 0;
static uint16_t curAccel_DN = 0;

static int curVel = 0;
static int curAccel = 0;
static int cur60Time = 0;
static int cur60TimeUN = 0;
static int cur60TimeLN = 0;

static int maxVel = 0;
static int maxAccel = 0;
static int max60Time = 0;
static int max60TimeUN = 0;
static int max60TimeLN = 0;

void ACCEL_INIT();
void TIMER_INIT();
void writeDisp(int, int);
void calcYVel();
void measYAccel();
void measSixtyTime();

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    int pageNum = 1;
    int z = 0;
    uint8_t key = K_NP;

    set_DCO(CLK);
    LCD_INIT(CLK);
    KEYPAD_INIT();
    I2C_INIT(ACCEL_ADDR);
    ACCEL_INIT();
    TIMER_INIT();

    while(1) {
        measYAccel();       //Takes new acceleration measurement
        measSixtyTime();    //Checks 0-60 timer

        //Slows down LCD update rate to be readable
        if (z > 1000) {
            key = get_Key();    //Checks for keypad press

            if (key == K_1) {
                //Current velocity/acceleration page
                pageNum++;
                key = K_NP;
                if (pageNum > 3) {
                    pageNum = 1;
                }
            }
            else if (key == K_3) {
                //Max velocity/acceleration page
                pageNum--;
                key = K_NP;
                if (pageNum < 0) {
                    pageNum = 3;
                }
            }
            else if (key == K_2) {
                //Soft reset
                __disable_irq();
                stopTimer = 0;
                cur60Time = 0;
                curVel = 0;
                maxVel = 0;
                curAccel = 0;
                maxAccel = 0;
                __enable_irq();

            }
            else {
                key = K_NP;
            }

            writeDisp(pageNum, CLK);    //Writes new values to display
            z = 0;
        }
        z++;    //Increments LCD write counter
    }
}

void writeDisp(int pageNum, int CLK)
{
    //Current Velocity & Acceleration (page 1) - Max Velocity & Acceleration(page 2) - 0-60 timer (page 3)

    char curVelStr[17];
    char curAccelStr[17];
    char maxVelStr[17];
    char maxAccelStr[17];
    char cur60TimeStr[17];
    char max60TimeStr[17];

    //Copies new measurements into output strings
    snprintf(curVelStr,    17, "CUR VEL: %3d mph", curVel);
    snprintf(curAccelStr,  17, "CUR ACL: %4d mg", curAccel);
    snprintf(maxVelStr,    17, "MAX VEL: %3d mph", maxVel);
    snprintf(maxAccelStr,  17, "MAX ACL: %4d mg", maxAccel);
    snprintf(cur60TimeStr, 17, "CUR 0-60: %2d.%-2ds", cur60TimeUN, cur60TimeLN);
    snprintf(max60TimeStr, 17, "MAX 0-60: %2d.%-2ds", max60TimeUN, max60TimeLN);

    //Prints different measurements based on page number
    if (pageNum == 1)
    {
        clear_LCD(CLK);
        write_string_LCD(curVelStr, 0x00, CLK);
        write_string_LCD(curAccelStr, 0x40, CLK);
    }
    else if (pageNum == 2)
    {
        clear_LCD(CLK);
        write_string_LCD(maxVelStr, 0x00, CLK);
        write_string_LCD(maxAccelStr, 0x40, CLK);
    }
    else if (pageNum == 3)
    {
        clear_LCD(CLK);
        write_string_LCD(cur60TimeStr, 0x00, CLK);
        write_string_LCD(max60TimeStr, 0x40, CLK);
    }
}

//Begins counting ms increments if velocity calculating condition is met
//Stops counting when velocity is greater than 60 mph
void measSixtyTime()
{
    if (stopTimer == 1) {
        cur60Time = max60Time;  //Sets current time to max time if run finished
    }
    else if(maxAccel < 500) {
        msCount = 0;    //Keeps
    }
    else {
        cur60Time = msCount / 10;  //Converts to ms
    }

    if (curVel >= 60) {
        stopTimer = 1;  //Sets flag is run finished
    }

    if (cur60Time > max60Time) {
        max60Time = cur60Time;  //Checks stop condition
    }

    //Calculates non-decimal and decimal components to store outside a float
    cur60TimeUN = cur60Time / 100;
    cur60TimeLN = cur60Time - cur60TimeUN * 100;

    max60TimeUN = max60Time / 100;
    max60TimeLN = max60Time - max60TimeUN * 100;
}

//Reads Y output registers of accelerometer through I2C
//As we only care about forward velocity, we assume straight line travel
//Thus, we don't need to read X or Z
void measYAccel()
{
    static uint16_t accelUN = 0x00;
    static uint16_t accelLN = 0x00;

    accelUN = readByte_I2C(Y_MSB_REG);  //Reads upper 8 bits of 12 bit acceleration value
    accelLN = readByte_I2C(Y_LSB_REG);  //Reads lower 4 bits of 12 bit acceleration value
    curAccel_DN = ((accelUN << 4) & 0x0FF0) | ((accelLN >> 4) & 0x000F); //Combines and masks

    curAccel = curAccel_DN / 4; //2g/2^12 per digit for g; converted to mg

    //Checks max condition
    if (curAccel > maxAccel) {
        maxAccel = curAccel;
    }
}

//Calculated velocity is the Riemann sum of the acceleration with a dt of 1 ms
//Begins calculation when acceleration exceeds 500 mg to prevent noise triggers
void calcYVel()
{
    static int areaSum = 0;
    if (maxAccel < 500) {
        areaSum = 0;                //Holds velocity at 0 until vehicle begins real movement
    }
    else {
        areaSum += curAccel_DN;     //Sums acceleration area
    }
    //N * 1ms * 1s / 1000ms * 1g/2048N * 9.80665m/s2/g * 2.23694 mph/m/s = 1/93359 mph
    curVel = (areaSum*2) / 93359;   //Calculates current velocity

    //Checks for max
    if (curVel > maxVel) {
        maxVel = curVel;
    }
}

void ACCEL_INIT()
{
    //Set Accelerometer system mode to standby
    uint8_t CTRL1 = MODE_STBY;
    writeByte_I2C(CTRL1_REG, CTRL1);
    //Set I2C data rate to 400 kHz
    CTRL1 = (DR_400kHz);
    writeByte_I2C(CTRL1_REG, CTRL1);
    //Set system mode to active for measuring
    CTRL1 = (DR_400kHz | MODE_ACT);
    writeByte_I2C(CTRL1_REG, CTRL1);
}

void TIMER_INIT()
{
    //TIMER_A0 setup
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = TIMER0_DEL;              //Division needed for desired timer increments
    __enable_irq();                             //Enables global interrupts
}

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
    TIMER_A0->CCR[0] += TIMER0_DEL; //Increments to next timer amout

    msCount++;                      //Increments the millisecond counter
    calcYVel();                     //Calculates the current velocity
}

