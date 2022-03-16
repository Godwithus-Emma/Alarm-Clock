# 1 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino"
//#include <RTClib.h>
# 3 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino" 2
# 4 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino" 2
# 5 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino" 2
# 6 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino" 2

# 6 "e:\\CODE\\PRACTICE\\ARDUINO\\Alarm Clock\\Alarm_Clock\\Alarm_Clock.ino"
DS3231 clock;
RTClib myRTC;

bool set = false;
TM1637Display display(2, 3);
const int selectButton = 4;
const int valueButton = 5;
const int okButton = 6;
const int buzz = 7;
int alarmTIme = 0;
DateTime getTime()
{
  return myRTC.now();
}

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

      //          Serial.println("former display=  ");
      //          Serial.println(numbers[0]);
      initial = numbers[0] * 1000 + numbers[1] * 100 + numbers[2] * 10 + numbers[3];
      Serial.println("new display=  ");
      Serial.println(initial);
      display.showNumberDecEx(initial, 0xff, true);
      set = true;
    }
    if (millis() - time > 500)
    {
      blanking = !blanking;
      //if(alarm)
      //Serial.println("Alarm...Alarm");
      DisplayWithBlank(initial, !blanking ? 255 : currentNumber);
      time = millis();
    }
    if (digitalRead(okButton) == 0x0)
    {
      delay(300);
      Serial.println("ok clicked");
      //DisplayWithBlank(initial, currentNumber);
      Serial.println(initial);
      int hour = numbers[0] * 10 + numbers[1];
      Serial.println(numbers[0] * 10 + numbers[1]);
      int minute = numbers[2] * 10 + numbers[3];
      Serial.println(numbers[2] * 10 + numbers[3]);

      //         clock.setHour(hour);
      //         clock.setMinute(minute);
      Serial.println(hour * 100 + minute);
      return hour * 100 + minute;
    }
  }
}

void setTime(int initial)
{
  int time = Editor(initial);
  int hour = (time / 100);
  clock.setHour(hour);
  int minute = time % 100;
  clock.setMinute(minute);
}
//  int editAlarm(){
//     int alarm = Editor(1200);
//      return alarm;
//  }
int setAlarm(int alarm)
{
  byte hour = (alarm / 100);
  byte minute = alarm % 100;
  alarmTIme = alarm;
  clock.setA2Time(30, hour, minute, 0b100, false, false, false);
  Serial.println("Alarm set!");
}
int AlarmTime()
{
  int alarmT = alarmTIme;
  return alarmT;
}
void displayTime(int time)
{
  display.showNumberDecEx(time, 0xff, true);
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

  clock.setMinute(0);
  clock.setHour(12);
  clock.setSecond(55);
  clock.setA2Time(1, 12, 1, 0b100 << 2, false, false, false);
  clock.turnOnAlarm(2);
  display.setBrightness(0x0f);
}

void loop()
{
  DateTime _now = getTime();
  int _currentTime = _now.hour() * 100 + _now.minute();
  displayTime(_currentTime);
  //  return;
  byte day;
  byte hr;
  byte min;
  byte bit = 0;
  bool dow;
  bool h12;
  bool ampm = false;
  clock.getA2Time(day, hr, min, bit, dow, h12, ampm);
  bool al = clock.checkAlarmEnabled(2);
  bool alt = clock.checkIfAlarm(2);
  char buf[128];
  sprintf(buf, "%u %u, %u:%u -- %u, %u, %u, %u, %u", dow, day, hr, min, h12, ampm, bit, al, alt);
  Serial.println(buf);

  //void getA2Time(byte& A2Day, byte& A2Hour, byte& A2Minute, byte& AlarmBits, bool& A2Dy, bool& A2h12, bool& A2PM);
  return;
  Serial.println(day);
  Serial.println(hr);
  Serial.println(min);
  Serial.println(bit);
  Serial.println(dow);
  //  Serial.println(h24);
  //  Serial.println(ampm);
  display.setBrightness(0x0f);
  DateTime now = getTime();
  int currentTime = now.hour() * 100 + now.minute();
  displayTime(currentTime);
  bool running = false;
  if (digitalRead(okButton) == 0x0)
  {
    delay(200);
    Serial.println("Alarm mode activated");
    Serial.println("Current Time = ");
    Serial.println(currentTime);
    int alarm = Editor(1300);
    setAlarm(alarm);
    Serial.println("Alarm Time = ");
    Serial.println(alarm);
  }
  if (digitalRead(selectButton) == 0x0)
  {
    delay(200);
    Serial.println("Set time mode activated");

    setTime(currentTime);
  }
  clock.turnOnAlarm(2);

  //if (sound){
  //  Serial.println(Alarm Activated);
  //}
  //Serial.print(AlarmTime());

  //if (currentTime == AlarmTime()){
  //if (currentTime ==clock.getA2Time(){
  //
  //    if(digitalRead(selectButton)==HIGH || digitalRead(valueButton)==HIGH ||digitalRead(okButton)==HIGH){
  //      delay(200);
  //      running = true;
  //      Serial.print("Alarm stopped!!!");
  //      if (running == true){
  //         Serial.println("Alarm Start!!!");
  //          digitalWrite(buzz, HIGH);
  //          _delay();
  //          digitalWrite(buzz, LOW);
  //          _delay();
  //          digitalWrite(buzz, HIGH);
  //          _delay();
  //          digitalWrite(buzz, LOW);
  //          _delay();
  //              Serial.print("Alarm Ended!!!");
  //      }else{
  //            Serial.print("What are u!!");
  //      }
  //    }
  //
  //  }
}
