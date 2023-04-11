int pwm = 6;
int in1 = 5;
int in2 = 4;
int button = 9;
bool toggle = true;
//Create software serial object to communicate with SIM800L
String inputstring = "";
void setup()
{
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);

  //Begin serial communication with Arduino and SIM800L
  Serial1.begin(9600);

  Serial.println("Initializing...");
  delay(1000);

  Serial1.println("AT"); //Once the handshake test is successful, it will back to OK
  updateSerial();

  Serial1.println("AT+CMGF=1"); // Configuring TEXT mode
  updateSerial();
  Serial1.println("AT+CNMI=1,2,0,0,0"); // Decides how newly arrived SMS messages should be handled
  updateSerial();
  pinMode(pwm, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(button, INPUT_PULLUP);
}

void loop() {
  updateSerial();
  if (digitalRead(button)) {
    delay(200);
    toggle != toggle;
    if (toggle) {
      analogWrite(pwm, 155);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
      delay(3000);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    } else {
      analogWrite(pwm, 155);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
      delay(3000);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }
  }
}

void updateSerial()
{
  delay(1000);
  while (Serial.available())
  {
    Serial1.write(Serial.read());//Forward what Serial received to Software Serial Port
  }
  while (Serial1.available())
  {
    Serial.write(Serial1.read());//Forward what Software Serial received to Serial Port
    char inChar = (char)Serial1.read();
    inputstring += inChar; // if the incoming character is a newline, set a flag // so the main loop can do something about it:
    if (inChar == '\n') {
      inputstring.toUpperCase();
      Serial.println(inputstring);
      inputstring = "";
    }
  }
}
