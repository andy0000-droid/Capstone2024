#include <iostream>
#include <fstream>
#include <memory>
#include <cstring>
#include "aria.h"

using namespace std;

void printBlock(Byte* b);
void printBlockOfLength(Byte* b, int len);

void test();

bool encryptFile(const char* srcFilePath, const char* destFilePath);
bool decryptFile(const char* srcFilePath, const char* destFilePath);
bool cryptFile(const char* srcFilePath, const char* destFilePath, bool decrypt = false);

// DLL에서 가져오는 함수
// 
// ariaEncKeySetup() : 마스터 키를 이용하여 암호화 라운드 키를 생성
// ariaDecKeySetup() : 마스터 키를 이용하여 복호화 라운드 키를 생성
// ariaCrypt() : 16byte 단위로 암호화/복호화 수행

int main() {
	auto status = loadARIA();
	if (status != DLLLOAD_STATUS::LOAD_SUCCESS) {
		cerr << "Failed to load ARIA.dll" << endl;
	}

	const char* filePath = "example.txt";
	bool success;
	void test();
	success = encryptFile("example.txt", "example.txt.enc");
	if (!success) return 1;

	success = decryptFile("example.txt.enc", "decrypted.txt");
	if (!success) return 1;

	freeARIA();
	return 0;
}

int reducePadding(Byte* target, int size) {
	auto lastPad = target[size - 1];

	return size - lastPad;
}

int getPadding(int target, int blockSize) {
	return blockSize - (target % blockSize);
}

int readBinaryFile(const char* filePath, Byte** buffer, bool isAddPad = false) {
	std::ifstream file(filePath, std::ios::binary);
	if (!file) {
		std::cerr << "File to read file : " << filePath << std::endl;
		return -1;
	}
	file.seekg(0, std::ios::end);
	auto fileSize = static_cast<int>(file.tellg());
	int bufferSize = 0;
	int pad = 0;

	if (isAddPad) {
		pad = getPadding(fileSize, 16);
		bufferSize = fileSize + pad;
	}
	else {
		bufferSize = fileSize;
	}
	*buffer = new Byte[bufferSize];
	memset(*buffer, pad, sizeof(Byte)* bufferSize);

	file.seekg(0, std::ios::beg);
	file.read(reinterpret_cast<char*>(*buffer), fileSize);
	file.close();

	return bufferSize;
}

int writeBinaryFile(const char* filePath, Byte* buffer, int size) {
	std::ofstream file(filePath, std::ios::binary);
	if (!file) {
		std::cerr << "Fail to write file : " << filePath << std::endl;
		return -1;
	}

	file.write(reinterpret_cast<char*>(buffer), size);
	file.close();

	return 0;
}

bool encryptFile(const char* srcFilePath, const char* destFilePath) {
	Byte* buffer = nullptr;
	int outputSize;

	// 파일을 바이너리로 읽고, 16byte 블록 단위로 패딩을 추가
	outputSize = readBinaryFile(srcFilePath, &buffer, true);

	if (outputSize < 0) return false;

	Byte masterKey[32] = { 0x00, };
	Byte roundKeys[16 * 17];
	Byte* cryptBuffer = new Byte[outputSize];

	// 테스트용 마스터 키 생성
	for (Byte i = 0; i < 16; i++) masterKey[i] = i * 0x11;
	for (Byte i = 16; i < 24; i++) masterKey[i] = (i - 16) * 0x11;

	int round;
	round = ariaEncKeySetup(masterKey, roundKeys, 192);

	// 16byte 단위 암호화
	// ECB 형태
	int offset = 0;
	Byte* partBuffer = buffer;
	Byte* partCryptBuffer = cryptBuffer;
	for (int i = 0; i < outputSize; i += 16) {
		ariaCrypt(partBuffer, round, roundKeys, partCryptBuffer);
		partBuffer += 16;
		partCryptBuffer += 16;
	}

	writeBinaryFile(destFilePath, cryptBuffer, outputSize);

	return true;
}

bool decryptFile(const char* srcFilePath, const char* destFilePath) {
	Byte* buffer = nullptr;
	int outputSize;

	outputSize = readBinaryFile(srcFilePath, &buffer);
	if (outputSize < 0) return false;

	Byte masterKey[32] = { 0x00, };
	Byte roundKeys[16 * 17];
	Byte* cryptBuffer = new Byte[outputSize];

	// 테스트용 마스터 키 생성
	for (Byte i = 0; i < 16; i++) masterKey[i] = i * 0x11;
	for (Byte i = 16; i < 24; i++) masterKey[i] = (i - 16) * 0x11;

	int round;
	round = ariaDecKeySetup(masterKey, roundKeys, 192);

	int offset = 0;
	Byte* partBuffer = buffer;
	Byte* partCryptBuffer = cryptBuffer;
	for (int i = 0; i < outputSize; i += 16) {
		ariaCrypt(partBuffer, round, roundKeys, partCryptBuffer);
		partBuffer += 16;
		partCryptBuffer += 16;
	}

	auto sizeWithoutPad = reducePadding(cryptBuffer, outputSize);

	writeBinaryFile(destFilePath, cryptBuffer, sizeWithoutPad);
	return true;
}

bool cryptFile(const char* srcFilePath, const char* destFilePath, bool decrypt) {
	Byte* buffer = nullptr;
	unsigned int outputSize;
	
	outputSize = readBinaryFile(srcFilePath, &buffer, true);

	if (outputSize) {
		Byte masterKey[32] = { 0x00, };
		Byte roundKeys[16 * 17];
		Byte* cryptBuffer = new Byte[outputSize];

		// 마스터 키 생성
		for (Byte i = 0; i < 16; i++) masterKey[i] = i * 0x11;
		for (Byte i = 16; i < 24; i++) masterKey[i] = (i - 16) * 0x11;

		int round;
		// 마스터 키로 라운드 키 생성
		if (decrypt) {
			round = ariaDecKeySetup(masterKey, roundKeys, 256);
		}
		else {
			round = ariaEncKeySetup(masterKey, roundKeys, 256);
		}

		// 암호화 수행
		ariaCrypt(reinterpret_cast<Byte*>(buffer), round, roundKeys, cryptBuffer);

		std::ofstream outFile(destFilePath);
		outFile.write(reinterpret_cast<char*>(cryptBuffer), outputSize);
		outFile.close();
	}
	else {
		std::cerr << "Fail to read" << std::endl;
		return false;
	}

	if (buffer != nullptr) {
		delete[] buffer;
	}

	return true;
}

void test() {
	Byte rk[16 * 17], c[16], mk[32];
	Byte p[16] = { 0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa,
	  0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb };
	const Byte AriaCryptResult[] = {
	  0x8d, 0x14, 0x70, 0x62, 0x5f, 0x59, 0xeb, 0xac,
	  0xb0, 0xe5, 0x5b, 0x53, 0x4b, 0x3e, 0x46, 0x2b };
	int i, flag;

	for (i = 0; i < 16; i++)
		mk[i] = i * 0x11;
	for (i = 16; i < 24; i++)
		mk[i] = (i - 16) * 0x11;

	ariaCrypt(p, ariaEncKeySetup(mk, rk, 192), rk, c);
	printf("BEGIN testing basic enAriaCryption...\n");
	printf("Testing whether the enAriaCryption would come out correctly, \
for 14-round ARIA.\n");
	printf("key      : "); printBlockOfLength(mk, 24); printf("\n");
	printf("plaintext: "); printBlock(p); printf("\n");
	printf("result is: "); printBlock(c); printf("\n");
	printf("should be: "); printBlock((Byte*)AriaCryptResult); printf("\n");
	flag = 0;
	for (i = 0; i < 16; i++)
		if (c[i] != AriaCryptResult[i])
			flag = 1;
	if (flag == 1)
		printf("The result is incorrect!\n");
	else
		printf("Okay.  The result is correct.\n");
	printf("END   testing basic enAriaCryption.\n\n");

	for (i = 0; i < 32; i++)
		mk[i] = 0;

	for (i = 0; i < 16; i++)
		p[i] = 0;

	printf("BEGIN testing the roundtrip...\n");
	printf("For key size of 256 bits, starting with \
the zero plaintext and the zero key, let's see if \
we may recover the plaintext by deAriaCrypting the \
enAriaCrypted ciphertext.\n");
	ariaEncKeySetup(mk, rk, 256);
	printf("plaintext : "); printBlock(p); printf("\n");
	ariaCrypt(p, 16, rk, c);
	printf("ciphertext: "); printBlock(c); printf("\n");
	ariaDecKeySetup(mk, rk, 256);
	ariaCrypt(c, 16, rk, p);
	printf("deAriaCrypted : "); printBlock(p); printf("\n");
	flag = 0;
	for (i = 0; i < 16; i++)
		if (p[i] != 0)
			flag = 1;
	if (flag == 1)
		printf("The result is incorrect!\n");
	else
		printf("Okay.  The result is correct.\n");
	printf("END   testing the roundtrip.\n");
}


void printBlockOfLength(Byte* b, int len) {
	int i;

	for (i = 0; i < len; i++, b++) {
		printf("%02x", *b);
		if (i % 4 == 3 && i < len - 1) printf(" ");
	}
}

void printBlock(Byte* b) {
	printBlockOfLength(b, 16);
}
