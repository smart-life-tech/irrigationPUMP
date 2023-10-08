String phoneNumbers[] = {"+306973991989", "+1234567890", "+9876543210"};  // Add the phone numbers you want to send the SMS to

void setup() {
  // Initialize GSM and serial communication
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
  for (int i = 0; i < sizeof(phoneNumbers) / sizeof(phoneNumbers[0]); i++) {
    sendSMS(phoneNumbers[i], "15% speed deviation");
    delay(5000);  // Add a delay between sending SMS to each number (in milliseconds)
  }
}

void sendSMS(String phoneNumber, String message) {
  Serial.println("Setting the GSM in text mode");
  Serial1.println("AT+CMGF=1\r");
  delay(200);
  Serial.println("Sending SMS to the desired phone number!");
  Serial1.print("AT+CMGS=\"");
  Serial1.print(phoneNumber);
  Serial1.println("\"\r");
  delay(500);
  Serial1.println(message); // SMS Text
  delay(200);
  Serial1.println((char)26); // ASCII code of CTRL+Z
  delay(1000);
  Serial1.println();
  Serial1.println("AT");
  delay(200);
  Serial1.println("AT+CMGF=1\r");
}
