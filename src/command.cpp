#include "command.h"

namespace tools
{
    namespace command
    {
#ifdef __linux__
        std::string Execute(std::string command)
        {
            char buffer[128];
            std::string result;
            FILE *pipe = popen((command + " 2>&1").c_str(), "r");
            if (!pipe)
                throw std::runtime_error("popen() failed");
            while (fgets(buffer, 128, pipe) != nullptr)
            {
                result += buffer;
            }
            pclose(pipe);
            return result;
        }
#elif _WIN32
        std::string Execute(std::string command)
        {
            char buffer[128];
            std::string result;
            FILE *pipe = _popen(command.c_str(), "r");
            if (!pipe)
                throw std::runtime_error("popen() failed");
            while (fgets(buffer, 128, pipe) != nullptr)
            {
                result += buffer;
            }
            _pclose(pipe);
            return result;
        }
#endif
    }
}