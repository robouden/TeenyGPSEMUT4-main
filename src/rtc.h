
/********************************************************************/
// RTC Date/Time Struct
typedef struct {
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t  hour;
  uint8_t  minute;
  uint8_t  second;
  uint8_t  pad0;
} rtc_datetime_t;

/********************************************************************/
// RTC
#include <TimeLib.h>
bool clockTime_valid;

/********************************************************************/
time_t getTeensy3Time() {
  return Teensy3Clock.get();
}

/********************************************************************/
void rtc_setup() {
  clockTime_valid = false;
  setSyncProvider(getTeensy3Time);
}

/********************************************************************/
rtc_datetime_t getRTCTime() {
  time_t _now = getTeensy3Time();
  rtc_datetime_t _datetime;
  _datetime.year   = year(_now);
  _datetime.month  = month(_now);
  _datetime.day    = day(_now);
  _datetime.hour   = hour(_now);
  _datetime.minute = minute(_now);
  _datetime.second = second(_now);
  return _datetime;
}

/********************************************************************/
void setRTCTime(uint8_t hour, uint8_t minute, uint8_t second,
                uint8_t day, uint8_t month, uint16_t year) {
  setTime(hour, minute, second, day, month, year);
  Teensy3Clock.set(now());
}

