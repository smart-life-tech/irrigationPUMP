
int pwm = 6;
int in1 = 5;
int in2 = 4;
//Create software serial object to communicate with SIM800L

void setup(){
  //Begin serial communication with Arduino and Arduino IDE (Serial Monitor)
  Serial.begin(9600);
  pinMode(pwm, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
}
void loop() {
  delay(5000);
  analogWrite(pwm, 255);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(5000);
  analogWrite(pwm, 255);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
}
