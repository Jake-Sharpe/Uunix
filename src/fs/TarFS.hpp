#ifndef TARFS_HPP
#define TARFS_HPP

#include "../lib/Vector.hpp"
#include "../lib/string.hpp"
#include <stdint.h>

struct tar_header {
  char name[100];     // File name (null terminated)
  char mode[8];       // Permissions (ignore for now)
  char uid[8];        // User ID (ignore)
  char gid[8];        // Group ID (ignore)
  char size[12];      // File size in ASCII OCTAL
  char mtime[12];     // Last modification time (ignore)
  char chksum[8];     // Checksum (ignore)
  char typeflag;      // Type: '0' = file, '5' = directory
  char linkname[100]; // If it's a link (ignore)
  char magic[6];      // Must be "ustar" (with null)
  char version[2];    // "00"
  char uname[32];     // Owner name (ignore)
  char gname[32];     // Group name (ignore)
  char devmajor[8];   // Device major (ignore)
  char devminor[8];   // Device minor (ignore)
  char prefix[155];   // Extra name prefix
};

int get_size(const char *in);
uint8_t *ReadFile(const char *path);
void ListFiles(Vector<String> *files);

#endif