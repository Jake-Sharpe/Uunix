#include "keyboard.hpp"
#include "../Port.hpp"

namespace Keyboard {

static const char kbd_map[] = {
    0,   27,  '1',  '2',  '3',  '4', '5', '6',  '7', '8', '9', '0',
    '-', '=', '\b', '\t', 'q',  'w', 'e', 'r',  't', 'y', 'u', 'i',
    'o', 'p', '[',  ']',  '\n', 0,   'a', 's',  'd', 'f', 'g', 'h',
    'j', 'k', 'l',  ';',  '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v',
    'b', 'n', 'm',  ',',  '.',  '/', 0,   '*',  0,   ' '};

static const char kbd_map_shifted[] = {
    0,   27,  '!',  '@',  '#',  '$', '%', '^', '&', '*', '(', ')',
    '_', '+', '\b', '\t', 'Q',  'W', 'E', 'R', 'T', 'Y', 'U', 'I',
    'O', 'P', '{',  '}',  '\n', '0', 'A', 'S', 'D', 'F', 'G', 'H',
    'J', 'K', 'L',  ':',  '"',  '~', '0', '|', 'Z', 'X', 'C', 'V',
    'B', 'N', 'M',  '<',  '>',  '?', 0,   '*', 0,   ' '};

char scancode_to_char(uint8_t scancode, bool shifted) {
  if (scancode & 0x80) {
    return 0;
  }

  if (scancode >= sizeof(kbd_map)) {
    return 0;
  }

  if (shifted) {
    return kbd_map_shifted[scancode];
  } else {
    return kbd_map[scancode];
  }
}

void init() {}

bool KeyReady() { return inb(0x64) & 1; }

bool Shift = false;
bool CapsLock = false;
bool Ctrl = false;
bool Alt = false;
char Last = '0';
char get_char() {
  if (KeyReady()) {
    uint8_t scancode = inb(0x60);
    if (scancode == 0x2A || scancode == 0x36) {
      Shift = true;
      return 0;
    } else if (scancode == 0xAA || scancode == 0xB6) {
      Shift = false;
      return 0;
    } else if (scancode == 0x3A) {
      CapsLock = !CapsLock;
      return 0;
    } else if (scancode == 0x1D || scancode == 0x9D) {
      Ctrl = !Ctrl;
      return 0;
    } else if (scancode == 0x38 || scancode == 0xB8) {
      Alt = !Alt;
      return 0;
    }

    bool shifted = CapsLock != Shift;
    char c = scancode_to_char(scancode, shifted);
    if (c == Last) {
      Last = '0';
      return 0;
    } else {
      Last = c;
      return c;
    }
  }
  return 0;
}
} // namespace Keyboard