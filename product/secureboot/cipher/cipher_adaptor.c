// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "cipher_adaptor.h"

#include <common.h>

#include "macro_utils.h"

int cipher_adaptor_rsa_verify(const ot_cipher_verify_data *verify_data, const ot_cipher_rsa_verify *rsa_verify)
{
	int ret;
	ot_cipher_rsa_sign_scheme sign_scheme = 0;

	var_is_null_return(verify_data, -1);
	var_is_null_return(rsa_verify, -1);

	/* Only Support PKCS1_PSS_SHA256 or Higher scheme. */
	sign_scheme = rsa_verify->scheme;
	if (sign_scheme != OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA256 &&
		sign_scheme != OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA384 &&
		sign_scheme != OT_CIPHER_RSA_SIGN_SCHEME_RSASSA_PKCS1_PSS_SHA512) {
		printf("Error: Unsupport RSA Sign Scheme!!!\n");
		return -1;
	}
	// 1) init cipher
	ret = ot_mpi_cipher_init();
	if (ret != 0) {
		printf("cipher_adaptor_rsa_verify ot_mpi_cipher_init failed\n");
		return -1;
	}

	// 2) verify
	ret = ot_mpi_cipher_rsa_verify(rsa_verify, verify_data);
	if (ret != 0) {
		printf("cipher_adaptor_rsa_verify ot_mpi_cipher_rsa_verify failed\n");
		goto __EXIT_FAILURE__;
	}

__EXIT_FAILURE__:
	(void)ot_mpi_cipher_deinit();
	return ret;
}

int cipher_adaptor_decrypt_data(const cipher_data_t *cipher_data, const ot_cipher_attr *cipher_attr,
	const ot_cipher_ctrl *cipher_ctrl)
{
	int ret;
	unsigned int cipher_handle;

	var_is_null_return(cipher_data, -1);
	var_is_null_return(cipher_attr, -1);
	var_is_null_return(cipher_ctrl, -1);
	/* Required:
	 * 1. Hard Klad Support Only.
	 * 2. Only Support AES-128-CBC/AES-256-CBC.
	 */
	if (cipher_ctrl->key_by_ca != TD_TRUE ||
		cipher_ctrl->alg != OT_CIPHER_ALG_AES ||
		cipher_ctrl->bit_width != OT_CIPHER_BIT_WIDTH_128BIT ||
		cipher_ctrl->work_mode != OT_CIPHER_WORK_MODE_CBC ||
		(cipher_ctrl->key_len != OT_CIPHER_KEY_AES_128BIT && cipher_ctrl->key_len != OT_CIPHER_KEY_AES_256BIT)) {
		printf("Error: Invalid Config for Cipher Decrypt!!!\n");
		return -1;
	}

	// start decrypt
	// 1) init cipher
	ret = ot_mpi_cipher_init();
	if (ret != 0) {
		printf("cipher_adaptor_decrypt_data ot_mpi_cipher_init failed\n");
		return -1;
	}

	// 2) create cipher handle
	ret = ot_mpi_cipher_create_handle(&cipher_handle, cipher_attr);
	if (ret != 0) {
		printf("cipher_adaptor_decrypt_data ot_mpi_cipher_create_handle failed\n");
		goto __EXIT_FAILURE_1__;
	}

	// 3) config cipher handle
	ret = ot_mpi_cipher_config_handle(cipher_handle, cipher_ctrl);
	if (ret != 0) {
		printf("cipher_adaptor_decrypt_data ot_mpi_cipher_config_handle failed\n");
		goto __EXIT_FAILURE_0__;
	}

	// 4) decrypt data
	if (cipher_data->in_addr == NULL || cipher_data->out_addr == NULL) {
		printf("cipher_adaptor_decrypt_data cipher_data->in_addr == NULL || cipher_data->out_addr == NULL\n");
		goto __EXIT_FAILURE_0__;
	}
	ret = ot_mpi_cipher_decrypt(cipher_handle, (unsigned long)cipher_data->in_addr,
		(unsigned long)cipher_data->out_addr, cipher_data->len);
	if (ret != 0) {
		printf("decrypt_data ot_mpi_cipher_decrypt failed\n");
		goto __EXIT_FAILURE_0__;
	}

__EXIT_FAILURE_0__:
	(void)ot_mpi_cipher_destroy_handle(cipher_handle);
__EXIT_FAILURE_1__:
	(void)ot_mpi_cipher_deinit();
	return ret;
}

int cipher_adaptor_hash_data(const unsigned char *input_data, unsigned int input_data_len, unsigned char *output_hash)
{
	int ret;
	unsigned int hash_handle;
	ot_cipher_hash_attr hash_attr = { 0 };

	var_is_null_return(input_data, -1);
	var_is_null_return(output_hash, -1);

	// 1) init cipher
	ret = ot_mpi_cipher_init();
	if (ret != 0) {
		printf("cipher_adaptor_hash_data ot_mpi_cipher_init failed\n");
		return -1;
	}

	// 2) init hash_attr
	hash_attr.hmac_key = NULL;
	hash_attr.hmac_key_len = 0;
	hash_attr.sha_type = OT_CIPHER_HASH_TYPE_SHA256;

	// 3) hash init
	ret = ot_mpi_cipher_hash_init(&hash_attr, &hash_handle);
	if (ret != 0) {
		printf("cipher_adaptor_hash_data ot_mpi_cipher_hash_init failed\n");
		goto __EXIT_FAILURE__;
	}

	// 4) hash update
	ret = ot_mpi_cipher_hash_update(hash_handle, input_data, input_data_len);
	if (ret != 0) {
		printf("cipher_adaptor_hash_data ot_mpi_cipher_hash_update failed\n");
		goto __EXIT_FAILURE__;
	}

	// 5) hash final
	ret = ot_mpi_cipher_hash_final(hash_handle, output_hash);
	if (ret != 0) {
		printf("cipher_adaptor_hash_data ot_mpi_cipher_hash_final failed\n");
		goto __EXIT_FAILURE__;
	}

	// 6) deinit
__EXIT_FAILURE__:
	(void)ot_mpi_cipher_deinit();
	return ret;
}