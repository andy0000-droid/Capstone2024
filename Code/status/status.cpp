#include "status.h"
#include "integrity_check.h"

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
    /*
    모듈의 버전 및 상태 출력
    */
    std::cout << "현재 상태: Idle ( 상태)" << std::endl;
    std::cout << "Version : 0.0.1" << std::endl;
    std::cout << "Name : sdfsdfsd" << std::endl;
    // 추가적인 처리 로직
}

void InitializationState::handle() {
    /*
    모듈의 전역 변수 초기화
    */
    std::cout << "현재 상태: Initialization (초기화 상태)" << std::endl;
    // 초기화 작업 처리 로직
}

void SelfTestState::handle() {
    /*
    자가 시험 호출
    */
    std::cout << "자가시험 상태: 무결성 검증을 시작합니다." << std::endl;
    //제로화 기능 추가
}

void TestmodeState::handle() {
    /*
    자가 시험 수행
    */
    std::cout << "조건부 자가시험을 수행합니다." << std::endl;
    if (knownAnswerTest()) {
        std::cout << "기지 답안 시험 통과: 알고리즘이 예상대로 동작합니다." << std::endl;
    }
    else {
        std::cerr << "기지 답안 시험 실패: 알고리즘이 예상대로 동작하지 않습니다!" << std::endl;
        //InitializationState;
    }

    if (keyPairMatchTest()) {
        std::cout << "암호키 쌍 일치 시험 통과: 암호화와 복호화 키가 일치합니다." << std::endl;
    }
    else {
        std::cerr << "암호키 쌍 일치 시험 실패: 암호화와 복호화 키가 일치하지 않습니다!" << std::endl;
        InitializationState;
    }
}

void ErrorState::handle() {
    /*
    에러 처리
    */
    std::cout << "현재 상태: Error" << std::endl;
    if (errorSeverity == SERIOUS_ERROR) {
        std::cout << "심각한 오류 발생. 시스템 종료 필요." << std::endl;
        // ShutdownState.handle();
        // Perform system shutdown or logging
    }
    else if (errorSeverity == SIMPLE_ERROR) {
        std::cout << "단순한 오류. 복구 가능.ERROR MESSAGE" << std::endl;
        std::cout << "오류 내용: " << errorMessage << std::endl;  // 오류 메시지 출력
        // Recoverable error handling
    }
}

void ShutdownState::handle() {
    /*
    종료
    */
    std::cout << "현재 상태: Shutdown (종료 상태)" << std::endl;
    exit(1);
}