/**
 **************************************************
 *
 * @file        Custom_Chars.ino
 * @brief       Example of using custom chars. See more at solde.red/333003
 *
 *
 *
 * @authors     Zvonimir Haramustek for Soldered.com.com
 ***************************************************/

#include "16x2-LCD-SOLDERED.h"

LCD lcd; // create LCD object


uint8_t happy[8] = {
    // happy face
    0b00000, 0b10001, 0b00000, 0b00000, 0b10001, 0b01110, 0b00000, 0b00000,
};

uint8_t wow[8] = {
    // wow face
    0b00000, 0b10001, 0b00000, 0b01110, 0b10001, 0b01110, 0b00000, 0b00000,
};

uint8_t anchor[8] = {
    // anchor bit array
    0b01110, 0b01010, 0b01110, 0b00100, 0b10101, 0b10101, 0b01110, 0b00100};

uint8_t snow[8] = {
    // snow bit array
    0b01000, 0b11101, 0b01011, 0b00001, 0b00100, 0b01110, 0b00100, 0b10000};

void setup()
{
    lcd.begin();     // begin the LCD
    lcd.backlight(); // turn on the backlight

    lcd.createChar(0, happy);  // create happy face on char 0
    lcd.createChar(1, wow);    // create wow face on char 1
    lcd.createChar(2, anchor); //  create anchor bit array on char 2
    lcd.createChar(3, snow);   //  create snow bit array on char 3


    lcd.write(0); // write happy face (char 0)
    lcd.write(1); // write wow face (char 1)
    lcd.write(2); // write anchor bit array (char 2)
    lcd.write(3); // write snow bit array (char 3)
}

void loop()
{
}
