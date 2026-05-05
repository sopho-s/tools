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

std::string ToHexString(unsigned char data);
std::string ToHexString(unsigned char* data, int amount);
std::string ToHexString(uint16_t* data, int amount);
std::string ToHexString(uint16_t data);
std::string ToHexString(uint32_t* data, int amount);
std::string ToHexString(uint32_t data);
std::string ToIPString(unsigned char* data);

bool ArgExist(char** begin, char** end, const std::string& option);
char* GetArg(char** begin, char** end, const std::string& option);