// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */
#include "secureboot_log.h"

#if defined(CONFIG_SECUREBOOT_DEBUG_ENABLE)
void dump_data(const char *name, const unsigned char *data, unsigned int data_len)
{
	unsigned int i;
	unsigned int dump_data_len = data_len > MAX_DUMP_LEN ? MAX_DUMP_LEN : data_len;
	printf("%s(addr: 0x%p, length: %u):\n", name, data, dump_data_len);
	for (i = 0; i < dump_data_len; i++) {
		printf("0x%02x ", data[i]);
		if ((i + 1) % HEX_BASE == 0) {
			printf("\n");
		}
	}
	if (dump_data_len % HEX_BASE != 0) {
		printf("\n");
	}
}

void image_header_dump(const image_head_struct *image_head)
{
	const sign_info_struct *sign_info = &image_head->sign_info;
	const encrypt_info_struct *encrypt_info = &image_head->encrypt_info;

	/* Dump Sign Info. */
	printf("magic_number is %s\n", sign_info->magic_number);
	printf("header_version is %s\n", sign_info->header_version);
	printf("total_len is %u\n", sign_info->total_len);
	printf("signature_len is %u\n", sign_info->signature_len);
	printf("rsa_sign_scheme is 0x%x\n", sign_info->rsa_sign_scheme);
	printf("otp_update_flag is 0x%x\n", sign_info->otp_update_flag);
	printf("image_version_num is 0x%x\n", sign_info->image_version_num);
	printf("image_name is %s\n", sign_info->image_name);

	/* Dump Encrypt Info. */
	if (encrypt_info->is_encrypted) {
		printf("is_encrypted is %u\n", encrypt_info->is_encrypted);
		printf("encrypt_alg is %u\n", encrypt_info->encrypt_alg);
		printf("encrypt_work_mode is %u\n", encrypt_info->encrypt_work_mode);
		printf("encrypt_bit_width is %u\n", encrypt_info->encrypt_bit_width);
	}
}
#else
void dump_data(const char *name, const unsigned char *data, unsigned int data_len)
{
	(void)name;
	(void)data;
	(void)data_len;
}

void image_header_dump(const image_head_struct *image_head)
{
	(void)image_head;
}
#endif