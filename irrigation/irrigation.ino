#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4
#include "RTClib.h"
#include <EEPROM.h>
#include "dht.h"
#include <LayadCircuits_SalengGSM.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
SoftwareSerial gsmSerial(19, 18);
LayadCircuits_SalengGSM salengGSM = LayadCircuits_SalengGSM(&Serial1);
#define DHT22_PIN 11 // DHT 22  (AM2302) - what pin we're connected to
dht DHT;
// dht1wire DHT(DHT22_PIN, dht::DHT22);
//  Constants
#define MAX_LENGTH 50

void writeToEEPROM(int address, const String &data);
String readFromEEPROM(int address);
int address = 0;

// Variables
float hum;  // Stores humidity value
float temp; // Stores temperature value
bool releasing = true;

int newSpeed = 15;
float vOUT = 0.0;
float vIN = 0.0;
float R1 = 30000.0;
float R2 = 7500.0;
int value = 0;
float timeLeft = 0;
const int RecordTime = 2; // Define Measuring Time (Seconds)
const int SensorPin = 3;  // Define Interrupt Pin (2 or 3 @ Arduino Uno)
bool almostDone = false;
int InterruptCounter;
float WindSpeed;
bool wind, voltage = true;
unsigned long timeNow;

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
float setSpeed = 123.00;
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
int metraAdd = 51; // address for the meter variable
int speedAdd = 50; // address for the meter variable
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
float pressure_bar = 0;
int percent = 0;
char ps[30];
int winding = 0;
float wheel = 0.95;
float collectWheel = 0.81;
int speedCounter = 0;
bool speedFlag = true;
unsigned long monitorStopage = 0;
bool stopped = true;
void setup()
{
  Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
  delay(1000);
  Serial.println("Preparing to read received SMSes");
  Serial1.begin(9600);   // Setting the baud rate of GSM Module
  salengGSM.begin(9600); // this is the default baud rate
  Serial.begin(9600);
  Serial.print(F("Preparing Saleng GSM Shield.Pls wait for 10 seconds..."));
  delay(5000); // allow 10 seconds for modem to boot up and register
  salengGSM.initSalengGSM();
  Serial.println(F("Done"));
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
  attachInterrupt(0, speedInt, RISING); // Initialize the intterrupt pin (Arduino digital pin 2)
  pinMode(IR1, INPUT_PULLUP);
  // attachInterrupt(0, reads, RISING);
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
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // January 21, 2014 at 3am you would call:
  // rtc.adjust(DateTime(2023, 5, 15, 23, 30, 0));
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
  {
    metra = 6.0;
  }
  else
  {
    metra = EEPROM.read(metraAdd);
    setSpeed = EEPROM.read(speedAdd);
  }

  dir = true;
  half_revolutions = 0;
  Serial.print("meter in use from last memory");
  Serial.println(metra);
  // timeConvert(6);// mins hour
  // 6 inches
  metra = 0.95;
  lcd.backlight();
}

void loop()
{
  DateTime now = rtc.now();
  Minute = now.minute();
  Hour = now.hour();
  Second = now.second();
  salengGSM.smsMachine();         // we need to pass here as fast as we can. this allows for non-blocking SMS transmission
  if (salengGSM.isSMSavailable()) // we also need to pass here as frequent as possible to check for incoming messages
  {
    salengGSM.readSMS(); // updates the read flag
    Serial.print("Sender=");
    Serial.println(salengGSM.smsSender);
    Serial.print("Whole Message=");
    Serial.println(salengGSM.smsRxMsg); // if we receive an SMS, print the contents of the receive buffer
    String receivedMessage = salengGSM.smsRxMsg;
    Serial.print("Whole Message in string =");
    Serial.println(receivedMessage);
    if (receivedMessage.indexOf("info") > 0)
    {
      Serial.println("requesting machine information info");
      infoMessage(salengGSM.smsSender);
    }
    processData(receivedMessage);
  }
  // getSpeed();
  // getSpeeding(); // this controls the motor retraction

  if (getVoltage() < 9) // should be 11 .5
  {
    if (voltage)
    {
      errorVoltage();
      voltage = false;
    }
  }
  else if (getVoltage() > 10)
  {
    voltage = true;
  }

  if (!digitalRead(buttonOk))
  {
    Serial.println("ok button pressed");
    lcd.backlight();
    delay(1000);
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
      delay(500);
      if (!digitalRead(buttonUp))
      {
        delay(100);
        lcd.clear();
        speedSet = speedSet + 10.0;
        if (speedSet > 800)
          speedSet = 800;
      }
      else if (!digitalRead(buttonDown))
      {
        delay(100);
        lcd.clear();
        speedSet = speedSet - 10.0;
        if (speedSet <= 0)
          speedSet = 0;
      }
      else if (!digitalRead(buttonOk))
      {
        // EEPROM.update(speedSet, speedAdd);
        length = half_revolutions * metra;
        Serial.print("ok button pressed, speed set is :");
        Serial.println(speedSet);
        setSpeed = speedSet;
        lcd.clear();
        // lcd.noBacklight();
        prev = millis();
        done = true;
        ends = true;
        dir = false;
        setMet = false;
        releasing = false;
        almostDone = true;
        break;
      }
    }
  }
  if (done)
  {
    wheel = collectWheel;
    DisplayPSI(); // pressure and battery measurement
    lcd.setCursor(0, 1);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("dar:");      // this prints whats in between the quotes
    lcd.print(getPsi(), 1); // this prints whats in between the quotes
    lcd.print(" wi:");      // this clears the display field so anything left is deleted
    lcd.print(winding);
    lcd.print(" ");
    if (now.hour() < 10)
    {
      lcd.print("0" + String(now.hour())); // this prints whats in between the quotes
    }
    else
    {
      lcd.print(now.hour());
    }
    lcd.print(":");
    Minute = now.minute();
    Minutes = String(Minute);
    if (Minute < 10)
      Minutes = "0" + String(Minute);
    lcd.print(Minutes);

    lcd.setCursor(0, 1);
    lcd.print("volt:");
    lcd.print(getVoltage(), 0);
    lcd.print(" watt:");
    int percent = (outputValue / 12.0) * 100.0;
    if (percent != newp)
      newp = percent;
    lcd.print(percent);
    lcd.print("%");
    // reads();
    lcd.setCursor(0, 2);
    lcd.print("dist:");
    lcd.print(int(half_revolutions * wheel));
    lcd.print(" m/h.");
    lcd.print(int(velocity));
    lcd.print(" H:");
    lcd.print(getHum());

    int simSpeed = map(analogRead(A2), 0, 1024, 10, 200);
    simSpeed = map(simSpeed, 60, 90, 10, 200);
    controlMotor(simSpeed);

    currentDistance = half_revolutions * wheel;
    timeLeft = currentDistance / (velocity);
    lcd.setCursor(0, 3);
    lcd.print("Time(min):");
    lcd.print(int(timeLeft));
    lcd.print(" C:");
    lcd.print(getTemp());
    Serial.print("Time left(min):");
    Serial.println(int(timeLeft));
    Serial.print("current distance :");
    Serial.println(int(currentDistance));
    // currentDistance = half_revolutions * metra;
    //  total_len = total_len * metra;
    //  float gets = getSpeed();
    monitorStopage++;
    if (currentDistance < total_len && monitorStopage > 100)
    {
      if (stopped)
      {
        sendStopSms();
        stopped = false;
      }
    }

    if (half_revolutions <= 0)
    {
      if (stopWatering)
      {
        errorStopWatering();
        stopWatering = false;
      }
    }
    // delay(1000);
    (getWind());
    unsigned long timeNow = millis();
    if (!digitalRead(buttonUp) || !digitalRead(buttonDown))
    {
      lcd.backlight();
      prev = millis();
    }
    if (timeNow - prev > 10000)
    {
      prev = timeNow;
      // Serial.println("lcd cleared");
      // lcd.clear();
      lcd.noBacklight();
      ends = true;
      // ReadUnreadMessages();
    }
  }
  else
  {
    // lcd.clear();
    // reads();
    lcd.setCursor(0, 0);
    lcd.print("hose is releasing..."); // this prints whats in between the quotes
    lcd.setCursor(0, 1);
    lcd.print("press the ok button"); // this prints the tag value
    lcd.setCursor(0, 2);
    lcd.print("for watering start ");
    lcd.setCursor(0, 3);
    lcd.print("km/h:");       // this prints whats in between the quotes
    lcd.print((velocity), 1); // this prints the tag value
    lcd.setCursor(8, 3);
    lcd.print(" hall : "); // this prints the tag value
    lcd.print((half_revolutions * wheel), 1);
    // Serial.print("revolutions in loop");
    // Serial.println(half_revolutions);
    // Serial.print("velocity ::: ");
    // Serial.println(velocity);
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
  pressure_bar = pressure_pascal / 10e5;
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
  /*lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("BA:"); // this prints whats in between the quotes
   lcd.print(pressure_bar);
   // lcd.print(" VOLT:");  // this prints whats in between the quotes
   //  lcd.print(outputValue);             // this prints the tag value
   lcd.print("  batt:");*/
  percent = (outputValue / 12.5) * 100.0;
  if (percent != newp)
    // lcd.clear();
    newp = percent;
  // lcd.print(percent);
  //  Serial.println(percent);
  // lcd.print("%");
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
    inputString += inByte; // Serial1.readStringUntil('\n');//
  }
  Serial.print("recived message : ");
  Serial.println(inputString);
  inputString.toUpperCase(); // Uppercase the Received Message
  // read from port 0, send to port 1:
  //  Serial.print("received data in caps ");
  // Serial.println(inputString);

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

void processData(String inputString)
{
  bool found;
  // Convert the String objects to character arrays
  char *sentenceChars = strdup(inputString.c_str());
  String searchWord = "PROGNUBER";
  char *searchWordChars = strdup(searchWord.c_str());

  if (strstr(sentenceChars, searchWordChars) != NULL)
  {
    Serial.println("The word is found!");
    found = true;
  }
  else
  {
    Serial.println("The word is not found!");
    found = false;
  }

  Serial.println(inputString.indexOf("PROGNUBER"));
  if (found)
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
      Serial.print("new phone number 1 set to : ");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      writeToEEPROM(0, phoneNum);
      //  str1AddrOffset = writeStringToEEPROM(eepromOffset, phoneNum);
    }
    else if (id == "2")
    {
      Serial.print("new phone number 2 set to :");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      writeToEEPROM(20, phoneNum);
      // str2AddrOffset = writeStringToEEPROM(str1AddrOffset, phoneNum);
      //  writeStringToEEPROM(str2AddrOffset, str3);
    }
    else if (id == "3")
    {
      Serial.print("new phone number 3 set to :");
      Serial.println(inputString.substring(num + 3));
      phoneNum = inputString.substring(num + 3);
      writeToEEPROM(40, phoneNum);
      // str3AddrOffset = writeStringToEEPROM(str2AddrOffset, phoneNum);
      //   writeStringToEEPROM(str2AddrOffset, str3);
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
    Serial.print("program speed number  set to :");
    Serial.println(inputString.substring(num + 1));
    progstep = inputString.substring(num + 1);
    // str4AddrOffset = writeStringToEEPROM(str3AddrOffset, progstep);
    // EEPROM.write(speedAdd, progstep.toInt());
    setSpeed = progstep.toInt();
    newSpeed = setSpeed;
    metra = newSpeed;
    Serial.print("program speed number  set to :");
    Serial.println(setSpeed);

    inputString = "";
  }
  if (inputString.indexOf("MTR#") > -1)
  {
    Serial.print("received : ");
    Serial.println(inputString);
    // inputString.toUpperCase(); // Uppercase the Received Message
    // Serial.println(inputString.indexOf("#"))MTR#134.0#
    int num = inputString.indexOf("#");
    // Serial.print("THE meter wheel number  set to :");
    // Serial.println(inputString.substring(num + 1));
    wheelDia = inputString.substring(num + 1);
    collectWheel = wheelDia.toInt();
    Serial.print("THE meter wheel number  set to :");
    Serial.println(collectWheel);
    // metra = wheelDia.toInt() / 1000;
    EEPROM.write(metraAdd, collectWheel);
    inputString = "";
  }
  Serial.print("phone number in  use : ");
  Serial.println(phoneNum);

  Serial.print("prog step in  use : ");
  Serial.println(metra);

  Serial.print("speed in  use : ");
  Serial.println(setSpeed);
  found = false;
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
  delay(500 * RecordTime);
  detachInterrupt(digitalPinToInterrupt(SensorPin));
  WindSpeed = (float)InterruptCounter / (float)RecordTime * 2.4;
  Serial.print("wind speed: ");
  Serial.println(WindSpeed);
  winding = WindSpeed;
  if (WindSpeed > 20)
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

void speedInt()
{
  Serial.println("hall sensor active");
  monitorStopage = 0;
  speedCounter++;
  if (speedCounter > 4)
  {
    speedFlag = !speedFlag;
    getSpeed();
    speedCounter = 0;
  }
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
  Serial.print("Pressure = ");
  Serial.println(pressure_bar);

  dtostrf(pressure_bar, 3, 1, ps);
  return pressure_bar;
}

String getTime()
{
  char buf1[20];
  DateTime now = rtc.now();
  sprintf(buf1, "%02d:%02d:%02d", now.hour(), now.minute(), now.second());
  // Serial.print(F("Date/Time: "));
  // Serial.println(buf1);
  return buf1;
}

String getDate()
{
  char buf1[20];
  DateTime now = rtc.now();
  sprintf(buf1, " %02d/%02d/%02d", now.day(), now.month(), now.year());
  // Serial.print(F("Date/Time: "));
  // Serial.println(buf1);
  return buf1;
}
// reteuns current velocity
float getSpeed()
{
  if (done)
  {
    half_revolutions--;
    stopped = true;
  }

  if (!done)
  {
    half_revolutions++;
    total_len = half_revolutions * metra;
  }
  if (!speedFlag)
  {
    Serial.println(speedCounter);
    t2 = millis();
    Serial.print("t2 ");
    Serial.println(t2);
    read = true;
    done2 = true;
    count1 = true;
    count2 = false;
    speedCounter = 0;
    // reads();
  }

  if (speedFlag)
  {
    speedCounter++;
    t1 = millis();
    Serial.print("t1 ");
    Serial.println(t1);
    done2 = true;
    count1 = false;
    count2 = true;
    // reads();
  }

  if (read)
  {
    velocity = t2 - t1;
    velocity = velocity / 1000;                 // convert millisecond to second for timig
    velocity = (wheel / velocity) * 3.6 * 1000; // m/h

    Serial.print("time differnce: ");
    Serial.println(t2 - t1);
    speeding = velocity;
    // Serial.println(" m/hr");
    currentDistance = half_revolutions * metra;
    float wateringTimeNow = wateringEnd(currentDistance, velocity); // meter/hr
    // float totalWateringTime = wateringEnd(total_len * metra, getSpeed());         // mph
    timeLeft = currentDistance / velocity;

    Serial.print("speed ");
    Serial.println(velocity); // hours
    Serial.print("current len : ");
    Serial.println(currentDistance);
    Serial.print("time left for watering in minutes: ");
    timeLeft = timeLeft * 60;
    Serial.println(timeLeft);

    if (timeLeft > 300) // 30 mites, should be less than 30 meyers < 30 npt > 300
    {
      if (almostDone)
      {
        sendAlmostDone();
        almostDone = false;
      }
    }
    else if (timeLeft > 1)
    {
      // almostDone = true;
    }
    delay(500);
    read = false;
  }
  else
  {
    // velocity = 0;
  }
  return velocity;
}

void controlMotor(float speed)
{
  if (speed > 0)
  {
    Serial.print("machine speed is: ");
    Serial.println(speed);
    Serial.print("machine set speed is: ");
    Serial.println(setSpeed);
    if (speed > setSpeed + 40.00)
    {
      // delay(5000);
      Serial.println("moving forward, more speed");
      analogWrite(pwm, 255);
      digitalWrite(in2, HIGH);
      digitalWrite(in1, LOW);
    }
    else if (speed < setSpeed - 30.00)
    {
      Serial.println("moving backward, less speed");
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
  Serial1.println(" wind is greater than 20 Km/h"); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void sendAlmostDone()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" 30 minutes left to the ending of the watering "); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void sendStopSms()
{
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(500);
  Serial1.println(" wheel has stopped, watering isnt over yet, fault detected "); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}

void infoMessage(String number)
{
  Serial.print("number :");
  Serial.println(number);
  Serial.print("number numbers:");
  Serial.println(readFromEEPROM(0));
  delay(500);
  Serial.print("number numbers2:");
  Serial.println(readFromEEPROM(20));
  delay(500);
  Serial.print("number numbers3:");
  Serial.println(readFromEEPROM(40));
  String command = "";

  if (number == readFromEEPROM(0))
  {
    Serial.print("number in use  number 1:");
    Serial.println(readFromEEPROM(0));
    command = "AT+CMGS=\"" + readFromEEPROM(0) + "\"\r";
  }
  else if (number == readFromEEPROM(20))
  {
    Serial.print("number in use  number 2:");
    Serial.println(readFromEEPROM(20));
    command = "AT+CMGS=\"" + readFromEEPROM(20) + "\"\r";
  }
  else if (number == readFromEEPROM(40))
  {
    Serial.print("number in use  number 3:");
    Serial.println(readFromEEPROM(40));
    command = "AT+CMGS=\"" + readFromEEPROM(40) + "\"\r";
  }
  Serial.print("commnad length: ");
  Serial.println(command.length());
  Serial.print("commnad sent: ");
  Serial.println(command);
  if (command.length() > 0)
  {
    String data = "current distance: " + String(currentDistance) + "\nTime left: " + String(timeLeft) + "\ncollection m/h: " + String(velocity) + "\n bars: " + String(ps);
    data += "\n volt: " + String(int(getVoltage())) + "\n watt: " + String(int((getVoltage() / 13) * 100)) + " %" + "\n time: " + getTime() + "\n date: " + getDate() + "\n hygro: " + String(getHum()) + "\n celsius: " + String(getTemp());
    data += "\n wind Km/h: " + String(winding);
    Serial.println("Setting the GSM in text mode");
    Serial1.println("AT+CMGF=1\r");
    delay(200);
    Serial.println("Sending SMS to the desired phone number!");
    // Serial1.println("AT+CMGS=\"+306973991989\"\r");
    Serial1.println(command);
    // Replace x with mobile number
    delay(500);
    Serial1.println(data); // SMS Text
    delay(200);
    Serial1.println((char)26); // ASCII code of CTRL+Z
    delay(1000);
    Serial1.println();
    Serial1.println("AT");
    delay(200);
    Serial1.println("AT+CMGF=1\r");
  }
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
      {
        half_revolutions++;
        total_len = half_revolutions * metra;
      }
      lastMillis = millis();
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
  if (half_revolutions <= 0)
    half_revolutions = 0;
}

float wateringEnd(float distance, float speed)
{
  endTime = distance / speed;
  return endTime;
}

void writeToEEPROM(int address, const String &data)
{
  int length = data.length();
  for (int i = 0; i < length; i++)
  {
    EEPROM.write(address + i, data[i]);
  }
  EEPROM.write(address + length, '\0');
}

String readFromEEPROM(int address)
{
  String data;
  char character = EEPROM.read(address);
  int i = 0;

  while (character != '\0' && i < MAX_LENGTH)
  {
    data += character;
    address++;
    character = EEPROM.read(address);
    i++;
  }

  return data;
}

// version 10.12
