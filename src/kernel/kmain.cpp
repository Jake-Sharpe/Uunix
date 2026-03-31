#include "../drivers/Display/framebuffer.hpp"
#include "../drivers/keyboard/keyboard.hpp"
#include "../drivers/terminal.hpp"
#include "../fs/Disk.hpp"
#include "../fs/TarFS.hpp"
#include "../lib/Vector.hpp"
#include "../lib/string.hpp"
#include "MemoryManager/Kmalloc.hpp"
#include "MemoryManager/MMap.hpp"
#include "Shell/Shell.hpp"
#include "Shell/SysFL/DefineTSS.hpp"
#include "Timing/RTC.hpp"
#include "debug.hpp"
#include "syscalls.hpp"

char Buffer[256];
extern "C" uint64_t stack_top;

extern "C" void kmain(void *multiboot_info) {
  // install_tss(stack_top);
  Debug::init();
  Debug::print("--- Uunix Kernel Starting ---");
  Debug::newline();

  Framebuffer::init(multiboot_info);
  Shell::global_term.init();
  Syscall::init(&Shell::global_term);

  pmm_init(multiboot_info);
  kmalloc_init();
  SetDisk(0);
  Shell::global_term.Draw();
  Vector<String> *FilesList = new Vector<String>();
  ListFiles(FilesList);
  delete FilesList;

  Debug::print("Fully Initialized");
  while (1) {
    Shell::KernelLoop();
  }
}

extern "C" uint8_t _kernel_end;