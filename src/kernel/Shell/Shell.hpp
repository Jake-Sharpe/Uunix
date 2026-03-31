#ifndef SHELL_HPP
#define SHELL_HPP

#include "../../drivers/terminal.hpp"
#include "../../lib/Vector.hpp"
#include "../../lib/string.hpp"

namespace Shell {

extern Terminal global_term;
Vector<String> *RunCommand(String command);
void KernelLoop();
}; // namespace Shell

#endif
