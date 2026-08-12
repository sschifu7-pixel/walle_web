/**
 **************************************************
 *
 * @file        Hello_World.ino
 * @brief       Simple Hello World example for LCD. See more at solde.red/333003
 *
 *
 *
 * @authors     Zvonimir Haramustek for Soldered.com.com
 ***************************************************/

#include "16x2-LCD-SOLDERED.h"

LCD lcd; // Define LCD object

void setup()
{
    lcd.begin();     // Initialize LCD
    lcd.backlight(); // Turn the backlight on
}


void loop()
{
    lcd.print("     Hello"); // Prints Hello on the LCD
    lcd.setCursor(5, 1);     // set cursor to 5th character in line 1
    lcd.print("World!");     // Prints World! on the LCD
    delay(500);

    // Flashing the backlight
    for (int i = 0; i < 5; ++i)
    {
        lcd.backlight();    //Turn on backlight
        delay(50);
        lcd.noBacklight();  //Turn off backlight
        delay(50);
    }

    lcd.backlight(); // Turn the backlight on
    lcd.clear();     // Clear the LCD
    delay(500);
}
