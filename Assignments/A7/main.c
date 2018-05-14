
//1. Change TestFunction top declaration type
//2. Change TestFunction heading declaration type
//3. Change num type in top declaration
//4. Change num type in heading declaration
//5. Change mainVar type in main
//6. Change testVar type
// Note: All intx_t data type statements need changing; use Ctrl+f replace
//7. Change testVar process
//8. Repeat 7 for each process
//9. Repeat 1-8 for each data type

#include "msp.h"
#include <math.h>

int8_t TestFunction(int8_t num);

int main(void)
{
    int8_t mainVar;

    WDTCTL = WDTPW | WDTHOLD; // Stop watchdog timer

    P1->SEL1 &= ~BIT0; //set P1.0 as simple I/O
    P1->SEL0 &= ~BIT0;
    P1->DIR |= BIT0; //set P1.0 as output

    P2->SEL1 &= ~(BIT2 | BIT1 | BIT0); //set P2.0-2.2 as simple I/O
    P2->SEL0 &= ~(BIT2 | BIT1 | BIT0);
    P2->DIR |= (BIT2 | BIT1 | BIT0); //set P2.0-2.2 as output pins

    P2->OUT |= (BIT2 | BIT1 | BIT0); // turn on RGB LED

    mainVar = TestFunction(15); // test function for timing

    P2->OUT &= ~(BIT2 | BIT1 | BIT0); // turn off RGB LED

    while (1) // infinite loop to do nothing
        mainVar++; // increment mainVar to eliminate not used warning
}

int8_t TestFunction(int8_t num)
{
    int8_t testVar; //var_type testVar;

    P1->OUT |= BIT0; // set P1.0 LED on

    testVar = num; //{insert_function_here (ie testVar = num;)}

    P1->OUT &= ~BIT0; // set P1.0 LED off
    return testVar;
}
