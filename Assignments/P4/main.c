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
#define Y_MSB_REG       0x03
#define Y_LSB_REG       0x04
#define STATUS_REG      0x00

#define TIMER_DEL       1200    //100us
#define CNT_MAGN        10000

static int pageNum;
static uint32_t msCount;

static uint16_t curAccel = 0;
static uint16_t accelSamp[10] = {0,0,0,0,0,0,0,0,0,0};
static uint32_t timeWatch[10] = {0,0,0,0,0,0,0,0,0,0};
static int accelIndex = 0;

static int curVel = 0;

static int velMax = 0;
static int aclMax = 0;
static int 0t6Max = 0;

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    int CLK = 120;
    int accelStatus = 0;

    set_DCO(CLK);
    LCD_INIT();
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
        accelStatus = readByte_I2C(STATUS_REG);
        YDR_Flag = accelStatus & BIT1;

        if(YDR_FLAG) {
            measYAccel();
            calcYVel();
        }

        if (curVel > velMax) {
            velMax = curVel;

        }

        if (button1 == 0)
        {
            pageNum++;
            if (pageNum > 3)
                ;
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


    }
}

void writeDisp(int pageNum, int CLK)
{
    //three pages to display information on
    //Current Velocity & Acceleration (page 1) - Max Velocity & Acceleration(page 2) - 0-60 timer (page 3)

    if (pageNum == 1)
    {
        write_string_LCD("CUR VEL: ", 0x00, CLK);
        write_string_LCD(currVel, 0x09, CLK);       //This wont work cause numbers != chars
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
    }
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
    float time = 0.0;

    if(velocity <= 0)
    {
        msCount = 0;
    }
    else if (velocity >= 60)
    {
        time = msCount/CNT_MAGN;
    }

    //Convert time float to BCD
}

void ACCEL_INIT()
{
    //Fetch previous max values from EEPROM before loop
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

    __disable_irq();
    accelUN = readByte_I2C(Y_MSB_REG);
    accelLN = readByte_I2C(Y_LSB_REG);
    curAccel = ((accelUN << 4) & 0x0FF0) & ((accelLN >> 4) & 0x000F);
    accelSamp[accelIndex] = curAccel;
    timeWatch[accelIndex] = msCount;
    __enable_irq();

    accelIndex++;
    if (accelIndex > 9) {
        accelIndex = 0;
    }

    //Convert inst. accel to BCD

}

void calcYVel()
{
    int i;
    uint32_t timeWidth = 0;
    uint16_t areaSum = 0;
    for(i = 1; i < 10; i++) {
        timeWidth = (timeWatch[i] - timeWatch[i-1]);
        areaSum = areaSum + (timeWidth * accelSamp[i]);
    }
    curVel = areaSum; //CNT_MAGN orders shifted
}

void TIMER_INIT()
{
    //TIMER_A0 setup
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt

    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK
                  | TIMER_A_CTL_MC__CONTINUOUS; //Runs Timer A on SMCLK and in continuous mode

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);
    TIMER_A0->CCR[0] = TIMER_DEL;               //Division needed for desired timer increments
    __enable_irq();                             //Enables global interrupts
}


void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag
    TIMER_A0->CCR[0] += TIMER_DEL;

    msCount++;
}
