// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __RSA_PUBLIC_H__
#define __RSA_PUBLIC_H__

#define RSA_2048_N_LEN 256
#define RSA_2048_E_LEN 4

#define RSA_4096_N_LEN 512
#define RSA_4096_E_LEN 4

typedef enum {
	RSA_PUB_KEY_TYPE_2048,
	RSA_PUB_KEY_TYPE_4096
} rsa_pub_key_type;

typedef struct rsa_pub_key_tag {
	unsigned char *n;
	unsigned int n_len;
	unsigned char *e;
	unsigned int e_len;
	rsa_pub_key_type type;
} rsa_pub_key;

/**
 * malloc memory for creating rsa pub key by type(2048 / 4096).
 */
rsa_pub_key *create_rsa_pub_key(rsa_pub_key_type type);

/**
 * free memory of rsa pub key.
 */
void destroy_rsa_pub_key(rsa_pub_key *pub_key);

#endif