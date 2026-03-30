#include "TarFS.hpp"
#include "../kernel/MemoryManager/Kmalloc.hpp"
#include "../kernel/debug.hpp"
#include "../lib/Vector.hpp"
#include "../lib/string.hpp"
#include "Disk.hpp"

int get_size(const char *in) {
  int size = 0;
  int j;
  int count = 1;

  for (j = 10; j >= 0; j--) {
    size += (in[j] - '0') * count;
    count *= 8;
  }
  return size;
}

uint8_t *ReadFile(const char *path) {
  bool Finished = false;
  uint8_t Buffer[512];
  uint64_t pos = 0;
  tar_header *header;
  while (!Finished) {
    ReadSector(pos, Buffer);
    header = (tar_header *)Buffer;
    header->name[99] = '\0';
    if (StrCmp(header->name, path)) {
      Finished = true;
      uint64_t size = get_size(header->size);
      uint32_t sectors_to_read = (size + 511) / 512;
      uint32_t allocation_size = sectors_to_read * 512;
      uint8_t *buffer = (uint8_t *)kmalloc(allocation_size + 1);
      uint32_t data_pos = pos + 1;
      for (uint64_t i = 0; i < sectors_to_read; i++) {
        ReadSector(data_pos + i, buffer + i * 512);
      }
      buffer[size] = '\0';
      return buffer;
    } else if (header->name[0] == '\0') {
      Debug::print("File not found: ");
      Debug::print(path);
      Debug::newline();
      return nullptr;
    } else {
      uint64_t size = get_size(header->size);
      pos += 1 + ((size + 511) / 512);
    }
  }

  return nullptr;
}

void ListFiles(Vector<String> *files) {
  bool Finished = false;
  uint8_t Buffer[512];
  uint64_t pos = 0;
  tar_header *header;
  while (!Finished) {
    ReadSector(pos, Buffer);
    header = (tar_header *)Buffer;
    header->name[99] = '\0';
    if (header->name[0] == '\0') {
      Finished = true;
    } else {
      files->push_back(String(header->name));
      uint64_t size = get_size(header->size);
      pos += 1 + ((size + 511) / 512);
    }
  }
}