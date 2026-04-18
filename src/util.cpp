#include "util.h"
template <typename T>
T* PadTo(T* data, int curramount, int finalamount, T padval) {
    T* newdata = new T[finalamount];
    std::memcpy(newdata, data, sizeof(T) * finalamount);
    std::memset(newdata + curramount, padval, finalamount - curramount);
    return newdata;
}

std::vector<std::string> split(std::string s, std::string delim)
{
    std::vector<std::string> res;
    int pos = 0;
    while (true)
    {
        pos = s.find(delim);
        if (pos == -1) {
            res.push_back(s);
            break;
        }
        res.push_back(s.substr(0, pos));
        s.erase(0, pos + delim.size());
    }
    return res;
}