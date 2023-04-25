/*
  🔴 operating screen
  ✔️reel pressure in bars
  ✔️volts and watts
  ✔️meters per hour m/h
  ✔️remaining measures
  ✔️remaining meters in time
  ✔️time and date
  ✔️temperature C H

  🔴Errors that will inform us
  ✔️Stop watering
  ✔️15% deviation in collection measures
  ✔️Wind greater than 3 Beaufort
  ✔️Half an hour before the end of watering
  ✔️Low battery voltage

  🔴Messages on mobile for information
  ✔️Counts collection counters / hour
  ✔️Measures left until it ends
  ✔️How much time is left for watering until the end
  ✔️The water pressure (bar) on the reel
  ✔️Battery indicator
  ✔️Time and date display
  ✔️Temperature & humidity indicator
  ✔️Wind speed
*/
#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4
#include "RTClib.h"
#include <EEPROM.h>
#include <EEPROM.h>
#include "dht.h"
#define DHT22_PIN 11 // DHT 22  (AM2302) - what pin we're connected to
dht DHT;
// dht1wire DHT(DHT22_PIN, dht::DHT22);
//  Constants

// Variables
float hum;  // Stores humidity value
float temp; // Stores temperature value
bool releasing = true;
int writeStringToEEPROM(int addrOffset, const String &strToWrite)
{
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++)
  {
    EEPROM.write(addrOffset + 1 + i, strToWrite[i]);
  }
  return addrOffset + 1 + len;
}
int readStringFromEEPROM(int addrOffset, String *strToRead)
{
  int newStrLen = EEPROM.read(addrOffset);
  char data[newStrLen + 1];
  for (int i = 0; i < newStrLen; i++)
  {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrLen] = '\0';
  *strToRead = String(data);
  return addrOffset + 1 + newStrLen;
}
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
const int RecordTime = 2; // Define Measuring Time (Seconds)
const int SensorPin = 3;  // Define Interrupt Pin (2 or 3 @ Arduino Uno)

int InterruptCounter;
float WindSpeed;
bool wind, voltage = true;
unsigned long timeNow;
int
    eepromOffset = 0,
    str1AddrOffset,
    str2AddrOffset,
    str3AddrOffset,
    str4AddrOffset,
    str5AddrOffset,
    newStr1AddrOffset,
    newStr2AddrOffset,
    newStr3AddrOffset,
    newStr4AddrOffset,
    newStr5AddrOffset;
// Writing
String inputString = "";
String phoneNum = "";
String wheelDia = "";
String progstep = "";

RTC_DS3231 rtc;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"}; // days
int Hour, Minute, Second = 0;
String Seconds = "";
int half_revolutions = 0;
unsigned int rpm = 0;

unsigned long timeold;
int IR1 = 2;
int IR2 = 3;
unsigned long t1 = 0;
unsigned long t2 = 0;
float setSpeed = 1500.00;
float velocity = 0.00;
bool read = false;
// int vkmh = (100*3600)/1000;
// int counter = 0;
bool clear, count1, count2 = true;

volatile byte half_revolutions2;
unsigned int rpm2;
unsigned long timeold2;
unsigned long prev = 0;
int mNow, hNow = 0;
bool count, dir = true;
bool done = false;
bool done2 = true;
float speed = 0.00;
const int button[] = {7, 8, 9};
int countering = 0;
int counter = 0;
int ButtonState[] = {0, 0, 0, 0};
int lastButtonState[] = {1, 1, 1, 1};
int password[] = {0, 1, 2, 2};
int level = 0;
int green = 7;
int red = 8;
String pass = "";
bool doOnce, doOnce2, doOnce3 = true;
unsigned long currentDistance = 0;
String inputstring = "";
bool setMet = true;
int radius = 2;
int newp = 0;
int hall1 = 2;
String Minutes = "";
float metra = 0.25;
// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A1; // Analog input pin that the potentiometer is attached to batery
const int analogOutPin = 9; // Analog output pin that the LED is attached to

unsigned long lastMillis = 0;

int sensorValue = 0; // value read from the pot
int outputValue = 0; // value output to the PWM (analog out)
int buttonUp = 9;
int buttonDown = 8;
int buttonOk = 7;
int metraAdd = 0; // address for the meter variable
int speedAdd = 0; // address for the meter variable
boolean ends = true;
float total_len = 0;
int endTime = 0;
float timePerTurn = 0.0;
unsigned long
    speeding = 0,
    turnStart = 0,
    turnEnd = 0;
int speedSet = 30;
int length = 0;
int pwm = 6;
int in1 = 5;
int in2 = 4;
bool deviation = true;
int cursorPos;
int dig[4];
int upButton = 9;
int downButton = 8;
int select = 7;
bool stopWatering = true;
void setup()
{
  Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
  delay(1000);
  Serial.println("Preparing to read received SMSes");
  Serial1.begin(9600); // Setting the baud rate of GSM Module
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonOk, INPUT_PULLUP);
  pinMode(hall1, INPUT_PULLUP);
  pinMode(pwm, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  count = true;
  speedSet = setSpeed;
  // Serial.println("Preparing to send SMS");
  // SendMessage();
  // Serial.begin(115200);
  // attachInterrupt(0, magnet_detect, FALLING);//Initialize the intterrupt pin (Arduino digital pin 2)
  // attachInterrupt(0, getSpeeding, RISING); // Initialize the intterrupt pin (Arduino digital pin 2)
  pinMode(IR1, INPUT_PULLUP);
  attachInterrupt(0, reads, RISING);
  half_revolutions = 10;
  rpm = 100;
  timeold = 0;
  lcd.init();      // start the lcd library
  lcd.backlight(); // turn on the lcd backlight
  lcd.clear();     // clear the cld screen
  Wire.begin();
  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
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
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(upButton, INPUT_PULLUP);
  pinMode(downButton, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);
  // set values of variables
  /*lcd.print("Enter password:");
    lcd.setCursor(6, 1);
    lcd.print("0000");
    lcd.cursor();
    lcd.noBlink();
    lcd.setCursor(6, 1);
    cursorPos = 0;
    for (int i = 0; i <= 3; i++)
    {
    dig[i] = 0;
    }

    while (1)
    {
    passwordStart();
    if (countering == 100)
      break;
    }
    lcd.noCursor();
    // This line sets the RTC with an explicit date & time, for example to set
    Serial.println("Initializing gsm ...");

    Serial1.println("AT"); // Once the handshake test is successful, it will back to OK
    updateSerial();
    Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
    updateSerial();
    delay(100);
    Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
    updateSerial();
  */
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("hose is releasing..."); // this prints whats in between the quotes
  lcd.setCursor(0, 1);
  lcd.print("ok press when done"); // this prints the tag value
  lcd.setCursor(0, 2);
  lcd.print("for watering start "); // this prints the tag value

  metra = EEPROM.read(metraAdd);
  if (metra == 255)
    metra = 6.0;
  dir = true;
  half_revolutions = 0;
  // timeConvert(6);// mins hour
  metra = 0.1524; // 6 inches
}
void loop()
{
  DateTime now = rtc.now();
  Minute = now.minute();
  Hour = now.hour();
  Second = now.second();
  getSpeed();
  getSpeeding(); // this controls the motor retraction

  // controlMotor(getSpeed());
  if (getVoltage() < 11.5)
  {
    if (voltage)
    {
      errorVoltage();
      voltage = false;
    }
  }
  //================================================================
  if (Serial1.available() > 0)
  {
    // n delay(50);
    while (1)
    {
      ReadUnreadMessages();
      if (millis() - timeNow > 10000)
      {
        timeNow = millis();
        break;
      }
    }
    Serial.print("phone number in  use : ");
    Serial.println(phoneNum);

    Serial.print("prog step in  use : ");
    Serial.println(progstep);

    Serial.print("diameters in  use : ");
    Serial.println(wheelDia);
  }
  // delay(1000);

  if (!digitalRead(buttonOk))
  {
    Serial.println("ok button pressed");
    delay(100);
    lcd.clear();
    while (1)
    {
      // if (digitalRead(hall1))count = true;
      //  if (!digitalRead(hall1))magnet_detect();
      lcd.setCursor(0, 0);
      lcd.print("set speed"); // this prints whats in between the quotes
      lcd.setCursor(0, 1);
      lcd.print("press up/down button"); // this prints the tag value
      lcd.setCursor(0, 2);
      lcd.print("meter/h adjust ");
      lcd.setCursor(15, 2);
      lcd.print(speedSet);
      if (!digitalRead(buttonUp))
      {
        delay(100);
        lcd.clear();
        speedSet = speedSet + 1.0;
        if (speedSet > 8000)
          speedSet = 8000;
      }
      else if (!digitalRead(buttonDown))
      {
        delay(100);
        lcd.clear();
        speedSet = speedSet - 1.0;
        if (speedSet <= 0)
          speedSet = 0;
      }
      else if (!digitalRead(buttonOk))
      {
        EEPROM.update(speedSet, speedAdd);
        length = half_revolutions * metra;
        Serial.print("speed set is :");
        Serial.println(speedSet);
        setSpeed = speedSet;
        lcd.clear();
        done = true;
        ends = true;
        dir = false;
        setMet = false;
        releasing = false;
        break;
      }
    }
  }
  if (done)
  {
    DisplayPSI(); // pressure and battery measurement
    lcd.setCursor(0, 1);
    lcd.print("T=");       // this prints whats in between the quotes
    lcd.print(now.hour()); // this prints whats in between the quotes
    lcd.print(":");        // this clears the display field so anything left is deleted
    if (now.minute() < 10)
      Minutes = "0" + String(Minute);
    else
      Minutes = String(Minute);
    lcd.print(Minutes); // this prints the tag value
    // lcd.print(":");         // this clears the display field so anything left is deleted
    if (now.second() < 10)
      Seconds = "0" + String(Second);
    else
      Seconds = String(Second);
    // lcd.print(Seconds);

    //================================================
    lcd.print(" Y=");       // this prints whats in between the quotes
    lcd.print(now.day());   // this prints whats in between the quotes
    lcd.print(":");         // this clears the display field so anything left is deleted
    lcd.print(now.month()); // this prints the tag value
    lcd.print(":");         // this clears the display field so anything left is deleted
    lcd.print(now.year());
    lcd.setCursor(0, 2);
    lcd.print("W:");
    lcd.print(getWind(), 0);
    lcd.print(" T:");
    lcd.print(getTemp());
    lcd.print(" H:");
    lcd.print(getHum());
    lcd.print(" V :");
    lcd.print(getVoltage(), 0);
    lcd.setCursor(0, 3);
    lcd.print("len rem.");
    lcd.print(half_revolutions * metra);

    currentDistance = half_revolutions * metra;
    float wateringTimeNow = wateringEnd(currentDistance, getSpeed()); // meter/hr
    float totalWateringTime = wateringEnd(total_len*metra, getSpeed());     // mph
    float timeLeft =abs (totalWateringTime - wateringTimeNow);             // hours
    Serial.print("time left for watering : ");
    Serial.println(timeLeft);
    if (half_revolutions <= 0)
    {
      if (stopWatering)
      {
        errorStopWatering();
        stopWatering = false;
      }
    }
    delay(1000);
    unsigned long timeNow = millis();
    if (timeNow - prev > 5000)
    {
      prev = timeNow;
      // Serial.println("lcd cleared");
      // lcd.clear();
      ends = true;
      // ReadUnreadMessages();
    }
  }
  else
  {
    // lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("hose is releasing..."); // this prints whats in between the quotes
    lcd.setCursor(0, 1);
    lcd.print("press the ok button"); // this prints the tag value
    lcd.setCursor(0, 2);
    lcd.print("for watering start ");
    lcd.setCursor(0, 3);
    if (speeding > 0)
    {
      lcd.print("km/h:");     // this prints whats in between the quotes
      lcd.print(speeding, 1); // this prints the tag value
    }
    lcd.setCursor(8, 3);
    lcd.print(" len:"); // this prints the tag value
    lcd.print(int(half_revolutions * metra));
  }
}
void updateSerial()
{
  delay(1000);
  while (Serial.available())
  {
    Serial1.write(Serial.read()); // Forward what Serial received to Software Serial Port
  }
  while (Serial1.available())
  {
    Serial.write(Serial1.read()); // Forward what Software Serial received to Serial Port
    char inChar = (char)Serial1.read();
    inputstring += inChar; // if the incoming character is a newline, set a flag // so the main loop can do something about it:
    if (inChar == '\n')
    {
      inputstring.toUpperCase();
      //      FN_processData (inputstring);
      inputstring = "";
    }
  }
  inputstring = "";
}
void magnet_detect() // This function is called whenever a magnet/interrupt is detected by the arduino
{                    // lcd.clear();
  if (count)
  {
    turnStart = millis();
    timePerTurn = turnStart - turnEnd;
    Serial.print("timer per turn is : ");
    Serial.println(timePerTurn);                    // msec
    speed = ((metra / (timePerTurn / 1000)) * 3.6); // per hour
    turnEnd = turnStart;
    if (dir)
    {
      half_revolutions++;
      half_revolutions2++;
      // lcd.setCursor(0,  3);
      // lcd.print("released (M):");            // this prints the tag value
      // lcd.setCursor(16,  3);
      // lcd.print( half_revolutions*metra);
    }
    else if (!dir)
    {

      half_revolutions--;
      half_revolutions2--;
      if (half_revolutions <= 0)
        half_revolutions = 0;
      half_revolutions2 = 0;
      ends = true;
    }
    Serial.print("counts : ");
    Serial.print(half_revolutions);
    Serial.print("\t rpm : ");
    Serial.print(rpm);
    Serial.print("\t speed km/s: ");
    Serial.println(speed);
    count = false;
  }
}

// this section declares the minutecount variable and sets its initial values
volatile unsigned long secondCount = 0; // use volatile for shared variables

void DisplayPSI() // main display
{
  // this section monitors the live psi and turns the compressor run bit on or off based off setpoints
  int psi = analogRead(0); // this reads the analog input(A0) and scales it
  // psi = map(psi, 102, 921, 0, 150);             // this maps the raw analog input value to the converted PSI value
  psi = analogRead(A0);
  int sensorVal = analogRead(A0);
  // Serial.print("Sensor Value: ");
  // Serial.print(sensorVal);

  float voltage = (sensorVal * 5.0) / 1024.0;
  // Serial.print("Volts: ");
  // Serial.print(voltage);

  float pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  float pressure_bar = pressure_pascal / 10e5;
  // Serial.print("Pressure = ");
  // Serial.print(pressure_bar);
  // Serial.println(" bars");
  // this is the lcd section
  value = analogRead(analogInPin);
  vOUT = (value * 5.0) / 1024.0;
  vIN = vOUT / (R2 / (R1 + R2));
  // Serial.print("Input = ");
  // Serial.println(vIN);
  outputValue = vIN;

  // sensorValue = analogRead(analogInPin); // FOR THE BATTERY
  //  map it to the range of the analog out:
  // if (sensorValue > 540)
  //  sensorValue = 540;
  // outputValue = map(sensorValue, 0, 540, 0, 13);
  // outputValue=constrain(outputValue,0,12);
  //   Serial.print("raw battery value : ");
  //   Serial.println(sensorValue);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BA:"); // this prints whats in between the quotes
  lcd.print(pressure_bar);
  // lcd.print(" VOLT:");  // this prints whats in between the quotes
  //  lcd.print(outputValue);             // this prints the tag value
  lcd.print("  batt:");
  int percent = (outputValue / 12.5) * 100.0;
  if (percent != newp)
    // lcd.clear();
    newp = percent;
  lcd.print(percent);
  // Serial.println(percent);
  lcd.print("%");
}
// this section stores the setpoints into the arduino eeprom so the unit can start up after a power fail
void storeAll()
{
  lcd.clear();
  // this is the lcd section
  lcd.setCursor(0, 0);
  lcd.print("SAVING IN");
  lcd.setCursor(0, 1);
  lcd.print("PROGRESS...");
  delay(2000);
}

float timeConvert(int time)
{
  int minsConv;
  if (time < 60)
  {
    minsConv = time;
  }
  else
  {
    minsConv = time % 60;
  }
  int hourConv = time / 60;
  // Serial.print(hourConv);
  // Serial.print(" : ");
  // Serial.println(minsConv);
  return minsConv, hourConv;
}

void ReadUnreadMessages()
{
  while (Serial1.available() > 0)
  {
    char inByte = Serial1.read();
    Serial.write(inByte);
    inputString += inByte;
  }
  Serial.print("recived message : ");
  Serial.println(inputString);
  inputString.toUpperCase(); // Uppercase the Received Message
  // read from port 0, send to port 1:
  //  Serial.print("received data in caps ");
  // Serial.println(inputString);
  processData();
  inputString = "";

  if (Serial.available())
  {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
}
void SendMessage()
{

  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(1000);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(1000);

  Serial1.println(" watering machine has been booted"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial.println("sent the text booted from SIM800"); // SMS Text
  Serial1.println("AT");
  delay(1000);
  Serial.println("Connecting...");
  Serial.println("Connected!");
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(1000);

  Serial1.println("AT+CMGF=1"); // Set SMS to Text Mode
  delay(1000);
  Serial1.println("AT+CNMI=1,2,0,0,0"); // Procedure to handle newly arrived messages(command name in text: new message indications to TE)
  delay(1000);
  Serial.println("Fetching list of received unread SMSes!");
  //  Serial1.println("AT+CMGL=\"REC UNREAD\"\r");
  delay(1000);
}
void processData()
{
  if (inputString.indexOf("ER#") > -1)
  {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    //  Serial.println(inputString.indexOf("#"));PROGNUMBER#X#1234567890#
    int num = inputString.indexOf("#");
    String id = (inputString.substring(num + 1, num + 2));
    Serial.print("phone id : ");
    Serial.println(id);
    if (id == "1")
    {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      str1AddrOffset = writeStringToEEPROM(eepromOffset, phoneNum);
    }
    else if (id == "2")
    {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      str2AddrOffset = writeStringToEEPROM(str1AddrOffset, phoneNum);
      //  writeStringToEEPROM(str2AddrOffset, str3);
    }
    else if (id == "3")
    {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      str3AddrOffset = writeStringToEEPROM(str2AddrOffset, phoneNum);
      //  writeStringToEEPROM(str2AddrOffset, str3);
    }
    inputString = "";
  }
  if (inputString.indexOf("PROGSTEP#") > -1)
  {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    //  Serial.println(inputString.indexOf("#"));PROGSTEP#1234567890#
    int num = inputString.indexOf("#");
    Serial.print("program step number  set to :");
    Serial.println(inputString.substring(num + 1));
    progstep = inputString.substring(num + 1);
    str4AddrOffset = writeStringToEEPROM(str3AddrOffset, progstep);
    inputString = "";
  }
  if (inputString.indexOf("MTR#") > -1)
  {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    // Serial.println(inputString.indexOf("#"))MTR#134.0#
    int num = inputString.indexOf("#");
    Serial.print("THE meter  number  set to :");
    Serial.println(inputString.substring(num + 1));
    wheelDia = inputString.substring(num + 1);
    speedSet = wheelDia.toInt();
    str5AddrOffset = writeStringToEEPROM(str4AddrOffset, wheelDia);
    inputString = "";
  }
  Serial.print("phone number in  use : ");
  Serial.println(phoneNum);

  Serial.print("prog step in  use : ");
  Serial.println(progstep);

  Serial.print("diameters in  use : ");
  Serial.println(wheelDia);
}
void readMem()
{
  newStr1AddrOffset = readStringFromEEPROM(eepromOffset, &phoneNum);
  newStr4AddrOffset = readStringFromEEPROM(newStr3AddrOffset, &progstep);
  readStringFromEEPROM(newStr4AddrOffset, &wheelDia);
}
void passwordStart()
{
  int readUp = digitalRead(upButton);
  int readDown = digitalRead(downButton);
  int readSelect = digitalRead(select);
  if (readSelect == LOW)
  {
    delay(250);
    moveRight();
    countering++;
  }
  if (readDown == LOW)
  {
    delay(250);
    decrease(cursorPos);
    // counter--;
  }
  else if (readUp == LOW)
  {
    delay(250);
    increase(cursorPos);
  }
  // this runs once the button is presses
  if (countering >= 4)
  {
    Serial.println("checking");
    checkCode();

    // delay(500);
  }
}
void moveRight()
{
  if (cursorPos == 3)
  {
    lcd.setCursor(6, 1); // here in this part of the code we define controls of joystick
    cursorPos = 0;       // how  we can use every part of the move of joystick code here
  }
  else
  {                        // define that for every movement of the joystick change the direction
    int a = cursorPos + 7; // change the numbers display on lcd for ok button click on joystick take in center and click the passcode will enter.
    lcd.setCursor(a, 1);
    cursorPos = cursorPos + 1;
  }
}

void moveLeft()
{
  if (cursorPos == 0)
  {
    lcd.setCursor(9, 1);
    cursorPos = 3;
  }
  else
  {
    int b = cursorPos + 5;
    lcd.setCursor(b, 1);
    cursorPos = cursorPos - 1;
  }
}

void increase(int cursorPos)
{
  if (dig[cursorPos] == 9)
  {
    dig[cursorPos] = 0;
  }
  else
  {
    dig[cursorPos] = dig[cursorPos] + 1;
  }
  int c = cursorPos + 6;
  lcd.setCursor(c, 1);
  lcd.print(dig[cursorPos]);
  lcd.setCursor(c, 1);
}

void decrease(int cursorPos)
{
  if (dig[cursorPos] == 0)
  {
    dig[cursorPos] = 9;
  }
  else
  {
    dig[cursorPos] = dig[cursorPos] - 1;
  }
  int c = cursorPos + 6;
  lcd.setCursor(c, 1);
  lcd.print(dig[cursorPos]);
  lcd.setCursor(c, 1);
}
// change this part for your own passcode
// I've picked 1234....
void checkCode()
{
  if (dig[0] == 1 && dig[1] == 2 && dig[2] == 3 && dig[3] == 4)
  { // Change '1', '2', '3', '4' on this line!!!!
    lcd.setCursor(4, 1);
    lcd.print("correct");
    // delay(1000);
    countering = 100;
  }
  else
  {
    lcd.setCursor(5, 1);
    lcd.print("Wrong!");
    delay(1000);
    lcd.clear();
    lcd.print("Enter password:");
    lcd.setCursor(6, 1);
    lcd.print("0000");
    lcd.setCursor(6, 1);
    countering = 0;
    dig[0] = 0;
    dig[1] = 0;
    dig[2] = 0;
    dig[3] = 0;
    // setup();
  }
}
float getVoltage()
{
  value = analogRead(analogInPin);
  vOUT = (value * 5.0) / 1024.0;
  vIN = vOUT / (R2 / (R1 + R2));
  // Serial.print("Input = ");
  // Serial.println(vIN);
  return vIN;
}
float getWind()
{
  InterruptCounter = 0;
  attachInterrupt(digitalPinToInterrupt(SensorPin), countup, RISING);
  delay(50 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  WindSpeed = (float)InterruptCounter / (float)RecordTime * 2.4;
  if (WindSpeed > 3)
  {
    if (wind)
    {
      errorWind();
      wind = false;
    }
  }
  else
  {
    wind = true;
  }
  return WindSpeed;
}

void countup()
{
  InterruptCounter++;
}
int getTemp()
{
  /* //int chk = DHT.read22(DHT22_PIN);
     dht::ReadStatus chk = DHT.read();
    // Read data and store it to variables hum and temp
    hum = DHT.getHumidity();
    temp = DHT.getTemperature();
    // Print temp and humidity values to serial monitor
    Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");*/
  int chk = DHT.read22(DHT22_PIN);
  // Read data and store it to variables hum and temp
  hum = DHT.humidity;
  temp = DHT.temperature;
  // Print temp and humidity values to serial monitor
  /*Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");*/
  // delay(2000); // Delay 2 sec.
  return temp;
}

int getHum()
{
  // int chk = DHT.read22(DHT22_PIN);
  // dht::ReadStatus chk = DHT.read();
  // Read data and store it to variables hum and temp
  // hum = DHT.getHumidity();
  // temp = DHT.getTemperature();
  // Print temp and humidity values to serial monito
  int chk = DHT.read22(DHT22_PIN);
  // Read data and store it to variables hum and temp
  hum = DHT.humidity;
  temp = DHT.temperature;
  // Print temp and humidity values to serial monitor
  /*Serial.print("Humidity: ");
    Serial.print(hum);
    Serial.print(" %, Temp: ");
    Serial.print(temp);
    Serial.println(" Celsius");*/
  // delay(2000); // Delay 2 sec.
  return hum;
}
void getSpeeding()
{
  float s = getSpeed();
  controlMotor(s);
}
float getPsi()
{
  // this section monitors the live psi and turns the compressor run bit on or off based off setpoints
  // int psi = analogRead(0); // this reads the analog input(A0) and scales it
  // psi = map(psi, 102, 921, 0, 150);             // this maps the raw analog input value to the converted PSI value
  int psi = analogRead(A0);
  int sensorVal = analogRead(A0);
  // Serial.print("Sensor Value: ");
  // Serial.print(sensorVal);

  float voltage = (sensorVal * 5.0) / 1024.0;
  // Serial.print("Volts: ");
  // Serial.print(voltage);

  float pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  float pressure_bar = pressure_pascal / 10e5;
  // Serial.print("Pressure = ");
  // Serial.print(pressure_bar);
  return pressure_bar;
}
String getTimeDate()
{
  char buf1[20];
  DateTime now = rtc.now();
  sprintf(buf1, "%02d:%02d:%02d %02d/%02d/%02d", now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  // Serial.print(F("Date/Time: "));
  // Serial.println(buf1);
  return buf1;
}
float getSpeed()
{
  if (count2)
  {
    t2 = millis();
    Serial.print("t2 ");
    Serial.println(t2);
    read = true;
    done2 = true;
    count1 = false;
    count2 = false;
  }
  if (count1)
  {
    t1 = millis();
    Serial.print("t1 ");
    Serial.println(t1);
    done2 = true;
    count1 = false;
    count2 = false;
  }

  if (read)
  {
    velocity = t2 - t1;
    velocity = velocity / 1000;                // convert millisecond to second for timig
    velocity = (0.18 / velocity) * 3.6 * 1000; // km/s
    velocity = velocity / 1000;
    Serial.println("velocity/speed: ");
    Serial.print(velocity);
    speeding = velocity;
    Serial.println(" km/hr");
    delay(500);
    read = false;
  }
  else
  {
    velocity = 0;
  }
  return velocity;
}
void controlMotor(float speed)
{
  if (speed > 0)
  {
    Serial.println(speed);
    if (speed > setSpeed + 400.00)
    {
      // delay(5000);
      // Serial.println("moving forward, more speed");
      analogWrite(pwm, 255);
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
    }
    else if (speed < setSpeed - 300.00)
    {
      // Serial.println("moving backward, less speed");
      analogWrite(pwm, 255);
      digitalWrite(in2, LOW);
      digitalWrite(in1, HIGH);
    }
    else
    {
      analogWrite(pwm, 0);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }
  }
}

// error handling section
void errorStopWatering()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" watering has stopped"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void errorDeviation()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" speed deviation from set by 15%"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void errorVoltage()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" battery is low"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void errorWind()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" wind is greater than 3 barefoot"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void reads()
{
  if (done2)
  {
    if (millis() - lastMillis > 250)
    {
      if (done)
        half_revolutions--;
      if (!done)
        half_revolutions++;
      total_len = half_revolutions;
      lastMillis = millis();
      // Serial.println("counting");
      counter++;
      if (counter == 1)
      {
        // Serial.println("count1");
        count1 = true;
        count2 = false;
        // done = false;
      }
      else if (counter == 2)
      {
        // Serial.println("count2");
        count2 = true;
        count1 = false;
        counter = 0;
      }
      done2 = false;
    }
  }
}

float wateringEnd(float distance, float speed)
{
  endTime = distance / speed;
  return endTime;
}