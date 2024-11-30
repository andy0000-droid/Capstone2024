// #pragma warning(disable:4996)

#include "file_check.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

// #define _CRT_SECURE_NO_WARNINGS

std::string CalculateSHA256(const std::string& file_path) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    char buf[8192];
    std::ifstream file(file_path, std::ios::binary);

    if (!file) {
        throw std::runtime_error("파일을 열 수 없습니다: " + file_path);
    }

    SHA256_CTX sha256;
    SHA256_Init(&sha256);

    while (file.good()) {
        file.read(buf, sizeof(buf));
        SHA256_Update(&sha256, buf, file.gcount());
    }

    SHA256_Final(hash, &sha256);

    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        result << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return result.str();
}

bool VerifyIntegrity(const std::string& dll_path, const std::string& expected_hash) {
    try {
        std::string calculated_hash = CalculateSHA256(dll_path);
        if (calculated_hash == expected_hash) {
            std::cout << "무결성 검증 성공: 파일이 변조되지 않았습니다." << std::endl;
            return true;
        }
        else {
            std::cerr << "무결성 검증 실패: 파일이 변조되었을 수 있습니다." << std::endl;
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "무결성 검증 오류: " << e.what() << std::endl;
        return false;
    }
}

int main() {
    std::string dll_path = "C:\\Users\\김호준\\Desktop\\Years\\2024\\CapstoneProject\\capstoneproject\\dllmain.cpp";
    std::string expected_hash = "b88f753eefec22ab11a80460cbcc3e75b24bfeb0c5f3dfd27a6a1672243a05ac"; // 예상 해시 값 입력

    VerifyIntegrity(dll_path, expected_hash);

    return 0;
}