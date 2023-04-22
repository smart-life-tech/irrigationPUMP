int IR1 = 2;
unsigned long t1 = 0;
unsigned long t2 = 0;
float velocity;
int speeding = 0;
bool count1, clear = true;
bool count2, read = false;
int counter = 0;
// int vkmh = (100*3600)/1000;

void setup()
{

    pinMode(IR1, INPUT);
    attachInterrupt(0, reads, FALLING);
    Serial.begin(9600);
}

void loop()
{
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
    else if (counter >= 2)
    {
        if (count2)
        {
            t2 = millis();
            Serial.println(t2);
            read = true;
            count1 = true;
            count2 = false;
        }
        counter = 0;
    }
    if (read)
    {
        velocity = t2 - t1;
        velocity = velocity / 1000;                 // convert millisecond to second for timig
        velocity = (0.155 / velocity) * 3.6 * 1000; // km/s
        Serial.println("velocity/speed: ");
        Serial.print(velocity);
        speeding = velocity;
        Serial.println(" m/hr");
        delay(500);
        read = false;
    }

    delay(500);
}
void reads()
{
    // if (clear) {
    counter++;
    Serial.print("counts : ");
    Serial.println(counter);
    clear = false;
    // }
    // clear = true;
    // delay(10);
}