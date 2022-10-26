#include <Arduino.h>
#include <M5Stack.h>

SHT3X sht30;
QMP6988 qmp6988;

float tmp = 0.0;
float hum = 0.0;
float pressure = 0.0;

void setup()
{
  M5.begin();       // Init M5Core.
  M5.Power.begin(); // Init Power module.
  /* Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project */
  //M5.Lcd.print("Hello World"); // Print text on the screen (string)

  M5.lcd.setTextSize(2); // Set the text size to 2.
  Wire.begin();          // Wire init, adding the I2C bus.
  qmp6988.init();
  M5.lcd.println(F("ENVIII Unit(SHT30 and QMP6988) test"));
}

void loop()
{
  pressure = qmp6988.calcPressure();
  if (sht30.get() == 0)
  {                       // Obtain the data of shT30.
    tmp = sht30.cTemp;    // Store the temperature obtained from shT30.

    hum = sht30.humidity; // Store the humidity obtained from the SHT30.
  }
  else
  {
    tmp = 0, hum = 0;
  }
  M5.lcd.fillRect(0, 20, 100, 60,
                  BLACK); // Fill the screen with black (to clear the
                          // screen).
  M5.lcd.setCursor(0, 20);
  M5.Lcd.printf("Temp: %2.1f  \r\nHumi: %2.0f%%  \r\nPressure:%2.0fPa\r\n",
                tmp, hum, pressure);
  delay(2000);
}