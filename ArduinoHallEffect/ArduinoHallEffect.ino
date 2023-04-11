#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4
volatile byte half_revolutions;
unsigned int rpm;
unsigned long timeold;

volatile byte half_revolutions2;
unsigned int rpm2;
unsigned long timeold2;
float speed = 0;
float radius = 6 * 0.0254; //diamter in m from inches
unsigned long
prevTime = 0,
timeNow = 0;
int hall1 = 2;
float circum = 0.0;

void setup() {
  Serial.begin(115200);
  pinMode(hall1, INPUT_PULLUP);
  //attachInterrupt(0, magnet_detect, RISING);//Initialize the intterrupt pin (Arduino digital pin 2)
  // attachInterrupt(1, magnet_detect2, RISING);//Initialize the intterrupt pin (Arduino digital pin 3)
  half_revolutions = 0;
  rpm = 0;
  timeold = 0;
  lcd.init();         // start the lcd library
  lcd.backlight();    // turn on the lcd backlight
  lcd.clear();        // clear the cld screen
  Wire.begin();       // start the I2C library
  pinMode(hall1, INPUT_PULLUP);
  circum =  radius;// length of a revolution
  Serial.println(circum);
}
bool once = true;
void loop() {
  //rpm = 30 * 1000 / (millis() - timeold) * half_revolutions;
  timeold = millis();
  if (!digitalRead(hall1)) {
    if (once) {
      magnet_detect();
      once = false;
    }
  } else {
    once = true;
  }
  //half_revolutions = 0;
  speed = (2 * 3.142 * radius * rpm * 60) / 63360;// converting inches to meters

  lcd.setCursor(0, 0); lcd.print(timeNow / 1000);          // this prints the tag value
  lcd.setCursor(7, 0); lcd.print(" secs/rev");            // this prints whats in between the quotes
  lcd.setCursor(0, 1); lcd.print(speed);            // this prints the tag value
  lcd.setCursor(10, 1); lcd.print(" cm/h");            // this prints whats in between the quotes
  lcd.setCursor(0, 2); lcd.print(half_revolutions);            // this prints the tag value
  lcd.setCursor(5, 2); lcd.print("rev collected");            // this prints whats in between the quotes
  lcd.setCursor(0, 3); lcd.print((half_revolutions * circum));          // this prints the tag value conversion to metrs
  lcd.setCursor(9, 3); lcd.print(" distance");            // this prints whats in between the quotes

}

void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
{
  timeNow = millis() - prevTime;
  speed =  circum / timeNow;
  half_revolutions++;
  prevTime = millis();
}
