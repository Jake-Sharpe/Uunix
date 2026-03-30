#include "Kmalloc.hpp"
#include "../debug.hpp"
#include "MMap.hpp"

static HeapBlock *heap_start = nullptr;

void kmalloc_init() {
  // 1. Actually allocate 1MB (256 pages * 4096)
  uint64_t page_count = 256;
  uint64_t heap_size = page_count * 4096;

  void *ptr = AllocPages(page_count);
  if (!ptr) {
    Debug::print("FAILED TO INIT HEAP\n");
    return;
  }

  heap_start = (HeapBlock *)ptr;

  // 2. Initial block must be FREE so kmalloc can find it
  heap_start->set_free(heap_size - sizeof(HeapBlock));
  heap_start->next = nullptr;

  Debug::print("Heap Initialized: 1MB at ");
  Debug::print_hex((uint64_t)heap_start);
  Debug::print("\n");
}

void *kmalloc(uint64_t size) {
  if (size == 0)
    return nullptr;

  size = (size + 7) & ~7; // 8-byte alignment
  HeapBlock *current = heap_start;

  while (current) {
    uint64_t current_size = current->size();

    if (!current->is_used() && current_size >= size) {
      // Can we split? (Need room for header + at least 16 bytes)
      if (current_size > size + sizeof(HeapBlock) + 16) {
        HeapBlock *new_block =
            (HeapBlock *)((uint8_t *)current + sizeof(HeapBlock) + size);

        // Setup new free block
        new_block->set_free(current_size - size - sizeof(HeapBlock));
        new_block->next = current->next;

        // Setup current used block
        current->set_used(size);
        current->next = new_block;
      } else {
        // Not enough room to split, just take the whole block
        current->set_used(current_size);
      }

      return (void *)((uint8_t *)current + sizeof(HeapBlock));
    }
    current = current->next;
  }

  Debug::print("kmalloc: OUT OF MEMORY\n");
  return nullptr;
}

void kfree(void *ptr) {
  if (!ptr)
    return;

  // Step back to header
  HeapBlock *block = (HeapBlock *)((uint8_t *)ptr - sizeof(HeapBlock));

  // 1. Mark as free
  block->set_free(block->size());

  // 2. Simple Coalesce (Merge with next)
  if (block->next && !block->next->is_used()) {
    uint64_t new_size = block->size() + sizeof(HeapBlock) + block->next->size();
    block->set_free(new_size);
    block->next = block->next->next;
  }
}

// Normal 'new'
void *operator new(size_t size) { return kmalloc(size); }

// Array 'new[]'
void *operator new[](size_t size) { return kmalloc(size); }

// Normal 'delete'
void operator delete(void *p) { kfree(p); }

// Array 'delete[]'
void operator delete[](void *p) { kfree(p); }

/**
 * C++14/17 Sized Delete
 * Some compilers require these to prevent errors with newer standards.
 */
void operator delete(void *p, size_t size) {
  (void)size; // We don't need the size for our simple allocator
  kfree(p);
}

void operator delete[](void *p, size_t size) {
  (void)size;
  kfree(p);
}
