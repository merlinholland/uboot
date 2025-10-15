// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "rsa_public.h"

#include <common.h>
#include <malloc.h>

/**
 * malloc memory for creating rsa pub key by type(2048 / 4096).
 */
rsa_pub_key *create_rsa_pub_key(rsa_pub_key_type type)
{
	// define default n_len and e_len
	unsigned int n_len;
	unsigned int e_len;
	rsa_pub_key *pub_key = NULL;
	switch (type) {
	case RSA_PUB_KEY_TYPE_2048:
		n_len = RSA_2048_N_LEN;
		e_len = RSA_2048_E_LEN;
		break;
	case RSA_PUB_KEY_TYPE_4096:
		n_len = RSA_4096_N_LEN;
		e_len = RSA_4096_E_LEN;
		break;
	default:
		n_len = RSA_2048_N_LEN;
		e_len = RSA_2048_E_LEN;
		break;
	}
	pub_key = (rsa_pub_key *)malloc(sizeof(rsa_pub_key));
	if (pub_key == NULL) {
		printf("create_rsa_pub_key malloc pub_key failed\n");
		return NULL;
	}
	pub_key->type = type;
	pub_key->n = (unsigned char *)malloc(n_len);
	if (pub_key->n == NULL) {
		printf("create_rsa_pub_key malloc pub_key->n failed\n");
		goto __EXIT_FAILURE__;
	}
	pub_key->n_len = n_len;

	pub_key->e = (unsigned char *)malloc(e_len);
	if (pub_key->e == NULL) {
		printf("create_rsa_pub_key malloc pub_key->e failed\n");
		goto __EXIT_FAILURE__;
	}
	pub_key->e_len = e_len;

	return pub_key;
__EXIT_FAILURE__:
	destroy_rsa_pub_key(pub_key);
	pub_key = NULL;
	return NULL;
}

/**
 * free memory of rsa pub key.
 */
void destroy_rsa_pub_key(rsa_pub_key *pub_key)
{
	if (pub_key == NULL)
		return;

	if (pub_key->n != NULL) {
		free(pub_key->n);
		pub_key->n = NULL;
	}
	if (pub_key->e != NULL) {
		free(pub_key->e);
		pub_key->e = NULL;
	}
	free(pub_key);
}