#include "RTClib.h"
#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4
RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};//days
int Hour, Minute, Ssecond = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  lcd.init();         // start the lcd library
  lcd.backlight();    // turn on the lcd backlight
  lcd.clear();        // clear the cld screen
  Wire.begin();
  lcd.setCursor(16, 3); lcd.print("TIME");
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1) delay(10);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    // When time needs to be set on a new device, or after a power loss, the
    // following line sets the RTC to the date & time this sketch was compiled
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
  // When time needs to be re-set on a previously configured device, the
  // following line sets the RTC to the date & time this sketch was compiled
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set

  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
}

void loop() {
  // put your main code here, to run repeatedly:
  DateTime now = rtc.now();
  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour());
  Serial.print(':');
  Serial.print(now.minute());
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  Minute = now.minute();
  Hour = now.hour();

  lcd.setCursor(0,  3); 
  lcd.print("TIME : "); // this prints whats in between the quotes
  lcd.print(now.hour());        // this prints whats in between the quotes
  lcd.print("/");         // this clears the display field so anything left is deleted
  lcd.print(now.minute());   // this prints the tag value
  lcd.print("/");         // this clears the display field so anything left is deleted
  lcd.print(now.second());   // this prints the tag value
}
