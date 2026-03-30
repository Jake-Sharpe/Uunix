#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include <stdint.h>

namespace Keyboard {
char scancode_to_char(uint8_t scancode, bool shifted);
void init();
bool KeyReady();
char get_char();
} // namespace Keyboard

#endif