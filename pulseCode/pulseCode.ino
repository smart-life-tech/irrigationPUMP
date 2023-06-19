const int outputPin = 2;
const int conversionFactor = 3600; // Conversion factor for km/h to pulses per hour
float pulsePeriod = 1000000.0;
void setup()
{
    pinMode(outputPin, OUTPUT);
    Serial.begin(9600);
}

void loop()
{
    if (Serial.available() > 0)
    {
        float speedKmh = Serial.parseFloat(); // Read the speed value from serial

        // Calculate the pulse frequency based on the speed
        // The higher the speed, the higher the pulse frequency
        float pulseFrequency = speedKmh * conversionFactor / 1000.0;

        // Calculate the pulse period in microseconds
        // The pulse period is the inverse of the pulse frequency
        pulsePeriod = 1000000.0 / pulseFrequency;
    }
    // Generate a continuous pulse on the output pin
    digitalWrite(outputPin, HIGH);
    delayMicroseconds(pulsePeriod / 2);
    digitalWrite(outputPin, LOW);
    delayMicroseconds(pulsePeriod / 2);
}
