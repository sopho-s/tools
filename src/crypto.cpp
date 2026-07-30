#include "crypto.h"

namespace tools
{
    namespace crypto
    {


        static const unsigned char sbox[256] = {
            0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
            0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
            0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
            0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
            0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
            0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
            0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
            0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
            0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
            0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
            0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
            0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
            0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
            0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
            0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
            0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

        static const char unsigned invsbox[256] = {
            0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
            0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
            0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
            0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
            0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
            0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
            0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
            0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
            0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
            0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
            0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
            0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
            0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
            0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
            0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
            0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };
        template <typename T>
        T *EncryptXOR(T *data, T key, int amount)
        {
            T *out = new T[amount];
            for (int i = 0; i < amount; i++)
            {
                out[i] = data[i] ^ key;
            }
            return out;
        }

        template <typename T>
        T *EncryptXOR(T *data, T *key, int amount)
        {
            T *out = new T[amount];
            for (int i = 0; i < amount; i++)
            {
                out[i] = data[i] ^ key[i];
            }
            return out;
        }

        template <typename T>
        T EncryptXOR(T data, T key)
        {
            return data ^ key;
        }

        // Explicit template instantiations
        template unsigned char *EncryptXOR<unsigned char>(unsigned char *, unsigned char, int);
        template unsigned char *EncryptXOR<unsigned char>(unsigned char *, unsigned char *, int);
        template unsigned char EncryptXOR<unsigned char>(unsigned char, unsigned char);
        template char *EncryptXOR<char>(char *, char, int);
        template char *EncryptXOR<char>(char *, char *, int);
        template char EncryptXOR<char>(char, char);

        unsigned char GFMult(unsigned char a, unsigned char b)
        {
            unsigned char result = 0;
            while (b != 0)
            {
                if (b & 1)
                {
                    result ^= a;
                }
                if ((a & 0x80) == 0x80)
                {
                    a <<= 1;
                    a ^= 0x1B;
                }
                else
                {
                    a <<= 1;
                }
                b >>= 1;
            }
            return result;
        }

        unsigned char GFInv(unsigned char a)
        {
            if (a == 0)
            {
                return 0;
            }
            for (int i = 1; i < 256; i++)
            {
                if (GFMult(a, i) == 1)
                {
                    return i;
                }
            }
            return 0;
        }

        unsigned char AffineTransform(unsigned char b)
        {
            unsigned char result = 0;
            for (int i = 0; i < 8; i++)
            {
                unsigned char bit = 0;
                for (int t = 0; t < 8; t++)
                {
                    if (((t - i) % 8) < 5)
                    {
                        bit ^= (b >> t) & 1;
                    }
                }
                bit ^= (0x63 >> i) & 1;
                result |= bit << i;
            }
            return result;
        }

        void RotWord(unsigned char *data)
        {
            unsigned char copy[4] = {data[0], data[1], data[2], data[3]};
            for (int i = 0; i < WORDSIZE; i++)
            {
                data[i] = copy[(i + 1) % 4];
            }
        }

        void SubWord(unsigned char *data)
        {
            for (int i = 0; i < WORDSIZE; i++)
            {
                data[i] = sbox[data[i]];
            }
        }

        unsigned char *ExpandRoundKey128(unsigned char *key)
        {
            unsigned char *keys = new unsigned char[11 * WORDSIZE * 4];
            memcpy(keys, key, WORDSIZE * 4);
            unsigned char rcon = 1;
            for (int i = 1; i < ROUNDAMOUNT; i++)
            {
                unsigned char *keylast = new unsigned char[WORDSIZE];
                memcpy(keylast, keys + (i - 1) * WORDSIZE * 4 + 3 * WORDSIZE, WORDSIZE);
                RotWord(keylast);
                SubWord(keylast);
                keylast[0] ^= rcon;
                unsigned char *xorresult = EncryptXOR<unsigned char>(keys + (i - 1) * WORDSIZE * 4, keylast, WORDSIZE);
                memcpy(keys + i * WORDSIZE * 4, xorresult, WORDSIZE);
                delete[] xorresult;
                for (int t = 1; t < 4; t++)
                {
                    xorresult = EncryptXOR<unsigned char>(keys + (i - 1) * WORDSIZE * 4 + t * WORDSIZE, keys + i * WORDSIZE * 4 + (t - 1) * WORDSIZE, WORDSIZE);
                    memcpy(keys + i * WORDSIZE * 4 + t * WORDSIZE, xorresult, WORDSIZE);
                    delete[] xorresult;
                }
                delete[] keylast;
                rcon = (rcon << 1) ^ ((rcon & 0x80) ? 0x1B : 0);
            }
            return keys;
        }

        void AddRoundKey128(unsigned char *data, unsigned char *key)
        {
            for (int i = 0; i < 16; i++)
            {
                data[i] ^= key[i];
            }
        }

        void SubBytes128(unsigned char *data)
        {
            for (int i = 0; i < 16; i++)
            {
                data[i] = sbox[data[i]];
            }
        }

        void ShiftRows128(unsigned char *data)
        {
            for (int i = 0; i < 4; i++)
            {
                unsigned char copy[] = {data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]};
                for (int t = 0; t < 4; t++)
                {
                    data[i * 4 + t] = copy[(t + i) % 4];
                }
            }
        }

        void MixColumns128(unsigned char *data)
        {
            for (int i = 0; i < 4; i++) {
                unsigned char a0 = data[i * 4];
                unsigned char a1 = data[i * 4 + 1];
                unsigned char a2 = data[i * 4 + 2];
                unsigned char a3 = data[i * 4 + 3];
                data[i * 4] = GFMult(2, a0) ^ GFMult(3, a1) ^ a2 ^ a3;
                data[i * 4 + 1] = a0 ^ GFMult(2, a1) ^ GFMult(3, a2) ^ a3;
                data[i * 4 + 2] = a0 ^ a1 ^ GFMult(2, a2) ^ GFMult(3, a3);
                data[i * 4 + 3] = GFMult(3, a0) ^ a1 ^ a2 ^ GFMult(2, a3);

            }
        }

        void EncryptAES128(unsigned char *data, unsigned char *key)
        {
            unsigned char *roundkeys = ExpandRoundKey128(key);
            AddRoundKey128(data, roundkeys);
            for (int i = 0; i < ROUNDAMOUNT - 1; i++)
            {
                SubBytes128(data);
                ShiftRows128(data);
                MixColumns128(data);
                AddRoundKey128(data, roundkeys + i * WORDSIZE * 4);
            }
            SubBytes128(data);
            ShiftRows128(data);
            AddRoundKey128(data, roundkeys + 10 * WORDSIZE * 4);
            delete[] roundkeys;
        }

        void InvSubBytes128(unsigned char *data) {
            for (int i = 0; i < 16; i++)
            {
                data[i] = invsbox[data[i]];
            }
        }
        void InvShiftRows128(unsigned char *data) {
            for (int i = 0; i < 4; i++)
            {
                unsigned char copy[] = {data[i * 4], data[i * 4 + 1], data[i * 4 + 2], data[i * 4 + 3]};
                for (int t = 0; t < 4; t++)
                {
                    data[i * 4 + (t + i) % 4] = copy[t];
                }
            }
        }
        void InvMixColumns128(unsigned char *data) {
            for (int i = 0; i < 4; i++) {
                unsigned char a0 = data[i * 4];
                unsigned char a1 = data[i * 4 + 1];
                unsigned char a2 = data[i * 4 + 2];
                unsigned char a3 = data[i * 4 + 3];
                data[i * 4] = GFMult(14, a0) ^ GFMult(11, a1) ^ GFMult(13, a2) ^ GFMult(9, a3);
                data[i * 4 + 1] = GFMult(9, a0) ^ GFMult(14, a1) ^ GFMult(11, a2) ^ GFMult(13, a3);
                data[i * 4 + 2] = GFMult(13, a0) ^ GFMult(9, a1) ^ GFMult(14, a2) ^ GFMult(11, a3);
                data[i * 4 + 3] = GFMult(11, a0) ^ GFMult(13, a1) ^ GFMult(9, a2) ^ GFMult(14, a3);
            }
        }

        void DecryptAES128(unsigned char *data, unsigned char *key)
        {
            unsigned char *roundkeys = ExpandRoundKey128(key);
            AddRoundKey128(data, roundkeys + 10 * WORDSIZE * 4);
            InvShiftRows128(data);
            InvSubBytes128(data);
            for (int i = 0; i < ROUNDAMOUNT - 1; i++)
            {
                AddRoundKey128(data, roundkeys + 10 * WORDSIZE * 4 - (i + 1) * WORDSIZE * 4);
                InvMixColumns128(data);
                InvShiftRows128(data);
                InvSubBytes128(data);
            }
            AddRoundKey128(data, roundkeys);
            delete[] roundkeys;
         }

        void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[])
        {
            for (int i = 0; i < 2; i++)
            {
            }
        }

        AESState AESPRNGInit(uint32_t seed)
        {
            uint8_t seedmaterial[6] = {0};
            std::memcpy(seedmaterial, &seed, 4);
            return AESState();
        }
    }
}