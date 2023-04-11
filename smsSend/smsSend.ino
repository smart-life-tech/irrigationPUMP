
void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial.println("Initializing...");
  delay(1000);

  // Send attention command to check if all fine, it returns OK
  Serial1.println("AT");
  updateSerial();
  // Configuring module in TEXT mode
  Serial1.println("AT+CMGF=1");
  updateSerial();

  // Decides how newly arrived SMS messages should be handled
  Serial1.println("AT+CNMI=1,2,0,0,0");
  updateSerial();
}

void loop() {
  updateSerial();
}

// For data transmission from Serial to Software Serial port & vice versa
void updateSerial() {
  delay(500);
  while (Serial.available()) {
    Serial1.write(Serial.read());//Forward what Serial received to Software Serial Port
  }

  while (Serial1.available()) {
    Serial.write(Serial1.read());//Forward what Software Serial received to Serial Port
  }
}
/*
 * 3) the bar pressure we have
The machine starts tilting the water switch
until it detects the course of m/h we have given
and maintains them until the end of watering.

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
    /*boolean isSmsValid = FN_checkSms (data, true, true); if(isSmsValid) { return};
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
    // FN_printLcd ("prog ok", 0, 0, 0);
  }
  else if (data.startsWith("PROGSTEP")) //progstep
  { /*boolean is5msValid FN_checkSms (data, false, false); if(!isSmsValid) return;/ //programming step
    int commaIndex = data.indexOf("#");
    String stepDataString = data.substring(commaIndex + 1);
    char buf[stepDataString.length()];
    stepDataString.toCharArray (buf, stepDataString.length());
    // stepValue = atof(buf); eepromUtil. eeprom_write_string (100, buf);
    delay(100); Serial.println ("Programmed step: "); Serial.print (stepValue);
    //String message = "Programmed step: "+String(stepDataString); //FN_smsInfo(message, true);
    //FN_printLcd ("prog step ok", 0, 0, 0);
  }
  else if (data.startsWith("MTR")) {
    int commaIndex = data.indexOf("#");
    int secondCommaIndex = data.indexOf("#", commaIndex + 1);
    smsMetra = data.substring(commaIndex + 1, secondCommaIndex).toInt();
    rpm = smsMetra / stepValue;
    // FN_printLcd ("&", rpm, 0, 0);
    Serial.println (smsMetra);
    Serial.println(rpm);
  }
 */
