#include <LiquidCrystal_I2C.h>      //SYSTEM PARAMETER  - ESP32 LCD Compatible Library (By: Robojax)
LiquidCrystal_I2C lcd(0x27, 16, 2); //SYSTEM PARAMETER  - Configure LCD RowCol Size and I2C Address
const int hall_readpin_1 = 2;
const int hall_readpin_2 = 3;


int hall_incoming_1 = 1;
int hall_incoming_2 = 1;
int hall_last_1 = 2;
int hall_count = 0;
bool hall_stop = false;
String hall_direction = "";
int hall_time_last = 0;
int hall_min_time = 1000;

void hall()
{
  hall_incoming_1 = digitalRead(hall_readpin_1);
  hall_incoming_2 = digitalRead(hall_readpin_2);

  if (hall_incoming_1 != hall_last_1)
  {
    hall_last_1 = hall_incoming_1;
    check_direction();
  }


  if (hall_incoming_1 == 0 && hall_stop == false) {
    hall_stop = true;
    if (hall_direction == "left")
    {
      hall_count++;
    }
    else if (hall_direction == "right")
    {
      hall_count--;
    }
    Serial.println(hall_count);
    lcd.setCursor(0, 0);
    lcd.print("dir. read & count");
    lcd.setCursor(0, 1);
    lcd.print("dir ");
    lcd.print(hall_direction );
    lcd.setCursor(0, 2);
    lcd.print("count ");
    lcd.print(hall_count );
  }
  if (hall_incoming_1 == 1 and hall_stop == true)
  {
    hall_stop = false;
  }
}

void check_direction()
{
  if (millis() - hall_time_last > hall_min_time)
  {
    if (hall_incoming_1 == hall_incoming_2)
    {
      Serial.println("left");
      hall_direction = "left";
    }
    else if (hall_incoming_1 != hall_incoming_2)
    {
      Serial.println("right");
      hall_direction = "right";
    }
  }
  else
  {
    Serial.println("TO FAST!!!");
  }
  hall_time_last = millis();
}

void setup ()
{
  hall_incoming_1 = digitalRead(hall_readpin_1);
  hall_last_1 = hall_incoming_1;
  pinMode (hall_readpin_1, INPUT_PULLUP) ;
  pinMode (hall_readpin_2, INPUT_PULLUP) ;
  Serial.begin(9600);
  lcd.init();
  lcd.setBacklight(HIGH);
  lcd.setCursor(0, 0);
  lcd.print("dir. read & count");
  lcd.setCursor(0, 1);
  lcd.print("dir ");
  lcd.print(hall_direction );
  lcd.setCursor(0, 2);
  lcd.print("count ");
  lcd.print(hall_count );
}



void loop()
{
  hall();
}
