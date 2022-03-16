
#include <TM1637Display.h>
#include <DS3231M.h>
#include <Wire.h>
#include "stdio.h"

DS3231M_Class DS3231M;

bool set = false;
TM1637Display display(2, 3);
const int selectButton = 6;
const int valueButton = 5;
const int okButton = 4;
const int buzz = 9;
const int wakeByButton = 11;


void _delay()
{
  delay(100);
}

void DisplayWithBlank(int value, uint8_t blankIndex)
{
  uint8_t digits[4] = {
    display.encodeDigit(value / 1000),
    display.encodeDigit((value / 100) % 10),
    display.encodeDigit((value / 10) % 10),
    display.encodeDigit(value % 10)
  };
  if (blankIndex < 4)
    digits[blankIndex] = 0;
  display.setSegments(digits);
}

int Editor(int initial)
{
  uint8_t numbers[4] = {(uint8_t)(initial / 1000), (uint8_t)((initial / 100) % 10), (uint8_t)((initial / 10) % 10), (uint8_t)(initial % 10)};
  uint8_t currentNumber = 0;

  DisplayWithBlank(initial, currentNumber);
  bool blanking = false;
  uint32_t time = millis();

  while (true)
  {

    if (digitalRead(selectButton) == LOW)
    {

      // Serial.println('Editing' + initial + "...");
      // Serial.println("Editing digit at" + currentNumber);
      currentNumber++;
      delay(200);
      if (currentNumber > 3)
        currentNumber = 0;
    }
    if (digitalRead(valueButton) == LOW)
    {
      delay(200);
      numbers[currentNumber]++;
      Serial.println("new value=  ");
      Serial.println(numbers[currentNumber]);
      if (numbers[currentNumber] > 9 || numbers[2] > 5)
        numbers[currentNumber] = 0;
      if (numbers[0] > 2)
        numbers[0] = 0;
      if (numbers[0] == 2 && numbers[1] > 3)
        numbers[1] = 0;
      initial = numbers[0] * 1000 + numbers[1] * 100 + numbers[2] * 10 + numbers[3];
      Serial.println("new display=  ");
      Serial.println(initial);
      display.showNumberDecEx(initial, 0xff, true);
      set = true;
    }
    if (millis() - time > 500)
    {
      blanking = !blanking;
      DisplayWithBlank(initial, !blanking ? 255 : currentNumber);
      time = millis();
    }
    if (digitalRead(okButton) == LOW)
    {
      delay(300);
      Serial.println("ok clicked");
      Serial.println(initial);
      int hour = numbers[0] * 10 + numbers[1];
      Serial.println(numbers[0] * 10 + numbers[1]);
      int minute = numbers[2] * 10 + numbers[3];
      Serial.println(numbers[2] * 10 + numbers[3]);
      Serial.println(hour * 100 + minute);
      return hour * 100 + minute;
    }
  }
}

void setTime(int initial)
{
  Serial.println("entered");
  int time = Editor(initial);
  int hour = (time / 100);
  int minute = time % 100;
  DateTime t(2021, 7, 21, hour, minute, 0);
  DS3231M.adjust(t);
  Serial.println("set");
}
int setAlarm(int alarm)
{
  byte hour = (alarm / 100);
  byte minute = alarm % 100;
  DateTime t(2021, 7, 21, hour, minute, 0);
  DS3231M.setAlarm(minutesHoursMatch, t, true);
  Serial.println("Alarm set!");
}

void displayTime(int time)
{
  display.showNumberDecEx(time, 0xff, true);
}

bool wakeByAlarm;

void ShutDown()
{
  digitalWrite(wakeByButton, HIGH);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(selectButton, INPUT_PULLUP);
  pinMode(valueButton, INPUT_PULLUP);
  pinMode(okButton, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);
  digitalWrite(buzz, LOW);
  //pinMode(led, OUTPUT);
  pinMode(wakeByButton, INPUT);
  wakeByAlarm = digitalRead(wakeByButton) == LOW;
  if (!wakeByAlarm)
  { //wake up by button?
    pinMode(wakeByButton, OUTPUT);
    digitalWrite(wakeByButton, LOW);
  }

  display.setBrightness(0x0f);

  DS3231M.pinAlarm();
}

unsigned long lastActivityTime = 0;
unsigned long nowTime;
unsigned long span;
void loop()
{

  DateTime now = DS3231M.now();
  int currentTime = now.hour() * 100 + now.minute();
  displayTime(currentTime);


  bool keyPressed = (digitalRead(selectButton) == LOW || digitalRead(valueButton) == LOW || digitalRead(okButton) == LOW);
  if (wakeByAlarm || DS3231M.isAlarm())
  {
    Serial.println("Alarm Time");
    if (lastActivityTime == 0)
      lastActivityTime = millis();
    nowTime = millis();
    span = nowTime - lastActivityTime;
    if (span > 60000)
    {
      DS3231M.clearAlarm();
      Serial.println("Stopped");
      lastActivityTime = millis();
    }

    if (keyPressed)
    {
      
      delay(200);
      DS3231M.clearAlarm();
      Serial.println("Alarm cleared");
      DS3231M.clearAlarm();
      lastActivityTime = millis();
      return;
    }
    digitalWrite(buzz, HIGH);
    delay(250);
    digitalWrite(buzz, LOW);
    delay(50);
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    delay(50);
    digitalWrite(buzz, HIGH);
    delay(200);
    digitalWrite(buzz, LOW);
    delay(300);
  }
  else
  {
    //digitalWrite(wakeByButton, LOW);
    nowTime = millis();
    span = nowTime - lastActivityTime;

    if (span > 60000) {
      Serial.println("Shutting Down");
      ShutDown();
    }
    if (digitalRead(okButton) == LOW)
    {

      delay(200);
      Serial.println("Set Alarm mode activated");
      int alarm = Editor(currentTime - 1);

      setAlarm(alarm);
      lastActivityTime = millis();
 
      Serial.println(lastActivityTime);

    }
    if (digitalRead(selectButton) == LOW)
    {

      delay(200);
      Serial.println("Set time mode activated");

      setTime(currentTime);
      lastActivityTime = millis();
   
      Serial.println(lastActivityTime);
     

  }
  }
  }
