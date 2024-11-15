#pragma once

#ifndef CRYPTMODULE_H
#define CRYPTMODULE_H

#include <iostream>
#include <memory>

class State {
public:
    virtual ~State() {}
    virtual void handle() = 0; 
};

class CryptoModule {
public:
    void setState(std::unique_ptr<State> newState);
    void process();

    std::unique_ptr<State>& getState() { return state; }
private:
    std::unique_ptr<State> state;
};

class IdleState : public State {
public:
    void handle() override;
};

class InitializationState : public State {
public:
    void handle() override;
};

class SelfTestState : public State {
public:
    void handle() override;
};

class TestmodeState : public State {
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
    std::string errorMessage;
};

class ShutdownState : public State {
public:
    void handle() override;
};

#endif 