// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "x509_parser.h"

#include <common.h>

#include "security_boot_struct.h"
#include "macro_utils.h"
#include "securec.h"

#define MAX_INDEX (RSA_4096_N_LEN * 2 + 1)

#define DEFAULT_RSA_E {     \
	0x00, 0x01, 0x00, 0x01  \
}

static unsigned int g_index = 0;
static unsigned int g_sign_len = 0;

static void deep_in(const unsigned char *arr, unsigned int len)
{
	unsigned int b1;
	g_index++;
	if ((g_index + 1) < len) {
		b1 = 0xFF & arr[g_index++];
		if (b1 > 0x0F)
			g_index += (0x0F & b1);
	}
}

static void parse(const unsigned char *arr, int need_return, unsigned char *back)
{
	g_index++;
	const unsigned int b1 = 0xFF & arr[g_index++];
	unsigned int i;
	unsigned int len = 0;
	if (b1 < 128) { // 128:the upper limit of char
		len = b1;
	} else {
		for (i = (b1 & 0x0f); i > 0; i--)
			len = (len << 8) + (0xff & arr[g_index++]); // 8:the hex length
	}

	g_index += len;
	if (need_return == 1) {
		int loop = 0;
		if (g_index > MAX_INDEX) {
			printf("x509_parser.c parse g_index is bigger than MAX_INDEX: %d\n", MAX_INDEX);
			return;
		}
		for (i = g_index - len; i < g_index; i++) {
			back[loop] = arr[i];
			loop++;
		}
	}
}

static void get_pub_key(const unsigned char *arr, unsigned char *modulus, unsigned int arr_len)
{
	unsigned int i;
	for (i = 0; i < 2; i++) // 2: outer code
		deep_in(arr, arr_len);

	for (i = 0; i < 6; i++) // 6: outer code
		parse(arr, 0, NULL);

	deep_in(arr, arr_len);
	parse(arr, 0, NULL);

	unsigned char temp[RSA_4096_N_LEN * 2 + 1] = { 0 }; // 2: double size
	parse(arr, 1, temp);
	g_index = 1;
	deep_in(temp, sizeof(temp));
	parse(temp, 1, modulus);
}

static void get_x509_modulus(const unsigned char *x509_content, unsigned int len, unsigned char *n, unsigned int n_len)
{
	int i;
	if (g_sign_len == SIGN_LEN_2048) {
		unsigned char modulus[RSA_2048_N_LEN + 1] = { 0 };
		get_pub_key(x509_content, modulus, len);
		for (i = 0; i < RSA_2048_N_LEN; i++)
			n[i] = modulus[i + 1];
	} else {
		unsigned char modulus[RSA_4096_N_LEN + 1] = { 0 };
		get_pub_key(x509_content, modulus, len);
		for (i = 0; i < RSA_4096_N_LEN; i++)
			n[i] = modulus[i + 1];
	}
}

/**
 * parse x509 certification to rsa public key.
 */
int x509_convert_to_rsa_pub_key(const unsigned char *x509_cert_content, unsigned int cert_len, rsa_pub_key *pub_key)
{
	int ret;
	unsigned char e_tmp[RSA_2048_E_LEN] = DEFAULT_RSA_E;
	var_is_null_return(x509_cert_content, -1);
	var_is_null_return(pub_key, -1);
	var_is_null_return(pub_key->n, -1);
	var_is_null_return(pub_key->e, -1);
	// reset g_index
	g_index = 0;
	// get g_sign_len
	if (pub_key->type == RSA_PUB_KEY_TYPE_2048) {
		g_sign_len = SIGN_LEN_2048;
	} else {
		g_sign_len = SIGN_LEN_4096;
	}

	// parser x509 and get the rsa public key n
	get_x509_modulus(x509_cert_content, cert_len, pub_key->n, pub_key->n_len);

	// init rsa public key e
	ret = memcpy_s(pub_key->e, pub_key->e_len, e_tmp, RSA_2048_E_LEN);
	if (ret != EOK) {
		printf("x509_convert_to_rsa_pub_key memcpy e failed\n");
		return -1;
	}
	return 0;
}