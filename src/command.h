#include <string>
#include <cstdlib>
#include "exceptions.h"
#pragma once

/// @brief Executes a shell command and returns its standard output
/// @param command The shell command to run
/// @return The command's stdout as a string
/// @throws std::runtime_error if popen() fails to start the process
std::string Execute(std::string command)
{
    char buffer[128];
    std::string result;
    FILE *pipe = popen(command.c_str(), "r");
    if (!pipe)
        throw std::runtime_error("popen() failed");
    while (fgets(buffer, 128, pipe) != nullptr)
    {
        result += buffer;
    }
    pclose(pipe);
    return result;
}
