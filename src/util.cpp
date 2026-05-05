#include "util.h"
template <typename T>
T* PadTo(T* data, int curramount, int finalamount, T padval) {
    T* newdata = new T[finalamount];
    std::memcpy(newdata, data, sizeof(T) * curramount);
    std::memset(newdata + curramount, padval, finalamount - curramount);
    return newdata;
}

// Explicit template instantiations
template char* PadTo<char>(char*, int, int, char);
template int* PadTo<int>(int*, int, int, int);
template unsigned char* PadTo<unsigned char>(unsigned char*, int, int, unsigned char);

std::vector<std::string> split(std::string s, std::string delim)
{
    std::vector<std::string> res;
    int pos = 0;
    while (true)
    {
        pos = s.find(delim);
        if (pos == -1) {
            if (s.length() != 0) {
                res.push_back(s);
            }
            break;
        }
        res.push_back(s.substr(0, pos));
        s.erase(0, pos + delim.size());
    }
    return res;
}

std::string ToHexString(unsigned char data) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    hexstring += hex_chars[ ( data & 0xF0 ) >> 4 ];
    hexstring += hex_chars[ ( data & 0x0F ) >> 0 ];
    return hexstring;
}

std::string ToHexString(unsigned char* data, int amount) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int i = 0; i < amount; i++) {
        hexstring += hex_chars[ ( data[i] & 0xF0 ) >> 4 ];
        hexstring += hex_chars[ ( data[i] & 0x0F ) >> 0 ];
        if (i != amount - 1) {
            hexstring += " ";
        }
    }
    return hexstring;
}

std::string ToHexString(uint16_t* data, int amount) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int i = 0; i < amount; i++) {
        uint16_t currdata = data[i];
        for (int t = 0; t < 2; t++) {
            hexstring += hex_chars[ ( currdata & 0xF0 ) >> 4 ];
            hexstring += hex_chars[ ( currdata & 0x0F ) >> 0 ];
            if (t != 1) {
                hexstring += " ";
            }
            currdata = currdata >> 8;
        }
        if (i != amount - 1) {
            hexstring += " ";
        }
    }
    return hexstring;
}

std::string ToHexString(uint16_t data) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int t = 0; t < 2; t++) {
        hexstring += hex_chars[ ( data & 0xF0 ) >> 4 ];
        hexstring += hex_chars[ ( data & 0x0F ) >> 0 ];
        if (t != 1) {
            hexstring += " ";
        }
        data = data >> 8;
    }
    return hexstring;
}



std::string ToHexString(uint32_t* data, int amount) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int i = 0; i < amount; i++) {
        uint32_t currdata = data[i];
        for (int t = 0; t < 4; t++) {
            hexstring += hex_chars[ ( currdata & 0xF0 ) >> 4 ];
            hexstring += hex_chars[ ( currdata & 0x0F ) >> 0 ];
            if (t != 3) {
                hexstring += " ";
            }
            currdata = currdata >> 8;
        }
        if (i != amount - 1) {
            hexstring += " ";
        }
    }
    return hexstring;
}

std::string ToHexString(uint32_t data) {
    std::string hexstring = "";
    char const hex_chars[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
    for (int t = 0; t < 4; t++) {
        hexstring += hex_chars[ ( data & 0xF0 ) >> 4 ];
        hexstring += hex_chars[ ( data & 0x0F ) >> 0 ];
        if (t != 3) {
            hexstring += " ";
        }
        data = data >> 8;
    }
    return hexstring;
}

std::string ToIPString(unsigned char* data) {
    std::string IP = std::to_string((uint8_t)data[0]) + "." + std::to_string((uint8_t)data[1]) + "." + std::to_string((uint8_t)data[2]) + "." +std::to_string((uint8_t)data[3]);
    return IP;
}

std::string ToIPString(uint32_t data) {
    std::string IP = std::to_string((uint8_t)((data & (0xFF << 24)) >> 24)) + "." + std::to_string((uint8_t)((data & (0xFF << 16)) >> 16)) + "." + std::to_string((uint8_t)((data & (0xFF << 8)) >> 8)) + "." +std::to_string((uint8_t)(data & 0xFF));
    return IP;
}

bool ArgExist(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

char* GetArg(char** begin, char** end, const std::string& option) {
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}