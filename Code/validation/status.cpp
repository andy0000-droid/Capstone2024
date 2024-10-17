#include "status.h"

void CryptoModule::setState(std::unique_ptr<State> newState) {
    state = std::move(newState);
}

void CryptoModule::process() {
    if (state) {
        state->handle();
    }
}

// �� ������ ���� ����
void IdleState::handle() {
    std::cout << "���� ����: Idle ( ����)" << std::endl;
    // �߰����� ó�� ����
}

void InitializationState::handle() {
    std::cout << "���� ����: Initialization (�ʱ�ȭ ����)" << std::endl;
    // �ʱ�ȭ �۾� ó�� ����
}

void EncryptionState::handle() {
    std::cout << "���� ����: Encryption (��ȣȭ ����)" << std::endl;
    // ��ȣȭ �۾� ó�� ����
}

void DecryptionState::handle() {
    std::cout << "���� ����: Decryption (��ȣȭ ����)" << std::endl;
    // ��ȣȭ �۾� ó�� ����
}

void ErrorState::handle() {
    std::cout << "���� ����: Error" << std::endl;
    if (errorSeverity == SERIOUS_ERROR) {
        std::cout << "�ɰ��� ���� �߻�. �ý��� ���� �ʿ�." << std::endl;
        ShutdownState.handle();
        // Perform system shutdown or logging
    }
    else if(errorSeverity == SIMPLE_ERROR) {
        std::cout << "�ܼ��� ����. ���� ����.ERROR MESSAGE" << std::endl;
        std::cout << "���� ����: " << errorMessage << std::endl;  // ���� �޽��� ���
        // Recoverable error handling
    }
}

void ShutdownState::handle() {
    std::cout << "���� ����: Shutdown (���� ����)" << std::endl;
    exit(1);
}