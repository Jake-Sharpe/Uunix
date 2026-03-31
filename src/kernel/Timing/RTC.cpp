#include "RTC.hpp"

// --- PRIVATE HARDWARE HELPERS ---
static inline void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

void RTC::get_time_string(char *out_buffer) {
  DateTime now = get_datetime();

  // Helper to put two digits into the buffer
  auto format_digit = [&](int val, int offset) {
    out_buffer[offset] = (val / 10) + '0';
    out_buffer[offset + 1] = (val % 10) + '0';
  };

  format_digit(now.day, 0);
  out_buffer[2] = ':';
  format_digit(now.month, 3);
  out_buffer[5] = ':';
  format_digit(now.year, 6);
  out_buffer[8] = '-';
  format_digit(now.hour, 9);
  out_buffer[11] = ':';
  format_digit(now.minute, 12);
  out_buffer[14] = ':';
  format_digit(now.second, 15);
  out_buffer[17] = '\0';
}

void RTC::get_time_string(DateTime dt, char *out_buffer) {
  auto format_digit = [&](int val, int offset) {
    out_buffer[offset] = (val / 10) + '0';
    out_buffer[offset + 1] = (val % 10) + '0';
  };

  format_digit(dt.day, 0);
  out_buffer[2] = ':';
  format_digit(dt.month, 3);
  out_buffer[5] = ':';
  format_digit(dt.year, 6);
  out_buffer[8] = '-';
  format_digit(dt.hour, 9);
  out_buffer[11] = ':';
  format_digit(dt.minute, 12);
  out_buffer[14] = ':';
  format_digit(dt.second, 15);
  out_buffer[17] = '\0';
}

namespace RTC {
// Private constants for RTC registers
enum { CMOS_ADDR = 0x70, CMOS_DATA = 0x71 };

static uint8_t read_register(uint8_t reg) {
  outb(CMOS_ADDR, reg);
  return inb(CMOS_DATA);
}

static bool is_updating() {
  outb(CMOS_ADDR, 0x0A);
  return (inb(CMOS_DATA) & 0x80);
}

// --- PUBLIC FUNCTION IMPLEMENTATION ---
DateTime get_datetime() {
  while (is_updating())
    ;

  DateTime dt;
  dt.second = read_register(0x00);
  dt.minute = read_register(0x02);
  dt.hour = read_register(0x04);
  dt.day = read_register(0x07);
  dt.month = read_register(0x08);
  dt.year = read_register(0x09);
  uint8_t century = read_register(0x32);
  uint16_t full_year = (century) ? (century * 100) + dt.year : 2000 + dt.year;
  dt.year = full_year;

  uint8_t regB = read_register(0x0B);

  if (!(regB & 0x04)) {
    dt.second = (dt.second & 0x0F) + ((dt.second / 16) * 10);
    dt.minute = (dt.minute & 0x0F) + ((dt.minute / 16) * 10);
    dt.hour =
        ((dt.hour & 0x0F) + (((dt.hour & 0x70) / 16) * 10)) | (dt.hour & 0x80);
    dt.day = (dt.day & 0x0F) + ((dt.day / 16) * 10);
    dt.month = (dt.month & 0x0F) + ((dt.month / 16) * 10);
    dt.year = (dt.year & 0x0F) + ((dt.year / 16) * 10);
  }

  if (!(regB & 0x02) && (dt.hour & 0x80)) {
    dt.hour = ((dt.hour & 0x7F) + 12) % 24;
  }

  dt.year += 2000;
  return dt;
}
} // namespace RTC