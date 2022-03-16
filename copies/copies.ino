#include <DS3232RTC.h>        // https://github.com/JChristensen/DS3232RTC
#include <LowPower.h>

//const time_t ALARM_INTERVAL(20 * 60 * 60); // 20 Hour - DOESN'T WORK-> use UL
unsigned long ALARM_INTERVAL=28UL*24UL*60UL*60UL; //28 Days - Works
tmElements_t tm;
const byte rtcAlarmPin = 3; // External interrupt on pin D3

void setup()
{
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  Serial.begin(9600);
  pinMode(rtcAlarmPin, INPUT_PULLUP); // Set interrupt pin
  // initialize the alarms to known values, clear the alarm flags, clear the alarm interrupt flags
  RTC.setAlarm(ALM1_MATCH_DATE, 0, 0, 0, 1);
  RTC.setAlarm(ALM2_MATCH_DATE, 0, 0, 0, 1);
  RTC.alarm(ALARM_1);
  RTC.alarm(ALARM_2);
  RTC.alarmInterrupt(ALARM_1, false);
  RTC.alarmInterrupt(ALARM_2, false);
  RTC.squareWave(SQWAVE_NONE);

  RTC.read(tm);
   time_t t = makeTime(tm);        // change the tm structure into time_t (seconds since epoch)
  Serial.println(F("Aktuelle Zeit t: "));
  printDateTime(t);
  
  time_t alarmTime = t + ALARM_INTERVAL;    // calculate the alarm time
  Serial.println(F("Alarm wird gesetzt auf: ")); 
  printDateTime(alarmTime);

  // set the current time
  // RTC.set(t);
  // set the alarm
  RTC.setAlarm(ALM1_MATCH_HOURS, second(alarmTime), minute(alarmTime), hour(alarmTime), 0);
  // clear the alarm flag
  RTC.alarm(ALARM_1);
  RTC.alarmInterrupt(ALARM_1, true); // Enable alarm 1 interrupt A1IE
}

void loop()
{
  // check to see if the alarm flag is set (also resets the flag if set)
  Serial.println("Entering sleep");
  delay(100);
  sleepNow(); // Enter slee
  setRTCagain();
}
// Interrupt Service Routine (ISR) function
void wake ()
{
  detachInterrupt (digitalPinToInterrupt (3)); // Disable interrupts on D3
}

void sleepNow ()
{
  noInterrupts (); // Disable interrupts before entering sleep
  attachInterrupt (digitalPinToInterrupt(3), wake, FALLING);  // Wake on falling edge of D3
  interrupts (); // Enable interrupts to ensure next instruction is executed
  LowPower.powerDown(SLEEP_FOREVER, ADC_OFF, BOD_OFF);
}

void setRTCagain() {
  Serial.println("Awake...");

  if ( RTC.alarm(ALARM_1) )
  {
    Serial.println("Alarm1 now..: ");
    RTC.read(tm);
    time_t t = makeTime(tm);        // change the tm structure into time_t (seconds since epoch)
    printDateTime(t);
    time_t alarmTime = t + ALARM_INTERVAL;    // calculate the alarm time
    Serial.println(F("Alarm is set to: "));
    printDateTime(alarmTime);
    // set the alarm
    RTC.setAlarm(ALM1_MATCH_HOURS, second(alarmTime), minute(alarmTime), hour(alarmTime), 0);
  }
}
// Print current time and date
void printDateTime(time_t t)
{
  Serial.print((day(t) < 10) ? "0" : ""); Serial.print(day(t), DEC); Serial.print('/');
  Serial.print((month(t) < 10) ? "0" : ""); Serial.print(month(t), DEC); Serial.print('/');
  Serial.print(year(t), DEC); Serial.print(' ');
  Serial.print((hour(t) < 10) ? "0" : ""); Serial.print(hour(t), DEC); Serial.print(':');
  Serial.print((minute(t) < 10) ? "0" : ""); Serial.print(minute(t), DEC); Serial.print(':');
  Serial.print((second(t) < 10) ? "0" : ""); Serial.println(second(t), DEC);
}
