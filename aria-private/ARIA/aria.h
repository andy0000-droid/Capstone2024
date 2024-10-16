#pragma once
#ifdef ARIA_EXPORTS
#define ARIA_API __declspec(dllexport)
#else
#define ARIA_API
#endif
#pragma once
typedef unsigned char Byte;

extern "C" ARIA_API int AriaEncKeySetup(const Byte* w0, Byte* e, int keyBits);
extern "C" ARIA_API int AriaDecKeySetup(const Byte* w0, Byte* d, int keyBits);
extern "C" ARIA_API void AriaCrypt(const Byte* p, int R, const Byte* e, Byte* c);
extern "C" ARIA_API int ARIA_test();

