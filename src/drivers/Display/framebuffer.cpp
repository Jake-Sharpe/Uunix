#include "framebuffer.hpp"

uint32_t Framebuffer::width = 0;
uint32_t Framebuffer::height = 0;
uint8_t Framebuffer::bpp = 0;
uint8_t *Framebuffer::addr = nullptr;
uint32_t Framebuffer::pitch = 0;

void Framebuffer::init(void *multiboot_info) {
  if (!multiboot_info)
    return;

  uint8_t *ptr = (uint8_t *)multiboot_info;
  ptr += 8;

  for (struct multiboot_tag *tag = (struct multiboot_tag *)ptr; tag->type != 0;
       tag =
           (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7))) {

    if (tag->type == 8) {
      struct multiboot_tag_framebuffer *fb_tag =
          (struct multiboot_tag_framebuffer *)tag;
      addr = (uint8_t *)fb_tag->common_addr;
      width = fb_tag->common_width;
      height = fb_tag->common_height;
      pitch = fb_tag->common_pitch;
      bpp = fb_tag->common_bpp;
      break;
    }
  }
}

void Framebuffer::draw_rect(int x, int y, int w, int h, uint32_t color) {
  for (int i = 0; i < w; i++) {
    for (int j = 0; j < h; j++) {
      draw_pixel(x + i, y + j, color);
    }
  }
}

void Framebuffer::draw_pixel(int x, int y, uint32_t color) {
  if (x < 0 || x >= (int)width || y < 0 || y >= (int)height || !addr)
    return;

  uint8_t *pixel = addr + (y * pitch) + (x * (bpp / 8));

  if (bpp == 32) {
    *(uint32_t *)pixel = color;
  } else if (bpp == 24) {
    pixel[0] = color & 0xFF;
    pixel[1] = (color >> 8) & 0xFF;
    pixel[2] = (color >> 16) & 0xFF;
  } else if (bpp == 16) {
    uint16_t c = (uint16_t)(((color & 0xF80000) >> 8) |
                            ((color & 0xFC00) >> 5) | ((color & 0xF8) >> 3));
    *(uint16_t *)pixel = c;
  }
}

void Framebuffer::clear(uint32_t color) {
  if (!addr)
    return;

  if (bpp == 32) {
    uint32_t *pix = (uint32_t *)addr;
    uint32_t count = (pitch / 4) * height;
    for (uint32_t i = 0; i < count; i++) {
      pix[i] = color;
    }
  } else {
    for (uint32_t y = 0; y < height; y++) {
      for (uint32_t x = 0; x < width; x++) {
        draw_pixel(x, y, color);
      }
    }
  }
}
