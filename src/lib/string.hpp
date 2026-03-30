#ifndef STRING_HPP
#define STRING_HPP

#include <stdint.h>

struct String {
public:
  char *buffer;
  int length;

  String() : buffer(nullptr), length(0) {}
  String(const char *str);
  static String Alloc(int length);
  ~String();
  String(const String &other);

  friend String operator+(const String &a, const String &b);
  String &operator=(const String &other);
  bool operator==(const String &other) const;

  String(int64_t number);
};
bool StrCmp(const char *a, const char *b);

#endif