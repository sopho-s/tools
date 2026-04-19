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
T EncryptXOR(T data, T key) {
    return data ^ key;
}

// Explicit template instantiations
template unsigned char* EncryptXOR<unsigned char>(unsigned char*, unsigned char, int);
template unsigned char* EncryptXOR<unsigned char>(unsigned char*, unsigned char*, int);
template unsigned char EncryptXOR<unsigned char>(unsigned char, unsigned char);
template char* EncryptXOR<char>(char*, char, int);
template char* EncryptXOR<char>(char*, char*, int);
template char EncryptXOR<char>(char, char);

unsigned char GFMult(unsigned char a, unsigned char b) {
    unsigned char result = 0;
    while (b != 0) {
        if (b & 1) {
            result ^= a;
        }
        if ((a & 0x80) == 0x80) {
            a <<= 1;
            a ^= 0x1B;
        } else {
            a <<= 1;
        }
        b >>= 1;
    }
    return result;
}

unsigned char GFInv(unsigned char a) {
    if (a == 0) {
        return 0;
    }
    for (int i = 1; i < 256; i++) {
        if (GFMult(a, i) == 1) {
            return i;
        }
    }
    return 0;
}

unsigned char AffineTransform(unsigned char b) {
    unsigned char result = 0;
    for (int i = 0; i < 8; i++) {
        unsigned char bit = 0;
        for (int t = 0; t < 8; t++) {
            if (((t - i) % 8) < 5) {
                bit ^= (b >> t) & 1;
            }
        }
        bit ^= (0x63 >> i) & 1;
        result |= bit << i;
    }
    return result;
}

unsigned char* GetSBox() {
    unsigned char* sbox = new unsigned char[256];
    for (int i = 0; i < 256; i++) {
        sbox[i] = AffineTransform(GFInv(i));
    }
    return sbox;
}

void RotWord(unsigned char* data) {
    unsigned char copy[4] = {data[0], data[1], data[2], data[3]};
    for (int i = 0; i < WORDSIZE; i++) {
        data[i] = copy[(i + 1) % 4];
    }
}

void SubWord(unsigned char* data) {
    unsigned char* sbox = GetSBox();
    for (int i = 0; i < WORDSIZE; i++) {
        data[i] = sbox[data[i]];
    }
    delete[] sbox;
}

unsigned char* ExpandRoundKey128(unsigned char* key) {
    unsigned char* keys = new unsigned char[11 * WORDSIZE * 4];
    memcpy(keys, key, WORDSIZE * 4);
    unsigned char rcon = 1;
    for (int i = 1; i < ROUNDAMOUNT; i++) {
        unsigned char* keylast = new unsigned char[WORDSIZE];
        memcpy(keylast, keys + (i - 1) * WORDSIZE * 4 + 3 * WORDSIZE, WORDSIZE);
        RotWord(keylast);
        SubWord(keylast);
        keylast[0] ^= rcon;
        unsigned char* xorresult = EncryptXOR<unsigned char>(keys + (i - 1) * WORDSIZE * 4, keylast, WORDSIZE);
        memcpy(keys + i * WORDSIZE * 4, xorresult, WORDSIZE);
        delete[] xorresult;
        for (int t = 1; t < 4; t++) {
            xorresult = EncryptXOR<unsigned char>(keys + (i - 1) * WORDSIZE * 4 + t * WORDSIZE, keys + i * WORDSIZE * 4 + (t - 1) * WORDSIZE, WORDSIZE);
            memcpy(keys + i * WORDSIZE * 4 + t * WORDSIZE, xorresult, WORDSIZE);
            delete[] xorresult;
        }
        delete[] keylast;
        rcon = (rcon << 1) ^ ((rcon & 0x80) ? 0x1B : 0);
    }
    return keys;
}

void AddRoundKey128(unsigned char* data, unsigned char* key) {
    for (int i = 0; i < 16; i++) {
        data[i] ^= key[i];
    }
}

void SubBytes128(unsigned char* data, unsigned char sbox[]) {
    for (int i = 0; i < 16; i++) {
        data[i] = sbox[data[i]];
    }
}

void ShiftRows128(unsigned char* data) {
    for (int i = 0; i < 4; i++) {
        unsigned char copy[] = {data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]};
        for (int t = 0; t < 4; t++) {
            data[i * 4 + (t + i) % 4] = copy[t];
        }
    }
}

void MixColumns128(unsigned char* data) {
    unsigned char state[4] = {2, 3, 1, 1};
    for (int i = 0; i < 4; i++) {
        for (int t = 0; t < 4; t++) {
            data[t] ^= GFMult(data[t], data[i + t*4]);
        }
        unsigned char copy[4] = {state[0], state[1], state[2], state[3]};
        for (int t = 0; t < 4; t++) {
            state[(t + 1) % 4] = copy[t];
        }
    }
}

void EncryptAES128(unsigned char* data, unsigned char* key) {
    unsigned char* roundkeys = ExpandRoundKey128(key);
    unsigned char* sbox = GetSBox();
    AddRoundKey128(data, roundkeys);
    for (int i = 0; i < 10; i++) {
        SubBytes128(data, sbox);
        ShiftRows128(data);
        MixColumns128(data);
        AddRoundKey128(data, roundkeys + i * WORDSIZE * 4);
    }
    SubBytes128(data, sbox);
    ShiftRows128(data);
    AddRoundKey128(data, roundkeys + 10 * WORDSIZE * 4);
    delete[] roundkeys;
    delete[] sbox;
}

void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[]) {
    for (int i = 0; i < 2; i++) {
        
    }
}

AESState AESPRNGInit(uint32_t seed) {
    uint8_t seedmaterial[6] = {0};
    std::memcpy(seedmaterial, &seed, 4);

}