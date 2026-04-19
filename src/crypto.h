#include <cstdint>
#include <cstring>

#pragma once
#define WORDSIZE 4
#define ROUNDAMOUNT 11

template <typename T>
T* EncryptXOR(T* data, T key, int amount);
template <typename T>
T* EncryptXOR(T* data, T* key, int amount);
template <typename T>
inline T EncryptXOR(T data, T key);

struct AESState {
    uint8_t key[32] = {0};
    uint8_t counter[16] = {0};
    uint64_t reseedcounter = 0;
};

unsigned char GFMult(unsigned char a, unsigned char b);
unsigned char GFInv(unsigned char a);
unsigned char AffineTransform(unsigned char b);
unsigned char* GetSBox();

unsigned char* ExpandRoundKey128(unsigned char *key);

void AddRoundKey128(unsigned char *data, unsigned char* key);
void SubBytes128(unsigned char *data, unsigned char sbox[]);
void ShiftRows128(unsigned char *data);
void MixColumns128(unsigned char *data);
void EncryptAES128(unsigned char* data, unsigned char* key);

void InvSubBytes128(unsigned char *data, unsigned char sbox[]);
void InvShiftRows128(unsigned char *data);
void InvMixColumns128(unsigned char *data);
void DecryptAES128(unsigned char* data);

void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[]);
AESState AESPRNGInit(uint32_t seed);