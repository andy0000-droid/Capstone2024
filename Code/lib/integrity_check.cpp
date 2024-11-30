#include <iostream>
#include <iomanip>
#include <cstring>
// #include "aria.h"
// #include "data.h"

void printHex(const Byte* data, int length) {
    for (int i = 0; i < length; ++i) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)data[i];
    }
    std::cout << std::dec << std::endl; // 10진수로 다시 변경
}

bool knownAnswerTest() {
    /*
    Change to TestVector
    */
    Byte key[32] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F };
    Byte plaintext[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    Byte expectedCiphertext[16] = { 0x1d, 0x6f, 0x56, 0x53, 0x19, 0x2c, 0xc0, 0x19, 0x0d, 0xf8, 0x5c, 0xa9, 0xde, 0xa6, 0xcd, 0xc4 };
    Byte ciphertext[16];
    Byte round_keys[512];

    memset(round_keys, 0, sizeof(round_keys));
    memset(ciphertext, 0, sizeof(ciphertext));

    int rounds = AriaEncKeySetup(key, round_keys, 256);
    AriaCrypt(plaintext, rounds, round_keys, ciphertext);

    // 예상 암호문과 생성된 암호문 비교
    bool result = true;
    for (int i = 0; i < 16; ++i) {
        if (ciphertext[i] != expectedCiphertext[i]) {
            std::cout << "Mismatch at byte " << i << ": "
                << "Generated = " << std::hex << static_cast<int>(ciphertext[i])
                << ", Expected = " << std::hex << static_cast<int>(expectedCiphertext[i]) << std::endl;
            result = false;
        }
    }
    std::cout << "Generated Ciphertext: ";
    printHex(ciphertext, 16);
    std::cout << "Expected Ciphertext: ";
    printHex(expectedCiphertext, 16);
    if (result == true) {
        std::cout << "Known Answer Test Result: Fail" << std::endl;
    }
    else {
        std::cout << "Known Answer Test Result: Pass" << std::endl;
        error = true;
    }
        

    return result;
}

// keyPairMatchTest 함수: 암호화/복호화 키 일치 여부 확인
bool keyPairMatchTest() {
    Byte key[32] = { 0x1F, 0x2E, 0x3D, 0x4C, 0x5B, 0x6A, 0x79, 0x88, 0x97, 0xA6, 0xB5, 0xC4, 0xD3, 0xE2, 0xF1, 0x10, 0x1F, 0x2E, 0x3D, 0x4C, 0x5B, 0x6A, 0x79, 0x88, 0x97, 0xA6, 0xB5, 0xC4, 0xD3, 0xE2, 0xF1, 0x10 };
    Byte plaintext[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF };
    Byte ciphertext[16], decrypted[16];
    Byte round_keys[512];

    memset(round_keys, 0, sizeof(round_keys));
    memset(ciphertext, 0, sizeof(ciphertext));
    memset(ciphertext, 0, sizeof(decrypted));

    // 암호화 및 복호화 라운드 키 생성
    int rounds = AriaEncKeySetup(key, round_keys, 256);
    AriaCrypt(plaintext, rounds, round_keys, ciphertext);
    AriaDecKeySetup(key, round_keys, 256);
    AriaCrypt(ciphertext, rounds, round_keys, decrypted);

    // 원본 평문과 복호화된 결과가 같은지 확인
    return std::memcmp(plaintext, decrypted, 16) == 0;
}

int main() {
    int a = knownAnswerTest();
    int b = keyPairMatchTest();

    std::cout << "Known Answer Test Result: " << a << std::endl;
    std::cout << "Key Pair Match Test Result: " << b << std::endl;
}