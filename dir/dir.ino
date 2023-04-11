#include <Wire.h>                   // Wire library
#include "RTClib.h"
//#include <LiquidCrystal.h>      // Liquid Crystal I2C library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
//LiquidCrystal lcd(7, 6, 5, 4, 3, 2); // Display address 0x27, I2C 20 x 4
LiquidCrystal_I2C lcd(0x27, 20, 4);
#include <EEPROM.h>
//#include <EepromUtils.h>
#include <SimpleTimer.h>
//#include <Event.h>
#define gsmdelay 1800
#define gsm_delay_small 100
#define EEPROM_MAX_ADDR 1023
#define BUFFER_SIZE 11
#define pinSensorHalfA 10
#define pinSensorHalfB 11
#define buttonPin1 2
#define buttonPin2 3
int trueStepValueVariable = 10;
// macros from DateTime.h
/* Useful Constants */
#define SECS_PER_MIN (68UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY (SECS_PER_HOUR * 24L) /* Useful Macros for getting elapsed time */
#define numberOfSeconds (__TIME__) (__TIME__ % SECS_PER_MIN)
#define numberOfMinutes (__TIME__) ((__TIME__ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours (__TIME__) (( __TIME__% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays (__TIME__) ( __TIME__ / SECS_PER_DAY)
//SoftwareSerial gsmSerial (8, 9); //rx-4 tx-5 33 34 LiquidCrystal lcd(7, 6, 5, 4, 3, 2); 35
SimpleTimer timer;
//EepromUtil eepromUtil;

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
int rpm = -1;
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
byte modeReadButtons = 1;
String inputstring = "";
String smsSender;
String textMessage = "welcome starting...";
String authorizedNumbers[3];
bool isclockwise;
bool isstartAgainRpmCount = 0;
RTC_DS3231 rtc;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);
  Serial1.begin (9600);
  delay(10);
  lcd.init(); // initialize the lcd
  adminNumberSize = sizeof(adminNumbers) / sizeof(char *);
  gsminitsize = sizeof(gsmInit) / sizeof(char *);
  lcd.noBacklight();
  FN_printLcd ("START", 0, 0, 0);
  delay(1500);
  lcd.clear();
  FN_printLcd("re", 0, 0, 0);
  pinMode(buttonPin1, INPUT_PULLUP);
  pinMode (buttonPin2, INPUT_PULLUP);
  delay(10);
  // deleteALLSMS();
  Serial.println("Reading eeprom"); readEeprom();
  delay(300);

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
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // This line sets the RTC with an explicit date & time, for example to set
  lcd.backlight();    // turn on the lcd backlight
  sendSmsMessage();
}

void loop() {
  // put your main code here, to run repeatedly:
  //
 // timer.run();
  FN_rpmCounter();
  DisplayPSI ();
  readSim800();
}
void FN_rpmCounter() {
  static boolean isClockwise = false;
  static boolean readCounter = false;
  byte sensorHalfA = digitalRead(pinSensorHalfA);
  byte sensorHalfB = digitalRead(pinSensorHalfB);
  if (sensorHalfA == LOW && sensorHalfB == HIGH) {
    //anoigma karpylloy
    isclockwise = true;
    readCounter = true;
  }
  else if (sensorHalfA == HIGH && sensorHalfB == LOW) { //klelsimo kardullby
    isclockwise = false;
    readCounter = true;
  }
  if (sensorHalfA == LOW && sensorHalfB == LOW && readCounter) {
    readCounter = false;
    if (isclockwise) {
      rpm++;
      FN_printLcd("", rpm, 0, 0);
    }
    else if (rpm > 0) {
      rpm--;
      counder2Rpm--; // flag netrhth
      stopTimer = millis(); //hdenismos voithitikom flag
      if (rpm < 5 && countCloseRpm > 15) {
        FN_resetFlags();
      }
      countCloseRpm++; // metrhths strofes kleisinatos
      isstartAgainRpmCount++;
      FN_printLcd("r",rpm, 0, 0);
      FN_calcTime();
      if (isstartAgain && isstartAgainRpmCount > 4) {
        FN_startAgain();
      }
    }
  }
}
void readEeprom() {
  char buf[BUFFER_SIZE];
  for (int i = 0; i < 3; i++) {
    // erase buffer
    memset(buf, 0, sizeof(buf));
    // eepromUtil.eeprom_read_string (i * BUFFER_SIZE, buf, BUFFER_SIZE);
    String phoneNumber = (String) buf; if (phoneNumber.startsWith("6")) {
      Serial.println("\nAdding Admin Number: ");
      Serial.print(phoneNumber); Serial.print("\n");
      authorizedNumbers [i] = phoneNumber;
    }
  } //erease buffer
  memset(buf, 0, sizeof(buf)); //read step
  //eepromUtil.eeprom _read_string(100, buf, BUFFER_SIZE);
  stepValue = atof(buf); String stepDataString = (String) buf;
  Serial.println("\nReading step: ");
  Serial.print (stepValue);
  Serial.println("\nInitializing GSM Modem");
  //timer.setTimer (gsmdelay, initGsmModem, gsmInitSize + 1);
}

void initGsmModem () {
  static int gsmInitStep = 0;
  if (gsmInitStep < gsminitsize) {
    Serial.println("\nSending command: ");
    Serial.print(gsmInit[gsmInitStep]);
    Serial1.println(gsmInit[gsmInitStep]);
    gsmInitStep++;
  }
  else {
    Serial.println("\nGSM Modem Initialization Completed");
   // timer.setInterval(signalRequestTime, checkGSMSignal);
  }
}

void checkGSMSignal() {
  if (!isSendingSms && !isDialing)
    Serial1.println("AT+CSQ");
}
void readSim800() {
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(500);
  Serial.println("Fetching list of received unread SMSes!");
  Serial1.println("AT+CMGL=\"REC UNREAD\"\r");
  delay(500);
  // Print the response on the Serial Monitor
  //while (Serial1.available() > 0) {
  //  Serial.write(Serial1.read());
  // }
  while (Serial1.available()) { // get the new byte:
    char inChar = (char)Serial1.read(); // add it to the inputString:
    inputstring += inChar; // if the incoming character is a newline, set a flag // so the main loop can do something about it:
    if (inChar == '\n') {
      inputstring.toUpperCase();
      FN_processData (inputstring);
      inputstring = "";
    }
  }
}
void FN_processData (String data) {
  Serial.println (data);
  if (data.indexOf("+CSQ:") >= 0) {
    FN_parseSMS(data);
  } else if (data.indexOf("+CMTI: ") >= 0) {
    SMS_location_number = data.substring (data.indexOf(",") + 1).toInt ();
    Serial1.print("AT+CMGR=");
    Serial1.println(SMS_location_number);
  } else if (data.indexOf("+CLIP:") >= 0) {
    int commaIndex = data.indexOf(","); String smsNumber = data.substring(data.indexOf(":") + 3, commaIndex - 1);
    Serial.println("Calling Number: ");
    Serial.println(smsNumber);
    isAuthorized = false;
    for (int i = 0; i < 3; i++) {
      if (smsNumber.indexOf (authorizedNumbers[i]) >= 0) {
        isAuthorized = true;
        smsSender = String (authorizedNumbers[i]);
        break;
      }
    } //hang up
    // timer.setTimeout(gsmDelay, FN_hangUp);
    if (isAuthorized) {
      //    timer.setTimeout(gsmDelay 2, FN_sms_InfoCall);
    }
  } else if (data.indexOf("+CMGR:") >= 0) {
    int commaIndex = data.indexOf(",");
    int secondCommaIndex = data.indexOf(",", commaIndex + 1);
    String smsNumber = data.substring (commaIndex + 2, secondCommaIndex - 1);
    isAuthorized = false;
    for (int i = 0; i < adminNumberSize; i++) {
      if (smsNumber.indexOf (adminNumbers[i]) >= 0) {
        isAuthorized = true;
        smsSender = String (adminNumbers[i]);
        break;
      }
    } if (!isAuthorized) { //parse notification Numbers array
      for (int i = 0; i < 3; i++) {
        if (smsNumber.indexOf (authorizedNumbers [1]) >= 0) {
          isAuthorized = true;
          smsSender = String (authorizedNumbers [i]);
          break;
        }
      }
    }
    Serial.println ("authorized ?");
    Serial.println (isAuthorized);
    isSMS = isAuthorized;
  } else if (isSMS) {
    FN_parseSMS (data); isSMS = false; //delete last SMS //
    //timer.setTimeout(gsmDelay, deleteLastSMS);
  }
}

void FN_parseSMS(String data) {
  if (data.startsWith("PROGNUMBER")) {
    //programming /*
    /*boolean isSmsValid = FN_checkSms (data, true, true); if(isSmsValid) { return};*/
    Serial.println ("programming....");
    int commaIndex = data.indexOf("#");
    int secondCommaIndex = data.indexOf("#", commaIndex + 1);
    int endCommaIndex = data.indexOf("#", secondCommaIndex + 1);
    String cellNumber = data.substring(secondCommaIndex + 1, endCommaIndex);
    cellNumber.trim();
    int mempos = data.substring(commaIndex + 1, secondCommaIndex).toInt();
    char buf[BUFFER_SIZE]; cellNumber.toCharArray (buf, BUFFER_SIZE);
    delay (1000);
    //eepromUtil.eeprom_write string((memPos - 1)*BUFFER_SIZE, buf);
    authorizedNumbers [mempos - 1] = cellNumber; //String message "Programmed Auth Number: "+String(cellNumber)+" in posit
    //FN_smsInfo(message, true);
    FN_printLcd ("prog ok", 0, 0, 0);
  }
  else if (data.startsWith("PROGSTEP")) //progstep
  { /*boolean is5msValid FN_checkSms (data, false, false); if(!isSmsValid) return;/ //programming step*/
    int commaIndex = data.indexOf("#");
    String stepDataString = data.substring(commaIndex + 1);
    char buf[stepDataString.length()];
    stepDataString.toCharArray (buf, stepDataString.length());
    // stepValue = atof(buf); eepromUtil. eeprom_write_string (100, buf);
    delay(100); Serial.println ("Programmed step: "); Serial.print (stepValue);
    //String message = "Programmed step: "+String(stepDataString); //FN_smsInfo(message, true);
    FN_printLcd ("prog step ok", 0, 0, 0);
  }
  else if (data.startsWith("MTR")) {
    int commaIndex = data.indexOf("#");
    int secondCommaIndex = data.indexOf("#", commaIndex + 1);
    smsMetra = data.substring(commaIndex + 1, secondCommaIndex).toInt();
    rpm = smsMetra / stepValue;
    FN_printLcd ("&", rpm, 0, 0);
    Serial.println (smsMetra);
    Serial.println(rpm);
  }
}

void FN_stopRpm() {
  smsSender = authorizedNumbers [1];
  String message ("**Stop**");
  FN_printLcd("//", 0, 12, 1);
  //trueStepValue = 0;

  Serial.println("stop");
  h = 0;
  m = 0;
  stepPerHour = 0;
  isStop = true;
  //1sLockStepManual - false;
  isstartAgain = true;
  isstartAgainRpmCount = 0;
  FN_smsInfo(message, true);
}

void Fn_Timers() {
  if ( (millis() - stopTimer > 75008) && (rpm > 10 && countCloseRpm >= 4 && isStop == false)) {
    Serial.println("Stop enable");
    FN_stopRpm();

  }
}
void FN_startAgain() {
  smsSender = authorizedNumbers [1];
  isstartAgain = false;
  isStop = false;
  Serial.println("startAgain");
  FN_printLcd ("11", 0, 12, 1);
  isstartAgainRpmCount = 0;
  FN_smsInfoCall();
}

void FN_smsInfoCall() {
  String message = ("info");
  isstartAgainRpmCount = 0;
  FN_smsInfo(message, false);
}
void sendSms (String messageText) {
  isSendingSms = true;
  textMessage = messageText;
  Serial1.println("AT+CMGF=1\r");
  //  timer.setTimeout(gsmDelay, sendSms AtCommand);
}
void sendSmsAtCommand() {
  Serial1.println ("AT+CMGS=\"+ smsSender +""\",145\n");
  // timer.setTimeout(gsmDelay, sendSmsMessage);
}
void sendSmsMessage() {
  Serial1.print(textMessage);
  Serial1.print ("\x1A");
  isSendingSms = false;
}
void FN_smsInfo(String txtInfo, boolean onlyText) {
  if (onlyText) {
    sendSms (txtInfo);
  } else {
    String message = "m";
    message += meter;
    message += "\ns";
    message += stepPerHour;
    message += "/";
    message += trueStepValue;
    message += "\n";
    message += int(h);
    if (m < 10) {
      message += ":0";
    }
    else {
      message += ":";
    }
    message + int (m);
    Serial.print(message);
    sendSms (message);
  }
}

void FN_printLcd(String LcdText, unsigned long lcdval, int column, int line) {
  lcd.setCursor(column, line);
  lcd.print (LcdText);
  if (lcdval != 0)
  { lcd.print (lcdval);
    if ((lcdval < 18 ) || (lcdval < 100 && lcdval > 90)) {
      lcd.print(" ");
    }
  }
}
void FN_calcTime() {
  static unsigned long startTimer = 0;
  static unsigned long rpmTime = 0;
  if (counder2Rpm == 1) {
    startTimer = millis();
    FN_printLcd ("R", rpm, 0, 0);
  }
  else if (counder2Rpm == 0) {
    rpmTime = millis() - startTimer;
    elapsedTimeAll = rpmTime * rpm; //xronos gia na fta
    stepPerHour = (3600080 * stepValue) / rpmTime; //st
    FN_printLcd ("v", stepPerHour, 9, 0);
    counder2Rpm = 2;
  }
  meter = rpm * stepValue; //metra gia na ftasei (glc
  FN_printLcd ("m", meter, 5, 0); //lcdprint
  if (isUnder_30Min == false && m < 29 && h == 0 && isstartAgainRpmCount > 1 && countCloseRpm > 30) {
    isUnder_30Min = true;
    smsSender = authorizedNumbers[1];
    String message = ("*30 min*");
    FN_smsInfo(message, true);
    FN_printLcd ("<3", 0, 12, 1);
    isstartAgainRpmCount = 0;
  }
  if (countCloseRpm == 35 && isLockStepManual == false) { //
    FN_calcTrueStep();
    FN_printLcd ("k", trueStepValue, 7, 1);
  }
  // metatrepei se wres kai lepta toy timer 1000;
  DateTime now = rtc.now();
  s = now.second();
  m = now.minute();
  h = now.hour();
  unsigned long elapsedTime1000 = elapsedTimeAll / 1000;
  // h = numberOfHours (elapsedTime1000);
  // m = numberOfMinutes (elapsedTime1000);
  // s = numberOfSeconds (elapsedTime1000);

  FN_printLcd ("t", h, 0, 1);
  FN_printLcd(":", m, 3, 1);
  if (h < 1) {
    FN_printLcd("e", 0, 1, 1);
  } // elegxos +- % step
  if (isMinMaxCalc && isstartAgainRpmCount > 10) {
    FN_calcTrueStep();
  }
}
void FN_calcTrueStep() {
  unsigned long divStepMinMax = trueStepValueVariable * (stepMinMax * 0.01);
  unsigned long divStepMinMaxABS = (trueStepValueVariable - stepPerHour);
  divStepMinMaxABS = abs(divStepMinMaxABS);
  if (divStepMinMaxABS > divStepMinMax ) {
    smsSender = authorizedNumbers[1];
    Serial.println("+-");
    FN_printLcd("+-", 0, 12, 1);
    isstartAgainRpmCount = 8;
    trueStepValueVariable = stepPerHour;
    FN_smsInfoCall();
  }
}
void FN_resetFlags() {
  smsSender = authorizedNumbers[1];
  isUnder_30Min = false;
  countCloseRpm = 0;
  isstartAgainRpmCount = 0;
  isMinMaxCalc = false;
  isLockStepManual = false;
  trueStepValue = 0;
  issignalok = false;
  isRunCode = false;

  FN_smsInfo("-termatismos-", true);
  // FN_deleteAllSms (25);
  rpm = -1;
  delay(100);
  lcd.clear();
  delay(100);
  FN_printLcd("Rse", 0, 0, 0);
}

void DisplayPSI () // main display
{
  // this section monitors the live psi and turns the compressor run bit on or off based off setpoints
  int psi = analogRead(0); // this reads the analog input(A0) and scales it
  // psi = map(psi, 102, 921, 0, 150);             // this maps the raw analog input value to the converted PSI value
  psi = analogRead(A0);
  int sensorVal = analogRead(A0);
  Serial.print("Sensor Value: ");
  Serial.print(sensorVal);
  float voltage = (sensorVal * 5.0) / 1024.0;
  Serial.print("Volts: ");
  Serial.print(voltage);
  float pressure_pascal = (3.0 * ((float)voltage - 0.47)) * 1000000.0;
  float pressure_bar = pressure_pascal / 10e5;
  Serial.print("Pressure = ");
  Serial.print(pressure_bar);
  Serial.println(" bars");
  // this is the lcd section
  // line 1
  lcd.setCursor(0,  2); lcd.print("Currently  @ ");  // this prints whats in between the quotes
  lcd.setCursor(13, 2); lcd.print("   ");            // this clears the display field so anything left is deleted
  lcd.setCursor(13, 2); lcd.print(pressure_bar);              // this prints the tag value
  lcd.setCursor(17, 2); lcd.print("PSI");            // this prints whats in between the quotes
  // line 2
}
