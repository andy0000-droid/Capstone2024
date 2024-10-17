#pragma once

#ifndef CRYPTMODULE_H
#define CRYPTMODULE_H

#include <iostream>
#include <memory>

// 상태 인터페이스
class State {
public:
    virtual ~State() {}
    virtual void handle() = 0;  // 상태가 처리할 작업 정의
};

// 암호 모듈 클래스 선언 (전이 메서드를 가질 수 있음)
class CryptoModule {
public:
    void setState(std::unique_ptr<State> newState);
    void process();

    std::unique_ptr<State>& getState() { return state; }
private:
    std::unique_ptr<State> state;
};

// 상태별 클래스 정의
class IdleState : public State {
public:
    void handle() override;
};

class InitializationState : public State {
public:
    void handle() override;
};

class EncryptionState : public State {
public:
    void handle() override;
};

class DecryptionState : public State {
public:
    void handle() override;
};

class ErrorState : public State {
public:
    enum ErrorSeverity {
        SIMPLE_ERROR = 1,
        SERIOUS_ERROR = 2
    };

    //ErrorState(ErrorSeverity severity) : errorSeverity(severity) {}
    ErrorState(ErrorSeverity severity, const std::string& message)
        : errorSeverity(severity), errorMessage(message) {}
    void handle() override;

private:
    ErrorSeverity errorSeverity;
};

class ShutdownState : public State {
public:
    void handle() override;
};




#endif 