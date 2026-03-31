#include "string.hpp"
#include <stdint.h>

String::String(const char *str) {
  length = 0;
  while (str[length] != '\0') {
    length++;
  }
  buffer = new char[length + 1];
  for (int i = 0; i < length; i++) {
    buffer[i] = str[i];
  }
  buffer[length] = '\0';
}

String::String(const String &other) {
  length = other.length;
  buffer = new char[length + 1];
  for (int i = 0; i <= length; i++) {
    buffer[i] = other.buffer[i];
  }
}

String String::Alloc(int length) {
  String result;
  result.buffer = new char[length + 1];
  result.length = length;
  result.buffer[length] = '\0';
  return result;
}

String::String(int64_t number) {
  if (number == 0) {
    this->length = 1;
    this->buffer = new char[2];
    this->buffer[0] = '0';
    this->buffer[1] = '\0';
    return;
  }

  bool negative = (number < 0);
  uint64_t abs_val =
      negative ? (uint64_t)(-(number + 1)) + 1 : (uint64_t)number;
  uint64_t temp = abs_val;
  int len = 0;
  while (temp > 0) {
    temp /= 10;
    len++;
  }
  if (negative)
    len++;

  this->length = len;
  this->buffer = new char[len + 1];
  temp = abs_val;
  for (int i = len - 1; i >= (negative ? 1 : 0); i--) {
    this->buffer[i] = (temp % 10) + '0';
    temp /= 10;
  }
  if (negative)
    this->buffer[0] = '-';
  this->buffer[len] = '\0';
}

String operator+(const String &a, const String &b) {
  uint64_t new_len = a.length + b.length;
  String result = String::Alloc(new_len);

  // Copy first string
  for (uint64_t i = 0; i < a.length; i++) {
    result.buffer[i] = a.buffer[i];
  }

  // Copy second string
  for (uint64_t i = 0; i < b.length; i++) {
    result.buffer[a.length + i] = b.buffer[i];
  }

  result.buffer[new_len] = '\0';
  return result; // C++ will now use your Copy Constructor safely!
}

String &String::operator=(const String &other) {
  if (this == &other)
    return *this; // Don't copy yourself

  if (buffer)
    delete[] buffer; // Clear old memory

  length = other.length;
  buffer = new char[length + 1];
  for (int i = 0; i <= length; i++) {
    buffer[i] = other.buffer[i];
  }
  return *this;
}

bool String::operator==(const String &other) const {
  if (length != other.length)
    return false;
  for (int i = 0; i < length; i++) {
    if (buffer[i] != other.buffer[i])
      return false;
  }
  return true;
}

String::~String() {
  if (buffer) {
    delete[] buffer;
  }
}

bool StrCmp(const char *a, const char *b) {
  while (*a && (*a == *b)) {
    a++;
    b++;
  }
  return (*a == *b);
}

bool String::StartsWith(const char *other) const {
  int len = 0;
  while (other[len] != '\0') {
    len++;
  }
  if (length < len) {
    return false;
  }
  for (int i = 0; i < len; i++) {
    if (buffer[i] != other[i]) {
      return false;
    }
  }
  return true;
}

String String::Substring(int start, int end) const {
  int len = end - start;
  String result = String::Alloc(len);
  for (int i = 0; i < len; i++) {
    result.buffer[i] = buffer[start + i];
  }
  result.buffer[len] = '\0';
  return result;
}
