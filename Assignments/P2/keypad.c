/*
 * keypad.c
 * Call KEYPAD_INIT to enable use of keypad
 * chk_Keypad meant for external use
 * Returns ASCII code of symbol pressed (Including numbers)
 *
 * Date: April 13, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "keypad.h"

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
