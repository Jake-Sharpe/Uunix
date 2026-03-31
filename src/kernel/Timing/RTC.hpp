#ifndef RTC_HPP
#define RTC_HPP

#include <stdint.h>

namespace RTC {
// This structure holds the final human-readable time
struct DateTime {
  uint8_t second;
  uint8_t minute;
  uint8_t hour;
  uint8_t day;
  uint8_t month;
  uint16_t year;
  DateTime operator-(const DateTime &other) const {
    DateTime result;
    result.second = second - other.second;
    result.minute = minute - other.minute;
    result.hour = hour - other.hour;
    result.day = day - other.day;
    result.month = month - other.month;
    result.year = year - other.year;
    return result;
  }
};

// This is the only function other files need to call
DateTime get_datetime();
void get_time_string(char *out_buffer); // Takes a buffer of at least 9 bytes
void get_time_string(DateTime dt, char *out_buffer);

} // namespace RTC

#endif