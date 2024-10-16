#include "aria.h"
#include "data.h"

// Diffusion Layer
static void DL(const Byte* i, Byte* o)
{
	Byte T;

	T = i[3] ^ i[4] ^ i[9] ^ i[14];
	o[0] = i[6] ^ i[8] ^ i[13] ^ T;
	o[5] = i[1] ^ i[10] ^ i[15] ^ T;
	o[11] = i[2] ^ i[7] ^ i[12] ^ T;
	o[14] = i[0] ^ i[5] ^ i[11] ^ T;
	T = i[2] ^ i[5] ^ i[8] ^ i[15];
	o[1] = i[7] ^ i[9] ^ i[12] ^ T;
	o[4] = i[0] ^ i[11] ^ i[14] ^ T;
	o[10] = i[3] ^ i[6] ^ i[13] ^ T;
	o[15] = i[1] ^ i[4] ^ i[10] ^ T;
	T = i[1] ^ i[6] ^ i[11] ^ i[12];
	o[2] = i[4] ^ i[10] ^ i[15] ^ T;
	o[7] = i[3] ^ i[8] ^ i[13] ^ T;
	o[9] = i[0] ^ i[5] ^ i[14] ^ T;
	o[12] = i[2] ^ i[7] ^ i[9] ^ T;
	T = i[0] ^ i[7] ^ i[10] ^ i[13];
	o[3] = i[5] ^ i[11] ^ i[14] ^ T;
	o[6] = i[2] ^ i[9] ^ i[12] ^ T;
	o[8] = i[1] ^ i[4] ^ i[15] ^ T;
	o[13] = i[3] ^ i[6] ^ i[8] ^ T;
}
// Right-rotate 128 bit source string s by n bits and XOR it to target string t
static void RotXOR(const Byte* s, int n, Byte* t)
{
	int i, q;

	q = n / 8; n %= 8;
	for (i = 0; i < 16; i++) {
		t[(q + i) % 16] ^= (s[i] >> n);
		if (n != 0) t[(q + i + 1) % 16] ^= (s[i] << (8 - n));
	}
}

// EnAriaCryption round key generation rountine
// w0 : master key, e : enAriaCryption round keys
int AriaEncKeySetup(const Byte* w0, Byte* e, int keyBits) {
	int  i, R = (keyBits + 256) / 32, q;
	Byte t[16], w1[16], w2[16], w3[16];

	q = (keyBits - 128) / 64;
	for (i = 0; i < 16; i++) t[i] = S[i % 4][KRK[q][i] ^ w0[i]];
	DL(t, w1);
	if (R == 14)
		for (i = 0; i < 8; i++) w1[i] ^= w0[16 + i];
	else if (R == 16)
		for (i = 0; i < 16; i++) w1[i] ^= w0[16 + i];

	q = (q == 2) ? 0 : (q + 1);
	for (i = 0; i < 16; i++) t[i] = S[(2 + i) % 4][KRK[q][i] ^ w1[i]];
	DL(t, w2);
	for (i = 0; i < 16; i++) w2[i] ^= w0[i];

	q = (q == 2) ? 0 : (q + 1);
	for (i = 0; i < 16; i++) t[i] = S[i % 4][KRK[q][i] ^ w2[i]];
	DL(t, w3);
	for (i = 0; i < 16; i++) w3[i] ^= w1[i];

	for (i = 0; i < 16 * (R + 1); i++) e[i] = 0;

	RotXOR(w0, 0, e); RotXOR(w1, 19, e);
	RotXOR(w1, 0, e + 16); RotXOR(w2, 19, e + 16);
	RotXOR(w2, 0, e + 32); RotXOR(w3, 19, e + 32);
	RotXOR(w3, 0, e + 48); RotXOR(w0, 19, e + 48);
	RotXOR(w0, 0, e + 64); RotXOR(w1, 31, e + 64);
	RotXOR(w1, 0, e + 80); RotXOR(w2, 31, e + 80);
	RotXOR(w2, 0, e + 96); RotXOR(w3, 31, e + 96);
	RotXOR(w3, 0, e + 112); RotXOR(w0, 31, e + 112);
	RotXOR(w0, 0, e + 128); RotXOR(w1, 67, e + 128);
	RotXOR(w1, 0, e + 144); RotXOR(w2, 67, e + 144);
	RotXOR(w2, 0, e + 160); RotXOR(w3, 67, e + 160);
	RotXOR(w3, 0, e + 176); RotXOR(w0, 67, e + 176);
	RotXOR(w0, 0, e + 192); RotXOR(w1, 97, e + 192);
	if (R > 12) {
		RotXOR(w1, 0, e + 208); RotXOR(w2, 97, e + 208);
		RotXOR(w2, 0, e + 224); RotXOR(w3, 97, e + 224);
	}
	if (R > 14) {
		RotXOR(w3, 0, e + 240); RotXOR(w0, 97, e + 240);
		RotXOR(w0, 0, e + 256); RotXOR(w1, 109, e + 256);
	}
	return R;
}

// DeAriaCryption round key generation rountine
// w0 : maskter key, d : deAriaCryption round keys
int AriaDecKeySetup(const Byte* w0, Byte* d, int keyBits) {
	int  i, j, R;
	Byte t[16];

	R = AriaEncKeySetup(w0, d, keyBits);
	for (j = 0; j < 16; j++) {
		t[j] = d[j];
		d[j] = d[16 * R + j];
		d[16 * R + j] = t[j];
	}
	for (i = 1; i <= R / 2; i++) {
		DL(d + i * 16, t);
		DL(d + (R - i) * 16, d + i * 16);
		for (j = 0; j < 16; j++) d[(R - i) * 16 + j] = t[j];
	}
	return R;
}

// EnAriaCryption and deAriaCryption rountine
// p: plain text, e: round keys, c: ciphertext
void AriaCrypt(const Byte* p, int R, const Byte* e, Byte* c)
{
	int i, j;
	Byte t[16];

	for (j = 0; j < 16; j++) c[j] = p[j];
	for (i = 0; i < R / 2; i++)
	{
		for (j = 0; j < 16; j++) t[j] = S[j % 4][e[j] ^ c[j]];
		DL(t, c); e += 16;
		for (j = 0; j < 16; j++) t[j] = S[(2 + j) % 4][e[j] ^ c[j]];
		DL(t, c); e += 16;
	}
	DL(c, t);
	for (j = 0; j < 16; j++) c[j] = e[j] ^ t[j];
}
