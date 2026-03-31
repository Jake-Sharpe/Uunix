#include "Shell.hpp"
#include "../../drivers/keyboard/keyboard.hpp"
#include "../../drivers/terminal.hpp"
#include "../../fs/TarFS.hpp"
#include "../Timing/RTC.hpp"

Terminal Shell::global_term;

RTC::DateTime start_time = RTC::get_datetime();

Vector<String> *Shell::RunCommand(String command) {
  if (command == "ls") {
    Vector<String> *files = new Vector<String>();
    ListFiles(files);
    return files;
  } else if (command.StartsWith("cat ")) {
    Vector<String> *files = new Vector<String>();
    uint8_t *ptr = ReadFile(command.Substring(4, command.length).buffer);
    if (ptr != nullptr) {
      files->push_back((char *)ptr);
      kfree(ptr);
    } else {
      files->push_back(String("File not found"));
    }
    return files;
  } else if (command.StartsWith("neofetch")) {
    Vector<String> *lines = new Vector<String>();
    lines->push_back("");
    lines->push_back(String("U       U"));
    lines->push_back(String("U       U"));
    lines->push_back(String("U       U  U    U   N N N  IIIII  X  X "));
    lines->push_back(String("U       U  U    U   N   N    I     XX"));
    lines->push_back(String("UUUUUUUUU  UUUUUU   N   N  IIIII  X  X"));
    lines->push_back(String(""));
    lines->push_back(String("OS: Uunix"));
    lines->push_back(String("Version: 0.0.1"));
    lines->push_back(String("Architecture: x86_64"));
    lines->push_back(String("Kernel: Uunix Kernel"));
    return lines;
  } else if (command == "help") {
    Vector<String> *lines = new Vector<String>();
    lines->push_back("Commands:");
    lines->push_back(String("ls - List files"));
    lines->push_back(String("cat <file> - Print file"));
    lines->push_back(String("Neofetch - Show system info"));
    lines->push_back(String("help - Show this message"));
    return lines;
  } else if (command == "larp") {
    Vector<String> *lines = new Vector<String>();
    lines->push_back("                   -                root@uunix");
    lines->push_back("                  .o+               ----------");
    lines->push_back("                 `ooo/              OS: Uunix x86_64 "
                     "(Antigravity Edition)");
    lines->push_back(
        "                `+oooo:             Kernel: 0.0.1-godmode-rolling");
    lines->push_back("               `+oooooo:            Uptime: 420 years, "
                     "69 days, 13 hours");
    lines->push_back(
        "              -+oooooo+:            Packages: 0 (Bloat is a sin)");
    lines->push_back(
        "             `/:-:++oooo+:          Shell: uunix-sh v9001.0");
    lines->push_back(
        "            `/++++/+++++++:         Resolution: 32K (30720x17280)");
    lines->push_back("           `/++++++++++++++:        DE: None (I am the "
                     "desktop environment)");
    lines->push_back("          `/+++ooooooooooooo/       WM: Antigravity-WM");
    lines->push_back("         ./ooosssso++osssssso+      CPU: Intel Core "
                     "i129-29900KS (1024) @ 128GHz");
    lines->push_back("        .ooossssso-````/ossssss+    GPU: NVIDIA RTX 9090 "
                     "Ti Quantum (1TB VRAM)");
    lines->push_back("       -osssssso.      :ssssssso.   Memory: 512TB / 2PB "
                     "(DDR9 @ 1,000,000MHz)");
    lines->push_back("      :osssssss/        osssso+++.  Disk: 50 Petabyte "
                     "Absolute-Zero SSD");
    lines->push_back("     /ossssssss/        +ssssooo/-  Network: 1000Gbps "
                     "Direct-to-Brain Link");
    lines->push_back("   `/osssso+/-          -:/+osssso+-");
    lines->push_back("  `+sso+:-               `.-/+oso:");
    lines->push_back(" ++: .                         ` -/+/");
    lines->push_back(" .`                                /");
    return lines;
  }
}

void Shell::KernelLoop() {
  global_term.Draw();
  RTC::DateTime dt = RTC::get_datetime();
  while (RTC::get_datetime().second == dt.second) {
    char c = Keyboard::get_char();
    if (c != 0) {
      if (c == '\n') {
        if (global_term.buffer[0][0] != '\0') {
          Vector<String> *files = Shell::RunCommand(global_term.buffer[0]);
          for (uint64_t i = 0; i < files->size(); i++) {
            global_term.Write(files->get(i).buffer);
            global_term.Newline();
          }
          global_term.Newline();
          delete files;
        } else {
          global_term.Newline();
        }
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