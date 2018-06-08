/*
 * main.c
 * Runs the keypad to servo PWM interface
 *
 * Date: June 6, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "keypad.h"
#include "delays.h"
#include "set_DCO.h"

void setDC();
void TIMER_INIT();

int highCnt = 4500;
int lowCnt = 55500;
char vals[2] = {0,9};
char valBuf[2] = {0,9};
int valCnt = 0;

void main()
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     //Stop watchdog timer

    int CLK = 30;
    char keyVal = K_NP;

    set_DCO(CLK);   //Sets clocks to 3MHz
    TIMER_INIT();   //Initializes timer used for PWM
    KEYPAD_INIT();  //Initializes keypad

    __enable_irq(); //Enables global interrupt

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    P6 -> DIR |= BIT1;
    P6 -> OUT &= ~BIT1;

    while(1)
    {
        keyVal = K_NP;

        //Waits for first key press
        while(keyVal == K_NP) {
            keyVal = get_Key();
        }

        //Waits for 1st key press
        while(valCnt == 0 && !(keyVal == K_1   || keyVal == K_0 ||
                               keyVal == K_Ast || keyVal == K_Pnd)) {
            keyVal = get_Key();
        }

        //Waits for 2nd key press
        while(valCnt == 1 && vals[0] == K_1 && keyVal == K_9) {
            keyVal = get_Key();
        }

        if(keyVal == K_Ast) { //Rotate 10 Degrees CW
            if(valBuf[1] == K_0 && valBuf[0] == K_1) {
                valBuf[0] = K_0;
                valBuf[1] = K_9;
            }
            else if(valBuf[1] != K_0) {
                valBuf[1] -= 1;
            }

            setDC();
        }
        else if(keyVal == K_Pnd) { //Rotate 10 Degrees CCW
            if(valBuf[1] == K_9) {
                valBuf[0] = K_1;
                valBuf[1] = K_0;
            }
            else if(!(valBuf[0] == K_1 && valBuf[1] == K_8)) {
                valBuf[1] += 1;
            }

            setDC();
        }
        else {
            vals[valCnt] = keyVal;
            valCnt++;
        }

        if(valCnt >= 2) {
            valBuf[0] = vals[0];
            valBuf[1] = vals[1];
            setDC();
            valCnt = 0;
        }

        delay_ms(700, CLK);
    }
}

void TIMER_INIT()
{
    TIMER_A0 -> CCTL[0] = TIMER_A_CCTLN_CCIE;     //TACCR0 interrupt enabled
    TIMER_A0 -> CCR[0]  = 60000;
    TIMER_A0 -> CTL     = TIMER_A_CTL_SSEL__SMCLK |   //Runs the timer on SMCLK, continuous mode
                          TIMER_A_CTL_MC__CONTINUOUS;
}

//Sets the duty cycle high count and low count based on the key pressed
void setDC()
{
    switch(valBuf[1]) {
    case(K_0):
        if(valBuf[0] == K_0)
            highCnt = 3000;
        else
            highCnt = 6000;
        break;
    case(K_1):
        if(valBuf[0] == K_0)
            highCnt = 3300;
        else
            highCnt = 6300;
        break;
    case(K_2):
        if(valBuf[0] == K_0)
            highCnt = 3600;
        else
            highCnt = 6600;
        break;
    case(K_3):
        if(valBuf[0] == K_0)
            highCnt = 3900;
        else
            highCnt = 6900;
        break;
    case(K_4):
        if(valBuf[0] == K_0)
            highCnt = 4200;
        else
            highCnt = 7200;
        break;
    case(K_5):
        if(valBuf[0] == K_0)
            highCnt = 4500;
        else
            highCnt = 7500;
        break;
    case(K_6):
        if(valBuf[0] == K_0)
            highCnt = 4800;
        else
            highCnt = 7800;
        break;
    case(K_7):
        if(valBuf[0] == K_0)
            highCnt = 5100;
        else
            highCnt = 8100;
        break;
    case(K_8):
        if(valBuf[0] == K_0)
            highCnt = 5400;
        else
            highCnt = 8400;
        break;
    case(K_9):
        highCnt = 5700;
        break;
    default:
        highCnt = highCnt;
        break;
    }

    lowCnt = 60000 - highCnt;   //Calculates low count of duty cycle
}

void TA0_0_IRQHandler()
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;
    static int PREV_ST = 1;

    //Toggles output state
    if(PREV_ST) {
        PREV_ST = 0;
        P6 -> OUT |= BIT1;
        TIMER_A0->CCR[0] += highCnt;
    }
    else {
        PREV_ST = 1;
        P6 -> OUT &= ~BIT1;
        TIMER_A0->CCR[0] += lowCnt;
    }
}
