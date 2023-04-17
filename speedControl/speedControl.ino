// in this code two hall sensor will be used and the motor to control speed of the device
int IR1 = 2;
int IR2 = 3;
unsigned long t1 = 0;
int pwm = 6;
int in1 = 5;
int in2 = 4;
unsigned long t2 = 0;
int setSpeed = 1500;
float velocity;
bool read = false;
// int vkmh = (100*3600)/1000;
int counter = 0;
bool clear, count1, count2 = true;
void setup()
{
  pinMode(IR1, INPUT_PULLUP);
  pinMode(IR2, INPUT);
  pinMode(pwm, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  analogWrite(pwm, 255);
  digitalWrite(in2, HIGH);
  digitalWrite(in1, LOW);
  delay(5000);
  analogWrite(pwm, 255);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  delay(5000);
  Serial.begin(9600);
}

void loop()
{
  controlMotor(getSpeed());
}
float getSpeed()
{
  if (digitalRead(IR1) == 0)
  {
    if (clear)
    {
      counter++;
      Serial.print("counts : ");
      Serial.println(counter);
      clear = false;
    }
  }
  else if (digitalRead(IR1) == 1)
  {
    clear = true;
    delay(100);
  }
  if (counter == 1)
  {
    if (count1)
    {
      t1 = millis();
      Serial.println(t1);
      read = false;
      count2 = true;
      count1 = false;
    }
  }
  else if (counter == 2)
  {
    if (count2)
    {
      t2 = millis();
      Serial.println(t2);
      counter = 0;
      read = true;
      count1 = true;
      count2 = false;
    }
  }
  if (read)
  {
    velocity = t2 - t1;
    velocity = velocity / 1000;        // convert millisecond to second for timig
    velocity = (0.2 / velocity) * 3.6; // km/s
    Serial.print(velocity * 1000);
    Serial.println(" m/hr");
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
    if (speed > setSpeed + 400)
    {
      // delay(5000);
      analogWrite(pwm, 255);
      digitalWrite(in1, HIGH);
      digitalWrite(in2, LOW);
    }
    else if (speed < setSpeed - 400)
    {
      analogWrite(pwm, 255);
      digitalWrite(in1, LOW);
      digitalWrite(in2, HIGH);
    }
    else
    {
      analogWrite(pwm, 0);
      digitalWrite(in1, LOW);
      digitalWrite(in2, LOW);
    }
  }
}