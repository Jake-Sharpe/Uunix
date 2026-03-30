#include "terminal.hpp"
#include "../kernel/Timing/RTC.hpp"
#include "Display/framebuffer.hpp"
#include "vga.hpp"

void Terminal::init() { Clear(); }

void Terminal::Clear() {
  for (int y = 0; y < 20; y++) {
    for (int x = 0; x < 256; x++) {
      buffer[y][x] = '\0';
    }
  }
  Framebuffer::clear(0x000000);
}

void Terminal::Shift() {
  for (int y = 39; y > 0; y--) {
    for (int x = 0; x < 256; x++) {
      buffer[y][x] = buffer[y - 1][x];
    }
  }
  for (int x = 0; x < 256; x++) {
    buffer[0][x] = '\0';
  }
}

void Terminal::Newline() { Shift(); }

void Terminal::Write(const char *str, int offset) {
  Shift();
  int x = 0;
  while (x < offset) {
    buffer[0][x] = ' ';
    x++;
  }
  x = 0;
  int l = 0;
  while (str[x] != '\0' && x < 255) {
    if (str[x] == '\n') {
      Newline();
      l = 0;
      while (l < offset) {
        buffer[0][l] = ' ';
        l++;
      }
      l = 0;
    } else {
      buffer[0][l + offset] = str[x];
      l++;
    }
    x++;
  }
  buffer[0][l + offset] = '\0';
}

extern char Buffer[256];
void Terminal::Draw() {
  Framebuffer::clear(0x000000);
  for (int y = 0; y < 40; y++) {
    vga_print(buffer[y], 0, 720 - y * 20, 3);
  }
  RTC::get_time_string(Buffer);
  vga_print(Buffer, 1064, 0, 2);
}