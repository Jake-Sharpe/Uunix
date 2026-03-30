#include "vga.hpp"
#include "../kernel/font.hpp"
#include "Display/framebuffer.hpp"

void vga_print(const char *str, int start_x, int start_y, int size) {
  if (str == nullptr)
    return;

  for (int Char = 0; str[Char] != '\0'; Char++) {
    uint8_t char_code = (uint8_t)str[Char];
    if (char_code < 32 || char_code > 126)
      continue;

    int font_idx = char_code - 32;
    for (int col = 0; col < 5; col++) {
      uint8_t column_data = font_5x7[font_idx][col];
      for (int row = 0; row < 8; row++) {
        uint32_t color = (column_data & (1 << row)) ? 0xFFFFFF : 0x000000;
        Framebuffer::draw_rect(start_x + (Char * 6 * size) + col * size,
                               start_y + row * size, size, size, color);
      }
    }

    for (int row = 0; row < 8; row++) {
      Framebuffer::draw_rect(start_x + (Char * 6 * size) + 5 * size,
                             start_y + row * size, size, size, 0x000000);
    }
  }
}
