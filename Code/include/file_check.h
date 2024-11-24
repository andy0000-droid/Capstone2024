#ifndef FILE_CHECK_H
#define FILE_CHECK_H

#include <string>

// KCMVP 무결성 검증을 위한 SHA-256 해시 계산 함수 선언
std::string CalculateSHA256(const std::string& file_path);

// dll 파일의 무결성을 검증하는 함수 선언
bool VerifyIntegrity(const std::string& dll_path, const std::string& expected_hash);

#endif // FILE_CHECK_H