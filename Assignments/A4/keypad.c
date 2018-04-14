/*
 * keypad.c
 *
 * Created on: Apr 13, 2018
 * Author: Zach Bunce, Garrett Maxon
 *
 * Call KEYPAD_INIT to enable use of keypad
 * chk_Keypad meant for external use
 * Returns ASCII code of symbol pressed (Including numbers)
 */

#include "msp.h"

#define ROWA    BIT0
#define ROWB    BIT1
#define ROWC    BIT2
#define ROWD    BIT3

#define COL1    BIT0
#define COL2    BIT1
#define COL3    BIT2

//Rows: A-D; Columns 1-3
//First row
#define A1      0x13
#define A2      0x15
#define A3      0x16

//Second row
#define B1      0x23
#define B2      0x25
#define B3      0x26

//Third row
#define C1      0x43
#define C2      0x45
#define C3      0x46

//Fourth row
#define D1      0x83
#define D2      0x85
#define D3      0x86

//ASCII hex values for keypad characters
#define K_1     0x31
#define K_2     0x32
#define K_3     0x33

#define K_4     0x34
#define K_5     0x35
#define K_6     0x36

#define K_7     0x37
#define K_8     0x38
#define K_9     0x39

#define K_Ast   0x2A
#define K_0     0x30
#define K_Pnd   0x23

#define K_NP    0x01

uint8_t KEY_LOCATE();

//Sets P4.0-4.3 as rows and P5.0-5.2 as columns
void KEYPAD_INIT()
{
    P4 -> DIR |= (ROWA | ROWB | ROWC | ROWD); //Sets DIR reg of the outputs
    P5 -> DIR &= ~(COL1 | COL2 | COL3);     //Clears DIR reg of the inputs
    P5 -> REN |=  (COL1 | COL2 | COL3);     //Enables PU or PD resistor
    P5 -> OUT |=  (COL1 | COL2 | COL3);     //Confusing syntax, but PU or PD is set through PxOUT reg
}

//Decodes row & column of key pressed into ASCII value
uint8_t chk_Keypad()
{
    uint8_t RC_Info = KEY_LOCATE();
    switch(RC_Info)
    {
    case A1:
        return K_1;
    case A2:
        return K_2;
    case A3:
        return K_3;

    case B1:
        return K_4;
    case B2:
        return K_5;
    case B3:
        return K_6;

    case C1:
        return K_7;
    case C2:
        return K_8;
    case C3:
        return K_9;

    case D1:
        return K_Ast;
    case D2:
        return K_0;
    case D3:
        return K_Pnd;
    default:
        return K_NP; //Returns no press; empty value in LCD table
    }
}

//Finds and returns row and column of uppermost key pressed
uint8_t KEY_LOCATE()
{
    uint8_t i;
    uint8_t col = 0;
    for (i = 1; i <= 0x08; i = i << 1) {
        P4 -> OUT |= (ROWA | ROWB | ROWC | ROWD); //Sets all rows high
        P4 -> OUT &= ~i & (ROWA | ROWB | ROWC | ROWD); //Sets selected row low
        col = P5->IN & 0x07;   //Reads column states into lower nibble
        if (col == 0x07) {
            //Do nothing
        }
        else {
            i = i << 4; //Shift row info into upper nibble
            col |= i;   //Add row info to column info
            return col; //Return column and row info
        }
    }
    return 0;
}
