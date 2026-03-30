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

  Debug::print("Fully Initialized");
  while (1) {
    global_term.Draw();
    RTC::DateTime dt = RTC::get_datetime();
    while (RTC::get_datetime().second == dt.second) {
      char c = Keyboard::get_char();
      if (c != 0) {
        if (c == '\n') {
          Vector<String> *files = Shell::RunCommand(global_term.buffer[0]);
          for (uint64_t i = 0; i < files->size(); i++) {
            global_term.Write(files->get(i).buffer);
            global_term.Newline();
          }
          global_term.Newline();
        } else if (c == '\b') {
          if (global_term.last != 0) {
            global_term.last--;
            global_term.buffer[0][global_term.last] = ' ';
          }
        } else {
          global_term.AddChar(c);
        }
        global_term.Draw();
      }
    }
  }
}

extern "C" uint8_t _kernel_end;