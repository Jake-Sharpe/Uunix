#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>

class Debug {
public:
  static void init();
  static void print(const char *str);
  static void print_hex(uint64_t val);
  static void newline();
};

#endif
