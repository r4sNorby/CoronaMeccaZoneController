#include <Arduino.h>
#include <M5Stack.h>

void setup()
{
  M5.begin();       // Init M5Core.
  M5.Power.begin(); // Init Power module.
  /* Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project */
  M5.Lcd.print("Hello World"); // Print text on the screen (string)
}

void loop()
{
  // put your main code here, to run repeatedly:
}