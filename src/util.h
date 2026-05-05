#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdint.h>
#pragma once

template <typename T>
T* PadTo(T* data, int curramount, int finalamount, T padval);

std::vector<std::string> split(std::string s, std::string delim);

std::string ToHexString(const unsigned char data);
std::string ToHexString(const unsigned char* data, const int amount);
std::string ToHexString(const uint16_t* data, const int amount);
std::string ToHexString(const uint16_t data);
std::string ToHexString(const uint32_t* data, const int amount);
std::string ToHexString(const uint32_t data);
std::string ToIPString(const unsigned char* data);
std::string ToIPString(const uint32_t data);

bool ArgExist(char** begin, char** end, const std::string& option);
char* GetArg(char** begin, char** end, const std::string& option);