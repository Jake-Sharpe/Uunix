#include "Shell.hpp"
#include "../../drivers/keyboard/keyboard.hpp"
#include "../../drivers/terminal.hpp"
#include "../../fs/TarFS.hpp"
#include "../MemoryManager/Kmalloc.hpp"
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
    lines->push_back(String("Kernel: The Unlike unix Kernel"));
    return lines;
  } else if (command == "help") {
    Vector<String> *lines = new Vector<String>();
    lines->push_back("Commands:");
    lines->push_back(String("ls - List files"));
    lines->push_back(String("cat <file> - Print file"));
    lines->push_back(String("Neofetch - Show system info"));
    lines->push_back(String("help - Show this message"));
    return lines;
  } else if (command.StartsWith("echo ")) {
    Vector<String> *lines = new Vector<String>();
    lines->push_back(command.Substring(5, command.length));
    return lines;
  } else {
    Vector<String> *lines = new Vector<String>();
    lines->push_back(String("Command not found: ") + command);
    return lines;
  }
}

void Shell::KernelLoop() {
  if (global_term.buffer[0][0] == '\0') {
    global_term.AddChar('>');
    global_term.AddChar(' ');
  }
  global_term.Draw();
  RTC::DateTime dt = RTC::get_datetime();
  while (RTC::get_datetime().second == dt.second) {
    char c = Keyboard::get_char();
    if (c != 0) {
      if (c == '\n') {
        if (global_term.buffer[0][2] != '\0') {
          String cmd = global_term.buffer[0];
          if (cmd.length > 1 && cmd.buffer[0] == '>' && cmd.buffer[1] == ' ') {
            cmd = cmd.Substring(2, cmd.length);
          }
          Vector<String> *files = Shell::RunCommand(cmd);
          global_term.Newline();
          for (uint64_t i = 0; i < files->size(); i++) {
            global_term.Write(files->get(i).buffer);
            global_term.Newline();
          }
          delete files;
          global_term.AddChar('>');
          global_term.AddChar(' ');
        } else {
          global_term.Newline();
          global_term.AddChar('>');
          global_term.AddChar(' ');
        }
      } else if (c == '\b') {
        if (global_term.last > 1) {
          global_term.last--;
          global_term.buffer[0][global_term.last] = ' ';
        }
      } else if (global_term.last == 0) {
        global_term.AddChar('>');
        global_term.AddChar(' ');
      } else {
        global_term.AddChar(c);
      }
      global_term.Draw();
    }
  }
}