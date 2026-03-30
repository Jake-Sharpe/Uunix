// common/syscall_nums.h
#ifndef SYSCALL_NUMS_H
#define SYSCALL_NUMS_H

#define SYS_EXIT 0
#define SYS_WRITECONSOLE 1
#define SYS_WRITEPORT 2
#define SYS_READPORT 3
#define SYS_MALLOC 4
#define SYS_FREE 5
#define SYS_LISTFILES 6
#define SYS_READFILE 7

struct Terminal;

namespace Syscall {
void init(Terminal *t);
}

#endif
