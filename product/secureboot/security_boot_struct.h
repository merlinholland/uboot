// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __SECURITY_BOOT_STRUCT_H__
#define __SECURITY_BOOT_STRUCT_H__

#define AES_KEY_MAX_LEN 64
#define AES_IV_MAX_LEN 64
#define AES_KEY_LEN 16
#define ENCRYPT_BIT_WIDTH 128
#define AES_IV_LEN 16
#define MAGIC_NUMBER_LEN 32
#define HEADER_VERSION_LEN 12
#define IMAGE_BLOCK_OFFSET_LEN 5
#define IMAGE_BLOCK_LENGTH_LEN 5
#define IMAGE_NAME_LEN 64
#define ALIGN_LEN (8 * 1024)
#define SIGN_LEN_2048 256
#define SIGN_LEN_4096 512
#define SIGN_INFO_RESERVED_LEN 25
#define ENCRYPT_INFO_RESERVED_LEN 20
#define HASH_LEN 32

#define MAX_IMAGE_NAME_LEN 64

#define IMAEG_MAGIC "ImgHead_MagicNum"
#define VBMETA_MAGIC "ImgHead_MagicNum"

#define KERNEL_IMAGE_NAME "kernel"
#define VBMETA_IMAGE_NAME "vbmeta"

#ifndef NULL
#define NULL 0
#endif

typedef struct {
	char magic_number[MAGIC_NUMBER_LEN];               // magic_number: "ImgHead_MagicNum"
	char header_version[HEADER_VERSION_LEN];           // header_version: "V1.0.0.3"
	unsigned int total_len;                                     // total length
	unsigned int code_offset;                                   // image offset
	unsigned int signed_image_len;                              // signed image file size
	unsigned int signature_offset;                              // signed image offset
	unsigned int signature_len;                                 // signature length
	unsigned int block_num;                                     // image block number
	unsigned int block_offset[IMAGE_BLOCK_OFFSET_LEN];          // each block offset
	unsigned int block_length[IMAGE_BLOCK_LENGTH_LEN];          // each block length
	unsigned int hash_type;
	unsigned int rsa_sign_scheme;
	unsigned int otp_update_flag;        // 0 don't update otp, 1- update otp
	unsigned int reserved[SIGN_INFO_RESERVED_LEN];              // reserved
	unsigned int image_version_num;                             // image version num
	char image_name[IMAGE_NAME_LEN];                      // image name
} sign_info_struct; /* 240 Bytes */

typedef struct {
	unsigned int is_encrypted;                                // if the image is encrypted: 1(true), 0(false)
	unsigned int encrypted_image_offset;                      // encrypted image offset
	unsigned int encrypted_image_len;                         // encrypted image length
	unsigned int encrypt_alg;                                 // AES
	unsigned int encrypt_work_mode;                           // CBC
	unsigned int encrypt_bit_width;                           // 128
	unsigned int otp_keylad_no;                               // 1 - otp1 2 - otp2 3 - otp3 4 -otp4
	unsigned char protection_key_enc[AES_KEY_MAX_LEN];        // key encrypted by otp_aes_root_key
	unsigned int protection_key_enc_len;                      // 16/32
	unsigned char iv[AES_KEY_MAX_LEN];                        // iv
	unsigned int iv_len;                                      // 16
	unsigned int encrypt_reserved[ENCRYPT_INFO_RESERVED_LEN]; // encrypt reserved
} encrypt_info_struct;  /* 244 Bytes */

typedef struct {
	sign_info_struct sign_info;
	encrypt_info_struct encrypt_info;
} image_head_struct; /* 484 Bytes */

#endif
