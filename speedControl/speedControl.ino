int IR1 = 2;
int IR2 = 3;
unsigned long t1 = 0;
int pwm = 6;
int in1 = 5;
int in2 = 4;
unsigned long t2 = 0;
int setSpeed = 10;
float velocity;

// int vkmh = (100*3600)/1000;

void setup()
{
    pinMode(IR1, INPUT);
    pinMode(IR2, INPUT);
    pinMode(pwm, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
}
float getSpeed()
{
    if (digitalRead(IR1) == 1)
    {
        t1 = millis();
    }
    if (digitalRead(IR2) == 1)
    {
        t2 = millis();
    }

    velocity = t2 - t1;
    velocity = velocity / 1000;        // convert millisecond to second
    velocity = (0.2 / velocity) * 3.6; // km/s
    Serial.print(velocity);
    Serial.println(" Km/hr");
    delay(500);
    return velocity;
}
void controlMotor(float speed)
{
    if (speed > setSpeed + 2)
    {
        // delay(5000);
        analogWrite(pwm, 255);
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
    }
    else if (speed < setSpeed - 2)
    {
        analogWrite(pwm, 255);
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
    }
    else
    {
        analogWrite(pwm, 255);
        digitalWrite(in1, LOW);
        digitalWrite(in2, LOW);
    }
}