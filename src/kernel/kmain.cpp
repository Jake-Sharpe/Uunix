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
#include "Timing/RTC.hpp"
#include "debug.hpp"
#include "syscalls.hpp"

char Buffer[256];

extern "C" void kmain(void *multiboot_info) {

  Debug::init();
  Debug::print("--- Uunix Kernel Starting ---");
  Debug::newline();
  Debug::print("Hello World ");

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

  Debug::print("Files in root directory: ");

  Debug::newline();

  Debug::print("Fully Initialized");
  while (1) {
    Shell::KernelLoop();
  }
}

extern "C" uint8_t _kernel_end;