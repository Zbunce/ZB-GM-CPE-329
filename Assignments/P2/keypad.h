/*
 * keypad.h
 * Header file for the keypad driver functions.
 *
 * Date: April 13, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define ROWA    BIT0
#define ROWB    BIT1
#define ROWC    BIT2
#define ROWD    BIT3

#define COL1    BIT2
#define COL2    BIT1
#define COL3    BIT0

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

#define K_NP    0x10

void KEYPAD_INIT();
uint8_t chk_Keypad();
uint8_t KEY_LOCATE();

#endif /* KEYPAD_H_ */
