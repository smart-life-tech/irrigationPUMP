int IR1 = 2;
int IR2 = 2;
unsigned long t1 = 0;
unsigned long t2 = 0;
float velocity;
int speeding = 0;
bool count1, clear, count2, read = true;
int counter = 0;
// int vkmh = (100*3600)/1000;

void setup()
{

    pinMode(IR1, INPUT);
    pinMode(IR2, INPUT);

    Serial.begin(9600);
}

void loop()

{
    if (digitalRead(IR1) == 0)
    {
        if (clear)
        {
            counter++;
            Serial.print("counts : ");

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
            // Serial.println(t1);
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
            //  Serial.println(t2);
            counter = 0;
            read = true;
            count1 = true;
            count2 = false;
        }
    }
    if (read)
    {
        velocity = t2 - t1;
        velocity = velocity / 1000;             // convert millisecond to second for timig
        velocity = (1 / velocity) * 3.6 * 1000; // km/s
        Serial.println("velocity/speed: ");
        Serial.print(velocity);
        speeding = velocity;
        Serial.println(" m/hr");
        delay(500);
        read = false;
    }

    delay(500);
}