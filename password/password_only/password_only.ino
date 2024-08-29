#include <Wire.h>                   // Wire library
#include <LiquidCrystal_I2C.h>      // Liquid Crystal I2C library
LiquidCrystal_I2C lcd(0x27, 20, 4); // Display address 0x27, I2C 20 x 4

// Variables
bool releasing = true;
bool deviation = true;
int cursorPos;
int dig[4];
int buttonUp = 9;
int buttonDown = 8;
int buttonOk = 7;
bool stopWatering = true;
int countering = 0;
int counter = 0;
int ButtonState[] = {0, 0, 0, 0};
int lastButtonState[] = {1, 1, 1, 1};
int password[] = {0, 1, 2, 2};
void setup()
{
    Serial.begin(9600); // Setting the baud rate of Serial Monitor (Arduino)
    delay(1000);
    Serial.println("Preparing to read received SMSes");
    Serial.println(F("Done"));
    pinMode(buttonUp, INPUT_PULLUP);
    pinMode(buttonDown, INPUT_PULLUP);
    pinMode(buttonOk, INPUT_PULLUP);
    lcd.init();      // start the lcd library
    lcd.backlight(); // turn on the lcd backlight
    lcd.clear();     // clear the cld screen

    // set values of variables
    lcd.print("Enter password:");
    lcd.setCursor(6, 1);
    lcd.print("0000");
    lcd.cursor();
    lcd.noBlink();
    lcd.setCursor(6, 1);
    cursorPos = 0;
    for (int i = 0; i <= 3; i++)
    {
        dig[i] = 0;
    }

    while (1)
    {
        passwordStart();
        if (countering == 100)
            break;
    }
    lcd.noCursor();
}
void loop()
{
}
void passwordStart()
{
    int readUp = digitalRead(buttonUp);
    int readDown = digitalRead(buttonDown);
    int readSelect = digitalRead(buttonOk);
    if (readSelect == LOW)
    {
        delay(250);
        moveRight();
        countering++;
    }
    if (readDown == LOW)
    {
        delay(250);
        decrease(cursorPos);
        // counter--;
    }
    else if (readUp == LOW)
    {
        delay(250);
        increase(cursorPos);
    }
    // this runs once the button is presses
    if (countering >= 4)
    {
        Serial.println("checking");
        checkCode();

        // delay(500);
    }
}
void moveRight()
{
    if (cursorPos == 3)
    {
        lcd.setCursor(6, 1); // here in this part of the code we define controls of joystick
        cursorPos = 0;       // how  we can use every part of the move of joystick code here
    }
    else
    {                          // define that for every movement of the joystick change the direction
        int a = cursorPos + 7; // change the numbers display on lcd for ok button click on joystick take in center and click the passcode will enter.
        lcd.setCursor(a, 1);
        cursorPos = cursorPos + 1;
    }
}

void moveLeft()
{
    if (cursorPos == 0)
    {
        lcd.setCursor(9, 1);
        cursorPos = 3;
    }
    else
    {
        int b = cursorPos + 5;
        lcd.setCursor(b, 1);
        cursorPos = cursorPos - 1;
    }
}

void increase(int cursorPos)
{
    if (dig[cursorPos] == 9)
    {
        dig[cursorPos] = 0;
    }
    else
    {
        dig[cursorPos] = dig[cursorPos] + 1;
    }
    int c = cursorPos + 6;
    lcd.setCursor(c, 1);
    lcd.print(dig[cursorPos]);
    lcd.setCursor(c, 1);
}

void decrease(int cursorPos)
{
    if (dig[cursorPos] == 0)
    {
        dig[cursorPos] = 9;
    }
    else
    {
        dig[cursorPos] = dig[cursorPos] - 1;
    }
    int c = cursorPos + 6;
    lcd.setCursor(c, 1);
    lcd.print(dig[cursorPos]);
    lcd.setCursor(c, 1);
}
// change this part for your own passcode
// I've picked 1234....
void checkCode()
{
    if (dig[0] == 1 && dig[1] == 2 && dig[2] == 3 && dig[3] == 4)
    { // Change '1', '2', '3', '4' on this line!!!!
        lcd.setCursor(4, 1);
        lcd.print("correct");
        // delay(1000);
        countering = 100;
    }
    else
    {
        lcd.setCursor(5, 1);
        lcd.print("Wrong!");
        delay(1000);
        lcd.clear();
        lcd.print("Enter password:");
        lcd.setCursor(6, 1);
        lcd.print("0000");
        lcd.setCursor(6, 1);
        countering = 0;
        dig[0] = 0;
        dig[1] = 0;
        dig[2] = 0;
        dig[3] = 0;
        setup();
    }
}
