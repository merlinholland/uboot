// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */
#include "user_adaptor.h"

#include <common.h>

#include "otp_user_hal.h"
#include "vbmeta.h"
#include "security_boot_struct.h"
#include "cipher_adaptor.h"
#include "flash_read.h"
#include "rsa_public.h"
#include "macro_utils.h"
#include "sec_boot.h"
#include "securec.h"
#include "secureboot_log.h"

#define RSA_2048_N_LEN 		256
#define RSA_4096_N_LEN		512
#define RSA_DEFAULT_E_LEN 	4
#define PARTITION_MAX_NUM 20
#define MAX_VBMETA_IMAGE_SIZE (64 * 1024)

#define ROLLBACK_DISABLE_FLAG	0x55555555

static rsa_pub_key_list *g_key_list = NULL;
static unsigned int g_key_num = 0;

// define variable to support anti-rollback
static unsigned int g_otp_rollback_flag = ~ROLLBACK_DISABLE_FLAG;
static unsigned int g_otp_update_flag = 0;          // 0 - don't update otp, 1 - update otp
static unsigned int g_otp_version = 0;
static unsigned int g_vbmeta_version = 0;

// define data_handle_addr
static unsigned char *g_load_addr = NULL;

unsigned int vbmeta_offset = 0;

static int inner_rsa_verify(const unsigned char *data, unsigned data_len, const rsa_pub_key *pub_key,
	const sign_info_struct *sign_info)
{
	int ret;
	unsigned int sign_len = sign_info->signature_len;
	ot_cipher_verify_data verify_data = { 0 };
	ot_cipher_rsa_verify rsa_verify = { 0 };

	// 1) init verify_data
	verify_data.in = data;
	verify_data.in_len = data_len - sign_len;
	verify_data.sign = (unsigned char *)(data + data_len - sign_len);
	verify_data.sign_len = sign_len;

	// 2) init rsa_verify
	rsa_verify.pub_key.n = pub_key->n;
	rsa_verify.pub_key.n_len = pub_key->n_len;
	rsa_verify.pub_key.e = pub_key->e;
	rsa_verify.pub_key.e_len = pub_key->e_len;
	rsa_verify.scheme = sign_info->rsa_sign_scheme;

	// 3) verify data
	ret = cipher_adaptor_rsa_verify(&verify_data, &rsa_verify);
	if (ret != 0) {
		printf("inner_rsa_verify cipher_adaptor_rsa_verify failed\n");
		return ret;
	}
	return ret;
}

static int inner_check_vbmeta_pub_key(const rsa_pub_key *vbmeta_key)
{
	int ret;
	unsigned char data_hash[HASH_LEN] = { 0 };
	unsigned char otp_hash[HASH_LEN] = { 0 };
	unsigned char input_data[RSA_4096_N_LEN * 2] = { 0 };	// 2: double

	if (vbmeta_key->n_len + vbmeta_key->e_len > RSA_4096_N_LEN * 2) {	// 2: double
		printf("inner_check_vbmeta_pub_key rsa key_len is wrong\n");
		return -1;
	}

	// 1) copy n and e
	ret = memcpy_s(input_data, RSA_4096_N_LEN, vbmeta_key->n, vbmeta_key->n_len);
	if (ret != EOK) {
		printf("inner_check_vbmeta_pub_key memcpy n failed\n");
		return -1;
	}
	ret = memcpy_s(input_data + vbmeta_key->n_len, RSA_4096_N_LEN, vbmeta_key->e, vbmeta_key->e_len);
	if (ret != EOK) {
		printf("inner_check_vbmeta_pub_key memcpy e failed\n");
		return -1;
	}

	// 2) get hash
	ret = cipher_adaptor_hash_data(input_data, vbmeta_key->n_len + vbmeta_key->e_len, data_hash);
	if (ret != 0) {
		printf("inner_check_vbmeta_pub_key cipher_adaptor_hash_data failed\n");
		return -1;
	}

	// 3) get hash from otp
	ret = otp_user_hal_read_hash(otp_hash, HASH_LEN);
	if (ret != 0) {
		printf("inner_check_vbmeta_pub_key otp_user_hal_read_hash failed\n");
		return -1;
	}

	// 4) compare hash with otp_hash
	if (memcmp(data_hash, otp_hash, HASH_LEN) != 0) {
		printf("inner_check_vbmeta_pub_key memcmp failed\n");
		return -1;
	}
	return 0;
}
static int inner_build_vbmeta_key(const unsigned char *vbmeta_load_addr, rsa_pub_key *vbmeta_key)
{
	vbmeta_key->n_len = *(unsigned int *)(vbmeta_load_addr + sizeof(unsigned int) * 2);	// 2: skip two int
	vbmeta_key->e_len = *(unsigned int *)(vbmeta_load_addr + sizeof(unsigned int) * 3);	// 3: skip three int
	if (vbmeta_key->n_len != vbmeta_key->e_len) {
		printf("inner_verify_vbmeta n_len != e_len\n");
		return -1;
	}
	if (vbmeta_key->n_len == RSA_2048_N_LEN) {
		vbmeta_key->type = RSA_PUB_KEY_TYPE_2048;
	} else if (vbmeta_key->n_len == RSA_4096_N_LEN) {
		vbmeta_key->type = RSA_PUB_KEY_TYPE_4096;
	} else {
		printf("inner_verify_vbmeta invalid sign len\n");
		return -1;
	}
	int skip_count = sizeof(unsigned int) * 4;	// 4: skip four int
	vbmeta_key->n = (unsigned char *)(vbmeta_load_addr + skip_count);
	vbmeta_key->e = (unsigned char *)(vbmeta_load_addr + skip_count + vbmeta_key->n_len);
	vbmeta_offset = skip_count + vbmeta_key->n_len * 2;	// 2: double
	return 0;
}


static int inner_verify_vbmeta(unsigned char *vbmeta_load_addr, unsigned long offset)
{
	rsa_pub_key vbmeta_key = { 0 };
	unsigned int vbmeta_version = 0;
	// 1) read total_len from flash
	int ret = flash_read_aligned(offset, FLASH_ALIGNED_SIZE, vbmeta_load_addr);
	if (ret != 0) {
		printf("inner_verify_vbmeta flash_read_aligned total_len failed\n");
		return -1;
	}
	unsigned int total_len = *(unsigned int *)(vbmeta_load_addr + sizeof(unsigned int));		// skip magic_num
	if ((total_len % FLASH_ALIGNED_SIZE) != 0)
		total_len = total_len + (FLASH_ALIGNED_SIZE - total_len % FLASH_ALIGNED_SIZE);

	if (total_len > MAX_VBMETA_IMAGE_SIZE) {	// total_len is not bigger than MAX_VBMETA_IMAGE_SIZE(64K)
		printf("inner_verify_vbmeta total_len is bigger than %d(64K)\n", MAX_VBMETA_IMAGE_SIZE);
		return -1;
	}

	// 2) read total vbmeta image into memory from flash
	ret = flash_read_aligned(offset, total_len, vbmeta_load_addr);
	if (ret != 0) {
		printf("inner_verify_vbmeta flash_read_aligned vbmeta image failed\n");
		return -1;
	}

	// 3) get n_len and e_len
	ret = inner_build_vbmeta_key(vbmeta_load_addr, &vbmeta_key);
	if (ret != 0) {
		printf("inner_verify_vbmeta inner_build_vbmeta_key failed\n");
		return -1;
	}

	ret = inner_check_vbmeta_pub_key(&vbmeta_key);
	if (ret != 0) {
		printf("inner_verify_vbmeta inner_check_vbmeta_pub_key failed\n");
		return -1;
	}

	// 4) relocate the real start of vbmeta image
	vbmeta_load_addr = vbmeta_load_addr + vbmeta_offset;
	image_head_struct *image_head = (image_head_struct *)vbmeta_load_addr;

	/* For Debug. */
	image_header_dump(image_head);

	// 5) verify vbmeta header
	ret = inner_rsa_verify(vbmeta_load_addr, ALIGN_LEN, &vbmeta_key, &(image_head->sign_info));
	if (ret != 0) {
		printf("inner_verify_vbmeta inner_rsa_verify header failed\n");
		return -1;
	}

	// 6) anti-rollback handle
	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) {  // support anti-rollback
		vbmeta_version = image_head->sign_info.image_version_num;
		if (vbmeta_version < g_otp_version) {
			printf("error: the image is rollback!!!\n");
			return -1;
		}
	}
	/* redundancy check */
	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) {
		vbmeta_version = image_head->sign_info.image_version_num;
		if (g_otp_version > vbmeta_version) {
			printf("error: the image is rollback!!!\n");
			return -1;
		}
	}

	// 7) verify vbmeta body
	ret = inner_rsa_verify(vbmeta_load_addr, image_head->sign_info.total_len, &vbmeta_key, &(image_head->sign_info));
	if (ret != 0) {
		printf("inner_verify_vbmeta inner_rsa_verify failed\n");
		return -1;
	}

	return ret;
}

static int inner_decrypt_data(unsigned char *in_out_addr, unsigned int data_len, const encrypt_info_struct *encrypt_info)
{
	int ret;
	cipher_data_t cipher_data = { 0 };
	ot_cipher_attr cipher_attr = { 0 };
	ot_cipher_ctrl cipher_ctrl = { 0 };

	// 1) judge if the image is encrypted
	if (encrypt_info->is_encrypted == 0)
		return 0;

	// 2) init cipher_data
	cipher_data.in_addr = in_out_addr;
	cipher_data.out_addr = in_out_addr;
	cipher_data.len = data_len;

	// 3) init cipher attr
	cipher_attr.cipher_type = OT_CIPHER_TYPE_NORMAL;

	// 4) init cipher_ctrl
	ret = memcpy_s(cipher_ctrl.key, AES_KEY_MAX_LEN, encrypt_info->protection_key_enc, AES_KEY_MAX_LEN);
	if (ret != EOK) {
		printf("inner_decrypt_data memcpy key failed\n");
		return -1;
	}
	ret = memcpy_s(cipher_ctrl.iv, AES_IV_LEN, encrypt_info->iv, AES_IV_LEN);
	if (ret != EOK) {
		printf("inner_decrypt_data memcpy iv failed\n");
		return -1;
	}
	cipher_ctrl.key_by_ca = 1;
	cipher_ctrl.ca_type = encrypt_info->otp_keylad_no;
	cipher_ctrl.alg = encrypt_info->encrypt_alg;
	cipher_ctrl.bit_width = encrypt_info->encrypt_bit_width;
	cipher_ctrl.work_mode = encrypt_info->encrypt_work_mode;
	cipher_ctrl.key_len = encrypt_info->protection_key_enc_len;
	cipher_ctrl.chg_flags.bits_iv = 1;
	// 5) decrypt data
	ret = cipher_adaptor_decrypt_data(&cipher_data, &cipher_attr, &cipher_ctrl);
	if (ret != 0) {
		printf("inner_decrypt_data cipher_adaptor_decrypt_data failed\n");
		return ret;
	}

	return ret;
}

int user_init(const partition_info *vbmeta_info, unsigned char *load_addr)
{
	int ret;
	image_head_struct *image_head = NULL;
	unsigned int rollback_flag = 1;

	var_is_null_return(vbmeta_info, -1);

	g_load_addr = load_addr;
	// 1) get rollback_flag
	ret = otp_user_hal_read(OTP_ROLLBACK_FLAG_ADDR, &rollback_flag);
	if (ret != 0) {
		printf("user_init otp_user_hal_read failed\n");
		return -1;
	}

	if (rollback_flag != 0) {
		g_otp_rollback_flag = ~ROLLBACK_DISABLE_FLAG;
	} else {
		g_otp_rollback_flag = ROLLBACK_DISABLE_FLAG;
	}

	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) {
		printf("info: start anti-rollback\n");
		// 2) get otp version if support anti-rollback
		ret = user_read_version_from_persistent_area(&g_otp_version);
		if (ret != 0) {
			printf("user_init user_read_version_from_persistent_area failed\n");
			return -1;
		}
	} else {
		printf("info: don't support anti-rollback\n");
	}

	// 3) verify vbmeta
	ret = inner_verify_vbmeta(g_load_addr, vbmeta_info->offset);
	if (ret != 0) {
		printf("user_init inner_verify_vbmeta failed\n");
		return -1;
	}

	// 4) get rsa public key list
	g_key_list = get_rsa_pub_key_list_from_vbmeta(g_load_addr + vbmeta_offset, &g_key_num);
	if (g_key_list == NULL) {
		printf("user_init get_rsa_pub_key_list_from_vbmeta failed\n");
		return -1;
	}

	// 5) get otp update flag
	image_head = (image_head_struct *)(g_load_addr + vbmeta_offset);
	var_is_null_return(image_head, -1);
	g_otp_update_flag = image_head->sign_info.otp_update_flag;

	// 6) get vbmeta_version
	g_vbmeta_version = image_head->sign_info.image_version_num;

	return 0;
}

int user_rsa_verify_partition(const char *partition_name, unsigned long offset, unsigned char *load_addr,
	const rsa_pub_key *pub_key)
{
	int ret;
	image_head_struct *image_head = NULL;

	var_is_null_return(pub_key, -1);

	// 1) read image header
	ret = flash_read_aligned(offset, ALIGN_LEN, g_load_addr);
	check_return(ret != 0, -1, printf("user_rsa_verify_partition flash_read_aligned failed for %s\n", partition_name));

	// 2) verify image header
	image_head = (image_head_struct *)g_load_addr;
	ret = inner_rsa_verify(g_load_addr, ALIGN_LEN, pub_key, &(image_head->sign_info));
	if (ret != 0) {
		printf("user_rsa_verify_partition inner_rsa_verify header failed for %s\n", partition_name);
		return -1;
	}

	/* For Debug. */
	image_header_dump(image_head);

	// 3) anti-rollback
	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) {
		if (image_head->sign_info.image_version_num != g_vbmeta_version) {
			printf("user_rsa_verify_partition failed image %s version is not equal to vbmeta\n", partition_name);
			return -1;
		}
	}
	/* redundancy check */
	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) {
		if (image_head->sign_info.image_version_num != g_vbmeta_version) {
			printf("user_rsa_verify_partition failed image %s version is not equal to vbmeta\n", partition_name);
			return -1;
		}
	}

	// 4) read image body
	unsigned int total_len = image_head->sign_info.total_len - ALIGN_LEN;
	if ((total_len % FLASH_ALIGNED_SIZE) != 0)
		total_len = total_len + (FLASH_ALIGNED_SIZE - total_len % FLASH_ALIGNED_SIZE);

	ret = flash_read_aligned(offset + ALIGN_LEN, total_len, g_load_addr + ALIGN_LEN);
	if (ret != 0) {
		printf("user_rsa_verify_partition flash_read_aligned body failed for %s\n", partition_name);
		return -1;
	}

	// 5) decrypt image body
	ret = inner_decrypt_data(g_load_addr + ALIGN_LEN, image_head->sign_info.total_len - ALIGN_LEN,
		&(image_head->encrypt_info));
	if (ret != 0) {
		printf("user_rsa_verify_partition inner_decrypt_data failed for %s\n", partition_name);
		return -1;
	}

	// 6) verify image body
	ret = inner_rsa_verify(g_load_addr, image_head->sign_info.total_len, pub_key, &(image_head->sign_info));
	if (ret != 0) {
		printf("user_rsa_verify_partition inner_rsa_verify body failed for %s\n", partition_name);
		(void)memset_s(g_load_addr, image_head->sign_info.total_len, 0, image_head->sign_info.total_len);
		return -1;
	}

	// 7) memmove the image
	if (load_addr != 0) {
		ret = memcpy_s(load_addr, image_head->sign_info.total_len,
			g_load_addr + ALIGN_LEN, image_head->sign_info.total_len - ALIGN_LEN);
		(void)memset_s(g_load_addr, image_head->sign_info.total_len, 0, image_head->sign_info.total_len);
		if (ret != EOK) {
			printf("user_rsa_verify_partition memcpy image failed\n");
			return -1;
		}
		printf("%s load at addr: %#lx\n", partition_name, (uintptr_t)load_addr);
	}
	return 0;
}

/**
 * get rsa public key of image.
 */
rsa_pub_key *user_get_rsa_pub_key(const char *partition_name)
{
	unsigned int i;
	if (g_key_num > PARTITION_MAX_NUM) {
		printf("user_get_rsa_pub_key g_key_num is bigger than PARTITION_MAX_NUM:%d\n", PARTITION_MAX_NUM);
		return NULL;
	}
	var_is_null_return(g_key_list, NULL);
	for (i = 0; i < g_key_num; i++) {
		var_is_null_return(g_key_list[i].partition_name, NULL);
		if (strcmp(partition_name, g_key_list[i].partition_name) == 0)
			return g_key_list[i].pub_key;
	}

	printf("user_get_rsa_pub_key not found rsa pub key for %s\n", partition_name);
	return NULL;
}

/**
 * get version from persistent area to support anti-rollback.
 */
int user_read_version_from_persistent_area(unsigned int *version)
{
	unsigned int current_version;
	unsigned int i;
	int ret;
	var_is_null_return(version, -1);
	ret = otp_user_hal_read(OTP_MIN_ADDR, &current_version);
	if (ret != 0) {
		printf("user_read_version_from_persistent_area otp_user_hal_read failed\n");
		return -1;
	}

	// locate the first unlock key slot
	for (i = OTP_MIN_ADDR + 1; i <= OTP_MAX_ADDR; i++) {
		unsigned int prev_version = current_version;
		ret = otp_user_hal_read(i, &current_version);
		if (ret != 0) {
			printf("user_read_version_from_persistent_area otp_user_hal_read failed at %u\n", i);
			return -1;
		}

		if (current_version == 0) {
			*version = prev_version;
			return 0;
		}
	}
	*version = current_version;
	return 0;
}

/**
 * write version to persistent area to support anti-rollback.
 */
int user_write_version_to_persistent_area(unsigned int version)
{
	unsigned int i;

	for (i = OTP_MIN_ADDR; i <= OTP_MAX_ADDR; i++) {
		unsigned int otp_data;
		int ret = otp_user_hal_read(i, &otp_data);
		if (ret != 0) {
			printf("user_write_version_to_persistent_area otp_user_hal_read failed at %u\n", i);
			return -1;
		}

		if (otp_data != 0) {
			continue;
		}

		// the first unlock otp
		ret = otp_user_hal_write(i, version);
		if (ret != 0) {
			printf("user_write_version_to_persistent_area otp_user_hal_write failed at %u\n", i);
			return -1;
		}
		printf("update otp version to %u\n", version);
		return 0;
	}
	printf("info: otp is full!\n");
	return 0;
}

unsigned int user_get_rollback_flag(void)
{
	return g_otp_rollback_flag;
}

unsigned int user_get_version_update_flag(void)
{
	return g_otp_update_flag;
}

unsigned int user_get_current_version(void)
{
	return g_vbmeta_version;
}

unsigned int user_get_persistent_version(void)
{
	return g_otp_version;
}

/**
 * handle update version.
 */
int user_update_persistent_version(void)
{
	if (g_otp_rollback_flag != ROLLBACK_DISABLE_FLAG) { // support anti-rollback
		if (g_vbmeta_version > g_otp_version) {
			if (g_otp_update_flag == 0) { // don't set otp_update_flag
				printf("info: the image version(%u) is bigger than otp version(%u)\n",
					g_vbmeta_version, g_otp_version);
				printf("Make sure you set otp_update_flag = 0 intendedly\n");
				return 0;
			}
			// update otp version
			int ret = user_write_version_to_persistent_area(g_vbmeta_version);
			if (ret != 0) {
				printf("user_update_persistent_version user_write_version_to_persistent_area failed\n");
				return -1;
			}
		}
	}
	return 0;
}

void user_deinit(void)
{
	destroy_rsa_pub_key_list(g_key_list, g_key_num);
	g_key_list = NULL;
}
