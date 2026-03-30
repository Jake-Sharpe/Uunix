#ifndef SHELL_HPP
#define SHELL_HPP

#include "../../drivers/terminal.hpp"
#include "../../lib/Vector.hpp"
#include "../../lib/string.hpp"

namespace Shell {
Vector<String> *RunCommand(String command);
};

#endif
