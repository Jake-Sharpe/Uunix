#include "syscalls.hpp"
#include "../drivers/terminal.hpp"
#include "../fs/TarFS.hpp"
#include "../lib/Vector.hpp"
#include "../lib/string.hpp"
#include "MemoryManager/Kmalloc.hpp"
#include "Shell/Shell.hpp"
#include "debug.hpp"
#include <stdint.h>

extern "C" void syscall_entry();

namespace Syscall {
Terminal *global_terminal = nullptr;

void init(Terminal *t) {
  global_terminal = t;

  uint32_t low, high;
  asm volatile("rdmsr" : "=a"(low), "=d"(high) : "c"(0xC0000080));
  low |= 1;
  asm volatile("wrmsr" : : "a"(low), "d"(high), "c"(0xC0000080));

  uint64_t entry = (uint64_t)syscall_entry;
  asm volatile("wrmsr"
               :
               : "a"((uint32_t)entry), "d"((uint32_t)(entry >> 32)),
                 "c"(0xC0000082));

  uint64_t star = (0x08ULL << 32) | (0x10ULL << 48);
  asm volatile("wrmsr"
               :
               : "a"(0), "d"((uint32_t)(star >> 32)), "c"(0xC0000081));

  asm volatile("wrmsr" : : "a"(0x200), "d"(0), "c"(0xC0000084));
}
} // namespace Syscall

extern "C" uint64_t handle_syscall(uint64_t arg1, uint64_t arg2,
                                   uint64_t arg3) {
  uint64_t num;
  asm volatile("mov %%rax, %0" : "=r"(num));

  switch (num) {
  case SYS_EXIT: {
    Shell::KernelLoop();
    return 0;
  }

  case SYS_WRITECONSOLE: {
    if (Syscall::global_terminal) {
      Syscall::global_terminal->Write((const char *)arg1);
      Syscall::global_terminal->Draw();
    }
    return 0;
  }

  case SYS_WRITEPORT: {
    asm volatile("outb %b0, %w1" : : "a"(arg1), "Nd"(arg2));
    return 0;
  }

  case SYS_READPORT: {
    uint8_t ret;
    asm volatile("inb %w1, %b0" : "=a"(ret) : "Nd"(arg2));
    return (uint64_t)ret;
  }

  case SYS_MALLOC: {
    return (uint64_t)kmalloc(arg1);
  }

  case SYS_FREE: {
    kfree((void *)arg1);
    return 0;
  }

  case SYS_LISTFILES: {
    Vector<String> *FilesList = (Vector<String> *)arg1;
    ListFiles(FilesList);
    return 0;
  }

  case SYS_READFILE: {
    return (uint64_t)ReadFile((const char *)arg1);
  }

  default: {
    Debug::print("Unknown syscall: ");
    Debug::print_hex(num);
    Debug::newline();
    return 0;
  }
  }
}
