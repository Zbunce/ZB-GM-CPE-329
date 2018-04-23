//Some timer stuff

/*
 *  main.c
 *  Creates an interrupt based 25 kHz 25% DC clock signal
 *
 *  Date: April 23, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */
/*
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_25k = 1;                                //Tracks divided clock state

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    int CLK = 240;
    set_DCO(CLK);                               //Sets ACLK, MCLK, and SMCLK to 25MHz
    P1 -> DIR |= BIT0;
    P1 -> OUT |= BIT0;                          //Initializes 25 kHz clock high

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    TIMER_A0->CCR[0] = 240;                     //Initializes first high time count
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC

    while (1)
    {
        __sleep();
        __no_operation();                       //Holdover from example program
    }
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag

    if (CLK_25k == 0) {
        CLK_25k = 1;                //Tracks clock state for future interrupts
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
        TIMER_A0->CCR[0] += 240;    //Adds high-time offset to TACCR0
    }
    else {
        CLK_25k = 0;                //Tracks clock state for future interrupts
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
        TIMER_A0 -> CCR[0] += 720;  //Adds low-time offset to TACCR0
    }
}
*/

/*
 *  main.c
 *  Creates an interrupt based 25 kHz 50% DC clock signal
 *  Also pulses P4.7 high during ISR execution, and outputs MCLK on P4.3
 *
 *  Date: April 23, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */
/*
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_25k;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    int CLK = 240;
    set_DCO(CLK);                               //Sets ACLK, MCLK, and SMCLK to 25MHz
    P1 -> DIR |= BIT0;
    P1 -> OUT |= BIT0;                          //Initializes 25 kHz clock high
    P4 -> DIR |= BIT3 | BIT7;
    P4 -> OUT &= ~BIT7;                         //Initializes ISR execution pulse low
    P4 -> SEL1 &= ~BIT3;                        //Enables MCLK output
    P4 -> SEL0 |= BIT3;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    TIMER_A0->CCR[0] = 480;                     //Initializes first high time count
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC

    while (1) {
        __sleep();
        __no_operation();   //Holdover from example program
    }
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    P4 -> OUT |= BIT7;              //Sets ISR execution pulse
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;    //Clears interrupt flag

    //25kHz clock with 50% duty cycle
    if (CLK_25k == 0) {
        CLK_25k = 1;
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
        TIMER_A0->CCR[0] += 480;    //Adds high-time offset to TACCR0
    }
    else {
        CLK_25k = 0;
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
        TIMER_A0->CCR[0] += 480;    //Adds low-time offset to TACCR0
    }

    P4 -> OUT &= ~BIT7;             //Clears ISR execution pulse
}
*/


/*
//24MHz short pulse
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_25k;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    int CLK = 240;
    set_DCO(CLK);

    // Configure GPIO
    P1 -> DIR |= BIT0;
    P1 -> OUT |= BIT0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 70;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS; // SMCLK, continuous mode

    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Enable sleep on exit from ISR

    // Enable global interrupt
    __enable_irq();

    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);

    while (1)
    {
        __sleep();

        __no_operation();                   // For debugger
    }
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;    //Clears int flag

    //25kHz clock with 50% duty cycle
    if (CLK_25k == 0) {
        CLK_25k = 1;
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
        TIMER_A0->CCR[0] += 69;    //Adds high-time offset to TACCR0
    }
    else {
        CLK_25k = 0;
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
        TIMER_A0->CCR[0] += 69;     //Adds low-time offset to TACCR0
    }
}
*/

/*
 *  main.c
 *  Creates an interrupt based clock signal with a 20s period
 *
 *  Date: April 23, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */

#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int z = 0;                                      //Tracks ISR entry count
int CLK_50m = 1;                                //Tracks divided clock state

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    int CLK = 15;
    set_DCO(CLK);                               //Sets ACLK, MCLK, and SMCLK to 1.5MHz
    P1 -> DIR |= BIT0;
    P1 -> OUT |= BIT0;                          //Initializes 50 mHz clock high

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    TIMER_A0->CCR[0] = 7500;                    //Initializes first 5ms count
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC

    while (1) {
        __sleep();
        __no_operation();                       //Holdover from example program
    }
}

// Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag

    //Changes 50 mHz state every 2000th change of the 10 ms period clock signal
    if (z == 2000) {
        if (CLK_50m == 0) {
            CLK_50m = 1;
            P1 -> OUT |= BIT0;          //Sets 50mHz clock
        }
        else {
            CLK_50m = 0;
            P1 -> OUT &= ~BIT0;         //Clears 50mHz clock
        }
        z = 0;                          //Clears ISR entry counter
    }
    z++;                                //Increments ISR entry counter
    TIMER_A0->CCR[0] += 7500;           //Adds 5ms offset to TACCR0
}

/*
 *  main.c
 *  Creates an interrupt based 2 bit counter
 *  LSB runs at 1 kHz, MSB at 500 Hz
 *
 *  Date: April 23, 2018
 *  Authors: Zach Bunce, Garret Maxon
 */
/*
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_1k = 0;                                 //Tracks divided clock state
int CLK_500 = 0;                                //Tracks divided clock state

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    int CLK = 15;
    set_DCO(CLK);                               //Sets ACLK, MCLK, and SMCLK to 1.5MHz

    P1 -> DIR |= BIT0;
    P1 -> OUT &= ~BIT0;                         //Initializes LSB low

    P2 -> DIR |= BIT0;
    P2 -> OUT &= ~BIT0;                         //Initializes MSB low

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;     //Enables TACCR1 interrupt
    TIMER_A0->CCR[0] = 750;                     //First count for LSB timer
    TIMER_A0->CCR[1] = 1500;                    //First count for MSB timer
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;


    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links CCR0 ISR to NVIC
    NVIC->ISER[0] = 1 << ((TA0_N_IRQn) & 31);   //Links CCR1 ISR to NVIC

    while (1) {
        __sleep();
        __no_operation();                       //Holdover from example program
    }
}

// Timer A0 CCR0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears CCR0 interrupt flag

    if (CLK_1k == 0) {
        CLK_1k = 1;
        TIMER_A0->CCR[0] += 750;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 1 kHz clock
    }
    else if (CLK_1k == 1) {
        CLK_1k = 0;
        TIMER_A0->CCR[0] += 750;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 1 kHz clock
    }
}

//Timer A0 CCR1 interrupt service routine
void TA0_N_IRQHandler(void) {
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;  //Clears CCR1 interrupt flag

    if (CLK_500 == 0) {
        CLK_500 = 1;
        TIMER_A0->CCR[1] += 1500;   //Adds high-time offset to TACCR1
        P2 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if (CLK_500 == 1) {
        CLK_500 = 0;
        TIMER_A0->CCR[1] += 1500;   //Adds low-time offset to TACCR1
        P2 -> OUT &= ~BIT0;         //Clears 500 Hz clock
    }
}
*/
