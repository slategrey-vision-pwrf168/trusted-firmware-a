/*
 * Copyright (c) 2026, Qualcomm Technologies, Inc. and/or its subsidiaries.
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Minimal SHA256 implementation for SEC.DAT hash verification.
 * This is a simplified version sufficient for hash verification only.
 */

#include <stdint.h>
#include <string.h>

#define SHA256_BLOCK_SIZE 64
#define SHA256_DIGEST_SIZE 32

/* SHA256 constants */
static const uint32_t sha256_k[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1,
	0x923f82a4, 0xab1c5ed5, 0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
	0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174, 0xe49b69c1, 0xefbe4786,
	0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147,
	0x06ca6351, 0x14292967, 0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
	0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85, 0xa2bfe8a1, 0xa81a664b,
	0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a,
	0x5b9cca4f, 0x682e6ff3, 0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
	0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

#define ROTR(x, n) (((x) >> (n)) | ((x) << (32 - (n))))
#define CH(x, y, z) (((x) & (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
#define SIGMA0(x) (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define SIGMA1(x) (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define GAMMA0(x) (ROTR(x, 7) ^ ROTR(x, 18) ^ ((x) >> 3))
#define GAMMA1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ ((x) >> 10))

static void sha256_process_block(uint32_t *h, const uint8_t *block)
{
	uint32_t w[64];
	uint32_t a, b, c, d, e, f, g, h_val;
	int i;

	/* Prepare message schedule */
	for (i = 0; i < 16; i++) {
		w[i] = ((uint32_t)block[i * 4] << 24) |
		       ((uint32_t)block[i * 4 + 1] << 16) |
		       ((uint32_t)block[i * 4 + 2] << 8) |
		       ((uint32_t)block[i * 4 + 3]);
	}

	for (i = 16; i < 64; i++) {
		w[i] = GAMMA1(w[i - 2]) + w[i - 7] + GAMMA0(w[i - 15]) +
		       w[i - 16];
	}

	/* Initialize working variables */
	a = h[0];
	b = h[1];
	c = h[2];
	d = h[3];
	e = h[4];
	f = h[5];
	g = h[6];
	h_val = h[7];

	/* Main loop */
	for (i = 0; i < 64; i++) {
		uint32_t t1 = h_val + SIGMA1(e) + CH(e, f, g) + sha256_k[i] + w[i];
		uint32_t t2 = SIGMA0(a) + MAJ(a, b, c);

		h_val = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	/* Add compressed chunk to current hash value */
	h[0] += a;
	h[1] += b;
	h[2] += c;
	h[3] += d;
	h[4] += e;
	h[5] += f;
	h[6] += g;
	h[7] += h_val;
}

void fuseprov_sha256(const uint8_t *data, size_t len, uint8_t *digest)
{
	uint32_t h[8] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};
	uint8_t block[SHA256_BLOCK_SIZE];
	uint64_t bit_len = len * 8;
	size_t i;

	/* Process complete 64-byte blocks */
	for (i = 0; i + SHA256_BLOCK_SIZE <= len; i += SHA256_BLOCK_SIZE) {
		sha256_process_block(h, &data[i]);
	}

	/* Handle remaining bytes */
	size_t remaining = len - i;
	memcpy(block, &data[i], remaining);
	block[remaining] = 0x80;
	memset(&block[remaining + 1], 0, SHA256_BLOCK_SIZE - remaining - 1);

	/* If remaining + 1 + 8 > 64, need another block */
	if (remaining + 1 + 8 > SHA256_BLOCK_SIZE) {
		sha256_process_block(h, block);
		memset(block, 0, SHA256_BLOCK_SIZE);
	}

	/* Append length in bits (big-endian) */
	for (i = 0; i < 8; i++) {
		block[SHA256_BLOCK_SIZE - 1 - i] = (bit_len >> (i * 8)) & 0xFF;
	}

	sha256_process_block(h, block);

	/* Convert hash to bytes (big-endian) */
	for (i = 0; i < 8; i++) {
		digest[i * 4] = (h[i] >> 24) & 0xFF;
		digest[i * 4 + 1] = (h[i] >> 16) & 0xFF;
		digest[i * 4 + 2] = (h[i] >> 8) & 0xFF;
		digest[i * 4 + 3] = h[i] & 0xFF;
	}
}
