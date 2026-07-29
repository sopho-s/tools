#include <string>
#include <cstdlib>
#include "exceptions.h"
#pragma once

namespace tools
{
    namespace command
    {
        /// @brief Executes a shell command and returns its standard output
        /// @param command The shell command to run
        /// @return The command's stdout as a string
        /// @throws std::runtime_error if popen() fails to start the process
        std::string Execute(std::string command);
    }
}