// Include Libraries
#include "Arduino.h"
#include "DHT.h"
#include "LiquidCrystal_PCF8574.h"
#include "SoilMoisture.h"
#include "SolenoidValve.h"

// Pin Definitions
#define DHT_PIN_DATA 2
#define SOILMOISTURE5V_PIN_SIG A3
#define SOLENOIDVALVE_PIN_COIL1 3

// There are several different versions of the LCD I2C adapter, each might have a different address.
//#define LCD_ADDRESS 0x3F
#define LCD_ADDRESS 0x27

// Define LCD characteristics
#define LCD_ROWS 2
#define LCD_COLUMNS 16
#define SCROLL_DELAY 150
#define BACKLIGHT 255
// object initialization
DHT dht(DHT_PIN_DATA);
LiquidCrystal_PCF8574 lcdI2C;
SoilMoisture soilMoisture5v(SOILMOISTURE5V_PIN_SIG);
SolenoidValve solenoidValve(SOLENOIDVALVE_PIN_COIL1);

// define vars for testing menu
const int timeout = 10000; //define timeout of 10 sec
char menuOption = 0;
long time0;

const int tooDry = 128;
const int waterDurationSeconds = 3;
bool isWatering = false;
long lastWateredTime = 0;

// Setup the essentials for your circuit to work. It runs first every time your circuit is powered with electricity.
void setup()
{
  dht.begin();
  // initialize the lcd
  lcdI2C.begin(LCD_COLUMNS, LCD_ROWS, LCD_ADDRESS, BACKLIGHT);
}

// Main logic of your circuit. It defines the interaction between the components you selected. After setup, it runs over and over again, in an eternal loop.
void loop()
{
  /* *********************************************** */
  /* Read all the values from the components we have */
  /* *********************************************** */
  // DHT22/11 Humidity and Temperature Sensor - Read humidity in %
  float humidity = dht.readHumidity();
  // Read temperature in Celsius, for Fahrenheit use .readTempF()
  float tempF = dht.readTempF();

  // Soil Moisture Sensor - Test Code
  int soilMoisture5vVal = soilMoisture5v.read();

  /* *********************************************** */
  /* Water the plant if necessary                    */
  /* *********************************************** */
  isWatering = false;
  if (soilMoisture5vVal < tooDry)
  {
    solenoidValve.on();
    delay(waterDurationSeconds * 1000);
    solenoidValve.off();

    isWatering = true;
    // millis is the time since starting up. See https://www.arduino.cc/reference/en/language/functions/time/millis/
    lastWateredTime = millis();
  }

  /* *********************************************** */
  /* Show the values on the LCD                      */
  /* *********************************************** */
  // Note: sprintf is a function to format a string
  // We have floats for the humidity and temp. Arduino's sprintf function doesn't include support for floats.
  // We'll simplify things by converting to integers. We're aiming for a string like "12% 16C nV" to show humidity, temp, and soil "Voltage".
  // We'll format an array of characters (a string) with the values using sprintf, then print that with the LCD library
  // Line 1 can be sensor values, and hopefully we can get the rest of the feedback into line 2

  char buffer[16];
  lcdI2C.clear();
 sprintf(buffer, "%02dF Humid %02d\%4 ", int(tempF), int(humidity));
  lcdI2C.print(buffer);
  lcdI2C.selectLine(2);
  sprintf(buffer, "%s %d Mins", (isWatering ? "Watering" : "        "), (millis() - lastWateredTime) / 1000 / 60);
  lcdI2C.print(buffer);

 // Wait before updating again
delay(3000); // 3000 is 3 seconds
}
