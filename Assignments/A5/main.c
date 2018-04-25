//Some timer stuff


//24MHz 25% DC 25kHz
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_25k = 1;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; //Stop watchdog

    int CLK = 240;
    set_DCO(CLK);

    // Configure GPIO
    P1 -> DIR |= BIT0;
    P1 -> OUT |= BIT0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 240;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS; // SMCLK, continuous mode

    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR

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
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //25kHz clock with 25% duty cycle
    if (CLK_25k == 0) {
        CLK_25k = 1;
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
        TIMER_A0->CCR[0] += 240;    //Adds high-time offset to TACCR0
    }
    else if (CLK_25k == 1) {
        CLK_25k = 0;
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
        TIMER_A0->CCR[0] += 720;    //Adds low-time offset to TACCR0
    }
    else {
        CLK_25k = 0;
        P1 -> OUT &= ~BIT0;
        TIMER_A0 -> CCR[0] += 720;
    }
}


/*
//24MHz 50% DC 25kHz and ISR timing
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int CLK_25k;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    int CLK = 240;
    set_DCO(CLK);

    // Configure GPIO
    P1 -> DIR |= BIT0 | BIT1;
    P1 -> OUT |= BIT0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 480;
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
    P1 -> OUT |= BIT1;              //Sets ISR watcher
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;    //Clears int flag

    //25kHz clock with 50% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 480;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 480;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    P1 -> OUT &= ~BIT1;             //Clears ISR watcher
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
    P1 -> DIR |= BIT0 | BIT1;
    P1 -> OUT |= BIT0;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 480;
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
    P1 -> OUT |= BIT1;              //Sets ISR watcher
    TIMER_A0 -> CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;    //Clears int flag

    //25kHz clock with 50% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 480;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 480;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    P1 -> OUT &= ~BIT1;             //Clears ISR watcher
}
*/

/*
//1.5 MHz 20s divider
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int z;
int CLK_25k;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // Stop WDT

    int CLK = 15;
    set_DCO(CLK);

    // Configure GPIO
    P1 -> DIR |= BIT0 | BIT1;
    P1 -> OUT |= BIT0;

    P4 -> DIR |= BIT3;
    P4 -> SEL1 &= ~BIT3;
    P4 -> SEL0 |= BIT3;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 20;
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;   // SMCLK, continuous mode

    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;    // Enable sleep on exit from ISR

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
    P1 -> OUT |= BIT1;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //25kHz clock with 25% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 240;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 720;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    //25kHz clock with 50% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 480;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 480;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    P1 -> OUT &= ~BIT1;
}
*/

/*
//1.5MHz 2-bit counter
#include "msp.h"
#include "set_DCO.h"

void set_DCO(int);

int z = 0;
int CLK_25k;

int main(void) {
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD; // Stop WDT

    int CLK = 240;
    set_DCO(CLK);

    // Configure GPIO
    P1 -> DIR |= BIT0 | BIT1;
    P1 -> OUT |= BIT0;

    P4 -> DIR |= BIT3;
    P4 -> SEL1 &= ~BIT3;
    P4 -> SEL0 |= BIT3;

    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     // TACCR0 interrupt enabled
    TIMER_A0->CCR[0] = 50000;                   //First timer
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK |   // SMCLK, continuous mode
            TIMER_A_CTL_MC__CONTINUOUS;

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
    P1 -> OUT |= BIT1;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;

    //25kHz clock with 25% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 240;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 720;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    //25kHz clock with 50% duty cycle
    if CLK_25k = 0 {
        CLK_25k = 1;
        TIMER_A0->CCR[0] += 480;    //Adds high-time offset to TACCR0
        P1 -> OUT |= BIT0;          //Sets 25kHz clock
    }
    else if CLK_25k = 1 {
        CLK_25k = 0;
        TIMER_A0->CCR[0] += 480;    //Adds low-time offset to TACCR0
        P1 -> OUT &= ~BIT0;         //Clears 25kHz clock
    }

    P1 -> OUT &= ~BIT1;
}
*/
