#ifndef TERMINAL_H
#define TERMINAL_H

struct Terminal {
  int last = 0;
  char buffer[40][256];
  void Shift();
  void init();
  void Draw();
  void Clear();
  void Write(const char *str, int offset = 0);
  void Newline();
  void AddChar(char c);
};

#endif
