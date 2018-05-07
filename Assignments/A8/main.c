/*
 * main.c
 * Manages the communication system between the serial terminal and DAC
 * Utilizes both UART and SPI communications
 *
 * Date: May 7, 2018
 * Author: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include "set_DCO.h"
#include "SPI.h"
#include "DAC.h"
#include "keypad.h"
#include "UART.h"
#include <STDINT.h>

void RX_Decode(int);

int UART_FLG = 0;

void main(void)
{
    int RX_Val = 0;
    int UART_FLG = 0;
    int CLK = 30;

    WDT_A -> CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;   //Stop watchdog timer
    set_DCO(CLK);   //Sets clock to 3MHz

    SPI_INIT();     //Initializes SPI comms for sending data to DAC
    UART_INIT();    //Initializes UART comms for receiving DAC values

    while(1) {
        UART_FLG = getIntFlag_UART();   //Checks to see if new UART data is available

        if(UART_FLG) {
            RX_Val = getRXByte();       //Gets new character
            RX_Decode(RX_Val);          //Decodes character into numerical value
            clrIntFlag_UART();          //Clears the internal UART interrupt flag
            UART_FLG = 0;               //Clears the external UART interrupt flag
        }
    }
}

//Due to big endian storage, 4 digits must be used for proper behavior
void RX_Decode(int RX_Val)
{
    static int RX_Vals[] = {0,0,0,0};
    static int byte_cnt = 0;
    int DN_Point = 0;

    //Decodes the ASCII data for each number into its integer value
    switch(RX_Val) {
    case (K_0):
        RX_Vals[byte_cnt] = K_0 - 0x30; //Stores the character
        byte_cnt++;                     //Increments the placement counter
        break;
    case (K_1):
        RX_Vals[byte_cnt] = K_1 - 0x30;
        byte_cnt++;
        break;
    case (K_2):
        RX_Vals[byte_cnt] = K_2 - 0x30;
        byte_cnt++;
        break;
    case (K_3):
        RX_Vals[byte_cnt] = K_3 - 0x30;
        byte_cnt++;
        break;
    case (K_4):
        RX_Vals[byte_cnt] = K_4 - 0x30;
        byte_cnt++;
        break;
    case (K_5):
        RX_Vals[byte_cnt] = K_5 - 0x30;
        byte_cnt++;
        break;
    case (K_6):
        RX_Vals[byte_cnt] = K_6 - 0x30;
        byte_cnt++;
        break;
    case (K_7):
        RX_Vals[byte_cnt] = K_7 - 0x30;
        byte_cnt++;
        break;
    case (K_8):
        RX_Vals[byte_cnt] = K_8 - 0x30;
        byte_cnt++;
        break;
    case (K_9):
        RX_Vals[byte_cnt] = K_9 - 0x30;
        byte_cnt++;
        break;
    //Determines the DAC information and resets system when the enter key is pressed
    case (K_Ret):
        byte_cnt = 0;   //Clears the placement counter
        //Calculates the DAC value based on the previous four characters entered
        DN_Point = RX_Vals[0] * 1000 + RX_Vals[1] * 100 + RX_Vals[2] * 10 + RX_Vals[3] * 1;

        //Ensures the value sent to the DAC is within the 12-bit resolution
        if (DN_Point < DAC_MAX) {
            write_DAC(DN_Point);
        }

        //Clears the stored values
        RX_Vals[0] = 0;
        RX_Vals[1] = 0;
        RX_Vals[2] = 0;
        RX_Vals[3] = 0;
        break;
    }
}
