#ifndef DEFINE_TSS_HPP
#define DEFINE_TSS_HPP
#include "stdint.h"

struct TSS {
  uint32_t reserved0;
  uint64_t rsp0; // <--- Very Important: Kernel Stack for Ring 3 -> Ring 0
  uint64_t rsp1;
  uint64_t rsp2;
  uint64_t reserved1;
  uint64_t ist[7];
  uint64_t reserved2;
  uint16_t reserved3;
  uint16_t iopb_offset;
} __attribute__((packed));

void install_tss(uint64_t stack_top);

#endif