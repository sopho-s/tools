#include "command.h"

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
