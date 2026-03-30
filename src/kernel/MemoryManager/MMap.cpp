#include "MMap.hpp"
#include "../debug.hpp"
#include <stdint.h>

// Standard constants
#define MAX_USABLE_ENTRIES 128
#define PAGE_SIZE 4096

// Structures for local caching
struct mmap_region {
  uint64_t addr;
  uint64_t len;
};

// Global State
uint32_t *bitmap;
uint64_t total_bitmap_entries = 0;
uint64_t last_alloc_index = 0;
uint64_t max_pages = 0;

extern "C" uint8_t _kernel_end;

// Helpers
void pmm_mark_used(uintptr_t addr) {
  uint64_t frame = addr / PAGE_SIZE;
  if (frame >= max_pages)
    return;
  bitmap[frame / 32] |= (1U << (frame % 32));
}

void pmm_mark_free(uintptr_t addr) {
  uint64_t frame = addr / PAGE_SIZE;
  if (frame >= max_pages)
    return;
  bitmap[frame / 32] &= ~(1U << (frame % 32));
}

void pmm_init(void *multiboot) {
  if (!multiboot)
    return;

  uint64_t max_addr = 0;
  mmap_region usable_regions[MAX_USABLE_ENTRIES];
  uint64_t usable_count = 0;

  // --- PASS 1: GATHER INFO INTO LOCAL CACHE ---
  // We must read everything before touching the bitmap area
  struct multiboot_tag *tag =
      (struct multiboot_tag *)((uint8_t *)multiboot + 8);
  while (tag->type != 0) {
    if (tag->type == 6) { // Memory Map Tag
      struct multiboot_tag_mmap *mmap = (struct multiboot_tag_mmap *)tag;
      uint64_t entry_count =
          (mmap->size - sizeof(struct multiboot_tag_mmap)) / mmap->entry_size;

      for (uint64_t i = 0; i < entry_count; i++) {
        if (mmap->entries[i].type == 1) { // Type 1 = Usable
          // Cache it locally
          if (usable_count < MAX_USABLE_ENTRIES) {
            usable_regions[usable_count].addr = mmap->entries[i].addr;
            usable_regions[usable_count].len = mmap->entries[i].len;
            usable_count++;
          }

          uint64_t end = mmap->entries[i].addr + mmap->entries[i].len;
          if (end > max_addr)
            max_addr = end;
        }
      }
    }
    tag = (struct multiboot_tag *)((uint8_t *)tag + ((tag->size + 7) & ~7));
  }

  if (usable_count == 0) {
    Debug::print("PMM ERROR: No usable RAM found!\n");
    return;
  }

  // --- SETUP BITMAP ---
  max_pages = max_addr / PAGE_SIZE;
  total_bitmap_entries = max_pages / 32;
  uint64_t bytes_needed = max_pages / 8;
  bitmap = (uint32_t *)&_kernel_end;

  // Initialize ALL as used
  for (uint64_t i = 0; i < total_bitmap_entries; i++) {
    bitmap[i] = 0xFFFFFFFF;
  }

  // --- PASS 2: FREE USABLE REGIONS FROM CACHE ---
  for (uint64_t i = 0; i < usable_count; i++) {
    for (uint64_t j = 0; j < usable_regions[i].len; j += PAGE_SIZE) {
      pmm_mark_free(usable_regions[i].addr + j);
    }
  }

  // --- PASS 3: SHIELD KERNEL AND METADATA ---
  // Shield everything from 0 to Bitmap End + 1MB safety buffer
  uint64_t shield_end = (uint64_t)(&_kernel_end) + bytes_needed + 1000000;
  for (uint64_t i = 0; i < shield_end; i += PAGE_SIZE) {
    pmm_mark_used(i);
  }

  // Shield Multiboot structure (in case it is outside the kernel shield)
  multiboot_fixed *mbf = (multiboot_fixed *)multiboot;
  uintptr_t mb_start = (uintptr_t)multiboot;
  uintptr_t mb_end = mb_start + mbf->total_size;
  for (uintptr_t i = mb_start; i < mb_end; i += PAGE_SIZE) {
    pmm_mark_used(i);
  }
}

void *AllocPages(uint64_t PageAmount) {
  if (PageAmount == 0 || total_bitmap_entries == 0)
    return nullptr;

  uint64_t consecutive_free = 0;
  uint64_t start_bit_index = 0;

  for (uint64_t i = 0; i < total_bitmap_entries; i++) {
    if (bitmap[i] == 0xFFFFFFFF) {
      consecutive_free = 0;
      continue;
    }

    for (int bit = 0; bit < 32; bit++) {
      if (!(bitmap[i] & (1U << bit))) {
        if (consecutive_free == 0) {
          start_bit_index = (i * 32) + bit;
        }
        consecutive_free++;

        if (consecutive_free == PageAmount) {
          for (uint64_t j = 0; j < PageAmount; j++) {
            uint64_t target_bit = start_bit_index + j;
            bitmap[target_bit / 32] |= (1U << (target_bit % 32));
          }
          return (void *)(start_bit_index * PAGE_SIZE);
        }
      } else {
        consecutive_free = 0;
      }
    }
  }

  Debug::print("ERROR: Out of physical memory\n");
  return nullptr;
}

void *AllocPage() { return AllocPages(1); }

void FreePages(void *ptr, uint64_t PageAmount) {
  if (!ptr)
    return;
  uint64_t start_bit = (uint64_t)ptr / PAGE_SIZE;
  for (uint64_t i = 0; i < PageAmount; i++) {
    uint64_t target_bit = start_bit + i;
    if (target_bit / 32 >= total_bitmap_entries)
      break;
    bitmap[target_bit / 32] &= ~(1U << (target_bit % 32));
  }
}