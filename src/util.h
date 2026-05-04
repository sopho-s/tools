#include <cstring>
#include <vector>
#include <string>
#include <iostream>
#pragma once
template <typename T>
T* PadTo(T* data, int curramount, int finalamount, T padval);

std::vector<std::string> split(std::string s, std::string delim);

std::string ToHexString(unsigned char* data, int amount);
std::string ToIPString(unsigned char* data);