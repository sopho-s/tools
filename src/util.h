#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#include <cstdlib>
#pragma once

namespace tools
{
    namespace util
    {
        template <typename T>
        T *PadTo(T *data, int curramount, int finalamount, T padval);

        std::vector<std::string> Split(std::string s, std::string delim);
        std::string RecursiveReplace(std::string str, std::string from, std::string to);

        std::string ToHexString(const unsigned char data);
        std::string ToHexString(const unsigned char *data, const int amount);
        std::string ToHexString(const uint16_t *data, const int amount);
        std::string ToHexString(const uint16_t data);
        std::string ToHexString(const uint32_t *data, const int amount);
        std::string ToHexString(const uint32_t data);
        std::string ToIPString(const unsigned char *data);
        std::string ToIPString(const uint32_t data);

        uint32_t ToLowerIP(const uint32_t ip, const uint8_t cidr);
        uint32_t ToUpperIP(const uint32_t ip, const uint8_t cidr);

        bool ArgExist(char **begin, char **end, const std::string &option);
        char *GetArg(char **begin, char **end, const std::string &option);

        template <typename T>
        T GCD(T a, T b);
        template <typename T>
        T PowerMod(T base, T expo, T m);
        template <typename T>
        T ModInverse(T e, T phi);
    }
}