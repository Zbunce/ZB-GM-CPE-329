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

void moveCursUp_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_A);
}

void moveCursDown_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_B);
}

void moveCursRight_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_C);
}

void moveCursLeft_ANSI(uint8_t line)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(line);
    sendByte_UART(A_D);
}

void cursPhoneHome_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_H);
}

//Broken, don't use
void moveCursRC_ANSI(uint8_t row, uint8_t col)
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(row);
    sendByte_UART(A_SEMIC);
    sendByte_UART(col);
    sendByte_UART(A_H);
}

void clearLineRight_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_K);
}

void clearLineLeft_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_1);
    sendByte_UART(A_K);
}

void clearLine_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_2);
    sendByte_UART(A_K);
}

void clearScreen_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_2);
    sendByte_UART(A_J);
}

void hideCurs_ANSI()
{
    sendByte_UART(A_ESC);
    sendByte_UART(A_LBRKT);
    sendByte_UART(A_QUEST);
    sendByte_UART(A_2);
    sendByte_UART(A_5);
    sendByte_UART(A_l);
}
