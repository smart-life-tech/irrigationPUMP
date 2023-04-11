/*
   to remind you
  the screen should show us
  1) collection of measures / hour
  2) distance of the remaining measures.
  3) Indication of time left for the completion of the watering process
  4) pressure
  5) Battery voltage indicator
  and all this in sms
*/
#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
#include <SoftwareSerial.h>
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4
SoftwareSerial gsmSerial(11,12);
#include "RTClib.h"
#include <EEPROM.h>
#include <EEPROM.h>
int writeStringToEEPROM(int addrOffset, const String &strToWrite) {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);
  for (int i = 0; i < len; i++) {
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
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};//days
int Hour, Minute, Second = 0;
String Seconds = "";
int half_revolutions = 0;
unsigned int rpm = 0;
unsigned long timeold;

volatile byte half_revolutions2;
unsigned int rpm2;
unsigned long timeold2;
unsigned long prev = 0;
int  mNow, hNow = 0;
bool count = true;
/*
   #define gsmdelay 1800
  #define gsm_delay_small 100
  #define EEPROM_MAX_ADDR 1023
  #define BUFFER_SIZE 11

  int trueStepValueVariable = 10;
  bool isstartAgain = true;
  int stopTimer = 0;
  int setStepTimer = 8;
  int SMS_location_number;
  int adminNumberSize;
  int counder2Rpm = 2;
  int smsMetra = 0;
  int varButtoni = 0, oldvarButton1 = 8;
  int varButton2 = 8, oldVarButton2 = 0;
  int meter = 0;
  int signalRequestTime = 30000;
  int gsminitsize;
  int gsmSignalLevel;
  //int rpm = -1;
*/
float speed = 10.0; // m/h

bool done = false;
bool dir = true;

/*
  int countCloseRpm = 0;
  unsigned int input_pos = 0;
  unsigned int stepMinMax = 20; // pososto
  unsigned long elapsedTimeAll = 0;
  unsigned long stepPerHour = 0;
  unsigned long trueStepValue = 0;
  float h = 0, m = 0, s = 0;
  float stepValue = 8;
  const char* adminNumbers[] = {"6937843744"};
  const char* gsmInit[] = { "ATEe", "AT + CHGF = 1", "AT + CLIP = 1", "AT + CRC = 1", "AT + CREG = 1" };
  boolean isSMS = false;
  boolean isSendingSms = false;
  boolean isDialing = false;
  boolean isUnder_30Min = false;
  boolean isMinMaxCalc = false;
  boolean isLockStepManual = false;
  boolean isOnTimerMinMax = false;
  boolean issignalok = false;
  boolean isStop = false;
  boolean isRunCode = false;
  boolean isAuthorized = false;
  bool done = false;
  byte modeReadButtons = 1;

  String smsSender;
  String textMessage = "welcome starting...";
  String authorizedNumbers[3];
  bool isclockwise;
  bool isstartAgainRpmCount = 0;

  // this is the Debounce section
  int buttonState;                    // this set the tag "buttonState", to the current button state from the input pin
  int lastButtonState = LOW;          // this set the tag "lastButtonState", to the previous button state from the input pin
  unsigned long lastDebounceTime = 0; // the last time the output pin was toggled
  unsigned long debounceDelay = 100;  // the debounce time; increase if the output flickers
*/String inputstring = "";
bool setMet = true;
int radius = 2;
int newp = 0;
int hall1 = 2;
String Minutes = "";
float metra = 0.25;
// These constants won't change. They're used to give names to the pins used:
const int analogInPin = A1;  // Analog input pin that the potentiometer is attached to batery
const int analogOutPin = 9; // Analog output pin that the LED is attached to

int sensorValue = 0;        // value read from the pot
int outputValue = 0;        // value output to the PWM (analog out)
int buttonUp = 9;
int buttonDown = 8;
int buttonOk = 7;
int metraAdd = 0; // address for the meter variable
int speedAdd = 0; // address for the meter variable
boolean ends = true;

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
void setup() {
  Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
  delay(1000);
  Serial.println("Preparing to read received SMSes");
  gsmSerial.begin(9600); // Setting the baud rate of GSM Module
  pinMode(buttonUp, INPUT_PULLUP);
  pinMode(buttonDown, INPUT_PULLUP);
  pinMode(buttonOk, INPUT_PULLUP);
  pinMode(hall1, INPUT_PULLUP);
  pinMode(pwm, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  count = true;
  Serial.println("Preparing to send SMS");
  //SendMessage();
  //Serial.begin(115200);
  //attachInterrupt(0, magnet_detect, FALLING);//Initialize the intterrupt pin (Arduino digital pin 2)
  attachInterrupt(0, magnet_detect, FALLING);//Initialize the intterrupt pin (Arduino digital pin 2)
  half_revolutions = 10;
  rpm = 100;
  timeold = 0;
  lcd.init();         // start the lcd library
  lcd.backlight();    // turn on the lcd backlight
  lcd.clear();        // clear the cld screen
  Wire.begin();
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
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  Serial.println("Initializing gsm ...");

  gsmSerial.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();
  gsmSerial.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial(); delay(100);
  gsmSerial.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  lcd.clear();
  lcd.setCursor(0,  0);
  lcd.print("hose is releasing...");  // this prints whats in between the quotes
  lcd.setCursor(0,  1);
  lcd.print("ok press when done");            // this prints the tag value
  lcd.setCursor(0,  2);
  lcd.print("for watering start ");            // this prints the tag value

  metra = EEPROM.read(metraAdd);
  if (metra == 255)metra = 6.0;
  dir = true; half_revolutions = 1;
  // timeConvert(6);// mins hour
  metra = 0.1524; //6 inches
}
void loop() {
  DateTime now = rtc.now();
  Minute = now.minute();
  Hour = now.hour();
  Second = now.second();
  //================================================================
  if (gsmSerial.available() > 0) {
    //n delay(50);
    while (1) {
      ReadUnreadMessages() ;
      if (millis() - timeNow > 10000) {
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
  //delay(1000);

  //==============================================================
  // lcd.clear();
  if (digitalRead(hall1))count = true;
  if (!digitalRead(hall1)) {
    magnet_detect();
  }
  if (!digitalRead(buttonOk)) {
    Serial.println("ok button pressed");
    delay(100);
    lcd.clear();
    while (1) {
      // if (digitalRead(hall1))count = true;
      //  if (!digitalRead(hall1))magnet_detect();
      lcd.setCursor(0,  0);
      lcd.print("set speed");  // this prints whats in between the quotes
      lcd.setCursor(0,  1);
      lcd.print("press up/down button");            // this prints the tag value
      lcd.setCursor(0,  2);
      lcd.print("meter/h adjust ");
      lcd.setCursor(15,  2);
      lcd.print(speedSet);
      if (!digitalRead(buttonUp)) {
        delay(100);
        lcd.clear();
        speedSet = speedSet + 1.0;
        if (speedSet > 80)speedSet = 80;
      }
      else if (!digitalRead(buttonDown)) {
        delay(100);
        lcd.clear();
        speedSet = speedSet - 1.0;
        if (speedSet <= 0)speedSet = 0;
      }
      else if (!digitalRead(buttonOk)) {
        EEPROM.update(speedSet, speedAdd);
        length = half_revolutions * metra;
        Serial.print("speed set is :");
        Serial.println(speedSet);
        lcd.clear();
        done = true;
        ends  = true;
        dir = false;
        setMet = false;
        break;
      }
    }
  }
  if (done) {
    DisplayPSI();// pressure and battery measurement
    /*  if (half_revolutions2 >= 20) {
        rpm = 30 * 1000 / (millis() - timeold) * half_revolutions2;
        timeold = millis();
        half_revolutions2 = 0;
        //speed = (2 * 3.142 * radius * rpm * 60) / 63360; // conversion to meter per hour speed
      }*/
    if (half_revolutions >= 0) {
      length = half_revolutions * metra;
      float timeNow = length / speed;
      //Serial.print("length ");
      //Serial.println(length);
      //Serial.print("speed ");
      //Serial.println(speed);
      //Serial.print("time in secs ");
      //Serial.println(timeNow);
      //      mNow, hNow = timeConvert(timeNow);
      //lcd.clear();
      lcd.setCursor(0,  1);
      lcd.print("m/h: ");  // this prints whats in between the quotes
      lcd.print(speed, 0);           // this prints the tag value
      lcd.print(" dis:");            // this prints the tag value
      lcd.print(half_revolutions * metra, 0);
      if (speed > speedSet ) {
        //reduce the speed of the motor
        analogWrite(pwm, 255);
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
      } else if (speed < speedSet ) {
        analogWrite(pwm, 255);
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
      } else {
        analogWrite(pwm, 0);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
      }
      // line 3
      if (ends) {
        lcd.setCursor(0,  2);
        lcd.print("end time: ");  // this prints whats in between the quotes
        //lcd.print(half_revolutions);            // this prints the tag value
        lcd.print(hNow + Hour);
        lcd.print(" : ");
        lcd.print(mNow + Minute);
        ends = false;
      }

    }

    lcd.setCursor(0,  3);
    lcd.print("T="); // this prints whats in between the quotes
    lcd.print(now.hour());        // this prints whats in between the quotes
    lcd.print(":");         // this clears the display field so anything left is deleted
    if (now.minute() < 10) Minutes = "0" + String (Minute);
    else Minutes = String (Minute);
    lcd.print(Minutes);   // this prints the tag value
    //lcd.print(":");         // this clears the display field so anything left is deleted
    if (now.second() < 10) Seconds = "0" + String (Second);
    else Seconds = String (Second);
    //lcd.print(Seconds);

    //================================================
    lcd.print(" Y="); // this prints whats in between the quotes
    lcd.print(now.day());        // this prints whats in between the quotes
    lcd.print(":");         // this clears the display field so anything left is deleted
    lcd.print(now.month());   // this prints the tag value
    lcd.print(":");         // this clears the display field so anything left is deleted
    lcd.print(now.year());

    unsigned long timeNow = millis();
    if (timeNow - prev > 1000 ) {
      prev = timeNow;
      // Serial.println("lcd cleared");
      lcd.clear();
      ends = true;
      //ReadUnreadMessages();
    }
  } else {
    // lcd.clear();
    lcd.setCursor(0,  0);
    lcd.print("hose is releasing...");  // this prints whats in between the quotes
    lcd.setCursor(0,  1);
    lcd.print("press the ok button");            // this prints the tag value
    lcd.setCursor(0,  2);
    lcd.print("for watering start ");
    lcd.setCursor(0,  3);
    lcd.print("km/h:");  // this prints whats in between the quotes
    lcd.print(speed, 0);           // this prints the tag value
    lcd.setCursor(8,  3);
    lcd.print(" len:");            // this prints the tag value
    lcd.print(half_revolutions * metra, 0);
  }
}
void updateSerial() {
  delay(1000);
  while (Serial.available()) {
    gsmSerial.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (gsmSerial.available()) {
    Serial.write(gsmSerial.read());//Forward what Software Serial received to Serial Port
    char inChar = (char)gsmSerial.read();
    inputstring += inChar; // if the incoming character is a newline, set a flag // so the main loop can do something about it:
    if (inChar == '\n') {
      inputstring.toUpperCase();
      //      FN_processData (inputstring);
      inputstring = "";
    }
  }
  inputstring = "";
}
void magnet_detect()//This function is called whenever a magnet/interrupt is detected by the arduino
{ //lcd.clear();
  if (count) {
    turnStart = millis();
    timePerTurn = turnStart - turnEnd;
    Serial.print("timer per turn is : ");
    Serial.println(timePerTurn);//msec
    speed = ((metra / (timePerTurn / 1000)) * 3.6); //per hour
    turnEnd = turnStart;
    if (dir) {
      half_revolutions++;
      half_revolutions2++;
      // lcd.setCursor(0,  3);
      //lcd.print("released (M):");            // this prints the tag value
      //lcd.setCursor(16,  3);
      //lcd.print( half_revolutions*metra);
    }
    else if (!dir) {

      half_revolutions--; half_revolutions2--;
      if (half_revolutions <= 0)half_revolutions = 0; half_revolutions2 = 0;
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
volatile unsigned long secondCount = 0;        // use volatile for shared variables

void DisplayPSI () // main display
{
  // this section monitors the live psi and turns the compressor run bit on or off based off setpoints
  int psi = analogRead(0); // this reads the analog input(A0) and scales it
  // psi = map(psi, 102, 921, 0, 150);             // this maps the raw analog input value to the converted PSI value
  psi = analogRead(A0);
  int sensorVal = analogRead(A0);
  //Serial.print("Sensor Value: ");
  //Serial.print(sensorVal);

  float voltage = (sensorVal * 5.0) / 1024.0;
  //Serial.print("Volts: ");
  //Serial.print(voltage);

  float pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  float pressure_bar = pressure_pascal / 10e5;
  // Serial.print("Pressure = ");
  //Serial.print(pressure_bar);
  // Serial.println(" bars");
  // this is the lcd section


  sensorValue = analogRead(analogInPin);// FOR THE BATTERY
  // map it to the range of the analog out:
  if (sensorValue > 540)sensorValue = 540;
  outputValue = map(sensorValue, 0, 540, 0, 13);
  //outputValue=constrain(outputValue,0,12);
  //  Serial.print("raw battery value : ");
  //  Serial.println(sensorValue);


  lcd.setCursor(0,  0);
  lcd.print("BA:");  // this prints whats in between the quotes
  lcd.print(pressure_bar);
  // lcd.print(" VOLT:");  // this prints whats in between the quotes
  //  lcd.print(outputValue);             // this prints the tag value
  lcd.print(" batt:");
  int percent = (outputValue / 12.0) * 100.0;
  if (percent != newp)lcd.clear(); newp = percent;
  lcd.print(percent);
  //Serial.println(percent);
  lcd.print("%");
}
// this section stores the setpoints into the arduino eeprom so the unit can start up after a power fail
void storeAll() {
  lcd.clear();
  // this is the lcd section
  lcd.setCursor(0, 0); lcd.print("SAVING IN");
  lcd.setCursor(0, 1); lcd.print("PROGRESS...");
  delay(2000);
}

float timeConvert(int time) {
  int  minsConv;
  if (time < 60) {
    minsConv = time;
  } else {
    minsConv = time % 60;
  }
  int hourConv = time / 60;
  // Serial.print(hourConv);
  // Serial.print(" : ");
  // Serial.println(minsConv);
  return minsConv, hourConv;
}

void ReadUnreadMessages() {
  while (gsmSerial.available() > 0) {
    char inByte = gsmSerial.read();
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

  if (Serial.available()) {
    int inByte = Serial.read();
    gsmSerial.write(inByte);
  }
}
void SendMessage() {

  Serial.println("Setting the GSM in text mode");
  gsmSerial.println("AT+CMGF=1\r");
  delay(1000);
  Serial.println("Sending SMS to the desired phone number!");
  gsmSerial.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(1000);

  gsmSerial.println(" watering machine has been booted");    // SMS Text
  delay(200);
  gsmSerial.println((char)26);               // ASCII code of CTRL+Z
  delay(1000);
  gsmSerial.println();
  Serial.println("sent the text booted from SIM800");    // SMS Text
  gsmSerial.println("AT");
  delay(1000);
  Serial.println("Connecting...");
  Serial.println("Connected!");
  Serial.println("Setting the GSM in text mode");
  gsmSerial.println("AT+CMGF=1\r");
  delay(1000);

  gsmSerial.println("AT+CMGF=1");  //Set SMS to Text Mode
  delay(1000);
  gsmSerial.println("AT+CNMI=1,2,0,0,0");  //Procedure to handle newly arrived messages(command name in text: new message indications to TE)
  delay(1000);  Serial.println("Fetching list of received unread SMSes!");
  //  gsmSerial.println("AT+CMGL=\"REC UNREAD\"\r");
  delay(1000);
}
void processData() {
  if (inputString.indexOf("ER#") > -1) {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    //  Serial.println(inputString.indexOf("#"));PROGNUMBER#X#1234567890#
    int num = inputString.indexOf("#");
    String id = (inputString.substring( num + 1, num + 2));
    Serial.print("phone id : ");
    Serial.println(id);
    if (id == "1") {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring( num + 3));
      phoneNum = inputString.substring( num + 3);
      str1AddrOffset = writeStringToEEPROM(eepromOffset, phoneNum);
    }
    else if (id == "2") {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring( num + 3));
      phoneNum = inputString.substring( num + 3);
      str2AddrOffset = writeStringToEEPROM(str1AddrOffset, phoneNum);
      //  writeStringToEEPROM(str2AddrOffset, str3);
    } else  if (id == "3") {
      Serial.print("new phone number set to :");
      Serial.println(inputString.substring( num + 3));
      phoneNum = inputString.substring( num + 3);
      str3AddrOffset = writeStringToEEPROM(str2AddrOffset, phoneNum);
      //  writeStringToEEPROM(str2AddrOffset, str3);
    }
    inputString = "";
  } if (inputString.indexOf("PROGSTEP#") > -1) {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    //  Serial.println(inputString.indexOf("#"));PROGSTEP#1234567890#
    int num = inputString.indexOf("#");
    Serial.print("program step number  set to :");
    Serial.println(inputString.substring( num + 1));
    progstep = inputString.substring( num + 1);
    str4AddrOffset = writeStringToEEPROM(str3AddrOffset, progstep);
    inputString = "";
  }
  if (inputString.indexOf("MTR#") > -1) {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    //Serial.println(inputString.indexOf("#"))MTR#134.0#
    int num = inputString.indexOf("#");
    Serial.print("THE meter  number  set to :");
    Serial.println(inputString.substring( num + 1));
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
void readMem() {
  newStr1AddrOffset = readStringFromEEPROM(eepromOffset, &phoneNum);
  newStr4AddrOffset = readStringFromEEPROM(newStr3AddrOffset, &progstep);
  readStringFromEEPROM(newStr4AddrOffset, &wheelDia);
}
