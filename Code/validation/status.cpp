#include "status.h"

void CryptoModule::setState(std::unique_ptr<State> newState) {
    state = std::move(newState);
}

void CryptoModule::process() {
    if (state) {
        state->handle();
    }
}

// 각 상태의 동작 구현
void IdleState::handle() {
    std::cout << "현재 상태: Idle ( 상태)" << std::endl;
    // 추가적인 처리 로직
}

void InitializationState::handle() {
    std::cout << "현재 상태: Initialization (초기화 상태)" << std::endl;
    // 초기화 작업 처리 로직
}

void EncryptionState::handle() {
    std::cout << "현재 상태: Encryption (암호화 상태)" << std::endl;
    // 암호화 작업 처리 로직
}

void DecryptionState::handle() {
    std::cout << "현재 상태: Decryption (복호화 상태)" << std::endl;
    // 복호화 작업 처리 로직
}

void ErrorState::handle() {
    std::cout << "현재 상태: Error" << std::endl;
    if (errorSeverity == SERIOUS_ERROR) {
        std::cout << "심각한 오류 발생. 시스템 종료 필요." << std::endl;
        ShutdownState.handle();
        // Perform system shutdown or logging
    }
    else if(errorSeverity == SIMPLE_ERROR) {
        std::cout << "단순한 오류. 복구 가능.ERROR MESSAGE" << std::endl;
        std::cout << "오류 내용: " << errorMessage << std::endl;  // 오류 메시지 출력
        // Recoverable error handling
    }
}

void ShutdownState::handle() {
    std::cout << "현재 상태: Shutdown (종료 상태)" << std::endl;
    exit(1);
}