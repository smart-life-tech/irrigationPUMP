int IR1 = 2;
unsigned long t1 = 0;
unsigned long t2 = 0;
float velocity;
int speeding = 0;
bool count1, clear = true;
bool count2, read = false;
int counter = 0;
// int vkmh = (100*3600)/1000;
bool done = true;
unsigned long lastMillis = 0;
void setup()
{

  pinMode(IR1, INPUT_PULLUP);
  attachInterrupt(0, reads, RISING);
  Serial.begin(9600);
}

void loop()
{
  if (count2)
  {
    t2 = millis();
    Serial.print("t2 ");
    Serial.println(t2);
    read = true;
    done = true;
    count1 = false;
    count2 = false;
  }
  if (count1)
  {
    t1 = millis();
    Serial.print("t1 ");
    Serial.println(t1);
    done = true;
    count1 = false;
    count2 = false;
  }

  if (read)
  {
    velocity = t2 - t1;
    velocity = velocity / 1000;                 // convert millisecond to second for timig
    velocity = (0.18 / velocity) * 3.6 * 1000; // km/s
    velocity = velocity/1000;
    Serial.println("velocity/speed: ");
    Serial.print(velocity);
    speeding = velocity;
    Serial.println(" km/hr");
    delay(500);
    read = false;
  }
}
void reads()
{
  if (done) {
    if (millis() - lastMillis >   250) {
      lastMillis = millis();
      //Serial.println("counting");
      counter++;
      if (counter == 1) {
       // Serial.println("count1");
        count1 = true;
        count2 = false;
        //done = false;
      } else if (counter == 2) {
       // Serial.println("count2");
        count2 = true;
        count1 = false;
        counter = 0;

      }
      done = false;
    }
  }
}
