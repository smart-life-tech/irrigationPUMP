#include "LiquidCrystal_I2C.h"
#define LCD_ADDRESS 0x27                                                                              // Define LCD characteristics
#define LCD_ROWS 2
#define LCD_COLUMNS 16
#define SCROLL_DELAY 150
#define BACKLIGHT 255                                                                                // object initialization
LiquidCrystal_I2C lcd(0x27, 20, 4);
int n = 1;
int val;                                                                                             // A simple data logger for the Arduino analog pins
#define LOG_INTERVAL  1000 // mills between entries (reduce to take more/faster data)
// how many milliseconds before writing the logged data permanently to disk
// set it to the LOG_INTERVAL to write each time (safest)
// set it to 10*LOG_INTERVAL to write all data every 10 datareads, you could lose up to
// the last 10 reads if power is lost but it uses less power and is much faster!

#define SYNC_INTERVAL 10000 // mills between calls to flush() - to write data to the card 600000 = 10 minutes
int filename = 1;
void error(char *str)
{
  Serial.print(F("error: "));
  Serial.println(str);
}


void setup(void) {
  Serial.begin(9600);                                                                                 // start serial port
  Serial.println(F("pressure reading off SKU237545 SENSOR"));                                         //F = macro function to save dynamic memory
  lcd.init();         // start the lcd library
  lcd.backlight();    // turn on the lcd backlight
  lcd.print(F("SKU237545 SENSOR" ));
  lcd.setCursor(0, 1);
  lcd.print(F("  bar and psi " ));
  delay (2000);

}
void loop(void) {
  int sensorVal = analogRead(A0);
  //Serial.print("Sensor Value: ");
  //Serial.print(sensorVal);
  float voltage = (sensorVal * 5.0) / 1024.0;
  //    Serial.print("Volts: ");
  //    Serial.print(voltage);
  float pressure_pascal = (3.0 * ((float)voltage - 0.475)) * 1000000.0;                 //calibrate here
  float pressure_bar = pressure_pascal / 10e5;
  float pressure_psi = pressure_bar * 14.5038;
  Serial.print("Pressure = ");
  Serial.print(pressure_bar);
  Serial.print(" bars  ");
  Serial.print ("psi ");
  Serial.print (pressure_psi);
  Serial.println();
  lcd.setCursor(0, 0);
  lcd.print(pressure_bar);                                   // Print String to LCD on first line
  lcd.print ("bar ");
  lcd.print (pressure_psi);
  lcd.print ("psi   ");

  lcd.setCursor(0, 1);
  lcd.print("RAW: ");                                   // Print String to LCD on first line
  lcd.print (analogRead(A0));
  lcd.print("raw reading : "); 
  Serial.println(analogRead(A0),5);

}
