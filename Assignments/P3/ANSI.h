/**
 * ANSI.h
 *
 * Header for running the ANSI escape code terminal manipulations
 *
 * Date: May 13, 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#ifndef ANSI_H_
#define ANSI_H_

#define A_ESC   0x1B

#define A_SPACE 0x20
#define A_EXCLM 0x21
#define A_DQUOT 0x22
#define A_PND   0x23
#define A_DOLLR 0x24
#define A_PRCNT 0x25
#define A_AMPSD 0x26
#define A_SACNT 0x27
#define A_LPARA 0x28
#define A_RPARA 0x29
#define A_ASTRK 0x2A
#define A_PLUS  0x2B
#define A_SQUOT 0x2C
#define A_HYPHN 0x2D
#define A_DECML 0x2E
#define A_BSLSH 0x2F
#define A_0     0x30
#define A_1     0x31
#define A_2     0x32
#define A_3     0x33
#define A_4     0x34
#define A_5     0x35
#define A_6     0x36
#define A_7     0x37
#define A_8     0x38
#define A_9     0x39
#define A_COLON 0x3A
#define A_SEMIC 0x3B
#define A_LBRAC 0x3C
#define A_EQUAL 0x3D
#define A_RBRAC 0x3E
#define A_QUEST 0x3F
#define A_AT    0x40
#define A_A     0x41
#define A_B     0x42
#define A_C     0x43
#define A_D     0x44
#define A_E     0x45
#define A_F     0x46
#define A_G     0x47
#define A_H     0x48
#define A_I     0x49
#define A_J     0x4A
#define A_K     0x4B
#define A_L     0x4C
#define A_M     0x4D
#define A_N     0x4E
#define A_O     0x4F
#define A_P     0x50
#define A_Q     0x51
#define A_R     0x52
#define A_S     0x53
#define A_T     0x54
#define A_U     0x55
#define A_V     0x56
#define A_W     0x57
#define A_X     0x58
#define A_Y     0x59
#define A_Z     0x5A
#define A_LBRKT 0x5B
#define A_FSLSH 0x5C
#define A_RBRKT 0x5D
#define A_CARET 0x5E
#define A_UNDSC 0x5F
#define A_ACCNT 0x60

#define A_l     0x6C

#define A_LCRLY 0x7B
#define A_SSLSH 0x7C
#define A_RCRLY 0x7D
#define A_TILDE 0x7E
#define A_DEL   0x7F
#define A_BLOCK 0xDB
#define A_ORTHG 0xC1

void moveCursUp_ANSI(uint8_t);
void moveCursDown_ANSI(uint8_t);
void moveCursLeft_ANSI(uint8_t);
void moveCursRight_ANSI(uint8_t);
void moveCursRC_ANSI(uint8_t, uint8_t);
void hideCurs_ANSI();
void cursPhoneHome_ANSI();
void clearLineRight_ANSI();
void clearLineLeft_ANSI();
void clearLine_ANSI();
void clearScreen_ANSI();

#endif /* ANSI_H_ */
