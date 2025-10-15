// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __CIPHER_ADAPTOR_H__
#define __CIPHER_ADAPTOR_H__

#include "ot_mpi_cipher.h"

#define CIPHER_MAX_KEY_SIZE_IN_WORD     8
#define CIPHER_MAX_IV_SIZE_IN_WORD      8

typedef struct {
	unsigned char *in_addr;
	unsigned char *out_addr;
	unsigned int len;
} cipher_data_t;

/**
 * rsa verify.
 */
int cipher_adaptor_rsa_verify(const ot_cipher_verify_data *verify_data, const ot_cipher_rsa_verify *rsa_verify);

/**
 * decrypt data.
 */
int cipher_adaptor_decrypt_data(const cipher_data_t *cipher_data, const ot_cipher_attr *cipher_attr,
	const ot_cipher_ctrl *cipher_ctrl);

/**
 * hash.
 */
int cipher_adaptor_hash_data(const unsigned char *input_data, unsigned int input_data_len, unsigned char *output_hash);
#endif
