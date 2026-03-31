#include "terminal.hpp"
#include "../kernel/MemoryManager/Kmalloc.hpp"
#include "../kernel/Timing/RTC.hpp"
#include "../lib/string.hpp"
#include "Display/framebuffer.hpp"
#include "vga.hpp"

void Terminal::init() { Clear(); }

void Terminal::Clear() {
  for (int y = 0; y < 80; y++) {
    for (int x = 0; x < 256; x++) {
      buffer[y][x] = '\0';
    }
  }
  Framebuffer::clear(0x000000);
}

void Terminal::Shift() {
  for (int y = 79; y > 0; y--) {
    for (int x = 0; x < 256; x++) {
      buffer[y][x] = buffer[y - 1][x];
    }
  }
  for (int x = 0; x < 256; x++) {
    buffer[0][x] = '\0';
  }
}

void Terminal::Newline() {
  Shift();
  last = 0;
}

void Terminal::Write(const char *str, int offset) {
  Shift();
  int x = 0;
  while (x < offset) {
    buffer[0][x] = ' ';
    x++;
  }
  x = 0;
  int l = 0;
  while (str[x] != '\0') {
    if (str[x] == '\n') {
      Newline();
      l = 0;
      while (l < offset) {
        buffer[0][l] = ' ';
        l++;
      }
      l = 0;
    } else {
      if (l + offset >= 255) {
        Newline();
        l = 0;
        while (l < offset) {
          buffer[0][l] = ' ';
          l++;
        }
        l = 0;
      }
      buffer[0][l + offset] = str[x];
      l++;
    }
    x++;
  }
  buffer[0][l + offset] = '\0';
  last = l + offset;
}

void Terminal::AddChar(char c) {
  if (last < 0) {
    last = 0;
  }
  if (c == '\n') {
    Newline();
    return;
  }
  if (last >= 255) {
    return;
  }
  buffer[0][last] = c;
  buffer[0][last + 1] = '\0';
  last++;
}

extern char Buffer[256];
void Terminal::Draw() {
  Framebuffer::clear(0x000000);
  for (int y = 0; y < 80; y++) {
    vga_print(buffer[y], 0, 700 - y * 10, 2);
  }
  RTC::get_time_string(Buffer);
  vga_print(Buffer, 1064, 0, 2);
  vga_print(("Mb:" + String(allocs)).buffer, 1064, 20, 2);
}