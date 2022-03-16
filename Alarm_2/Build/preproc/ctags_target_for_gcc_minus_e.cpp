# 1 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino"

# 3 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino" 2
# 4 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino" 2
# 5 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino" 2
# 6 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino" 2


# 7 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_2\\Alarm_2.ino"
DS3231M_Class DS3231M;

bool set = false;
TM1637Display display(2, 3);
const int selectButton = 6;
const int valueButton = 5;
const int okButton = 4;
const int buzz = 9;
const int wakeBtn = 10;
//const int led = A5;
//int alarmTIme = 0;

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
      display.encodeDigit(value % 10)};
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

    if (digitalRead(selectButton) == 0x0)
    {

     // Serial.println('Editing' + initial + "...");
     // Serial.println("Editing digit at" + currentNumber);
      currentNumber++;
      delay(200);
      if (currentNumber > 3)
        currentNumber = 0;
    }
    if (digitalRead(valueButton) == 0x0)
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
    if (digitalRead(okButton) == 0x0)
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

bool wakeUpByRTC;

void ShutDown()
{
  digitalWrite(wakeBtn, 0x1);
}

void setup()
{
  Serial.begin(115200);
  Wire.begin();

  pinMode(selectButton, 0x2);
  pinMode(valueButton, 0x2);
  pinMode(okButton, 0x2);
  pinMode(buzz, 0x1);
  digitalWrite(buzz, 0x0);
  //pinMode(led, OUTPUT);
  pinMode(wakeBtn, 0x1);
  wakeUpByRTC = digitalRead(wakeBtn) == 0x0;
  if (!wakeUpByRTC)
  { //wake up by button?
    pinMode(wakeBtn, 0x1);
    digitalWrite(wakeBtn, 0x0);
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


  bool keyPressed = (digitalRead(selectButton) == 0x0 || digitalRead(valueButton) == 0x0 || digitalRead(okButton) == 0x0);
 if (wakeUpByRTC||DS3231M.isAlarm())
  {
    Serial.println("Alarm Time");
    if (lastActivityTime == 0)
      lastActivityTime = millis();
    nowTime = millis();
    span = nowTime - lastActivityTime;
    Serial.println(nowTime);
    Serial.println(lastActivityTime);
    Serial.println(span);
    if (span > 60000)
    {
      DS3231M.clearAlarm();
      Serial.println("Stopped");
      lastActivityTime = 0;
    }

    if (keyPressed)
    {
      //lastActivityTime = 0;
      delay(200);
      DS3231M.clearAlarm();
      Serial.println("Alarm cleared");
      return;
    }
    digitalWrite(buzz, 0x1);
    delay(250);
    digitalWrite(buzz, 0x0);
    delay(50);
    digitalWrite(buzz, 0x1);
    delay(200);
    digitalWrite(buzz, 0x0);
    delay(50);
    digitalWrite(buzz, 0x1);
    delay(200);
    digitalWrite(buzz, 0x0);
    delay(300);
  }
  else
  {
    //digitalWrite(wakeBtn, LOW);
    nowTime = millis();
    span = nowTime-lastActivityTime;
   Serial.print("span 1 = ");
    Serial.println(span);
    if (span > 3000){
    Serial.println("Shutting Down");
    ShutDown();
    }
    if (digitalRead(okButton) == 0x0)
    {

      delay(200);
      Serial.println("Set Alarm mode activated");
      int alarm = Editor(currentTime - 1);

      setAlarm(alarm);
      lastActivityTime = 0;
      nowTime = millis();
      span = lastActivityTime - nowTime;
//          Serial.print("span 2 = ");
//    Serial.println(span);
      if (span > 120000)

        ShutDown();
      return;
    }
    if (digitalRead(selectButton) == 0x0)
    {
      lastActivityTime = 0;
      delay(200);
      Serial.println("Set time mode activated");

      setTime(currentTime);
      lastActivityTime = 0;
      nowTime = millis();
      span = lastActivityTime - nowTime;
      if (span > 120000)
//    Serial.print("span 3 = ");
//    Serial.println(span);
        ShutDown;
    }
          nowTime = millis();
      int tspan = lastActivityTime - nowTime;
      if (tspan > 120000)
    //Serial.println(1);
    //Serial.println(tspan);
        ShutDown;

  }
}
