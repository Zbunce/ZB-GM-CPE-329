/**
 * DAC.c
 *
 * Contains functions for running the DAC both in general and as a FG
 * Allows for DC voltages and various wave types to be automatically produced
 *
 * Date: April 25 2018
 * Authors: Zach Bunce, Garrett Maxon
 */

#include "msp.h"
#include <math.h>
#include <stdint.h>
#include "SPI.h"
#include "DAC.h"

static int c = 0;           //Square value index variable
static int s = 0;           //Sin value index variable
static int t = 0;           //Sawtooth value index variable

static int waveType = 1;    //Wave property variables for ISR
static int freq_idx = 1;
static int DC = 50;

static int sqrDel = 600;    //Wave value refresh delay counts
static int sinDel = 600;
static int sawDel = 600;
static int divs = 200;      //Total sample number
static int16_t DN_Point;    //DAC value transmitted

//Wave value LUTs
static uint16_t sqrVal10_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal20_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal30_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal40_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal50_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal60_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal70_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal80_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sqrVal90_DN[200] = {
0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,
0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF,0x0FFF
};

static uint16_t sinVal_DN[200] = {
0x0800,0x0840,0x0880,0x08C0,0x0900,0x0940,0x097F,0x09BE,0x09FD,0x0A3B,
0x0A78,0x0AB5,0x0AF1,0x0B2D,0x0B67,0x0BA1,0x0BDA,0x0C12,0x0C49,0x0C7F,
0x0CB3,0x0CE7,0x0D19,0x0D4A,0x0D79,0x0DA8,0x0DD4,0x0E00,0x0E2A,0x0E52,
0x0E78,0x0E9D,0x0EC1,0x0EE2,0x0F02,0x0F20,0x0F3D,0x0F57,0x0F70,0x0F86,
0x0F9B,0x0FAE,0x0FBF,0x0FCE,0x0FDB,0x0FE6,0x0FEF,0x0FF6,0x0FFB,0x0FFE,
0x0FFF,0x0FFE,0x0FFB,0x0FF6,0x0FEF,0x0FE6,0x0FDB,0x0FCE,0x0FBF,0x0FAE,
0x0F9B,0x0F86,0x0F70,0x0F57,0x0F3D,0x0F20,0x0F02,0x0EE2,0x0EC1,0x0E9D,
0x0E78,0x0E52,0x0E2A,0x0E00,0x0DD4,0x0DA8,0x0D79,0x0D4A,0x0D19,0x0CE7,
0x0CB3,0x0C7F,0x0C49,0x0C12,0x0BDA,0x0BA1,0x0B67,0x0B2D,0x0AF1,0x0AB5,
0x0A78,0x0A3B,0x09FD,0x09BE,0x097F,0x0940,0x0900,0x08C0,0x0880,0x0840,
0x0800,0x07BF,0x077F,0x073F,0x06FF,0x06BF,0x0680,0x0641,0x0602,0x05C4,
0x0587,0x054A,0x050E,0x04D2,0x0498,0x045E,0x0425,0x03ED,0x03B6,0x0380,
0x034C,0x0318,0x02E6,0x02B5,0x0286,0x0257,0x022B,0x01FF,0x01D5,0x01AD,
0x0187,0x0162,0x013E,0x011D,0x00FD,0x00DF,0x00C2,0x00A8,0x008F,0x0079,
0x0064,0x0051,0x0040,0x0031,0x0024,0x0019,0x0010,0x0009,0x0004,0x0001,
0x0000,0x0001,0x0004,0x0009,0x0010,0x0019,0x0024,0x0031,0x0040,0x0051,
0x0064,0x0079,0x008F,0x00A8,0x00C2,0x00DF,0x00FD,0x011D,0x013E,0x0162,
0x0187,0x01AD,0x01D5,0x01FF,0x022B,0x0257,0x0286,0x02B5,0x02E6,0x0318,
0x034C,0x0380,0x03B6,0x03ED,0x0425,0x045E,0x0498,0x04D2,0x050E,0x054A,
0x0587,0x05C4,0x0602,0x0641,0x0680,0x06BF,0x06FF,0x073F,0x077F,0x07BF
};

static uint16_t sawVal_DN[200] = {
0x0000,0x0014,0x0028,0x003D,0x0051,0x0066,0x007A,0x008F,0x00A3,0x00B8,
0x00CC,0x00E1,0x00F5,0x010A,0x011E,0x0133,0x0147,0x015C,0x0170,0x0185,
0x0199,0x01AE,0x01C2,0x01D7,0x01EB,0x0200,0x0214,0x0228,0x023D,0x0251,
0x0266,0x027A,0x028F,0x02A3,0x02B8,0x02CC,0x02E1,0x02F5,0x030A,0x031E,
0x0333,0x0347,0x035C,0x0370,0x0385,0x0399,0x03AE,0x03C2,0x03D7,0x03EB,
0x0400,0x0414,0x0428,0x043D,0x0451,0x0466,0x047A,0x048F,0x04A3,0x04B8,
0x04CC,0x04E1,0x04F5,0x050A,0x051E,0x0533,0x0547,0x055C,0x0570,0x0585,
0x0599,0x05AE,0x05C2,0x05D7,0x05EB,0x0600,0x0614,0x0628,0x063D,0x0651,
0x0666,0x067A,0x068F,0x06A3,0x06B8,0x06CC,0x06E1,0x06F5,0x070A,0x071E,
0x0733,0x0747,0x075C,0x0770,0x0785,0x0799,0x07AE,0x07C2,0x07D7,0x07EB,
0x0800,0x0814,0x0828,0x083D,0x0851,0x0866,0x087A,0x088F,0x08A3,0x08B8,
0x08CC,0x08E1,0x08F5,0x090A,0x091E,0x0933,0x0947,0x095C,0x0970,0x0985,
0x0999,0x09AE,0x09C2,0x09D7,0x09EB,0x0A00,0x0A14,0x0A28,0x0A3D,0x0A51,
0x0A66,0x0A7A,0x0A8F,0x0AA3,0x0AB8,0x0ACC,0x0AE1,0x0AF5,0x0B0A,0x0B1E,
0x0B33,0x0B47,0x0B5C,0x0B70,0x0B85,0x0B99,0x0BAE,0x0BC2,0x0BD7,0x0BEB,
0x0C00,0x0C14,0x0C28,0x0C3D,0x0C51,0x0C66,0x0C7A,0x0C8F,0x0CA3,0x0CB8,
0x0CCC,0x0CE1,0x0CF5,0x0D0A,0x0D1E,0x0D33,0x0D47,0x0D5C,0x0D70,0x0D85,
0x0D99,0x0DAE,0x0DC2,0x0DD7,0x0DEB,0x0E00,0x0E14,0x0E28,0x0E3D,0x0E51,
0x0E66,0x0E7A,0x0E8F,0x0EA3,0x0EB8,0x0ECC,0x0EE1,0x0EF5,0x0F0A,0x0F1E,
0x0F33,0x0F47,0x0F5C,0x0F70,0x0F85,0x0F99,0x0FAE,0x0FC2,0x0FD7,0x0FEB,
};

void FG_INIT()
{
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE;     //Enables TACCR0 interrupt
    //Runs Timer A on SMCLK and in continuous mode
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__CONTINUOUS;
    //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        //Enables sleep on exit from ISR

    __enable_irq();                             //Enables global interrupts
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);   //Links ISR to NVIC
    TIMER_A0->CCR[0] = 600;                     //Initializes first high time count
}

void makeDC(int volt)
{
    uint16_t DN = (((4096*volt)/(Vref))*10);    //Maps the 12-bit DAC value for the desired output voltage
    write_DAC(DN);                              //Writes value to the DAC
}

void makeWave(int waveT, int freq, int duty, int CLK)
{
    waveType = waveT;       //Links external specified parameters to ISR globals
    freq_idx = freq / 100;  //Determines table increment amount from frequency
    DC = duty;

    //All processing done before run time or in ISR
    write_DAC(DN_Point);
}

void write_DAC(uint16_t data)
{
    uint8_t up_Byte;
    uint8_t low_Byte;

    //Shifts the upper nibble into the lower, masks the 4 data bits, appends control bits
    up_Byte  =  ((data >> 8) & 0x0F) | (GAIN1 | SDOFF);
    low_Byte =   (data & 0x00FF);       //Masks bottom 8 data bits

    P5 -> OUT &= ~BIT5;                 //Lowers chip select
    sendByte_SPI(up_Byte);              //Transmits upper byte on SPI line
    sendByte_SPI(low_Byte);             //Transmits lower byte on SPI line
    P5 -> OUT |= BIT5;                  //Sets chip select
}

//Timer A0 interrupt service routine
void TA0_0_IRQHandler(void) {
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  //Clears interrupt flag

    if (waveType == square)
    {
        c += freq_idx;      //Increments table index
        if (c >= divs) {
            c = 0;          //Clears index at the end of the LUT
        }

        //Picks correct LUT based on duty cycle
        switch(DC) {
        case 10:
            DN_Point = sqrVal10_DN[c];
            break;
        case 20:
            DN_Point = sqrVal20_DN[c];
            break;
        case 30:
            DN_Point = sqrVal30_DN[c];
            break;
        case 40:
            DN_Point = sqrVal40_DN[c];
            break;
        case 50:
            DN_Point = sqrVal50_DN[c];
            break;
        case 60:
            DN_Point = sqrVal60_DN[c];
            break;
        case 70:
            DN_Point = sqrVal70_DN[c];
            break;
        case 80:
            DN_Point = sqrVal80_DN[c];
            break;
        case 90:
            DN_Point = sqrVal90_DN[c];
            break;
        default:
            DN_Point = sqrVal50_DN[c];
            break;
        }
        TIMER_A0->CCR[0] += sqrDel; //Adds next offset to TACCR0
    }
    else if(waveType == sine) {
        s += freq_idx;              //Increments table index
        if (s >= divs) {
            s = 0;                  //Clears index at the end of the LUT
        }
        DN_Point = sinVal_DN[s];    //Sets DAC value to value at the current index
        TIMER_A0->CCR[0] += sinDel; //Adds next offset to TACCR0
    }
    else if(waveType == sawtooth) {
        t += freq_idx;              //Increments table index
        if (t >= divs) {
            t = 0;                  //Clears index at the end of the LUT
        }
        DN_Point = sawVal_DN[t];    //Sets DAC value to value at the current index
        TIMER_A0->CCR[0] += sawDel; //Adds next offset to TACCR0
    }
}