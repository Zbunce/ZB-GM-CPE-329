/*
 * keypad.c
 *
 *  Created on: Apr 13, 2018
 *      Author: Garrett
 *      connect each top of the column to pulldown resistor that are then connected to ground
 *      the order process is set a,b,c,d to high
 *      then set a to zero and read 1 2 3 , a high indicates a press and a low indicates not pressed
 *      repeat for each row setting the row high reading then setting it back to low before going to the next row
 */
#include "msp.h"
#include "delays.h"
#include "LCD.h"

#define row1 BIT0
#define row2 BIT1
#define row3 BIT2
#define row4 BIT3

#define colm1 BIT0
#define colm2 BIT1
#define colm3 BIT2



// first row
#define A1 0x00
#define A2 0x01
#define A3 0x02


//second row
#define B1 0x00
#define B2 0x01
#define B3 0x02

//third row
#define C1 0x00
#define C2 0x01
#define C3 0x02

//fourth row
#define D1 0x00
#define D2 0x01
#define D3 0x02



void init_keypad()
{
    P4 -> DIR |= 0x0F;
    P4 -> OUT |= 0x0F;
    P5 -> REN |= (colm1 | colm2| colm3);

}

void chk_press ()
{

    if ()
}




