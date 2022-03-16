#include <TM1637Display.h>
#include <DS3231.h>
#include <Wire.h>
#include "stdio.h"
DS3231 clock;
RTClib myRTC;
TM1637Display display(2, 3);
DateTime getTime();

void setup()
{
  Serial.begin(115200);
  Wire.begin();
}

void loop()
{
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

}
