#include <cstdint>
#include <cstring>
#include <algorithm>
#include "util.h"
#include "maths.h"

#pragma once
#define WORDSIZE 4
#define ROUNDAMOUNT 11
using namespace tools::util;
using namespace tools::maths;
namespace tools
{
    namespace crypto
    {

        template <typename T>
        T *EncryptXOR(T *data, T key, int amount);
        template <typename T>
        T *EncryptXOR(T *data, T *key, int amount);
        template <typename T>
        inline T EncryptXOR(T data, T key);

        struct AESState
        {
            uint8_t key[32] = {0};
            uint8_t counter[16] = {0};
            uint64_t reseedcounter = 0;
        };

        struct RSAPublicKey 
        {
            unsigned int e;
            unsigned int n;
        };

        struct RSAPrivateKey
        {
            unsigned int d;
            unsigned int n;
        };

        struct NTRUPrivateKey
        {
            ConvolutionRingPolynomial f;
            ConvolutionRingPolynomial fp;
        };

        struct NTRUPublicKey
        {
            ConvolutionRingPolynomial h;
        };

        std::pair<RSAPublicKey, RSAPrivateKey> RSAGenerateKeyPair(int p, int q);
        template <typename T>
        void RSAEncrypt(T &message, RSAPublicKey key);
        template <typename T>
        void RSADecrypt(T &message, RSAPrivateKey key);

        std::pair<NTRUPublicKey, NTRUPrivateKey> NTRUGenerateKeyPair(int p, int q);

        unsigned char GFMult(unsigned char a, unsigned char b);
        unsigned char GFInv(unsigned char a);
        unsigned char AffineTransform(unsigned char b);
        unsigned char *GetSBox();

        unsigned char *ExpandRoundKey128(unsigned char *key);

        void AddRoundKey128(unsigned char *data, unsigned char *key);
        void SubBytes128(unsigned char *data);
        void ShiftRows128(unsigned char *data);
        void MixColumns128(unsigned char *data);
        void EncryptAES128(unsigned char *data, unsigned char *key);

        void InvSubBytes128(unsigned char *data);
        void InvShiftRows128(unsigned char *data);
        void InvMixColumns128(unsigned char *data);
        void DecryptAES128(unsigned char *data, unsigned char *key);

        void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[]);
        AESState AESPRNGInit(uint32_t seed);
    }
}