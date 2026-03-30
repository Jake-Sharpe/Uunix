#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "../../kernel/boot/Multiboot_Header.hpp"
#include <stdint.h>

struct multiboot_tag_framebuffer {
  uint32_t type;
  uint32_t size;
  uint64_t common_addr;
  uint32_t common_pitch;
  uint32_t common_width;
  uint32_t common_height;
  uint8_t common_bpp;
  uint8_t common_type;
  uint16_t reserved;
};

class Framebuffer {
public:
  static void init(void *multiboot_info);
  static void draw_pixel(int x, int y, uint32_t color);
  static void draw_rect(int x, int y, int w, int h, uint32_t color);
  static void clear(uint32_t color);

  static uint32_t width;
  static uint32_t height;
  static uint8_t bpp;

private:
  static uint8_t *addr;
  static uint32_t pitch;
};

#endif
