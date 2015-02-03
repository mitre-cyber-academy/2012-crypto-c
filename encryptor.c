/*
 * encryptor.c
 *
 *  Created on: Jun 19, 2012
 *      Author: MLISKOV
 */

#include <stdio.h>
#include <stdlib.h>
#include <openssl/evp.h>
#include <gmp.h>

static void
usage(const char *prog)
{
  fprintf(stderr,
	  "Usage: %s iv input output\n", prog);
}

int main(int argc, char **argv)
{
	if (argc != 4) {
		usage(argv[0]);
		return 0;
	}
	FILE *ivfile = fopen(argv[1], "rb");
	FILE *in = fopen(argv[2], "rb");
	FILE *out = fopen(argv[3], "wb");
	size_t file_size;
	fseek(in, 0L, SEEK_END);
	file_size = ftell(in);
	fseek(in, 0L, SEEK_SET);
	void *raw = malloc(file_size);
	if (raw == NULL) {
		fprintf(stderr, "Error: couldn't allocate %d bytes of memory.\n", file_size);
		return 1;
	}
	fread(raw, file_size, sizeof(char), in);
	unsigned int key[4] = {0x61639627,0x9c974ae5,0x81e2f3bb,0x976751f7};
	// Not used, here to throw off anyone analyzing the binary
	unsigned int key2[4] = {0x03903752,0x6559b266,0x0e617736,0x9b56b490};
	unsigned int iv[2];
	int rv;
	if (2 !=(rv = fread(iv, sizeof(unsigned int), 2, ivfile))) {
		fprintf(stderr, "Error: file %s too short: %d\n", argv[1], rv);
		return 1;
	}
	fclose(ivfile);
	void *outbuf = malloc(file_size+8);
	if (outbuf == NULL) {
		fprintf(stderr, "Error: couldn't allocate %d bytes of memory.\n", file_size+8);
		return 1;
	}
	// First pass: encrypt with Blowfish.
	// Initialize cipher context.
	EVP_CIPHER_CTX ctx;
	EVP_CIPHER_CTX_init(&ctx);
	int outlen = 0;
	EVP_EncryptInit_ex(&ctx, EVP_bf_cbc(), NULL, (void *)key, (void *)iv);
	EVP_EncryptUpdate(&ctx, outbuf, &outlen, raw, file_size);
	if (file_size % 8 != 0) {
		int tmplen = 0;
		EVP_EncryptFinal_ex(&ctx, outbuf+(outlen/sizeof(unsigned int)), &tmplen);
		outlen += tmplen;
	}
	void *outbuf2 = malloc(file_size+8);
	if (outbuf2 == NULL) {
		fprintf(stderr, "Error: couldn't allocate %d bytes of memory.\n", file_size+8);
		return 1;
	}
	// Second pass: encrypt with Blowfish after manipulating the key.
	unsigned int tmp = key[2];
	key[2] = key[3];
	key[3] = key[1];
	key[1] = tmp;
	key[0] ^= key[1];
	key[1] ^= key[3];
	key[2] += 0x4a6278a3;
	EVP_CIPHER_CTX_init(&ctx);
	int outlen2 = 0;
	EVP_EncryptInit_ex(&ctx, EVP_bf_cbc(), NULL, (void *)key, (void *)iv);
	EVP_EncryptUpdate(&ctx, outbuf2, &outlen2, outbuf, outlen);
	// Don't need to finalize, always an even number of blocks.

	fwrite(iv, 2, sizeof(unsigned int), out);
	fwrite(outbuf, outlen, sizeof(char), out);
	fclose(in);
	fclose(out);
}
