#include <EEPROM.h>
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
void setup() {
  Serial.begin(9600);
  Serial.println("Preparing to read received SMSes");
  Serial1.begin(9600); // Setting the baud rate of GSM Module
  // Setting the baud rate of Serial Monitor (Arduino)
  delay(1000);
  //  Serial.setTimeout();
  Serial.println("Preparing to send SMS");
  // readMem();
  SendMessage();
  timeNow = millis();
}
void loop() {
  if (Serial1.available() > 0) {
    while (1) {
      ReadUnreadMessages() ;
      if (millis() - timeNow > 13000) {
        timeNow = millis();
        break;
      }
    }
  }
  delay(1000);
  Serial.print("phone number in  use : ");
  Serial.println(phoneNum);

  Serial.print("prog step in  use : ");
  Serial.println(progstep);

  Serial.print("diameters in  use : ");
  Serial.println(wheelDia);
}
void ReadUnreadMessages() {
  while (Serial1.available() > 0) {
    char inByte = Serial1.read();
    Serial.write(inByte);
    inputString += inByte;
  }
  if (inputString.length() > 6) {
    Serial.print("recived message : ");
    Serial.println(inputString);
    inputString.toUpperCase(); // Uppercase the Received Message
    // read from port 0, send to port 1:
    //  Serial.print("received data in caps ");
    // Serial.println(inputString);
    processData();
    inputString = "";
  }

  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
}
void SendMessage() {

  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(1000);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.println("AT+CMGS=\"+306973991989\"\r");
  // Replace x with mobile number
  delay(1000);

  Serial1.println(" watering machine has been booted");    // SMS Text
  delay(200);
  Serial1.println((char)26);               // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial.println("sent the text booted from SIM800");    // SMS Text
  Serial1.println("AT");
  delay(1000);
  Serial.println("Connecting...");
  Serial.println("Connected!");
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(1000);

  Serial1.println("AT+CMGF=1");  //Set SMS to Text Mode
  delay(1000);
  Serial1.println("AT+CNMI=1,2,0,0,0");  //Procedure to handle newly arrived messages(command name in text: new message indications to TE)
  delay(1000);  Serial.println("Fetching list of received unread SMSes!");
  //  Serial1.println("AT+CMGL=\"REC UNREAD\"\r");
  delay(1000);
}
void processData() {
  if (inputString.indexOf("PROGNUMBER#") > -1) {
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
