#include "debug.hpp"

static const uint16_t PORT = 0x3f8;

static inline void outb(uint16_t port, uint8_t val) {
  asm volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
  return ret;
}

void Debug::init() {
  outb(PORT + 1, 0x00);
  outb(PORT + 3, 0x80);
  outb(PORT + 0, 0x03);
  outb(PORT + 1, 0x00);
  outb(PORT + 3, 0x03);
  outb(PORT + 2, 0xC7);
  outb(PORT + 4, 0x0B);
}

static int is_transmit_empty() { return inb(PORT + 5) & 0x20; }

static void write_serial(char a) {
  while (is_transmit_empty() == 0)
    ;
  outb(PORT, a);
}

void Debug::print(const char *str) {
  for (int i = 0; str[i] != '\0'; i++) {
    write_serial(str[i]);
  }
}

void Debug::print_hex(uint64_t val) {
  const char *hex = "0123456789ABCDEF";
  write_serial('0');
  write_serial('x');
  for (int i = 60; i >= 0; i -= 4) {
    write_serial(hex[(val >> i) & 0xF]);
  }
}

void Debug::newline() {
  write_serial('\r');
  write_serial('\n');
}
