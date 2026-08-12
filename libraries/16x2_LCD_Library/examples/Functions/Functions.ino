/**
 **************************************************
 *
 * @file        Functions.ino
 * @brief       Example of using custom functions. See more at solde.red/333003
 *
 *              
 * @authors     Zvonimir Haramustek for Soldered.com
 ***************************************************/

#include "16x2-LCD-SOLDERED.h"

LCD lcd; // LCD object


void setup()
{
    lcd.begin();     // Initialize LCD
    lcd.backlight(); // Turn on LCD backlight
}

void loop()
{
    // Autoscroll
    lcd.setCursor(5, 0); //Set cursor to 6th (starts from 0) place in row, 1st row
    lcd.print(F("Autoscrolling")); //Print text to LCD
    lcd.setCursor(10, 1);   //Set cursor
    lcd.autoscroll();   //Enable autoscroll

    for (int i = 0; i < 10; i++)    //Print first 10 integers
    {
        lcd.print(i);   
        delay(200);
    }

    lcd.noAutoscroll(); //Disable autoscroll
    lcd.clear();    //Clear content from LCD

    // Scroll left and right
    lcd.setCursor(10, 0); 
    lcd.print(F("To the left!"));
    for (int i = 0; i < 10; i++)
    {
        lcd.scrollDisplayLeft();    //This command shifts al content to left by 1 place
        delay(200);
    }
    lcd.clear(); //Clear content from LCD
    lcd.print(F("To the right!"));
    for (int i = 0; i < 10; i++)
    {
        lcd.scrollDisplayRight();   //This command shifts al content to right by 1 place
        delay(200);
    }
    lcd.clear();

    // Cursor
    lcd.setCursor(0, 0);
    lcd.cursor();
    lcd.print(F("Cursor"));
    delay(3000);
    lcd.clear();

    // Cursor blink
    lcd.setCursor(0, 0);
    lcd.blink();    //This command enables cursor blink
    lcd.print(F("Cursor blink"));
    delay(3000);
    lcd.clear();

    // Blink without cursor
    lcd.setCursor(0, 0);
    lcd.noCursor(); //Disable cursor, so it is no longer shown on screen
    lcd.print(F("Just blink"));
    delay(3000);
    lcd.noBlink();  //Stop cursor blinking
    lcd.clear();
}
