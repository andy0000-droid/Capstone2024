#pragma once

typedef unsigned char Byte;

typedef int(*AriaEncKeySetup)(const Byte* masterKey, Byte* encRoundKeys, int keyBits);
typedef int(*AriaDecKeySetup)(const Byte* masterKey, Byte* decRoundKeys, int keyBits);
typedef void(*AriaCrypt)(const Byte* plainText, int round, const Byte* roundKeys, Byte* cipherText);

extern AriaEncKeySetup ariaEncKeySetup;
extern AriaDecKeySetup ariaDecKeySetup;
extern AriaCrypt ariaCrypt;

enum DLLLOAD_STATUS {
	LOAD_SUCCESS = 0,
	LOAD_FAIL = 1,
	PROC_LOAD_FAIL = 2
};

extern DLLLOAD_STATUS loadARIA();
extern void freeARIA();
