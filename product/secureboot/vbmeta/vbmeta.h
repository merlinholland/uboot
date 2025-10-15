// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2020 Shenshu Technologies CO., LIMITED.
 */

#ifndef __VBMETA_H__
#define __VBMETA_H__

#define MAX_IMAGE_NAME_LEN 	64
#define ALIGN_LEN 		(8 * 1024)
#define MAGIC_NUMBER_LEN 	32

struct rsa_pub_key_tag;
typedef struct rsa_pub_key_tag rsa_pub_key;

typedef struct rsa_pub_key_list_tag {
	rsa_pub_key *pub_key;
	char partition_name[MAX_IMAGE_NAME_LEN];
} rsa_pub_key_list;

/**
 * get rsa public key list and key num from vbmeta.
 */
rsa_pub_key_list *get_rsa_pub_key_list_from_vbmeta(const unsigned char *vbmeta_addr, unsigned int *key_num);

/**
 * free memory for rsa_pub_key_list created by function get_rsa_pub_key_list_from_vbmeta().
 */
void destroy_rsa_pub_key_list(rsa_pub_key_list *key_list, unsigned int key_num);
#endif