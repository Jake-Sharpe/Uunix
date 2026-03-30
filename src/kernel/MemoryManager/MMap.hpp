#ifndef MMAP_HPP
#define MMAP_HPP

#include "../boot/Multiboot_Header.hpp"
#include <stdint.h>

#define PAGE_SIZE 4096

extern uint32_t *bitmap;

void pmm_mark_used(uintptr_t addr);
void pmm_mark_free(uintptr_t addr);
void pmm_init(void *multiboot);
void *AllocPage();
void *AllocPages(uint64_t PageAmount);
void FreePages(void *ptr, uint64_t PageAmount);

#endif