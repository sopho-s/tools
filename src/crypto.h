#include <cstdint>
#include <cstring>

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

inline char GFMult(char a, char b);
inline char GFInv(char a);
inline char AffineTransform(char b);
inline char* GetSBox();

void SubBytes128(char *data, char sbox[]);
void ShiftRows128(char *data);
void EncryptAES128(char* data);

void CTRDRBGUpdate(AESState &state, uint8_t seedmaterial[]);
AESState AESPRNGInit(uint32_t seed);