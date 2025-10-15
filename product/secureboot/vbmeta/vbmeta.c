// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#include "vbmeta.h"

#include <common.h>
#include <malloc.h>

#include "macro_utils.h"
#include "x509_parser.h"
#include "security_boot_struct.h"
#include "securec.h"
#define PARTITION_MAX_NUM 20

/**
 * get all partition's rsa public key into global variable.
 */
rsa_pub_key_list *get_rsa_pub_key_list_from_vbmeta(const unsigned char *vbmeta_addr, unsigned int *key_num)
{
	int ret;
	int offset = 0;
	int i;
	unsigned int x509_cert_len;

	var_is_null_return(vbmeta_addr, NULL);
	var_is_null_return(key_num, NULL);

	image_head_struct *image_head = (image_head_struct *)vbmeta_addr;

	// skip 8k header and magic number
	offset += (ALIGN_LEN + MAGIC_NUMBER_LEN);
	// get key_num
	ret = memcpy_s(key_num, sizeof(unsigned int), vbmeta_addr + offset, sizeof(unsigned int));
	check_return(ret != EOK, NULL, printf("get_rsa_pub_key_list_from_vbmeta memcpy key_num failed\n"));
	offset += sizeof(unsigned int);

	check_return((*key_num) > PARTITION_MAX_NUM, NULL,
		printf("get_rsa_pub_key_list_from_vbmeta key_num is bigger than PARTITION_MAX_NUM: %d\n", PARTITION_MAX_NUM));

	// malloc memory for g_pub_key_list
	rsa_pub_key_list *key_list = (rsa_pub_key_list *)malloc((*key_num) * sizeof(rsa_pub_key_list));
	check_return(key_list == NULL, NULL, printf("get_rsa_pub_key_list_from_vbmeta malloc key_list failed\n"));

	ret = memset_s(key_list, sizeof(rsa_pub_key_list) * (*key_num), 0, sizeof(rsa_pub_key_list) * (*key_num));
	check_return(ret != EOK, NULL, printf("get_rsa_pub_key_list_from_vbmeta memset faield\n"));

	for (i = 0; i < (*key_num); i++) {
		// skip key_struct_len
		offset += sizeof(unsigned int);
		// get partition_name
		ret = memcpy_s(key_list[i].partition_name, MAX_IMAGE_NAME_LEN, vbmeta_addr + offset, MAX_IMAGE_NAME_LEN);
		check_goto(ret != EOK, __EXIT_FAILURE__, printf("get_rsa_pub_key_list_from_vbmeta memcpy name failed\n"));

		offset += MAX_IMAGE_NAME_LEN;
		// get x509_cert_len
		ret = memcpy_s(&x509_cert_len, sizeof(unsigned int), vbmeta_addr + offset, sizeof(unsigned int));
		check_goto(ret != EOK, __EXIT_FAILURE__, printf("get_rsa_pub_key_list_from_vbmeta memcpy cert_len failed\n"));

		offset += sizeof(unsigned int);
		// parser x509_cert_content to rsa public key
		if (image_head->sign_info.signature_len == SIGN_LEN_2048) {
			key_list[i].pub_key = create_rsa_pub_key(RSA_PUB_KEY_TYPE_2048);
		} else {
			key_list[i].pub_key = create_rsa_pub_key(RSA_PUB_KEY_TYPE_4096);
		}

		check_goto(key_list[i].pub_key == NULL, __EXIT_FAILURE__,
			printf("vbmeta_pub_key_list_init create_rsa_pub_key failed\n"));

		ret = x509_convert_to_rsa_pub_key(vbmeta_addr + offset, x509_cert_len, key_list[i].pub_key);
		check_goto(ret != 0, __EXIT_FAILURE__, printf("vbmeta_pub_key_list_init x509_convert_to_rsa_pub_key failed\n"));
		offset += x509_cert_len;
	}
	return key_list;
__EXIT_FAILURE__:
	destroy_rsa_pub_key_list(key_list, *key_num);
	key_list = NULL;
	return NULL;
}

/**
 * free global variable storing all partition's rsa public key.
 */
void destroy_rsa_pub_key_list(rsa_pub_key_list *key_list, unsigned int key_num)
{
	unsigned int i;
	if (key_list == NULL)
		return;
	if (key_num > PARTITION_MAX_NUM) {
		printf("destroy_rsa_pub_key_list key_num is bigger than PARTITION_MAX_NUM: %d\n", PARTITION_MAX_NUM);
		key_num = PARTITION_MAX_NUM;
	}
	for (i = 0; i < key_num; i++)
		if (key_list[i].pub_key != NULL) {
			destroy_rsa_pub_key(key_list[i].pub_key);
			key_list[i].pub_key = NULL;
		}

	free(key_list);
}