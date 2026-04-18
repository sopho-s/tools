#include "crypto.h"
template <typename T>
T* EncryptXOR(T* data, T key, int amount) {
    T* out = new T[amount];
    for (int i = 0; i < amount; i++) {
        out[i] = data[i] ^ key;
    }
    return out;
}
template <typename T>
T* EncryptXOR(T* data, T* key, int amount) {
    T* out = new T[amount];
    for (int i = 0; i < amount; i++) {
        out[i] = data[i] ^ key[i];
    }
    return out;
}
template <typename T>
inline T EncryptXOR(T data, T key) {
    return data ^ key;
}

inline char GFMult(char a, char b) {
    char result = 0;
    while (b != 0) {
        if (b & 1) {
            result ^= a;
        }
        if (a & 0x80 == 0x80) {
            a <<= 1;
            a ^= 0x1B;
        } else {
            a <<= 1;
        }
    }
    return result;
}

inline char GFInv(char a) {
    if (a == 0) {
        return 0;
    }
    for (int i = 1; i < 256; i++) {
        if (GFMult(a, i) == 1) {
            return i;
        }
    }
}

inline char AffineTransform(char b) {
    char result = 0;
    for (int i = 0; i < 8; i++) {
        char bit = 0;
        for (int t = 0; t < 8; t++) {
            if (((t - i) % 8) < 5) {
                bit ^= (b >> t) & 1;
            }
        }
        bit ^= (0x63 >> i) & i;
        result |= bit << i;
    }
    return result;
}

inline char* GetSBox() {
    char* sbox = new char[256];
    for (int i = 0; i < 256; i++) {
        sbox[i] = AffineTransform(GFInv(i));
    }
    return sbox;
}

void ExpandRoundKey128(char *key) {
    
}

void AddRoundKey128(char *data, char* key) {
    for (int i = 0; i < 16; i++) {
        data[i] ^= key[i];
    }
}

void SubBytes128(char *data, char sbox[]) {
    for (int i = 0; i < 16; i++) {
        data[i] = sbox[(size_t)data];
    }
}

void ShiftRows128(char *data) {
    for (int i = 0; i < 4; i++) {
        char copy[] = {data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]};
        for (int t = 0; t < 4; t++) {
            data[i * 4 + (t + i) % 4] = copy[t];
        }
    }
}

void MixColumns128(char *data) {
    char state[4] = {2, 3, 1, 1};
    for (int i = 0; i < 4; i++) {
        for (int t = 0; t < 4; t++) {
            data[t] ^= GFMult(data[t], data[i + t*4]);
        }
        char copy[4] = {state[0], state[1], state[2], state[3]};
        for (int t = 0; t < 4; t++) {
            state[(t + 1) % 4] = copy[t];
        }
    }
}

void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[]) {
    for (int i = 0; i < 2; i++) {

    }
}

AESState AESPRNGInit(uint32_t seed) {
    uint8_t seedmaterial[6] = {0};
    std::memcpy(seedmaterial, &seed, 4);

}