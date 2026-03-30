#ifndef KMALLOC_HPP
#define KMALLOC_HPP

#define HEAP_USED_BIT (1ULL << 63)
#include <stddef.h> // for size_t
#include <stdint.h>

struct HeapBlock {
  uint64_t size_and_flag;
  HeapBlock *next;

  // Helper methods inside the struct
  uint64_t size() { return size_and_flag & ~HEAP_USED_BIT; }
  bool is_used() { return (size_and_flag & HEAP_USED_BIT) != 0; }
  void set_used(uint64_t s) { size_and_flag = s | HEAP_USED_BIT; }
  void set_free(uint64_t s) { size_and_flag = s & ~HEAP_USED_BIT; }
};

void kmalloc_init();
void *kmalloc(uint64_t size);
void kfree(void *ptr);

void *operator new(size_t size);
void *operator new[](size_t size);
void operator delete(void *p);
void operator delete[](void *p);
void operator delete(void *p, size_t size);
void operator delete[](void *p, size_t size);

#endif