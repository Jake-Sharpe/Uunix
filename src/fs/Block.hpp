#ifndef BLOCK_H
#define BLOCK_H

#include "../drivers/Port.hpp"
#include <stdint.h>

#define TAR_MAGIC 0x55AA6464

struct block_device {
  char name[32];
  void (*read_block)(uint32_t lba, uint8_t *buffer);
  void (*write_block)(uint32_t lba, uint8_t *buffer);
};

#endif