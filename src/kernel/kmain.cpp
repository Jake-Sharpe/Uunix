#include "../drivers/Display/framebuffer.hpp"
#include "../drivers/terminal.hpp"
#include "../fs/Disk.hpp"
#include "../fs/TarFS.hpp"
#include "../lib/Vector.hpp"
#include "../lib/string.hpp"
#include "MemoryManager/Kmalloc.hpp"
#include "MemoryManager/MMap.hpp"
#include "Timing/RTC.hpp"
#include "debug.hpp"
#include "font.hpp"
#include "syscalls.hpp"

static Terminal global_term;

char Buffer[256];

extern "C" void kmain(void *multiboot_info) {

  Debug::init();
  Debug::print("--- Uunix Kernel Starting ---");
  Debug::newline();
  Debug::print("Hello World ");

  Framebuffer::init(multiboot_info);
  global_term.init();
  Syscall::init(&global_term);

  pmm_init(multiboot_info);
  kmalloc_init();
  uint8_t disks = DisksAvailable();
  SetDisk(0);
  global_term.Draw();
  Vector<String> *FilesList = new Vector<String>();

  ListFiles(FilesList);
  Debug::print("Files in root directory: ");

  Debug::newline();
  global_term.Write("Files:\n");
  for (uint64_t i = 0; i < FilesList->size(); i++) {
    global_term.Write(FilesList->get(i).buffer);
    global_term.Newline();
    global_term.Write("{");
    global_term.Write((char *)ReadFile(FilesList->get(i).buffer), 2);
    global_term.Write("}");
  }

  Debug::print("Fully Initialized");
  while (1) {
    global_term.Draw();
    RTC::DateTime dt = RTC::get_datetime();
    while (RTC::get_datetime().second == dt.second) {
    }
  }
}

extern "C" uint8_t _kernel_end;