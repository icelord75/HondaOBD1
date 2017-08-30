/*

Include for LCD fonts

*/

#include <avr/pgmspace.h>
#include <Arduino.h>

// Scale Bar

const uint8_t S_1[8] PROGMEM =
{
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000,
        B10000
};
const uint8_t S_2[8] PROGMEM =
{
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000,
        B11000
};
const uint8_t S_3[8] PROGMEM =
{
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100,
        B11100
};
const uint8_t S_4[8] PROGMEM =
{
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110,
        B11110
};

const uint8_t S_5[8] PROGMEM =
{
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111,
        B11111
};

const uint8_t S_6[8] PROGMEM =
{
        B00000,
        B01110,
        B11111,
        B10011,
        B10001,
        B11011,
        B01110,
        B00000
};

const uint8_t S_7[8] PROGMEM =
{
        B00000,
        B01110,
        B11011,
        B11101,
        B11111,
        B11111,
        B01110,
        B00000
};
