#ifndef MULTIBOOT_HEADER_HPP
#define MULTIBOOT_HEADER_HPP

#include <stdint.h>

// The very first part of the multiboot info
struct multiboot_fixed {
  uint32_t total_size;
  uint32_t reserved;
};

// The generic header for every tag
struct multiboot_tag {
  uint32_t type;
  uint32_t size;
};

// The specific tag for the Memory Map (Type 6)
struct multiboot_mmap_entry {
  uint64_t addr;
  uint64_t len;
  uint32_t type; // 1 = Usable RAM
  uint32_t zero;
};

struct multiboot_tag_mmap {
  uint32_t type;
  uint32_t size;
  uint32_t entry_size;
  uint32_t entry_version;
  struct multiboot_mmap_entry entries[0];
};

#endif