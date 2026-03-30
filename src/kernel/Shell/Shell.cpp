#include "Shell.hpp"
#include "../../fs/TarFS.hpp"

Vector<String> *Shell::RunCommand(String command) {
  if (command == "ls") {
    Vector<String> *files = new Vector<String>();
    ListFiles(files);
    return files;
  } else {
    uint8_t *ptr = ReadFile(command.buffer);
    if (ptr != nullptr) {
      Vector<String> *files = new Vector<String>();
      files->push_back((char *)ptr);
      return files;
    } else {
      Vector<String> *files = new Vector<String>();
      files->push_back(String("File not found"));
      return files;
    }
  }
}