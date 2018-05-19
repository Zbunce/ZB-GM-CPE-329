/*
 * ANSI.c
 *
 * Contains functions for transmitting various ANSI escape codes
 *
 * Date: May 13, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "ANSI.h"
#include "UART.h"

//Moves the cursor up by x number of rows
void moveCursUp_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_A);
}

//Moves the cursor down by x number of rows
void moveCursDown_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_B);
}

//Moves the cursor right by x number of columns
void moveCursRight_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_C);
}

//Moves the cursor left by x number of columns
void moveCursLeft_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_D);
}

//Returns the cursor to the top-left corner
void cursPhoneHome_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_H);
}

//Moves the cursor to a specific row and column location
void moveCursRC_ANSI(uint8_t row, uint8_t col)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(row);
    sendByte_UART(A_SEMIC);
    sendByte_UART(col);
    sendByte_UART(A_H);
}

//Clears everything to the right of the cursor in the row
void clearLineRight_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_K);
}

//Clears everything to the left of the cursor in the row
void clearLineLeft_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_1);
    sendByte_UART(A_K);
}

//Clears the entire row
void clearLine_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_2);
    sendByte_UART(A_K);
}

//Clears the terminal
void clearScreen_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_2);
    sendByte_UART(A_J);
}

//Disables the blinking cursor mode
void hideCurs_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_QUEST);
    sendByte_UART(A_2);
    sendByte_UART(A_5);
    sendByte_UART(A_l);
}
